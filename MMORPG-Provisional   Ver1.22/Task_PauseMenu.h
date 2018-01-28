#ifndef TASK_PAUSEMENU_H
#define TASK_PAUSEMENU_H

#include "Task.h"

// ポーズメニューのメニュー項目
typedef enum _ETask_PauseMenu
{
	ETask_PauseMenu_Continue,		// ゲーム再開
	ETask_PauseMenu_BackToTitle,	// タイトル画面に戻る
	ETask_PauseMenu_GameEnd,		// ゲーム終了

	ETask_PauseMenu_Num,			// 項目の数
} ETask_PauseMenu;

// ポーズメニューの処理を開始する
//     戻り値 : ポーズメニュータスク情報構造体へのアドレス
extern STaskInfo * Task_PauseMenu_Start( void );

// ポーズメニューが終了したかをチェックする
//     戻り値 : ポーズメニューが終了したいるかどうか
//              ( true:終了している  false:終了していない )
extern bool Task_PauseMenu_CheckEnd(
	// ポーズメニュータスク情報構造体のアドレス
	STaskInfo *TInfo
);

// ポーズメニューで選択された項目を取得する
//     戻り値 : 選択された項目の列挙型の値
extern ETask_PauseMenu Task_PauseMenu_GetChoiceIndex(
	// ポーズメニュータスク情報構造体のアドレス
	STaskInfo *TInfo
);

#endif