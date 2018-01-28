#include "ToolLib_FileSelect.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Window.h"
#include "ToolLib_Button.h"
#include "ToolLib_StringInput.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include "DxLib.h"

// ファイル選択処理情報の最大数
#define TOOL_FILESELECT_MAX		(256)

// 名前を描画する座標
#define NAME_X					(4)
#define NAME_Y					(0)

// パス表示領域の左上座標
#define PATH_X					(4)
#define PATH_Y					(18)

// パス表示領域のサイズ
#define PATH_W					(242)
#define PATH_H					(17)

// ボタン表示領域のY座標
#define BUTTON_Y				(PATH_Y + PATH_H + 4)

// ボタン表示領域のサイズ
#define BUTTON_W				(42)
#define BUTTON_H				(20)

// 「参照」ボタンのX座標
#define OPEN_X					(4)

// 「適用」ボタンのX座標
#define APPLY_X					(OPEN_X + BUTTON_W + 4)

// ファイル選択構造体
typedef struct _SToolFileSelect
{
	// 関連付けられている表示状態情報ハンドル
	int    TVisibleHandle;

	// 関連付けられているウインドウ情報ハンドル
	int    TWindowHandle;

	// 「参照」ボタン情報ハンドル
	int    OpenTButtonHandle;

	// 「適用」ボタン情報ハンドル
	int    ApplyTButtonHandle;

	// ファイルパス編集用文字列入力情報ハンドル
 	int    TStringInputHandle;

	// 表示座標
	POINT  Position;

	// 名前の文字列が格納されているメモリアドレスを保存しているポインタ
	char  *Name;

	// 表示状態
	bool   Visible;

	// ファイル選択処理が終了しているかどうか
	bool   EndFlag;
} SToolFileSelect;

int              g_TFileSelectNum;
SToolFileSelect *g_TFileSelect[ TOOL_FILESELECT_MAX ];

// ファイル選択処理情報の状態推移処理を行う
static void ToolFileSelect_Step(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
);

// ファイル選択処理情報を画面に描画する
static void ToolFileSelect_Draw(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
);

// ファイル選択処理情報を作成する
//     戻り値 : ファイル選択処理情報ハンドル( 失敗した場合は -1 が返る )
int ToolFileSelect_Create(
	// 名前の文字列が格納されたメモリアドレス
	const char *Name,
	
	// ファイル選択処理の表示をする座標
	int x,
	int y
)
{
	int              NewHandle;
	SToolFileSelect *TFileSelect;

	// 既にボタン情報が最大数に達している場合はエラー終了
	if( g_TFileSelectNum == TOOL_FILESELECT_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TFileSelect[ NewHandle ]; NewHandle++ ){}

	// 新しいボタン情報を格納するためのメモリの確保
	TFileSelect = ( SToolFileSelect * )calloc( 1, sizeof( SToolFileSelect ) );
	if( TFileSelect == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TFileSelect[ NewHandle ] = TFileSelect;

	// 文字列入力情報の数を増やす
	g_TFileSelectNum++; 

	// 「参照」ボタンを作成
	TFileSelect->OpenTButtonHandle  =
					ToolButton_Create( false, "参照", 0, 0, BUTTON_W, BUTTON_H );
	if( TFileSelect->OpenTButtonHandle == -1 )
	{
		return -1;
	}

	// 「適用」ボタンを作成
	TFileSelect->ApplyTButtonHandle =
					ToolButton_Create( false, "適用", 0, 0, BUTTON_W, BUTTON_H ); 
	if( TFileSelect->ApplyTButtonHandle == -1 )
	{
		return -1;
	}

	// ファイルパス編集用文字列入力情報を作成
	TFileSelect->TStringInputHandle =
			ToolStringInput_Create( NULL, x + PATH_X, y + PATH_Y, false, 100, PATH_W );
	if( TFileSelect->TStringInputHandle == -1 )
	{
		return -1;
	}

	// 情報を初期化
	TFileSelect->TVisibleHandle = -1;
	TFileSelect->TWindowHandle  = -1;
	TFileSelect->Visible        = true;
	TFileSelect->EndFlag        = false;

	if( Name )
	{
		TFileSelect->Name = ( char * )malloc( sizeof( char ) * strlen( Name ) + 1 );
		if( TFileSelect->Name == NULL )
		{
			return -1;
		}
		strcpy( TFileSelect->Name, Name );
	}

	ToolFileSelect_SetPosition( NewHandle, x, y );

	// ハンドルを返す
	return NewHandle;
}

// ファイル選択処理情報を削除する
void ToolFileSelect_Delete(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ボタン情報や文字列入力情報ハンドルの削除
	ToolButton_Delete( TFileSelect->OpenTButtonHandle );
	ToolButton_Delete( TFileSelect->ApplyTButtonHandle );
	ToolStringInput_Delete( TFileSelect->TStringInputHandle );

	// 名前保存用にメモリを確保していた場合は解放
	if( TFileSelect->Name )
	{
		free( TFileSelect->Name );
		TFileSelect->Name = NULL;
	}

	// 確保していたメモリを解放する
	free( TFileSelect );
	g_TFileSelect[ TFileSelectHandle ] = NULL;

	// 文字列入力情報の数を減らす
	g_TFileSelectNum--;
}

// 全てのファイル選択処理情報を削除する
void ToolFileSelect_Delete_All( void )
{
	int i;

	// 有効な全てのファイル選択処理を削除する
	for( i = 0; i < TOOL_FILESELECT_MAX; i++ )
	{
		if( g_TFileSelect[ i ] != NULL )
		{
			ToolFileSelect_Delete( i );
		}
	}
}

// ファイル選択処理情報に関連付ける表示状態情報をセットする
void ToolFileSelect_SetVisibleHandle(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// 表示状態情報ハンドルの保存
	TFileSelect->TVisibleHandle = TVisibleHandle;

	// ボタン情報や文字列入力情報にも表示状態情報ハンドルを登録
	ToolButton_SetVisibleHandle( TFileSelect->OpenTButtonHandle, TVisibleHandle );
	ToolButton_SetVisibleHandle( TFileSelect->ApplyTButtonHandle, TVisibleHandle );
	ToolStringInput_SetVisibleHandle( TFileSelect->TStringInputHandle, TVisibleHandle );
}

// ファイル選択処理情報と関連付けるウインドウ情報を設定する
void ToolFileSelect_SetWindow(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ウインドウ情報ハンドルの保存
	TFileSelect->TWindowHandle = TWindowHandle;

	// ボタン情報や文字列入力情報にもウインドウ情報ハンドルを登録
	ToolButton_SetWindow( TFileSelect->OpenTButtonHandle, TWindowHandle );
	ToolButton_SetWindow( TFileSelect->ApplyTButtonHandle, TWindowHandle );
	ToolStringInput_SetWindow( TFileSelect->TStringInputHandle, TWindowHandle );
}

// ファイル選択処理情報の表示位置を変更する
void ToolFileSelect_SetPosition(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 変更後の座標
	int x,
	int y
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// 座標を保存
	TFileSelect->Position.x = x;
	TFileSelect->Position.y = y;

	// ボタン情報や文字列入力情報の座標も変更
	ToolButton_SetPosition(
		TFileSelect->OpenTButtonHandle,  x + OPEN_X,  y + BUTTON_Y );

	ToolButton_SetPosition(
		TFileSelect->ApplyTButtonHandle, x + APPLY_X, y + BUTTON_Y );

	ToolStringInput_SetPosition(
		TFileSelect->TStringInputHandle, x + PATH_X,  y + PATH_Y );
}

// ファイル選択処理情報の表示状態を変更する
void ToolFileSelect_SetVisible(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// 非表示の場合は文字列入力ハンドルのアクティブ状態も解除する
	if( !Visible )
	{
		ToolStringInput_ResetActive( TFileSelect->TStringInputHandle );
	}

	// 新しい表示状態を保存する
	TFileSelect->Visible = Visible;

	// ボタン情報や文字列入力情報の表示状態も変更
	ToolButton_SetVisible( TFileSelect->OpenTButtonHandle, Visible );
	ToolButton_SetVisible( TFileSelect->ApplyTButtonHandle, Visible );
	ToolStringInput_SetVisible( TFileSelect->TStringInputHandle, Visible );
}

// ファイル選択処理情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolFileSelect_GetVisible(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// 表示状態が false の場合は false を返す
	if( !TFileSelect->Visible )
	{
		return false;
	}

	// 関連付けられているウインドウ情報の表示状態が false の場合は false を返す
	if( TFileSelect->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TFileSelect->TWindowHandle ) )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TFileSelect->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TFileSelect->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// ファイル選択処理情報の選択処理が完了しているかどうかを取得する
//     戻り値 : 処理が完了しているかどうか( true:完了している  false:まだ終わっていない )
bool ToolFileSelect_CheckEnd(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,

	// 処理が完了しているかどうかのフラグを倒すかどうか
	bool Reset
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];
	bool             Result;

	// 処理が完了しているかどうかのフラグをローカル変数にコピーしておく
	Result = TFileSelect->EndFlag;

	// フラグをリセットする指定だった場合は完了フラグを倒す
	if( Reset )
	{
		TFileSelect->EndFlag = false;
	}

	// 処理が完了しているかどうかのフラグをコピーしておいたローカル変数の内容を返す
	return Result;
}

// ファイル選択処理情報のファイルパスを設定する
void ToolFileSelect_SetPath(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ファイルパス文字列が格納されたメモリ領域の先頭アドレス
	const char *FilePath
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ファイルパス編集用の文字列入力情報にパスをセットする
	ToolStringInput_SetString(
		TFileSelect->TStringInputHandle, FilePath == NULL ? "" : FilePath );
}

// ファイル選択処理情報のファイルパスを取得する
void ToolFileSelect_GetPath(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ファイルパスを格納するバッファの先頭アドレス
	char *DestBuffer
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ファイルパス編集用の文字列入力情報の文字列を取得する
	ToolStringInput_GetString(
		TFileSelect->TStringInputHandle, DestBuffer );
}

// ファイル選択処理情報の状態推移処理を行う
static void ToolFileSelect_Step(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// 非表示状態の場合は何もせず終了
	if( !ToolFileSelect_GetVisible( TFileSelectHandle ) )
	{
		return;
	}

	// ファイルパス編集用の文字列入力情報処理が完了していたら
	// ファイル選択処理も完了状態にする
	if( ToolStringInput_CheckEnd( TFileSelect->TStringInputHandle, true ) )
	{
		TFileSelect->EndFlag = true;
	}

	// 「適用」ボタンが押されたらファイル選択処理を完了状態にする
	if( ToolButton_GetClick( TFileSelect->ApplyTButtonHandle, true ) )
	{
		TFileSelect->EndFlag = true;
	}

	// 「参照」ボタンが押されたらファイル選択ダイアログを表示する
	if( ToolButton_GetClick( TFileSelect->OpenTButtonHandle, true ) )
	{
		OPENFILENAME ofn;
		char         FullPath[ 1024 ];
		char         FileName[ 1024 ];

		memset( &ofn,     0, sizeof( OPENFILENAME ) );
		memset( FullPath, 0, sizeof( FullPath     ) );
		memset( FileName, 0, sizeof( FileName     ) );

		ofn.lStructSize    = sizeof( OPENFILENAME );
		ofn.hwndOwner      = GetMainWindowHandle();
		ofn.lpstrFilter    =
			"Sound File(*.wav *.ogg *.mp3)\0*.wav;*.ogg;*.mp3\0All File\0*.*\0\0";
		ofn.lpstrFile      = FullPath;
		ofn.lpstrFileTitle = FileName;
		ofn.nMaxFile       = sizeof( FullPath );
		ofn.nMaxFileTitle  = sizeof( FileName );
		ofn.Flags          = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt    = "";
		ofn.lpstrTitle     = "開く";

		if( GetOpenFileName( &ofn ) != 0 )
		{
			// ファイル選択ダイアログで選択されたファイルのパスを
			// ファイルパス編集用文字列入力情報に反映する
			ToolStringInput_SetString( TFileSelect->TStringInputHandle, FullPath );
		}
	}
}

// ファイル選択処理情報を画面に描画する
static void ToolFileSelect_Draw(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];
	int              WinDrawX;
	int              WinDrawY;

	// 非表示状態の場合は何もせず終了
	if( !ToolFileSelect_GetVisible( TFileSelectHandle ) )
	{
		return;
	}

	// ウインドウ情報に関連付けられている場合はウインドウ情報の座標を取得する
	if( TFileSelect->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TFileSelect->TWindowHandle, &WinDrawX, &WinDrawY );

		// ウインドウ情報を基にした描画範囲制限設定を行う
		ToolWindow_SetupDrawArea( TFileSelect->TWindowHandle );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
	}

	// 名前の描画
	DrawStringToHandle(
		TFileSelect->Position.x + NAME_X + WinDrawX,
		TFileSelect->Position.y + NAME_Y + WinDrawY,
		TFileSelect->Name,
		GetColor( 255,255,255 ),
		ToolStatic_GetNormalFontHandle()
	);

	// 描画範囲制限を元に戻す
	SetDrawArea( 0, 0, 999999, 999999 );
}

// 全てのファイル選択処理情報の状態推移処理を行う
void ToolFileSelect_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TFileSelectNum; i++ )
	{
		if( g_TFileSelect[ i ] == NULL )
		{
			continue;
		}

		ToolFileSelect_Step( i );
		j++;
	}
}

// 全てのファイル選択処理情報の描画移処理を行う
void ToolFileSelect_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TFileSelectNum; i++ )
	{
		if( g_TFileSelect[ i ] == NULL )
		{
			continue;
		}

		ToolFileSelect_Draw( i );
		j++;
	}
}


