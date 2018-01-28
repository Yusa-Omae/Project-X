#include "ToolLib_Button.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "DxLib.h"
#include <string.h>

// 作成できるボタン情報の最大数
#define TOOL_BUTTON_MAX			(256)

// ボタンリピートの最初の待ち時間
#define FIRST_REPEAT_WAIT		(0.2f)

// ２回目以降のボタンリピートの間隔
#define REPEAT_WAIT				(0.017f)

// ボタン構造体
typedef struct _SToolButton
{
	// 関連付けられている表示状態情報ハンドル
	int    TVisibleHandle;

	// 関連付けられているウインドウ情報ハンドル
	int    TWindowHandle;

	// ＯＮ状態とＯＦＦ状態が押すたびに切り替わるタイプのボタンかどうか
	bool   On_Off_SwitchType;

	// ボタンの位置
	POINT  Position;

	// ボタンのサイズ
	SIZE   Size;

	// ボタンの名前の文字列が格納されているメモリアドレスを保存しているポインタ
	char  *Name;

	// 表示状態
	bool   Visible;

	// ボタンが押下状態かどうか
	bool   Bottom;

	// ボタンが押下されている時間
	float  BottomTime;

	// 最初のリピート処理が行われたかどうか
	bool   FirstRepeat;

	// リピート入力も考慮したボタン押下状態
	bool   RepeatBottom;

	// ボタンがクリックされたかどうか
	bool   Click;

	// ボタンのＯＮ／ＯＦＦ状態
	bool   OnOffFlag;
} SToolButton;

static int          g_TButtonNum;
static SToolButton *g_TButton[ TOOL_BUTTON_MAX ];

// ボタン情報の状態推移処理を行う
static void ToolButton_Step(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 進める時間( 単位 : 秒 )
	float StepTime
);

// ボタン情報を画面に描画する
static void ToolButton_Draw(
	// ボタン情報ハンドル
	int TButtonHandle
);


// ボタン情報を作成する
//     戻り値 : ボタン情報ハンドル( 失敗した場合は -1 が返る )
int ToolButton_Create(
	// ＯＮ状態とＯＦＦ状態が押すたびに切り替わるタイプのボタンかどうか
	bool On_Off_SwitchType,

	// 名前となる文字列のアドレス
	const char *Name,
	
	// ボタンを表示する左上座標
	int x,
	int y,
	
	// ボタンの幅と高さ
	int w,
	int h
)
{
	int          NewHandle;
	SToolButton *TButton;

	// 既にボタン情報が最大数に達している場合はエラー終了
	if( g_TButtonNum == TOOL_BUTTON_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TButton[ NewHandle ]; NewHandle++ ){}

	// 新しいボタン情報を格納するためのメモリの確保
	TButton = ( SToolButton * )calloc( 1, sizeof( SToolButton ) );
	if( TButton == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TButton[ NewHandle ] = TButton;

	// ボタン情報の数を増やす
	g_TButtonNum++; 

	// 情報を初期化
	TButton->TVisibleHandle    = -1;
	TButton->On_Off_SwitchType = On_Off_SwitchType;
	TButton->TWindowHandle     = -1;
	TButton->Position.x        = x;
	TButton->Position.y        = y;
	TButton->Size.cx           = w;
	TButton->Size.cy           = h;
	TButton->Bottom            = false;
	TButton->BottomTime        = 0.0f;
	TButton->FirstRepeat       = false;
	TButton->Click             = false;
	TButton->OnOffFlag         = false;
	TButton->Visible           = true;
	if( !ToolButton_SetName( NewHandle, Name ) )
	{
		return -1;
	}

	// ハンドルを返す
	return NewHandle;
}

// ボタン情報を削除する
void ToolButton_Delete(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// 名前保存用にメモリを確保していた場合は解放
	if( TButton->Name )
	{
		free( TButton->Name );
		TButton->Name = NULL;
	}

	// 確保していたメモリを解放する
	free( TButton );
	g_TButton[ TButtonHandle ] = NULL;

	// ボタン情報の数を減らす
	g_TButtonNum--;
}

// 全てのボタン情報を削除する
void ToolButton_Delete_All( void )
{
	int i;

	// 有効な全てのボタンを削除する
	for( i = 0; i < TOOL_BUTTON_MAX; i++ )
	{
		if( g_TButton[ i ] != NULL )
		{
			ToolButton_Delete( i );
		}
	}
}

// ボタン情報に関連付ける表示状態情報をセットする
void ToolButton_SetVisibleHandle(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->TVisibleHandle = TVisibleHandle;
}

// ボタン情報の名前を設定する
//     戻り値 : 処理が正常に終了したかどうか( true:正常に終了した  false:エラー発生 )
bool ToolButton_SetName(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 名前となる文字列のアドレス
	const char *Name
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// 既に名前用にメモリを確保していた場合は解放する
	if( TButton->Name )
	{
		free( TButton->Name );
		TButton->Name = NULL;
	}

	// 名前のアドレスが有効な場合はメモリを確保して保存する
	if( Name != NULL )
	{
		TButton->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TButton->Name == NULL )
		{
			return false;
		}
		strcpy( TButton->Name, Name );
	}

	// 正常終了
	return true;
}

// ボタン情報と関連付けるウインドウ情報を設定する
void ToolButton_SetWindow(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->TWindowHandle = TWindowHandle;
}

// ボタン情報の位置を変更する
void ToolButton_SetPosition(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 変更後のボタンの座標
	int x,
	int y
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->Position.x = x;
	TButton->Position.y = y;
}

// ボタン情報のＯＮ／ＯＦＦ状態を変更する
void ToolButton_SetOnOffFlag(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 変更後のＯＮ／ＯＦＦ状態( true:ON  false:OFF )
	bool Flag
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->OnOffFlag = Flag;
}

// ボタン情報のＯＮ／ＯＦＦ状態を取得する
//     戻り値 : ＯＮ／ＯＦＦ状態( ture:ON  false:OFF )
bool ToolButton_GetOnOffFlag(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->OnOffFlag;
}

// ボタン情報のボタンがクリックされたかどうかを取得する
//     戻り値 : ボタンがクリックされたかどうか
//              ( true:クリックされた  false:クリックされていない )
bool ToolButton_GetClick(
	// ボタン情報ハンドル
	int TButtonHandle,

	// クリックされたかどうかの状態をリセットするかどうか
	// ( true:リセットする  false:リセットしない )
	bool Reset
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	bool         Ret;

	Ret = TButton->Click;
	if( Reset )
	{
		TButton->Click = false;
	}

	return Ret;
}

//     戻り値 : ボタンが押下されているかどうか
//              ( true:押下されている  false:押下されていない )
bool ToolButton_GetBottom(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->Bottom;
}

// ボタン情報のボタンが押下されているかどうかを取得する( リピート機能付き版 )
//     戻り値 : ボタンが押下されているかどうか
//              ( true:押下されている  false:押下されていない )
bool ToolButton_GetRepeatBottom(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->RepeatBottom;
}

// ボタン情報の表示状態を変更する
void ToolButton_SetVisible(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// 新しい表示状態を保存する
	TButton->Visible = Visible;
}

// ボタン情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolButton_GetVisible(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// 表示状態が false の場合は false を返す
	if( !TButton->Visible )
	{
		return false;
	}

	// 関連付けられているウインドウ情報の表示状態が false の場合は false を返す
	if( TButton->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TButton->TWindowHandle ) )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TButton->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TButton->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// ボタン情報の状態推移処理を行う
static void ToolButton_Step(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 進める時間( 単位 : 秒 )
	float StepTime
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	bool         In;
	bool         MouseEdgeInput;
	bool         MouseInput;
	bool         MouseRepeatInput;
	int          MouseX;
	int          MouseY;
	int          TempX;
	int          TempY;

	// 表示状態が false の場合は何もせず終了
	if( !ToolButton_GetVisible( TButtonHandle ) )
	{
		return;
	}

	// マウスカーソルの位置を取得
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// マウスの入力状態を取得
	MouseRepeatInput = false;
	MouseInput       = ( ToolStatic_GetMouseButton()     & MOUSE_INPUT_LEFT ) != 0;
	MouseEdgeInput   = ( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) != 0;

	// ボタンの表示領域にマウスカーソルがあるかどうかを調べる
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TButton->TWindowHandle, &TempX, &TempY );
		TempX += TButton->Position.x;
		TempY += TButton->Position.y;

		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy ) &&
				ToolWindow_CheckInfoAreaIn(
				TButton->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		In = ToolStatic_CheckMouseIn(
				TButton->Position.x,
				TButton->Position.y,
				TButton->Position.x + TButton->Size.cx,
				TButton->Position.y + TButton->Size.cy );
	}

	// 左ボタンが押されているかどうかで処理を分岐
	if( MouseInput )
	{
		// 左ボタンが押されている場合はリピート処理を行う
		TButton->BottomTime += StepTime;
		if( TButton->FirstRepeat )
		{
			if( TButton->BottomTime >= REPEAT_WAIT )
			{
				MouseRepeatInput      = true;
				TButton->BottomTime  -= REPEAT_WAIT;
			}
		}
		else
		{
			if( TButton->BottomTime >= FIRST_REPEAT_WAIT )
			{
				MouseRepeatInput      = true;
				TButton->FirstRepeat  = true;
				TButton->BottomTime  -= FIRST_REPEAT_WAIT;
			}
		}

		// 左ボタンが押された瞬間は必ずリピート入力考慮のボタン押下状態も true にする
		if( MouseEdgeInput )
		{
			MouseRepeatInput = true;
		}

		// ボタン表示領域にカーソルがあり、左ボタンが押された瞬間の場合は
		// ボタン押下状態にする
		if( MouseEdgeInput && In )
		{
			TButton->Bottom = true;
		}
	}
	else
	{
		// 左ボタンが押されていない場合はリピート処理をリセットする
		TButton->FirstRepeat = false;
		TButton->BottomTime  = 0.0f;

		// マウスカーソルがボタンの表示領域にあり、且つ今までボタンが押下状態
		// だった場合はクリックフラグを立て、ＯＮ／ＯＦＦ状態をＯＮ状態にする
		if( In && TButton->Bottom )
		{
			TButton->Click     = true;
			TButton->OnOffFlag = true;
		}

		// ボタン押下状態をリセットする
		TButton->Bottom      = false;
	}

	// リピート入力がされていて、且つマウスカーソルがボタンの表示領域に
	// あったらリピート入力考慮のボタン押下状態を true にする
	if( MouseRepeatInput && In )
	{
		TButton->RepeatBottom = true;
	}
	else
	{
		TButton->RepeatBottom = false;
	}
}

// ボタン情報を画面に描画する
static void ToolButton_Draw(
	// ボタン情報ハンドル
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	int          MouseX;
	int          MouseY;
	bool         In;
	int          TempX;
	int          TempY;
	int          TopLeftC1;
	int          TopLeftC2;
	int          RightBottomC1;
	int          RightBottomC2;
	int          CenterColor;
	int          DrawWidth;
	int          addx;
	int          addy;
	int          ButtonFontHandle;

	// 表示状態が false の場合は何もせず終了
	if( !ToolButton_GetVisible( TButtonHandle ) )
	{
		return;
	}

	// マウスカーソルの位置を取得
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// ボタン名描画用のフォントハンドルを取得
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// ボタン名の描画幅を取得
	DrawWidth = GetDrawStringWidthToHandle( TButton->Name, -1, ButtonFontHandle );

	// ボタンの表示位置とボタンの表示領域にマウスカーソルがあるかどうかを取得
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TButton->TWindowHandle, &TempX, &TempY );
		TempX += TButton->Position.x;
		TempY += TButton->Position.y;
		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy ) &&
			 ToolWindow_CheckInfoAreaIn(
				TButton->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		TempX = TButton->Position.x;
		TempY = TButton->Position.y;
		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy );
	}

	// ボタンが押されているか、若しくはＯＮ／ＯＦＦ切り替えモードのＯＮ状態の場合は
	// 押されているボタンの配色にする
	if( ( In && TButton->Bottom ) ||
		( TButton->On_Off_SwitchType && TButton->OnOffFlag ) )
	{
		addx          = 1;
		addy          = 1;
		CenterColor   = GetColor( 192,192,192 );
		TopLeftC1     = GetColor(   0,  0,  0 );
		TopLeftC2     = GetColor(  64, 64, 64 );
		RightBottomC1 = GetColor( 255,255,255 );
		RightBottomC2 = GetColor( 191,191,191 );
	}
	else
	{
		addx          = 0;
		addy          = 0;
		CenterColor   = GetColor( 255,255,255 );
		RightBottomC1 = GetColor(   0,  0,  0 );
		RightBottomC2 = GetColor(  64, 64, 64 );
		TopLeftC1     = GetColor( 255,255,255 );
		TopLeftC2     = GetColor( 191,191,191 );
	}

	// ボタン情報にウインドウ情報が関連付けられている場合は描画領域の制限設定を行う
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_SetupDrawArea( TButton->TWindowHandle );
	}

	// ボタンの描画
	Tool_DrawButton(
		TempX,
		TempY,
		TButton->Size.cx,
		TButton->Size.cy,
		TopLeftC1,
		TopLeftC2,
		RightBottomC1,
		RightBottomC2,
		CenterColor
	);

	// ボタン名の描画
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 180 );
	DrawStringToHandle(
		TempX + ( TButton->Size.cx - DrawWidth               ) / 2 + addx,
		TempY + ( TButton->Size.cy - TOOL_BUTTON_FONT_HEIGHT ) / 2 + addy,
		TButton->Name,
		GetColor( 0,0,0 ),
		ButtonFontHandle
	);

	// ブレンドモードと描画可能領域を元に戻す
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	SetDrawArea( 0, 0, 999999, 999999 );
}


// 全てのボタン情報の状態推移処理を行う
void ToolButton_Step_All(
	// 進める時間( 単位 : 秒 )
	float StepTime
)
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TButtonNum; i++ )
	{
		if( g_TButton[ i ] == NULL )
		{
			continue;
		}

		ToolButton_Step( i, StepTime );
		j++;
	}
}

// 全てのボタン情報の描画移処理を行う
void ToolButton_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TButtonNum; i++ )
	{
		if( g_TButton[ i ] == NULL )
		{
			continue;
		}

		ToolButton_Draw( i );
		j++;
	}
}

