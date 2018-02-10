
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
#include "../Chara_Player.h"

#define STRING_LINE_LENGTH_MAX (30)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (5)								//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (800)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (GAME_SCREEN_HEIGHT - 20 * 7)			//文字列描画座標Y
#define ITEM_GRAPHIC_NUM (69)
#define MESSAGE_WINDOW_BASE_POSITION_X (STRING_DRAW_POSITION_X - 20)
#define MESSAGE_WINDOW_BASE_POSITION_Y (STRING_DRAW_POSITION_Y - 10)

enum eImage {
	eImage_BackImage,		//背景
	eImage_MessageWindow,
	eImage_Num,
};

enum ePopupState {
	ePopupState_Down,
	ePopupState_Up,
	ePopupState_None,
};

enum eBuyCheckState {
	eBuyCheckState_Select,
	eBuyCheckState_Buy,
	eBuyCheckState_Return,
};

enum eState{
	eState_Init,			//初期化
	eState_BuySelect,		//買い物の選択
	eState_BuyChacek,		//購入確認
	eState_WindowPopup,		//ウィンドウポップアップ
	eState_Exit,			//終了処理
	eState_Num,
};		

enum eSystemToke {
	eSystemToke_Aisatu,		//最初の挨拶
	eSystemToke_kau,		//購入する
	eSystemToke_yameru,		//購入をやめる
	eSystemToke_tarinai,		//お金が足りない
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
	int select;							//現在選択しているアイテム
	int oldSelect;						//前回選択していたアイテム
	int itemWindowAlpha;				//アイテム一覧のアルファ値
	eBuyCheckState buyCheckState;		//購入確認ステート
	int buyCheckSelect;					//購入確認選択
	ePopupState messageWindowPopState;	//メッセージウィンドウステート
	int messageWindowAlaph;				//メッセージウィンドウのアルファ値
	int messageWindowPosX;				//メッセージウィンドウX座標
	int messageWindowPosY;				//メッセージウィンドウY座標
	int messageWindowWidth;				//メッセージウィンドウ横幅
	int messageWindowHeight;			//メッセージウィンドウ縦幅
	float counter;						//タイムカウンタ
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
static void BuyCheckProc(TASK_SHOP_t* task);
static void BuyCheckDraw(TASK_SHOP_t* task);
static void MessageWindowPopupProc(TASK_SHOP_t* task);
static void MessageWindowPopupDraw(TASK_SHOP_t* task);

static SHOP_FUNC s_ShopFunc[eState_Num] = {
	//更新						//描画
	{ InitProc,					NULL					},	//初期化
	{ BuySelectProc,			BuySelectDraw			},	//アイテム選択
	{ BuyCheckProc,				BuyCheckDraw			},	//購入確認
	{ MessageWindowPopupProc,	MessageWindowPopupDraw	},	//ウィンドウポップアップ
	{ NULL,						NULL					},	//終了
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
	{ "ありがとうよ！" },					//購入する
	{ "なんだ、やめるのか" },					//購入をやめる
	{"おいおい、たりねぇじゃねーか"},			//お金が足りない
	{ "また来いよな！" },						//店を出る
};

//選択
static const char SYSTEM_SELECT_TBL[][256] = {
	{ "購入する" },
	{ "やめる" },
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

	if (System_CheckFade()) return;


	int posX = 0;
	int posY = 0;

	ScrollWindow_GetWindowPosition(&task->scrollWindow, &posX, &posY);

	if (posX < 5) {
		posX += 20;
		task->itemWindowAlpha += 10;
	}
	else {
		ScrollWindow_SetWindowPosition(&task->scrollWindow, 5, 0);
		task->itemWindowAlpha = 255;

		if (task->messageWindowPosY > MESSAGE_WINDOW_BASE_POSITION_Y) {
			task->messageWindowPosY -= 25;
		}
		else {
			task->messageWindowPosY > MESSAGE_WINDOW_BASE_POSITION_Y;
			task->messageWindowAlaph = 255;
			task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_Aisatu]);
		}

	}

	ScrollWindow_SetWindowPosition(&task->scrollWindow, posX, posY);

	if (Input(EInputType_Attack)) {
		task->messageWindowPosY > MESSAGE_WINDOW_BASE_POSITION_Y;
		task->messageWindowAlaph = 255;
		task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_Aisatu]);
		task->messageWindowPopState = ePopupState_Down;
		task->state = eState_WindowPopup;
		System_PlayCommonSE(ECommonSE_Enter);
	}
}

/*
	購入する項目を選択する
*/
static void BuySelectProc(TASK_SHOP_t* task) {

	task->oldSelect =  ScrollWindow_GetValue(task->scrollWindow);

	if (Input(EInputType_Down)) {
		ScrollWindow_Scroll(&task->scrollWindow, 1);
		System_PlayCommonSE(ECommonSE_Cursor);
	}
	else if (Input(EInputType_Up)) {
		ScrollWindow_Scroll(&task->scrollWindow, -1);
		System_PlayCommonSE(ECommonSE_Cursor);
	}
	
	task->select = ScrollWindow_GetValue(task->scrollWindow);

	if (Input(EInputType_Attack)) {
		task->state = eState_BuyChacek;
		System_PlayCommonSE(ECommonSE_Enter);
	}
	//店を出る
	else if (Input(EInputType_Jump)) {
		task->state = eState_Exit;
		System_PlayCommonSE(ECommonSE_Cancel);
		task->isExit = true;
	}


	if (task->select != task->oldSelect) {
		//ポップアップステップへ
		task->messageWindowPopState = ePopupState_Down;
		task->state = eState_WindowPopup;
	}

	

}

/*
	購入アイテム選択描画処理
*/
static void BuySelectDraw(TASK_SHOP_t* task) {

	
}

/*
	購入確認更新処理
*/
static void BuyCheckProc(TASK_SHOP_t* task) {
	
	

	switch (task->buyCheckState) {
	case eBuyCheckState_Select:
		if (Input(EInputType_Right)) {
			task->buyCheckSelect = 1;		//やめる
			System_PlayCommonSE(ECommonSE_Cursor);
		}
		else if (Input(EInputType_Left)) {
			task->buyCheckSelect = 0;		//購入する
			System_PlayCommonSE(ECommonSE_Cursor);
		}

		if (Input(EInputType_Attack)) {
			if (task->buyCheckSelect == 0) {
				task->buyCheckState = eBuyCheckState_Buy;
				System_PlayCommonSE(ECommonSE_Enter);
			}
			else {
				task->buyCheckState = eBuyCheckState_Return;
				System_PlayCommonSE(ECommonSE_Cancel);
			}
		}
		break;
	case eBuyCheckState_Buy:
	{

		int select = ScrollWindow_GetValue(task->scrollWindow);
		ITEM_PARAM_DATA_t item;
		ItemData_GetItemData(select, &item);

		int price = Chara_Player_GetMoney();

		if (price < item.Price) {
			task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_tarinai]);
		}
		else {
			price -= item.Price;
			Chara_Player_SetMoney(price);
			Chara_Player_SetItem(0, item.id);
			task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_kau]);
		}

		task->buyCheckState = eBuyCheckState_Select;
		task->state = eState_BuySelect;
	}
		break;
	case eBuyCheckState_Return:

		task->stringBase->SetString(SYSTEM_TOKE_TBL[eSystemToke_yameru]);
		task->buyCheckState = eBuyCheckState_Select;
		task->state = eState_BuySelect;
		break;
	}


}

/*
	購入確認描画処理
*/
static void BuyCheckDraw(TASK_SHOP_t* task) {
	
	int drawX = (GAME_SCREEN_WIDTH - task->messageWindowWidth) / 2;
	int drawY = (GAME_SCREEN_HEIGHT - task->messageWindowHeight) / 2;

	DrawGraph(drawX, drawY, task->imageHandle[eImage_MessageWindow], TRUE);
	
	for (int i = 0; i < 2; i++) {
		if (task->buyCheckSelect == i) {
			DrawString(100 + drawX + i * 100, drawY + 200, SYSTEM_SELECT_TBL[i], GetColor(255, 0,0));
		}
		else{
			DrawString(100 + drawX + i * 100, drawY + 200, SYSTEM_SELECT_TBL[i], GetColor(255, 255, 255));
		}
	}
}

/*
	メッセージウィンドウ更新処理
*/
static void MessageWindowPopupProc(TASK_SHOP_t* task) {

	switch (task->messageWindowPopState) {
	case ePopupState_Down:

		if (task->messageWindowPosY < GAME_SCREEN_HEIGHT) {
			task->messageWindowPosY+= 10;
			task->messageWindowAlaph -= (GAME_SCREEN_HEIGHT - MESSAGE_WINDOW_BASE_POSITION_Y) / 10;
		}
		else {
			task->messageWindowPosY = GAME_SCREEN_HEIGHT;
			task->messageWindowPopState = ePopupState_Up;
			task->messageWindowAlaph = 0;
		}

		break;
	case ePopupState_Up:

		if (task->messageWindowPosY > MESSAGE_WINDOW_BASE_POSITION_Y) {
			task->messageWindowPosY -= 25;
		}
		else {
			task->messageWindowPosY > MESSAGE_WINDOW_BASE_POSITION_Y;
			task->state = eState_BuySelect;
			task->messageWindowPopState = ePopupState_None;
			task->messageWindowAlaph = 255;
			ITEM_PARAM_DATA_t item;
			ItemData_GetItemData(task->select, &item);
			
			char str[1024] = "";

			sprintf_s(str, "Atk:%d Def:%d Spd:%0.04f\n", item.Attack, item.Def, item.Spd);
			strcat(str, item.Description);

			task->stringBase->SetString(str);
		}

		break;
	}

	

}

/*
	メッセージウィンドウ描画処理
*/
static void MessageWindowPopupDraw(TASK_SHOP_t* task) {

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
	task->stringBase->SetString("");

	task->imageHandle[eImage_BackImage] = LoadGraph("Data/2D/Shop_BG01.png");
	
	if (task->imageHandle[eImage_BackImage] == -1) {
		return NULL;
	}
	
	task->imageHandle[eImage_MessageWindow] = LoadGraph("Data/2D/MessageWindow.png");
	if (task->imageHandle[eImage_MessageWindow] == -1) {
		return NULL;
	}

	int width;
	int height;
	GetGraphSize(task->imageHandle[eImage_MessageWindow], &width, &height);
	task->messageWindowWidth = width;
	task->messageWindowHeight = height;

	int ret = LoadDivGraph("Data/Shop/Item_Pic.png", ITEM_GRAPHIC_NUM, 10, 10, 48, 48, task->itemImagehandle);
	if (ret == -1) {
		return NULL;
	}

	task->isExit = false;
	task->select = 0;
	task->oldSelect = -1;

	task->buyCheckState = eBuyCheckState_Select;
	task->buyCheckSelect = 0;

	task->state = eState_Init;

	task->itemWindowAlpha = 0;
	task->messageWindowAlaph = 0;
	task->messageWindowPosX = MESSAGE_WINDOW_BASE_POSITION_X;
	task->messageWindowPosY = GAME_SCREEN_HEIGHT;


	task->counter = 0.0f;

	if (ItemData_ReadData() == false) {
		return NULL;
	}

	ScrollWindow_Initialize(&task->scrollWindow, 0, 0, 640, 720, 600);
	ScrollWindow_SetWindowPosition(&task->scrollWindow, -720, 0);

	ScrollWindow_SetImageHndles(&task->scrollWindow, task->itemImagehandle, ITEM_GRAPHIC_NUM);

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
	//if (stask == NULL) return true;
	if (stask->Data == NULL) return true;

	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	
	
	eState state = task->state;

	
	if (s_ShopFunc[state].proc != NULL) {
		s_ShopFunc[state].proc(task);
	}

	task->stringBase->Update(true, 30, 5);
	if (task->state == eState_Init || task->state == eState_BuyChacek) {
		task->counter = -1.0f;
	}
	else {
		task->counter += 1.0f;
	}



	return true;
}

static void Task_Shop_Render(STaskInfo* stask) {
	
	//if (stask == NULL) return;
	if (stask->Data == NULL) return;

	TASK_SHOP_t* task = (TASK_SHOP_t*)stask->Data;

	//DrawGraph(0, 0, task->imageHandle[eImage_BackImage], TRUE);
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, task->itemWindowAlpha);
	ScrollWindow_DrawGraph(&task->scrollWindow, task->imageHandle[eImage_BackImage], eScrollWindow_ScrollbarVertical, task->counter);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, task->messageWindowAlaph);
	DrawGraph(task->messageWindowPosX, task->messageWindowPosY, task->imageHandle[eImage_MessageWindow], TRUE);
	task->stringBase->DrawString(STRING_DRAW_POSITION_X, task->messageWindowPosY + 20);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	eState state = task->state;

	if (s_ShopFunc[state].draw != NULL) {
		s_ShopFunc[state].draw(task);
	}

	DrawFormatString(GAME_SCREEN_WIDTH - 200, 10, GetColor(0, 0, 0), "Money:%5d Golde", Chara_Player_GetMoney());
	
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
