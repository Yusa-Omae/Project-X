#include "ToolLib.h"
#include "DxLib.h"

// サイズ変更可能な表示領域の初期右端X座標
#define TOOL_BASE_RIGHT_X			(350)

// サイズ変更可能な表示領域を最大まで広くした場合の右端の余白
#define TOOL_BASE_RIGHT_X_SPACE		(50)

// サイズ変更可能な表示領域のサイズ変更帯の幅
#define TOOL_BASE_RIGHT_BAR_WIDTH	(4)

// ダブルクリックされたと判断するクリックとクリックの間隔
#define DOUBLE_CLICK_TIME			(400)

// ToolLib管理用構造体
typedef struct _ToolManage
{
	// サイズ変更可能な表示領域の右端X座標
	int  BaseRightX;

	// サイズ変更可能な表示領域のサイズ変更帯を掴んでいるかどうか
	bool BaseRightGrip;

	// サイズ変更可能な表示領域のサイズ変更帯を掴んだときの右端X座標
	int  BaseRightGripX;

	// サイズ変更可能な表示領域のサイズ変更帯を掴んだときのマウスのX座標
	int  BaseRightGripMouseX;

	// マウスのボタン入力状態
	int  MouseInput;

	// マウスのボタン入力状態( 押された瞬間のボタンのみ )
	int  MouseEdgeInput;

	// マウスのボタンが押された瞬間のマウスカーソルの座標
	int  MouseEdgeX;
	int  MouseEdgeY;

	// マウスカーソルの座標
	int  MouseX;
	int  MouseY;
	
	// マウスカーソルの座標移動量
	int  MouseMovX;
	int  MouseMovY;

	// マウスの選択開始座標
	int  MouseSelectStartX;
	int  MouseSelectStartY;

	// マウスの選択終了座標
	int  MouseSelectEndX;
	int  MouseSelectEndY;

	// ダブルクリックがされたかどうか
	bool MouseDoubleClick;

	// マウスの左クリックがされたときの時間
	int  MouseClickTime;

	// 通常文字描画用のフォントハンドル
	int  NormalFontHandle;

	// ボタン内文字描画用のフォントハンドル
	int  ButtonFontHandle;
} ToolManage;

ToolManage g_ToolManage;


// ツール用処理の初期化を行う
//     戻り値 : 初期化が正常に完了したかどうか( true:正常完了  false:エラー発生 )
bool Tool_Initialize( void )
{
	// サイズ変更可能な表示領域の右端X座標の初期化
	g_ToolManage.BaseRightX = TOOL_BASE_RIGHT_X;

	// 通常文字描画用のフォントハンドルの作成
	g_ToolManage.NormalFontHandle = CreateFontToHandle( NULL, 12, 3, DX_FONTTYPE_EDGE );
	if( g_ToolManage.NormalFontHandle == -1 )
	{
		return false;
	}

	// ボタン内文字描画用のフォントハンドルの作成
	g_ToolManage.ButtonFontHandle = CreateFontToHandle( NULL, 12, 3, DX_FONTTYPE_NORMAL);
	if( g_ToolManage.ButtonFontHandle == -1 )
	{
		return false;
	}

	// 正常終了
	return true;
}

// ツール用処理の後始末を行う
void Tool_Terminate( void )
{
	// 全てのファイル選択情報の削除処理を行う
	ToolFileSelect_Delete_All();

	// 全てのパラメータバー情報の削除処理を行う
	ToolParamBar_Delete_All();

	// 全てのウインドウ情報の削除処理を行う
	ToolWindow_Delete_All();

	// 全てのボタン情報の削除処理を行う
	ToolButton_Delete_All();

	// 全ての文字列入力情報の削除処理を行う
	ToolStringInput_Delete_All();

	// 全てのリスト情報の削除処理を行う
	ToolList_Delete_All();

	// 全ての表示状態情報の削除処理を行う
	ToolVisible_Delete_All();

	// 全てのスペース管理情報の削除処理を行う
	ToolSpaceManage_Delete_All();

	// 通常文字描画用のフォントハンドルの削除
	DeleteFontToHandle( g_ToolManage.NormalFontHandle );
	g_ToolManage.NormalFontHandle = -1;

	// ボタン内文字描画用のフォントハンドルの削除
	DeleteFontToHandle( g_ToolManage.ButtonFontHandle );
	g_ToolManage.ButtonFontHandle = -1;
}

// ツール用処理の状態推移処理を行う
//     戻り値 : 処理が正常に完了したかどうか( true:正常完了  false:エラー発生 )
bool Tool_Step(
	// 進める時間( 単位 : 秒 )
	float StepTime
)
{
	int MouseX;
	int MouseY;
	int EdgeInput;
	int EdgeMouseX;
	int EdgeMouseY;
	int ScreenWidth;
	int ScreenHeight;

	// マウスのボタン入力を取得
	g_ToolManage.MouseInput = GetMouseInput();

	// マウスのボタンが押された瞬間の情報を取得
	if( GetMouseInputLog( &EdgeInput, &EdgeMouseX, &EdgeMouseY ) != -1 )
	{
		g_ToolManage.MouseEdgeInput = EdgeInput;
		g_ToolManage.MouseInput    |= EdgeInput;

		g_ToolManage.MouseEdgeX     = EdgeMouseX;
		g_ToolManage.MouseEdgeY     = EdgeMouseY;
	}
	else
	{
		g_ToolManage.MouseEdgeInput = 0;

		g_ToolManage.MouseEdgeX     = 0;
		g_ToolManage.MouseEdgeY     = 0;
	}

	// マウスカーソルの座標を取得
	GetMousePoint( &MouseX, &MouseY );

	// 前回からの座標移動量を算出
	g_ToolManage.MouseMovX = MouseX - g_ToolManage.MouseX;
	g_ToolManage.MouseMovY = MouseY - g_ToolManage.MouseY;

	// マウスカーソルの座標を保存
	g_ToolManage.MouseX    = MouseX;
	g_ToolManage.MouseY    = MouseY;

	// マウスの左ボタンが押された瞬間かどうかで処理を分岐
	if( ( g_ToolManage.MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		// アクティブな文字列入力ハンドルを無い状態にする
		SetActiveKeyInput( -1 );

		// 前回クリックされてから直ぐにクリックされた場合はダブルクリックされたと判断する
		if( GetNowCount() - g_ToolManage.MouseClickTime < DOUBLE_CLICK_TIME )
		{
			g_ToolManage.MouseDoubleClick = true;
		}
		else
		{
			g_ToolManage.MouseDoubleClick = false;
		}

		// クリックされたときの時間を保存
		g_ToolManage.MouseClickTime    = GetNowCount();

		// 選択範囲をクリックされた座標にする
		g_ToolManage.MouseSelectStartX = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectStartY = g_ToolManage.MouseY;
		g_ToolManage.MouseSelectEndX   = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectEndY   = g_ToolManage.MouseY;
	}
	else
	// マウスの左ボタンが押された瞬間ではなく押され続けている場合はマウスの選択範囲の
	// 終端座標を更新
	if( ( g_ToolManage.MouseInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		g_ToolManage.MouseSelectEndX = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectEndY = g_ToolManage.MouseY;
	}
	else
	{
		// マウスの左ボタンが押されてから一定時間経過していたら
		// ダブルクリック判定を解除する
		if( GetNowCount() - g_ToolManage.MouseClickTime > DOUBLE_CLICK_TIME )
		{
			g_ToolManage.MouseDoubleClick = false;
		}
	}

	// サイズ変更可能な表示領域のサイズ変更帯を掴んでいるかどうかで処理を変更
	if( g_ToolManage.BaseRightGrip )
	{
		// 掴んでいる状態で且つ左ボタンが押されている場合は表示領域のサイズを
		// マウスカーソルの位置に応じて変更する
		if( g_ToolManage.MouseInput & MOUSE_INPUT_LEFT )
		{
			g_ToolManage.BaseRightX = g_ToolManage.BaseRightGripX +
						g_ToolManage.MouseX - g_ToolManage.BaseRightGripMouseX;

			// 画面のサイズを取得する
			GetDrawScreenSize( &ScreenWidth, &ScreenHeight );

			// サイズが初期サイズよりも小さくならないようにする
			if( g_ToolManage.BaseRightX < TOOL_BASE_RIGHT_X )
			{
				g_ToolManage.BaseRightX = TOOL_BASE_RIGHT_X;
			}
			else
			// サイズが「画面横幅－余白」より大きくならないようにする
			if( g_ToolManage.BaseRightX > ScreenWidth - TOOL_BASE_RIGHT_X_SPACE )
			{
				g_ToolManage.BaseRightX = ScreenWidth - TOOL_BASE_RIGHT_X_SPACE;
			}
		}
		else
		{
			// 左ボタンが押されていない場合は掴んでいるかどうかのフラグを倒す
			g_ToolManage.BaseRightGrip = false;
		}
	}
	else
	{
		// マウスの左ボタンが押された瞬間の場合はサイズ変更可能な
		// 表示領域のサイズ変更帯を掴んでいるかチェックして、
		// 掴んでいたら掴み状態にする
		if( ( g_ToolManage.MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			g_ToolManage.BaseRightX                             <= g_ToolManage.MouseX &&
			g_ToolManage.BaseRightX + TOOL_BASE_RIGHT_BAR_WIDTH >= g_ToolManage.MouseX )
		{
			g_ToolManage.BaseRightGrip       = true;
			g_ToolManage.BaseRightGripMouseX = g_ToolManage.MouseX;
			g_ToolManage.BaseRightGripX      = g_ToolManage.BaseRightX;
		}
	}

	// ウインドウ情報の状態推移処理を行う
	ToolWindow_Step_All( StepTime );

	// ボタン情報の状態推移処理を行う
	ToolButton_Step_All( StepTime );

	// 文字列入力情報の状態推移処理を行う
	if( !ToolStringInput_Step_All() )
	{
		return false;
	}

	// パラメータバー情報の状態推移処理を行う
	ToolParamBar_Step_All();

	// リスト情報の状態推移処理を行う
	ToolList_Step_All();

	// ファイル選択情報の状態推移処理を行う
	ToolFileSelect_Step_All();

	// 正常終了
	return true;
}

// ツール用処理の描画処理を行う
void Tool_Draw( void )
{
	// ウインドウ情報の描画処理を行う
	ToolWindow_Draw_All();

	// ボタン情報の描画処理を行う
	ToolButton_Draw_All();

	// 文字列入力情報の描画処理を行う
	ToolStringInput_Draw_All();

	// パラメータバー情報の描画処理を行う
	ToolParamBar_Draw_All();

	// リスト情報の描画処理を行う
	ToolList_Draw_All();

	// ファイル選択情報の描画処理を行う
	ToolFileSelect_Draw_All();

	// サイズ変更可能な表示領域右端のサイズ変更帯を描画する
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 32 );
	DrawBox(
		g_ToolManage.BaseRightX,
		0,
		g_ToolManage.BaseRightX + TOOL_BASE_RIGHT_BAR_WIDTH,
		99999,
		GetColor( 255,255,255 ),
		TRUE
	);
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 255 );
}

// ツール用処理のサイズ変更可能な表示領域の右端のX座標を取得する
//     戻り値 : 可変表示領域右端のX座標
int Tool_GetBaseRightX( void )
{
	return g_ToolManage.BaseRightX;
}

// ツール用処理で扱っているマウスのボタン入力を取得する
int Tool_GetMouseEdgeInput(
	// 押された瞬間のマウスのボタンを代入する変数のアドレス
	int *EdgeInput,

	// マウスのボタンが押された瞬間のマウスカーソルの座標を代入する変数のアドレス
	int *EdgeX,
	int *EdgeY
)
{
	*EdgeInput = g_ToolManage.MouseEdgeInput;
	*EdgeX     = g_ToolManage.MouseEdgeX;
	*EdgeY     = g_ToolManage.MouseEdgeY;

	return *EdgeInput != 0;
}

// マウスカーソルが指定の矩形の内にあるかどうかをチェックする
//     戻り値 : 矩形の内にマウスカーソルがあるかどうか
//              ( true:内にある  false:外にある )
bool ToolStatic_CheckMouseIn(
	// チェックする矩形の左上端座標
	int x1,
	int y1,

	// チェックする矩形の右下端座標
	int x2,
	int y2
)
{
	return g_ToolManage.MouseX >= x1 && g_ToolManage.MouseX <= x2 &&
		   g_ToolManage.MouseY >= y1 && g_ToolManage.MouseY <= y2;
}

// マウスカーソルの座標を取得する
void ToolStatic_GetMousePosition(
	// マウスカーソルのx座標とy座標を代入する変数のアドレス
	int *x,
	int *y
)
{
	*x = g_ToolManage.MouseX;
	*y = g_ToolManage.MouseY;
}

// マウスカーソルのボタンが押されたときの座標を取得する
void ToolStatic_GetMouseEdgePosition(
	// マウスカーソルの座標を代入する変数のアドレス
	int *EdgeX,
	int *EdgeY
)
{
	*EdgeX = g_ToolManage.MouseEdgeX;
	*EdgeY = g_ToolManage.MouseEdgeY;
}

// マウスカーソルの前フレームから移動した距離を取得する
void ToolStatic_GetMouseMoveVecter(
	// マウスカーソルの移動距離を代入する変数のアドレス
	int *MoveX,
	int *MoveY
)
{
	*MoveX = g_ToolManage.MouseMovX;
	*MoveY = g_ToolManage.MouseMovY;
}

// 通常フォントハンドルを取得する
//     戻り値 : フォントハンドル
int ToolStatic_GetNormalFontHandle( void )
{
	return g_ToolManage.NormalFontHandle;
}

// ボタン内文字用のフォントハンドルを取得する
//     戻り値 : フォントハンドル
int ToolStatic_GetButtonFontHandle( void )
{
	return g_ToolManage.ButtonFontHandle;
}

// マウスのボタン押下状態を取得する
//     戻り値 : マウスボタンの押下状態値( GetMouseInput の戻り値と同じ形式 )
int ToolStatic_GetMouseButton( void )
{
	return g_ToolManage.MouseInput;
}

// マウスのボタン押下状態を取得する( 押された瞬間のみの情報 )
//     戻り値 : マウスボタンの押下状態値( GetMouseInput の戻り値と同じ形式 )
int ToolStatic_GetMouseEdgeButton( void )
{
	return g_ToolManage.MouseEdgeInput;
}

// マウスの左ボタンがダブルクリックされたかどうかを取得する
//     戻り値 : 左ボタンがダブルクリックされたかどうか
//              ( true:ダブルクリックされた  false:ダブルクリックされていない)
bool ToolStatic_GetMouseDoubleClick( void )
{
	bool Result;

	Result = g_ToolManage.MouseDoubleClick;

	// ダブルクリックされたかどうかのフラグを倒す
	g_ToolManage.MouseDoubleClick = false;

	return Result;
}

// マウスの選択範囲の開始座標と終了座標を取得する
void ToolStatic_GetMouseSelectStartPosition(
	// 選択範囲の開始座標を格納する変数のアドレス
	int *StartX,
	int *StartY,

	// 選択範囲の終了座標を格納する変数のアドレス
	int *EndX,
	int *EndY
)
{
	if( StartX != NULL )
	{
		*StartX = g_ToolManage.MouseSelectStartX;
	}

	if( StartY != NULL )
	{
		*StartY = g_ToolManage.MouseSelectStartY;
	}

	if( EndX != NULL )
	{
		*EndX = g_ToolManage.MouseSelectEndX;
	}

	if( EndY != NULL )
	{
		*EndY = g_ToolManage.MouseSelectEndY;
	}
}

// ツール処理のボタンを描画する
void Tool_DrawButton(
	// ボタンを描画する左上座標
	int x,
	int y,
	
	// ボタンの幅と高さ
	int w,
	int h,
	
	// ボタンの左端と上端の色
	int LeftTopColor1,
	int LeftTopColor2,

	// ボタンの右端と下端の色
	int RightBottomColor1,
	int RightBottomColor2,

	// ボタンの中央部分の色
	int CenterColor
)
{
	// 中央部分を描画
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
	DrawBox( x + 2, y + 2, x + w - 2, y + h - 2, CenterColor, TRUE );

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	// 左端と上端を描画
	DrawBox( x,     y,     x + w - 1, y + 1, LeftTopColor1, TRUE );
	DrawBox( x + 1, y + 1, x + w - 2, y + 2, LeftTopColor2, TRUE );

	DrawBox( x,     y + 1, x + 1, y + h - 1, LeftTopColor1, TRUE );
	DrawBox( x + 1, y + 2, x + 2, y + h - 2, LeftTopColor2, TRUE );

	// 右端と下端を描画
	DrawBox( x,     y + h - 1,  x + w, y + h,     RightBottomColor1, TRUE );
	DrawBox( x + 1, y + h - 2,  x + w, y + h - 1, RightBottomColor2, TRUE );

	DrawBox( x + w - 1, y,     x + w,     y + h - 1, RightBottomColor1, TRUE );
	DrawBox( x + w - 2, y + 1, x + w - 1, y + h - 2, RightBottomColor2, TRUE );

	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
}

// 文字列入力ハンドルの中のカーソルの位置を取得する
int Tool_GetKeyInputCursorPos(
	// 文字列を描画するX座標
	int StrDrawX,

	// マウスカーソルのX座標
	int MouseX,

	// 文字列入力ハンドル
	int InputHandle,

	// 文字列を描画する際に使用するフォントハンドル
	int FontHandle
)
{
	int  DrawStartPos;
	char String[ 2048 ];
	int  TempMouseX;
	int  i;
	int  CharSize;
	int  TotalX;
	int  DrawWidth;

	// 文字列開始位置座標からみたマウスの相対座標を算出
	TempMouseX = MouseX - StrDrawX;

	// 文字列入力ハンドルの現在の入力文字列を取得
	GetKeyInputString( String, InputHandle );

	// 入力中文字列の開始文字位置を取得
	DrawStartPos = GetKeyInputDrawStartPos( InputHandle );

	// 文字の終端がくるまでループ
	TotalX = 0;
	for( i = 0; String[ i + DrawStartPos ] != '\0'; i += CharSize )
	{
		// １文字のバイト数を取得
		if( MultiByteCharCheck( &String[ i + DrawStartPos ], DX_CHARSET_SHFTJIS ) )
		{
			CharSize = 2;
		}
		else
		{
			CharSize = 1;
		}

		// １文字分のピクセル幅を取得
		DrawWidth =
			GetDrawStringWidthToHandle( &String[ i + DrawStartPos],CharSize,FontHandle );

		// マウスカーソルのX座標が文字の領域にある場合はループから抜ける
		if( TempMouseX >= TotalX && TempMouseX < TotalX + DrawWidth )
		{
			break;
		}

		// チェック位置を文字の幅分だけずらす
		TotalX += DrawWidth;
	}

	// マウスカーソルがある文字の位置を返す
	return DrawStartPos + i;
}


