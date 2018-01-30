
/*
				ファイル名		:Shop.cpp
				作成者			:Syara
				作成日時		:2018/01/23
				ソース説明		:
				
					ショップシーン
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "../System.h"


#include "Shop.h"
#include "../Task.h"
#include "../System.h"
#include "../Define.h"
#include "../Gamemain.h"
#include "../Code/Common/Window/ScrollWindow.h"
#include "../Code/Common/String/StringBase.h"
#include "../Input.h"

#define STRING_LINE_LENGTH_MAX (30)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (5)								//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (800)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (GAME_SCREEN_HEIGHT - 20 * 7)			//文字列描画座標Y

enum eState{
	eState_Init,			//初期化
	eState_BuySelect,		//買い物の選択
	eState_ItemSelect,		//アイテム選択
	eState_Exit,			//終了処理
	eState_Num,
};		

enum eSystemToke {
	eSystemToke_Aisatu,		//最初の挨拶

	eSystemToke_Num,
};

typedef struct {
	STaskInfo task;
	eState state;						//ステート
	int itemType;						//アイテムの種類
	SCROLL_WINDOW_DATA_t scrollWindow;	//スクロールウィンドウ
	StringBase* stringBase;				//文字列
	bool isExit;						//終了した
}TASK_SHOP_t;

typedef struct {
	void(*proc)(TASK_SHOP_t* task);
	void(*draw)(TASK_SHOP_t* task);
}SHOP_FUNC;

static bool Task_Shop_Step(STaskInfo* task, float stepTime);
static void Task_Shop_Render(STaskInfo* task);
static void Task_Shop_Terminate(STaskInfo* task);

static void InitProc(TASK_SHOP_t* task);
static void BuySelectProc(TASK_SHOP_t* task);
static void BuySelectDraw(TASK_SHOP_t* task);


static SHOP_FUNC s_ShopFunc[eState_Num] = {
	//更新					//描画
	{ InitProc,				NULL		  },
	{ BuySelectProc,		BuySelectDraw },
};

// タイトル処理タスクの基本情報
static STaskBaseInfo g_Task_ShopTaskBaseInfo =
{
	8,
	8,
	Task_Shop_Step,
	Task_Shop_Render,
	Task_Shop_Terminate,
};

static const char SYSTEM_TOKE_TBL[][256] = {
	{"いらっしゃい！\n今回はどうするんだい？"},
};

static TASK_SHOP_t* s_Work;

/*
	入力状態を返却する
*/
static bool Input(int inputType) {

	int EdgeInput = GetEdgeInput();
	int Input = GetInput();

	bool result = false;

	if (inputType >= EInputType_Left && inputType <= EInputType_Down) {
		result = ((Input     & (1 << EInputType_Left)) != 0);
	}
	else {
		result = ((EdgeInput & (1 << inputType)) != 0);
	}
	return result;
}

static void InitProc(TASK_SHOP_t* task) {
	task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_Aisatu]);
	if (Input(EInputType_Attack)) {
		task->state = eState_BuySelect;
	}
}

/*
	購入する項目を選択する
*/
static void BuySelectProc(TASK_SHOP_t* task) {

}
static void BuySelectDraw(TASK_SHOP_t* task) {

}


STaskInfo* Task_Shop_Start() {
	TASK_SHOP_t* task;

	task = (TASK_SHOP_t*)calloc(1, sizeof(TASK_SHOP_t));
	if (task == NULL) {
		return false;
	}

	task->stringBase = new StringBase();
	if (task->stringBase == NULL) {
		return false;
	}
	task->stringBase->FontCreate("ＭＳ 明朝", 24, 1, -1);
	task->stringBase->SetColor(GetColor(255, 255, 255));

	task->isExit = false;

	task->task.Base = &g_Task_ShopTaskBaseInfo;
	task->task.Data = task;
	TaskSystem_AddTask(System_GetTaskSystemInfo(), &task->task);

	s_Work = task;

	return &task->task;
}

/*
	ショップが終了するか判断する
	return	true	:終了
			false	:終了しない
*/
bool Task_Shop_IsExit() {
	return s_Work->isExit;
}

static bool Task_Shop_Step(STaskInfo* stask, float stepTime) {
	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	
	
	eState state = task->state;

	
	if (s_ShopFunc[state].proc != NULL) {
		s_ShopFunc[state].proc(task);
	}

	task->stringBase->Update(true, 30, 5);

#ifdef __MY_DEBUG__
	//強制的に終了する
	if (Input(EInputType_Defence)) {
		task->isExit = true;
	}

#endif

	return true;
}

static void Task_Shop_Render(STaskInfo* stask) {
	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;


	eState state = task->state;

	if (s_ShopFunc[state].draw != NULL) {
		s_ShopFunc[state].draw(task);
	}

	task->stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y);
}

static void Task_Shop_Terminate(STaskInfo* stask) {
	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	delete (task->stringBase);
	task->stringBase = NULL;

}


#if false
#include "../Define.h"
#include "../Code/AppData/Item/ItemData.h"
#include "../Code/Common/Window/ScrollWindow.h"
#include "../Code/Common/Keyboard/Keyboard.h"
#include "../Code/Common/Mouse/Mouse.h"
#include "../Code/Common/String/StringBase.h"
#include "../Gamemain.h"

#include "Shop.h"

#define STRING_LINE_LENGTH_MAX (30)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (5)								//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (1200)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (INIT_AREA_Y2 - 20 * 7)			//文字列描画座標Y

enum eImage{

	eImage_BG,		//背景

	eImage_Num,
};

enum eStep {
	eStep_Init,		//初期化
	eStep_Select,	//アイテム選択
	eStep_Buy,		//購入するか？
};

typedef struct {

	SCROLL_WINDOW_DATA_t scrollWindow;
	StringBase* stringBase;
	int imgHandle[eImage_Num];	//画像ハンドル
	eStep step;
}WORK_OBJ_t;

static WORK_OBJ_t s_Work;


static void ItemSelectProc() {
	if (Mouse_WheelValueF() > 0) {
		ScrollWindow_Scroll(&s_Work.scrollWindow, -1);
	}
	else if (Mouse_WheelValueF() < 0) {
		ScrollWindow_Scroll(&s_Work.scrollWindow, 1);
	}

	if (Mouse_Press(eMouseInputBotton_Left) == true) {
		int posX;
		int posY;
		Mouse_GetPositioin(&posX, &posY);
		int ret = ScrollWindow_GetValue(s_Work.scrollWindow, posX, posY);
		
		if (ret != -1) {
			ScrolWindow_SetValue(&s_Work.scrollWindow, ret);
			ITEM_DATA_t item;
			ItemData_GetItemData(ret, &item);
			s_Work.stringBase->SetString(item.direction);
		}
	}
}


void Shop_Intialize() {

	//リソース読み込み

	s_Work.stringBase = new StringBase();
	s_Work.stringBase->FontCreate("ＭＳ 明朝", 24, 1, -1);
	s_Work.stringBase->SetColor(GetColor(255, 255, 255));
	

	ScrollWindow_Initialize(&s_Work.scrollWindow, 20, 20, 1000, 1000, 1000);

	s_Work.step = eStep_Init;
}

void Shop_Finalize() {

	//リソース解放
	delete (s_Work.stringBase);
	s_Work.stringBase = NULL;
}

void Shop_Update() {

	switch (s_Work.step) {
	case eStep_Init:
		s_Work.stringBase->SetString("いらっしゃい！\n今日はどうするんだい？");
		s_Work.step = eStep_Select;
		break;
	case eStep_Select:
		ItemSelectProc();
		break;
	case eStep_Buy:

		break;
	}

	

	if (Keyboard_Press(KEY_INPUT_X) == true) {
		GameMain_ChangeGameState(eGameState_MainGame, eFadeType_CrossFade);
	}

	
	s_Work.stringBase->Update(true, STRING_LINE_LENGTH_MAX,ONELETTER_DISP_INTERVAL);

}

void Shop_Draw() {

#ifdef __MY_DEBUG__
	DrawString(0, 0, "GAME_STATE_SHOP", GetColor(0, 255, 0));
	DrawString(0, 20, "Xキーでゲームメインへ移動", GetColor(255, 255, 255));
#endif


	switch (s_Work.step) {
	case eStep_Init:

		break;
	case eStep_Select:

		break;
	case eStep_Buy:

		break;
	}


	ScrollWindow_Draw(s_Work.scrollWindow, eScrollWindow_ScrollbarVertical);
	s_Work.stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y);

}
#endif