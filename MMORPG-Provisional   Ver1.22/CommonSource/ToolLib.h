#ifndef __TOOLLIB__
#define __TOOLLIB__

#include "ToolLib_SpaceManage.h"
#include "ToolLib_Button.h"
#include "ToolLib_FileSelect.h"
#include "ToolLib_List.h"
#include "ToolLib_ParamBar.h"
#include "ToolLib_StringInput.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Window.h"

// ツール処理のウインドウの名前表示部分の高さ
#define TOOL_WINDOW_NAME_HEIGHT			(16)

// ツール処理のウインドウのスクロールバーの幅
#define TOOL_SCROLLBAR_WIDTH			(16)

// ツール用処理の初期化を行う
//     戻り値 : 初期化が正常に完了したかどうか( true:正常完了  false:エラー発生 )
extern bool Tool_Initialize( void );

// ツール用処理の後始末を行う
extern void Tool_Terminate( void );

// ツール用処理の状態推移処理を行う
//     戻り値 : 処理が正常に完了したかどうか( true:正常完了  false:エラー発生 )
extern bool Tool_Step(
	// 進める時間( 単位 : 秒 )
	float StepTime
);

// ツール用処理の描画処理を行う
extern void Tool_Draw( void );

// ツール用処理のサイズ変更可能な表示領域の右端のX座標を取得する
//     戻り値 : 可変表示領域右端のX座標
extern int Tool_GetBaseRightX( void );

// ツール用処理で扱っているマウスのボタン入力を取得する
extern int Tool_GetMouseEdgeInput(
	// 押された瞬間のマウスのボタンを代入する変数のアドレス
	int *EdgeInput,

	// マウスのボタンが押された瞬間のマウスカーソルの座標を代入する変数のアドレス
	int *EdgeX,
	int *EdgeY
);

#endif



