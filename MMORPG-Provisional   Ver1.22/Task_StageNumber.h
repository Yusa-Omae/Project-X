#ifndef TASK_STAGESTART_H
#define TASK_STAGESTART_H

#include "Task.h"

// ステージ番号表示を開始する
//     戻り値 : ステージ番号表示タスク情報構造体へのアドレス
extern STaskInfo * Task_StageNumber_Start( void );

// ステージ番号表示が終了したかをチェックする
//     戻り値 : ステージ番号表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
extern bool        Task_StageNumber_CheckEnd(
	// ステージ番号表示タスク情報構造体のアドレス
	STaskInfo *TInfo
);

#endif