#ifndef TOOLLIB_WINDOW_H
#define TOOLLIB_WINDOW_H

#include "DxLib.h"

// ウインドウ情報を作成する
//     戻り値 : ウインドウ情報ハンドル( 失敗した場合は -1 が返る )
extern int  ToolWindow_Create(
	// スペース管理情報ハンドル( 必要のない場合は -1 を渡す )
	int TSpaceManageHandle,
	
	// 残りのスペースを全て使用するかどうか( true を渡すと全て使用する )
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool SpaceLast,

	// 固定タイプのスペースかどうか
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool LockSpace,
	
	// 水平スクロールバー用のスペースも確保するかどうか
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool HScrollSpace,

	// ウインドウの名前
	const char *Name,

	// ウインドウの左端座標
	int DrawX,
	
	// ウインドウの左端Y座標
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawY,

	// ウインドウの幅
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawW,
	
	// ウインドウの高さ
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawH,
	
	// ウインドウのクライアント領域の幅
	int ClientW,
	
	// ウインドウのクライアント領域の高さ
	int ClientH
);

// ウインドウ情報を削除する
extern void ToolWindow_Delete(
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// 全てのウインドウ情報を削除する
extern void ToolWindow_Delete_All( void );

// ウインドウ情報に関連付ける表示状態情報をセットする
extern void ToolWindow_SetVisibleHandle( 
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 表示状態情報ハンドル
	int TVisibleHandle
);

// ウインドウ情報を元に SetDrawArea で描画領域を制限する設定を行う
extern void ToolWindow_SetupDrawArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// ウインドウ情報の情報表示領域を取得する
//     戻り値 : 描画領域の矩形
extern RECT ToolWindow_GetInfoArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// ウインドウ情報のスクロールバーの状態も考慮した
// クライアント領域の描画原点となるスクリーン座標を取得する
extern void ToolWindow_GetDrawLeftUpPos(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 座標を代入する変数のアドレス
	int *x,
	int *y
);

// ウインドウ情報のスクロールバーの状態も考慮した
// クライアント領域の描画原点となる座標を取得する
extern void ToolWindow_GetClientArea(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 座標を代入する変数のアドレス
	int *x,
	int *y
);

// ウインドウ情報のクライアント領域のサイズを設定する
extern void ToolWindow_SetClientSize(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 新しいクライアント領域のサイズ
	int w,
	int h
);

// ウインドウ情報の名前を設定する
//     戻り値 : 処理が正常に終了したかどうか( true:正常に終了した  false:エラー発生 )
extern bool ToolWindow_SetName(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 名前となる文字列のアドレス
	const char *Name
);

// ウインドウ情報の表示状態を変更する
extern void ToolWindow_SetVisible(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 新しい表示状態
	bool Visible
);

// ウインドウ情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolWindow_GetVisible(
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// 指定の座標がウインドウの情報表示領域に入っているかどうかを調べる
//     戻り値 : 指定の座標がウインドウの描画領域に入っているかどうか
//              ( true:入っている   false:入っていない )
extern bool ToolWindow_CheckInfoAreaIn(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// ウインドウの描画領域に入っているかどうか調べる座標
	int x,
	int y
);

// 全てのウインドウ情報の状態推移処理を行う
extern void ToolWindow_Step_All(
	// 進める時間( 単位 : 秒 )
	float StepTime
);

// 全てのウインドウ情報の描画移処理を行う
extern void ToolWindow_Draw_All( void );


#endif


