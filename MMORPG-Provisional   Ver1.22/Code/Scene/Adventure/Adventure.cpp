/*
				ファイル名		:Adventure.cpp
				作成者			:Syara
				作成日時		:2018/01/26
				ソース説明		:
					
						会話シーン
				
				備考
				
				
				
!*/

#include "DxLib.h""
#include "../Task.h"
#include "../System.h"
#include "../Define.h"
#include "../Gamemain.h"

#include "../Code/Common/String/StringBase.h"
#include "../Input.h"
#include "Adventure.h"

#define STRING_LINE_LENGTH_MAX (30)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (20)							//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (120)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (GAME_SCREEN_HEIGHT - 20 * 7)			//文字列描画座標Y
#define STRING_TBL_NUM (sizeof(STRING_TBL) / sizeof(STRING_TBL[0]))

typedef struct {
	STaskInfo task;
	StringBase* stringBase;
	bool isOneletter;				//一文字ずつ描画するか？
	int OneletterDispInterval;		//一文字表示するまでの間隔
	bool isDisp;					//表示するか？
	int num;
	bool isEnd;						//会話が終了したか
}TASK_ADVENTURE_t;

const char* STRING_TBL[] = {
	{ "昔々あるところに、一つの小さな国がありました"},
	{"その国は行商人が行きかう、小さいけれど栄えた町でした。"},
	{"ある日、1人の行商人が魔王の土地に・・・"},
	{"なんと！足を踏み入れてしまいました！！"},
	{"行商人であるプレイヤーは魔王の土地から" },
	{"無事抜け出せることができるのでしょうか？！" },
};

static bool Task_Adbentrue_Step(STaskInfo* task, float stepTime);
static void Task_Adventure_Render(STaskInfo* task);
static void Task_Adventure_Terminate(STaskInfo* task);

// タイトル処理タスクの基本情報
static STaskBaseInfo g_Task_AdventureTaskBaseInfo =
{
	8,
	8,
	Task_Adbentrue_Step,
	Task_Adventure_Render,
	Task_Adventure_Terminate,
};

static TASK_ADVENTURE_t* s_Work;


STaskInfo* Task_Adventure_Start() {
	TASK_ADVENTURE_t* task;

	task = (TASK_ADVENTURE_t*)calloc(1, sizeof(TASK_ADVENTURE_t));
	if (task == NULL) {
		return false;
	}

	task->stringBase = new StringBase();
	task->stringBase->FontCreate("ＭＳ 明朝", 24, 1, -1);
	task->stringBase->SetColor(GetColor(255, 255, 255));
	task->isOneletter = true;
	task->OneletterDispInterval = ONELETTER_DISP_INTERVAL;
	task->isDisp = true;

	task->num = 0;

	task->task.Base = &g_Task_AdventureTaskBaseInfo;
	task->task.Data = task;
	TaskSystem_AddTask(System_GetTaskSystemInfo(), &task->task);

	task->isEnd = false;

	s_Work = task;

	return &task->task;
}

bool Task_Adventure_IsDelete() {
	return s_Work->isEnd;
}


static bool Task_Adbentrue_Step(STaskInfo* stask, float stepTime) {

	TASK_ADVENTURE_t* task = (TASK_ADVENTURE_t*)stask->Data;

	// 押された瞬間のみの入力状態を取得
	int EdgeInput = GetEdgeInput();
	int Input = GetInput();

	if ((EdgeInput & (1 << EInputType_Pause)) != 0) {
		//GameMain_ChangeGameState(eGameState_Tutorial, eFadeType_CrossFade);
		//TaskSystem_DelTask(System_GetTaskSystemInfo(), &task->task);
		task->isEnd = true;
		return true;
	}

	

	task->stringBase->SetString(STRING_TBL[task->num]);

	int result = task->stringBase->Update(task->isOneletter, STRING_LINE_LENGTH_MAX,task->OneletterDispInterval);

	//描画終了していたら次の文字列を設定する
	if (result == 1) {
		if ((EdgeInput & (1 << EInputType_Atk)) != 0) {
			task->num++;
			if (task->num >= STRING_TBL_NUM) {
				task->num = 0;
				task->isDisp = false;

				//次のタスクをスタートさせる


				//GameMain_ChangeGameState(eGameState_Tutorial, eFadeType_CrossFade);
				// タスクを削除する
				//TaskSystem_DelTask(System_GetTaskSystemInfo(), &task->task);
				task->isEnd = true;
			}
		}
	}
	//描画中の場合
	else if (result == 0) {

		if ((Input & (1 << EInputType_Atk)) != 0) {
			task->OneletterDispInterval = 2;
		}
		else {
			task->OneletterDispInterval = ONELETTER_DISP_INTERVAL;
		}

	}
	return true;
}

static void Task_Adventure_Render(STaskInfo* stask) {

	TASK_ADVENTURE_t* task = (TASK_ADVENTURE_t*)stask->Data;

	DrawString(10, 20, "スタートボタンでスキップ", GetColor(255, 255, 255));

	task->stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y, task->isDisp);

}

static void Task_Adventure_Terminate(STaskInfo* stask) {

	TASK_ADVENTURE_t* task = (TASK_ADVENTURE_t*)stask->Data;

	delete (task->stringBase);
	task->stringBase = NULL;

}

#if false

void Adeventure_Initialize() {

	
	s_Work.stringBase = new StringBase();
	s_Work.stringBase->FontCreate("ＭＳ 明朝",24,1,-1);
	s_Work.stringBase->SetColor(GetColor(255, 255, 255));
	s_Work.isOneletter = true;
	s_Work.OneletterDispInterval = ONELETTER_DISP_INTERVAL;
	s_Work.isDisp = true;

	s_Work.num = 0;

}

void Adeventure_Finalize() {

	delete (s_Work.stringBase);
	s_Work.stringBase = NULL;

}

void Adeventure_Update() {
	
	if (Mouse_Press(eMouseInputBotton_Rigth) == true) {
		GameMain_ChangeGameState(eGameState_Tutorial, eFadeType_CrossFade);
	}

	s_Work.stringBase->SetString(STRING_TBL[s_Work.num]);
	
	int result = s_Work.stringBase->Update(s_Work.isOneletter, STRING_LINE_LENGTH_MAX, s_Work.OneletterDispInterval);

	//描画終了していたら次の文字列を設定する
	if (result == 1) {
		if (Mouse_Press(eMouseInputBotton_Left)) {
			s_Work.num++;
			if (s_Work.num >= 9) {
				s_Work.num = 0;
				s_Work.isDisp = false;
				GameMain_ChangeGameState(eGameState_Tutorial,eFadeType_CrossFade);
			}
		}
	}
	//描画中の場合
	else if(result == 0){

		if (Mouse_Repeat(eMouseInputBotton_Left)) {
			s_Work.OneletterDispInterval = 2;
		}
		else {
			s_Work.OneletterDispInterval = ONELETTER_DISP_INTERVAL;
		}

	}

}

void Adeventure_Draw() {


	DrawString(10, 20, "右クリックでスキップ", GetColor(255, 255, 255));

	s_Work.stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y, s_Work.isDisp);


}
#endif