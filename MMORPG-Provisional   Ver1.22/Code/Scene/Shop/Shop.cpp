/*
				ファイル名		:Shop.cpp
				作成者			:Syara
				作成日時		:2018/01/23
				ソース説明		:
				
					ショップシーン
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "../Code/AppData/Item/ItemData.h"
#include "../Code/Common/Window/ScrollWindow.h"
#include "../Code/Common/Keyboard/Keyboard.h"
#include "../Code/Common/Mouse/Mouse.h"

#include "../Gamemain.h"

#include "Shop.h"


enum eImage{

	eImage_BG,		//背景

	eImage_Num,
};

typedef struct {

	SCROLL_WINDOW_DATA_t scrollWindow;
	int imgHandle[eImage_Num];	//画像ハンドル
	
}WORK_OBJ_t;

static WORK_OBJ_t s_Work;


void Shop_Intialize() {

	//リソース読み込み

	ScrollWindow_Initialize(&s_Work.scrollWindow, 20, 20, 1000, 1000, 1000);

}

void Shop_Finalize() {

	//リソース解放

}

void Shop_Update() {

	if (Mouse_WheelValueF() > 0) {
		ScrollWindow_Scroll(&s_Work.scrollWindow, -1);
	}
	else if (Mouse_WheelValueF() < 0) {
		ScrollWindow_Scroll(&s_Work.scrollWindow, 1);
	}

	if (Keyboard_Press(KEY_INPUT_X) == true) {
		GameMain_ChangeGameState(eGameState_MainGame, eFadeType_CrossFade);
	}

}

void Shop_Draw() {

#ifdef __MY_DEBUG__
	DrawString(0, 0, "GAME_STATE_SHOP", GetColor(0, 255, 0));
	DrawString(0, 20, "Xキーでゲームメインへ移動", GetColor(255, 255, 255));
#endif



	ScrollWindow_Draw(s_Work.scrollWindow, eScrollWindow_ScrollbarVertical);
	
	/*
	アイテムデータ一覧
	*/
	int itemNum = ItemData_GetItemDataNum();
	ITEM_DATA_t itemData;
	for (int i = 0; i < itemNum; i++) {
		ItemData_GetItemData(i, &itemData);
		DrawString(20, 60 + i * 20, itemData.name, GetColor(255, 255, 255));
	}

}