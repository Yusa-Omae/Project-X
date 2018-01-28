#include "ToolLib_ParamBar.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "ToolLib_StringInput.h"
#include "DxLib.h"
#include <malloc.h>
#include <string.h>

// パラメータバー情報の最大数
#define TOOL_PARAMBAR_MAX			(256)

// バーのつまみの高さ
#define BAR_CURSOR_HEIGHT			(12)

// バーのつまみの幅の半分
#define BAR_CURSOR_HALF_WIDTH		(3)

// バーのY座標
#define BAR_Y						(5)

// パラメータバー構造体
typedef struct _SToolParamBar
{
	// 関連付けられている表示状態情報ハンドル
	int    TVisibleHandle;

	// 関連付けられているウインドウ情報ハンドル
	int    TWindowHandle;

	// 数値直接編集用の文字列入力情報ハンドル
	int    NumberTStringInputHandle;

	// 名前の文字列が格納されているメモリアドレスを保存しているポインタ
	char  *Name;

	// パラメータバーの表示位置
	int    PositionX;
	int    PositionY;

	// バーを表示する相対X座標
	int    BarPositionX;

	// バーの横幅
	int    BarWidth;

	// バーで操作できる範囲の値の最小値と最大値
	float  ParamMin;
	float  ParamMax;

	// 現在の数値
	float  Param;

	// 数値に変化があったかどうか
	bool   Change;

	// バーのつまみを掴んでいるかどうか
	bool   Grips;

	// バーのつまみを掴んだときのバー左端からみたX座標
	int    GripsX;
} SToolParamBar;

static int            g_TParamBarNum;
static SToolParamBar *g_TParamBar[ TOOL_PARAMBAR_MAX ];

// パラメータバー情報の状態推移処理を行う
static void ToolParamBar_Step(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// パラメータバー情報を画面に描画する
static void ToolParamBar_Draw(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// パラメータバー情報を作成する
//     戻り値 : パラメータバー情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolParamBar_Create(
	// 名前の文字列が格納されたメモリアドレス
	const char *Name,

	// 表示座標
	int PositionX,
	int PositionY,

	// バーを表示する相対X座標
	int BarPositionX,

	// バーの横幅
	int BarWidth,

	// 表示する数値の桁数
	int NumberLength,
	
	// バーで操作できる範囲の値の最小値と最大値
	float ParamMin,
	float ParamMax,

	// 初期値
	float InitParam
)
{
	int            NewHandle;
	SToolParamBar *TParamBar;

	// 既にパラメータバー情報が最大数に達している場合はエラー終了
	if( g_TParamBarNum == TOOL_PARAMBAR_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TParamBar[ NewHandle ]; NewHandle++ ){}

	// 新しいボタン情報を格納するためのメモリの確保
	TParamBar = ( SToolParamBar * )calloc( 1, sizeof( SToolParamBar ) );
	if( TParamBar == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TParamBar[ NewHandle ] = TParamBar;

	// パラメータバー情報の数を増やす
	g_TParamBarNum++; 

	// 情報を初期化
	TParamBar->TVisibleHandle = -1;
	TParamBar->TWindowHandle  = -1;
	TParamBar->Param          = InitParam;
	TParamBar->PositionX      = PositionX;
	TParamBar->PositionY      = PositionY;
	TParamBar->BarWidth       = BarWidth;
	TParamBar->ParamMin       = ParamMin;
	TParamBar->ParamMax       = ParamMax;
	TParamBar->Change         = false;
	TParamBar->Grips          = false;
	TParamBar->BarPositionX   = BarPositionX;
	if( Name )
	{
		TParamBar->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TParamBar->Name == NULL )
		{
			return -1;
		}
		strcpy( TParamBar->Name, Name );
	}

	// 数値編集用の文字列入力情報を作成する
	TParamBar->NumberTStringInputHandle = ToolStringInput_Create(
		NULL,
		PositionX + BarPositionX + BarWidth + 8,
		PositionY,
		true,
		NumberLength,
		0
	);
	if( TParamBar->NumberTStringInputHandle == -1 )
	{
		return -1;
	}

	// 初期値を設定
	ToolStringInput_SetFloatNumber( TParamBar->NumberTStringInputHandle, InitParam );

	// ハンドルを返す
	return NewHandle;
}

// パラメータバー情報を削除する
extern void ToolParamBar_Delete(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 名前保存用にメモリを確保していた場合は解放
	if( TParamBar->Name )
	{
		free( TParamBar->Name );
		TParamBar->Name = NULL;
	}

	// 数値編集用の文字列入力情報ハンドルを削除する
	ToolStringInput_Delete( TParamBar->NumberTStringInputHandle );

	// 確保していたメモリを解放する
	free( TParamBar );
	g_TParamBar[ TParamBarHandle ] = NULL;

	// パラメータバー情報の数を減らす
	g_TParamBarNum--;
}

// 全てのパラメータバー情報を削除する
void ToolParamBar_Delete_All( void )
{
	int i;

	// 有効な全てのパラメータバーを削除する
	for( i = 0; i < TOOL_PARAMBAR_MAX; i++ )
	{
		if( g_TParamBar[ i ] != NULL )
		{
			ToolParamBar_Delete( i );
		}
	}
}

// パラメータバー情報に関連付ける表示状態情報をセットする
extern void ToolParamBar_SetVisibleHandle(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 表示状態情報ハンドルを保存
	TParamBar->TVisibleHandle = TVisibleHandle;

	// 数値編集用の文字列入力情報ハンドルにも設定する
	ToolStringInput_SetVisibleHandle(
			TParamBar->NumberTStringInputHandle, TVisibleHandle );
}

// パラメータバー情報の表示状態を変更する
extern void ToolParamBar_SetVisible(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 数値編集用の文字列入力情報ハンドルに対して表状態設定を行う
	ToolStringInput_SetVisible( TParamBar->NumberTStringInputHandle, Visible );
}

// パラメータバー情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolParamBar_GetVisible(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 数値編集用の文字列入力情報ハンドルが非表示状態だったら false を返す
	if( !ToolStringInput_GetVisible( TParamBar->NumberTStringInputHandle ) )
	{
		return false;
	}

	// 関連付けられているウインドウ情報の表示状態が false の場合は false を返す
	if( TParamBar->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TParamBar->TWindowHandle ) )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TParamBar->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TParamBar->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// パラメータバー情報と関連付けるウインドウ情報を設定する
extern void ToolParamBar_SetWindow(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ウインドウ情報ハンドルを保存
	TParamBar->TWindowHandle = TWindowHandle;

	// 数値編集用の文字列入力情報ハンドルにもウインドウ情報ハンドルをセット
	ToolStringInput_SetWindow(
		TParamBar->NumberTStringInputHandle, TParamBar->TWindowHandle );
}

// パラメータバー情報の座標を変更する
extern void ToolParamBar_SetPosition(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 変更後の座標
	int PositionX,
	int PositionY
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 座標を保存
	TParamBar->PositionX = PositionX;
	TParamBar->PositionY = PositionY;

	// 数値編集用の文字列入力情報ハンドルの座標も変更
	ToolStringInput_SetPosition(
		TParamBar->NumberTStringInputHandle,
		TParamBar->PositionX + TParamBar->BarPositionX + TParamBar->BarWidth + 8,
		TParamBar->PositionY
	);
}

// パラメータバー情報の数値を変更する
extern void ToolParamBar_SetParam(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 変更後の数値
	float Param
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// パラメータを保存
	TParamBar->Param = Param;

	// 数値編集用の文字列入力情報ハンドルにも数値をセット
	ToolStringInput_SetFloatNumber( TParamBar->NumberTStringInputHandle, Param );
}

// パラメータバー情報のバーで変更できる数値の最大値と最小値を変更する
extern void ToolParamBar_SetMinMax(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 新しい最小値と最大値
	float ParamMin,
	float ParamMax
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	TParamBar->ParamMin = ParamMin;
	TParamBar->ParamMax = ParamMax;
}

// パラメータバー情報の数値編集が行われていた場合に無条件で終了させる
extern void ToolParamBar_AlwaysEnd(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// 数値編集用の文字列入力情報ハンドルに対して無条件終了の処理を行う
	ToolStringInput_AlwaysEnd( TParamBar->NumberTStringInputHandle );
}

// パラメータバー情報の数値に変化があったかどうかを取得する
//     戻り値 : 数値に変化があったかどうか( true:変化あり false:変化なし )
extern bool ToolParamBar_CheckChange(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	bool Result;

	// 数値に変化があったかどうかのフラグをローカル変数にコピーしておく
	Result = TParamBar->Change;

	// 数値に変化があったかどうかのフラグを倒す
	TParamBar->Change = false;

	// コピーしておいた変数の内容を返す
	return Result;
}

// パラメータバー情報の現在の数値を取得する
//     戻り値 : パラメータバーの値
extern float ToolParamBar_GetParam(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	return TParamBar->Param;
}

// パラメータバー情報の状態推移処理を行う
static void ToolParamBar_Step(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	int            CursorX;
	int            AreaX;
	int            AreaY;
	int            WinDrawX;
	int            WinDrawY;
	float          TempF;
	bool           In;
	int            MouseX;
	int            MouseY;

	// 非表示状態だった場合は何もせず終了
	if( !ToolParamBar_GetVisible( TParamBarHandle ) )
	{
		return;
	}

	// マウスカーソルの位置を取得
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// 数値編集用文字列入力情報に変化があった場合はバーにも反映する
	if( ToolStringInput_CheckEnd( TParamBar->NumberTStringInputHandle, true ) )
	{
		// 数値に変化があったかどうかのフラグを立てる
		TParamBar->Change = true;

		// 数値編集用文字列入力情報の現在の数値を取得する
		TempF = ToolStringInput_GetFloatNumber( TParamBar->NumberTStringInputHandle );

		// 数値が最小値や最大値の範囲からはみ出ていたら補正する
		if( TempF < TParamBar->ParamMin )
		{
			TempF = TParamBar->ParamMin;
		}
		if( TempF > TParamBar->ParamMax )
		{
			TempF = TParamBar->ParamMax;
		}

		// 数値を設定する関数で値を反映
		ToolParamBar_SetParam( TParamBarHandle, TempF );
	}

	// マウスの左ボタンが押されていなかったら
	// バーのつまみを掴んでいるかどうかのフラグを倒して終了
	if( ( ToolStatic_GetMouseButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		TParamBar->Grips = false;
		return;
	}

	// バーのつまみを掴んでいない場合は
	// マウスの左ボタンを押した瞬間ではない場合はここで終了
	if( !TParamBar->Grips &&
		( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		return;
	}

	// ウインドウ情報に関連付けられている場合はウインドウ情報の座標や
	// ウインドウ内にマウスカーソルがあるかどうかなどを取得する
	if( TParamBar->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TParamBar->TWindowHandle, &WinDrawX, &WinDrawY );
		In = ToolWindow_CheckInfoAreaIn( TParamBar->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
		In       = true;
	}

	// バー表示領域の左上座標を算出
	AreaX = TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX;
	AreaY = TParamBar->PositionY + WinDrawY;

	// つまみの位置を算出
	CursorX = ( int )( ( TParamBar->Param - TParamBar->ParamMin ) * TParamBar->BarWidth /
										( TParamBar->ParamMax - TParamBar->ParamMin ) );

	// つまみを掴んでいるかどうかで処理を分岐
	if( !TParamBar->Grips )
	{
		// つまみを掴んでいない場合はバーの表示領域にカーソルが無い場合は終了
		if( !In )
		{
			return;
		}

		// バーの表示領域からずれていたら終了
		if( MouseX < AreaX - BAR_CURSOR_HALF_WIDTH ||
			MouseX > AreaX + BAR_CURSOR_HALF_WIDTH + TParamBar->BarWidth ||
		    MouseY < AreaY ||
			MouseY > AreaY + BAR_CURSOR_HEIGHT )
		{
			return;
		}

		// つまみの外側をクリックした場合はつまみの位置をクリックした位置に移動する
		if( MouseX <  CursorX + AreaX - BAR_CURSOR_HALF_WIDTH ||
			MouseX >= CursorX + AreaX + BAR_CURSOR_HALF_WIDTH )
		{
			CursorX           = MouseX - AreaX;
			TParamBar->GripsX = 0;
		}
		else
		{
			TParamBar->GripsX = MouseX - ( AreaX + CursorX );
		}

		// つまみを掴んでいるかどうかのフラグを立てる
		TParamBar->Grips = true;
	}
	else
	{
		// つまみを掴んでいる場合は掴み始めたマウスカーソル位置と
		// 現在のマウスカーソル位置から現在のつまみの位置を決定する
		if( MouseX - TParamBar->GripsX < AreaX - BAR_CURSOR_HALF_WIDTH )
		{
			CursorX = 0;
		}
		else
		if( MouseX - TParamBar->GripsX >
					AreaX + TParamBar->BarWidth + BAR_CURSOR_HALF_WIDTH )
		{
			CursorX = TParamBar->BarWidth;
		}
		else
		{
			CursorX = MouseX - TParamBar->GripsX - AreaX;

			if( CursorX < 0 )
			{
				CursorX = 0;
			}

			if( CursorX > TParamBar->BarWidth ) 
			{
				CursorX = TParamBar->BarWidth;
			}
		}
	}

	// つまみの位置に応じてパラメータを算出する
	TempF = CursorX * ( TParamBar->ParamMax - TParamBar->ParamMin ) /
											TParamBar->BarWidth + TParamBar->ParamMin;

	// 値が変化していたら値が変化したかどうかのフラグを立て、
	// 数値編集用の文字列入力情報にも反映する
	if( TempF != TParamBar->Param )
	{
		TParamBar->Change = true;
		TParamBar->Param  = TempF;
		ToolStringInput_SetFloatNumber(
					TParamBar->NumberTStringInputHandle, TParamBar->Param );
	}
}

// パラメータバー情報を画面に描画する
static void ToolParamBar_Draw(
	// パラメータバー情報ハンドル
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	int            CursorX;
	int            WinDrawX;
	int            WinDrawY;

	// 非表示設定になっていたら何もせずに終了
	if( !ToolParamBar_GetVisible( TParamBarHandle ) )
	{
		return;
	}

	// ウインドウ情報に関連付けられている場合はウインドウ情報の座標を取得する
	if( TParamBar->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TParamBar->TWindowHandle, &WinDrawX, &WinDrawY );

		// ウインドウ情報を基にした描画範囲制限設定を行う
		ToolWindow_SetupDrawArea( TParamBar->TWindowHandle );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
	}

	// 名前がある場合は名前を描画する
	if( TParamBar->Name != NULL )
	{
		DrawStringToHandle(
			TParamBar->PositionX + WinDrawX,
			TParamBar->PositionY + WinDrawY,
			TParamBar->Name,
			GetColor( 255,255,255 ),
			ToolStatic_GetNormalFontHandle()
		);
	}

	// バーのつまみの位置を算出
	CursorX = ( int )( ( TParamBar->Param - TParamBar->ParamMin ) * TParamBar->BarWidth /
										( TParamBar->ParamMax - TParamBar->ParamMin ) );

	// バーを描画
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
	DrawBox(
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX,
		TParamBar->PositionY + WinDrawY + BAR_Y,
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + TParamBar->BarWidth,
		TParamBar->PositionY + WinDrawY + BAR_Y + 1,
		GetColor( 255,255,255 ),
		TRUE
	);

	// バーのつまみを描画
	DrawBox(
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + CursorX - BAR_CURSOR_HALF_WIDTH,
		TParamBar->PositionY + WinDrawY,
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + CursorX + BAR_CURSOR_HALF_WIDTH,
		TParamBar->PositionY + WinDrawY + BAR_CURSOR_HEIGHT,
		GetColor( 255,255,255 ),
		TRUE
	);

	// 描画設定を元に戻す
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	SetDrawArea( 0, 0, 999999, 999999 );
}

// 全てのパラメータバー情報の状態推移処理を行う
void ToolParamBar_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TParamBarNum; i++ )
	{
		if( g_TParamBar[ i ] == NULL )
		{
			continue;
		}

		ToolParamBar_Step( i );
		j++;
	}
}

// 全てのパラメータバー情報の描画移処理を行う
void ToolParamBar_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TParamBarNum; i++ )
	{
		if( g_TParamBar[ i ] == NULL )
		{
			continue;
		}

		ToolParamBar_Draw( i );
		j++;
	}
}

