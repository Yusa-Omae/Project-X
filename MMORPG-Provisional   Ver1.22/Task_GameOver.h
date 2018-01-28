#ifndef TASK_GAMEOVER_H
#define TASK_GAMEOVER_H

#include "Task.h"

// ゲームオーバー表示を開始する
//     戻り値 : ゲームオーバー表示タスク情報構造体へのアドレス
extern STaskInfo * Task_GameOver_Start(    void );

// ゲームオーバー表示が終了したかをチェックする
//     戻り値 : ゲームオーバー表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
extern bool        Task_GameOver_CheckEnd(
	// ゲームオーバー表示タスク情報構造体のアドレス
	STaskInfo *TInfo
);

#endif