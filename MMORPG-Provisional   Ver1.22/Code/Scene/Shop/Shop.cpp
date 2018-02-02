
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
#include "../Code/AppData/Item/ItemData.h"
#include "../Input.h"

#define STRING_LINE_LENGTH_MAX (30)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (5)								//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (800)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (GAME_SCREEN_HEIGHT - 20 * 7)			//文字列描画座標Y
#define ITEM_GRAPHIC_NUM (69)
enum eImage {
	eImage_BackImage,		//背景
	eImage_MessageWindow,
	eImage_Num,
};

enum eState{
	eState_Init,			//初期化
	eState_BuySelect,		//買い物の選択
	eState_BuyChacek,		//購入確認
	eState_Exit,			//終了処理
	eState_Num,
};		

enum eSystemToke {
	eSystemToke_Aisatu,		//最初の挨拶
	eSystemToke_kau,		//購入する
	eSystemToke_yameru,		//購入をやめる
	eSystemToke_Exit,		//店を出る

	eSystemToke_Num,
};

typedef struct {
	STaskInfo task;
	int imageHandle[eImage_Num];
	int itemImagehandle[ITEM_GRAPHIC_NUM ];
	eState state;						//ステート
	int itemType;						//アイテムの種類
	SCROLL_WINDOW_DATA_t scrollWindow;	//スクロールウィンドウ
	StringBase* stringBase;				//文字列
	bool isExit;						//終了した				
	int select;
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

static const char SYSTEM_TOKE_TBL[eSystemToke_Num][256] = {
	{"いらっしゃい！\n今回はどうするんだい？"},	//最初の挨拶
	{ "これを買うのかい？" },					//購入する
	{ "なんだ、やめるのか" },					//購入をやめる
	{ "また来いよな！" },						//店を出る
};

//選択
static const char SYSTEM_SELECT_TBL[][256] = {
	{ "購入する" },
	{ "合成する" },
	{ "店を出る" },
};

static TASK_SHOP_t* s_Work;

/*
	入力状態を返却する
*/
static bool Input(int inputType) {

	int EdgeInput = GetEdgeInput();
	int Input = GetInput();

	bool result = false;

#if true
	result = ((EdgeInput & (1 << inputType)) != 0);
#else
	if (inputType >= EInputType_Left && inputType <= EInputType_Down) {
		result = ((Input     & (1 << inputType)) != 0);
	}
	else {
		result = ((EdgeInput & (1 << inputType)) != 0);
	}
#endif
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

	if (Input(EInputType_Down)) {
		ScrollWindow_Scroll(&task->scrollWindow, 1);
	}
	else if (Input(EInputType_Up)) {
		ScrollWindow_Scroll(&task->scrollWindow, -1);
	}

}
static void BuySelectDraw(TASK_SHOP_t* task) {


	for (int i = 0; i < 3; i++) {

		int drawX = i * 200 + 20;
		int drawY = 200;

		if (task->select == i) {
			DrawString(drawX, drawY, SYSTEM_SELECT_TBL[i], GetColor(255, 0, 0));
		}
		else {
			DrawString(drawX, drawY, SYSTEM_SELECT_TBL[i], GetColor(255, 255, 255));
		}
	}


#if false
	for (int i = 0; i < ITEM_GRAPHIC_NUM; i++) {
		
		int drawX = (i % 10) * 48;
		int drawY = (i / 10) * 48;
		DrawGraph(drawX,drawY, task->itemImagehandle[i], TRUE);
	}
#endif

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

	task->imageHandle[eImage_BackImage] = LoadGraph("Data/2D/Shop_BG01.png");
	
	if (task->imageHandle[eImage_BackImage] == -1) {
		return NULL;
	}
	
	task->imageHandle[eImage_MessageWindow] = LoadGraph("Data/2D/MessageWindow.png");
	if (task->imageHandle[eImage_MessageWindow] == -1) {
		return NULL;
	}

	int ret = LoadDivGraph("Data/Shop/Item_Pic.png", ITEM_GRAPHIC_NUM, 10, 10, 48, 48, task->itemImagehandle);
	if (ret == -1) {
		return NULL;
	}

	task->isExit = false;
	task->select = 0;

	task->task.Base = &g_Task_ShopTaskBaseInfo;
	task->task.Data = task;
	TaskSystem_AddTask(System_GetTaskSystemInfo(), &task->task);

	if (ItemData_ReadData() == false) {
		return NULL;
	}

	ScrollWindow_Initialize(&task->scrollWindow, 0, 0, 640, 720, 720);

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

	if (stask->Data == NULL) return true;

	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	
	
	eState state = task->state;

	
	if (s_ShopFunc[state].proc != NULL) {
		s_ShopFunc[state].proc(task);
	}

	task->stringBase->Update(true, 30, 5);

#ifdef __MY_DEBUG__
	//強制的に終了する
	if (Input(EInputType_Jump)) {
		task->isExit = true;
	}

#endif

	return true;
}

static void Task_Shop_Render(STaskInfo* stask) {
	
	if (stask->Data == NULL) return;

	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	//DrawGraph(0, 0, task->imageHandle[eImage_BackImage], TRUE);
	DrawGraph(STRING_DRAW_POSITION_X - 20, STRING_DRAW_POSITION_Y - 10, task->imageHandle[eImage_MessageWindow], TRUE);

	ScrollWindow_DrawGraph(task->scrollWindow, task->imageHandle[eImage_BackImage], eScrollWindow_ScrollbarVertical);

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

	DeleteGraph(task->imageHandle[eImage_BackImage]);
	DeleteGraph(task->imageHandle[eImage_MessageWindow]);

	for (int i = 0; i < ITEM_GRAPHIC_NUM; i++) {
		DeleteGraph(task->itemImagehandle[i]);
	}

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