#ifndef TOOLLIB_STATIC_H
#define TOOLLIB_STATIC_H

// ボタン用フォントの縦幅
#define TOOL_BUTTON_FONT_HEIGHT		(12)

// マウスカーソルが指定の矩形の内にあるかどうかをチェックする
//     戻り値 : 矩形の内にマウスカーソルがあるかどうか
//              ( true:内にある  false:外にある )
extern bool ToolStatic_CheckMouseIn(
	// チェックする矩形の左上端座標
	int x1,
	int y1,

	// チェックする矩形の右下端座標
	int x2,
	int y2
);

// マウスカーソルの座標を取得する
extern void ToolStatic_GetMousePosition(
	// マウスカーソルの座標を代入する変数のアドレス
	int *x,
	int *y
);

// マウスカーソルのボタンが押されたときの座標を取得する
extern void ToolStatic_GetMouseEdgePosition(
	// マウスカーソルの座標を代入する変数のアドレス
	int *EdgeX,
	int *EdgeY
);

// マウスカーソルの前フレームから移動した距離を取得する
extern void ToolStatic_GetMouseMoveVecter(
	// マウスカーソルの移動距離を代入する変数のアドレス
	int *MoveX,
	int *MoveY
);

// 通常フォントハンドルを取得する
//     戻り値 : フォントハンドル
extern int ToolStatic_GetNormalFontHandle( void );

// ボタン内文字用のフォントハンドルを取得する
//     戻り値 : フォントハンドル
extern int ToolStatic_GetButtonFontHandle( void );

// マウスのボタン押下状態を取得する
//     戻り値 : マウスボタンの押下状態値( GetMouseInput の戻り値と同じ形式 )
extern int ToolStatic_GetMouseButton( void );

// マウスのボタン押下状態を取得する( 押された瞬間のみの情報 )
//     戻り値 : マウスボタンの押下状態値( GetMouseInput の戻り値と同じ形式 )
extern int ToolStatic_GetMouseEdgeButton( void );

// マウスの左ボタンがダブルクリックされたかどうかを取得する
//     戻り値 : 左ボタンがダブルクリックされたかどうか
//              ( true:ダブルクリックされた  false:ダブルクリックされていない)
extern bool ToolStatic_GetMouseDoubleClick( void );

// マウスの選択範囲の開始座標と終了座標を取得する
extern void ToolStatic_GetMouseSelectStartPosition(
	// 選択範囲の開始座標を格納する変数のアドレス
	int *StartX,
	int *StartY,

	// 選択範囲の終了座標を格納する変数のアドレス
	int *EndX,
	int *EndY
);

// ツール処理のボタンを描画する
extern void Tool_DrawButton(
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
);

// 文字列入力ハンドルの中のカーソルの位置を取得する
extern int Tool_GetKeyInputCursorPos(
	// 文字列を描画するX座標
	int StrDrawX,

	// マウスカーソルのX座標
	int MouseX,

	// 文字列入力ハンドル
	int InputHandle,

	// 文字列を描画する際に使用するフォントハンドル
	int FontHandle
);

#endif
