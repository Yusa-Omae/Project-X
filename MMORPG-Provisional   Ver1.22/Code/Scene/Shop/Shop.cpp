#if false
/*
				ファイル名		:Shop.cpp
				作成者			:Syara
				作成日時		:2018/01/23
				ソース説明		:
				
					ショップシーン
				
				備考
				
				
				
!*/

#include "DxLib.h"
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