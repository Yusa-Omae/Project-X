#ifndef TASK_GAMEMAIN_H
#define TASK_GAMEMAIN_H

#include "Task.h"

// ゲームメインの処理を開始する
//     戻り値 : ゲームメインタスク情報構造体へのアドレス
extern STaskInfo * Task_GameMain_Start( void );

// ゲームメインの処理に敵を倒したことを伝える
extern void        Task_GameMain_AddKillEnemy(
	// ゲームメインタスク情報構造体のアドレス
	STaskInfo *TInfo,

	// 倒した敵の識別番号
	int CharaNo
);

#endif