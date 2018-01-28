#ifndef TASK_H
#define TASK_H

// タスクの実行優先順位の最大値 - 1
#define TASK_PRIORITY_MAX		(16)

// タスクの基本情報
typedef struct _STaskBaseInfo
{
	// 状態推移処理の実行優先順位
	int StepPriority;

	// 描画処理の実行優先順位
	int RenderPriority;

	// 状態推移処理を行う関数へのポインタ
	bool(*Step)(struct _STaskInfo *TInfo, float StepTime);

	// 描画処理を行う関数へのポインタ
	void(*Render)(struct _STaskInfo *TInfo);

	// 後始末処理を行う関数へのポインタ
	void(*Terminate)(struct _STaskInfo *TInfo);
} STaskBaseInfo;

// タスクの情報
typedef struct _STaskInfo
{
	// タスクの基本情報へのポインタ
	STaskBaseInfo *     Base;

	// タスクが使用するデータへのポインタ
	void *              Data;

	// 状態推移処理の実行優先順位が同じタスクのリストの次のタスクへのポインタ
	struct _STaskInfo * StepNext;

	// 描画処理の実行優先順位が同じタスクのリストの次のタスクへのポインタ
	struct _STaskInfo * RenderNext;

	// 追加又は削除するタスクのリストの次のタスクへのポインタ
	struct _STaskInfo * AddOrDelNext;
} STaskInfo;

// タスクシステム情報
typedef struct _STaskSystemInfo
{
	// タスクの処理を実行中かどうかのフラグ
	bool                Run;

	// 状態推移処理のタスクのリストの各優先順位の先頭タスクへのポインタ
	STaskInfo *         StepTask[TASK_PRIORITY_MAX];

	// 描画処理のタスクのリストの各優先順位の先頭タスクへのポインタ
	STaskInfo *         RenderTask[TASK_PRIORITY_MAX];

	// 追加するタスクのリストの先頭タスクへのポインタ
	STaskInfo *         AddTaskList;

	// 削除するタスクのリストの先頭タスクへのポインタ
	STaskInfo *         DelTaskList;
} STaskSystemInfo;

// タスクシステムの初期化
extern void TaskSystem_Initialize(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
);

// タスクシステムの後始末
extern void TaskSystem_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
);

// タスクシステムの状態推移処理を実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool TaskSystem_Step(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// タスクシステムの描画処理を実行する
extern void TaskSystem_Render(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
);

// タスクシステムにタスクを追加する
extern void TaskSystem_AddTask(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 追加するタスクの情報構造体のアドレス
	STaskInfo *TInfo
);

// タスクシステムに登録したタスクを削除する
extern void TaskSystem_DelTask(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 削除するタスクの情報構造体のアドレス
	STaskInfo *TInfo
);

#endif
