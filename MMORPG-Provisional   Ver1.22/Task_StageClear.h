#ifndef TASK_STAGECLEAR_H
#define TASK_STAGECLEAR_H

#include "Task.h"

// ステージクリア表示を開始する
//     戻り値 : ステージクリア表示タスク情報構造体へのアドレス
extern STaskInfo * Task_StageClear_Start(    void );

// ステージクリア表示が終了したかをチェックする
//     戻り値 : ステージクリア表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
extern bool        Task_StageClear_CheckEnd(
	// ステージクリア表示タスク情報構造体のアドレス
	STaskInfo *TInfo
);

#endif