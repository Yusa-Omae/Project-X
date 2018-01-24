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
#include "../Code/Common/Keyboard/Keyboard.h"
#include "../Gamemain.h"
#include "Shop.h"

enum eImage{

	eImage_BG,		//背景

	eImage_Num,
};

typedef struct {

	int imgHandle[eImage_Num];	//画像ハンドル
	
}WORK_OBJ_t;


void Shop_Intialize() {

	//リソース読み込み

}

void Shop_Finalize() {

	//リソース解放

}

void Shop_Update() {

	if (Keyboard_Press(KEY_INPUT_X) == true) {
		GameMain_ChangeGameState(eGameState_MainGame, eFadeType_CrossFade);
	}

}

void Shop_Draw() {

#ifdef __MY_DEBUG__
	DrawString(0, 0, "GAME_STATE_SHOP", GetColor(0, 255, 0));
	DrawString(0, 20, "Xキーでゲームメインへ移動", GetColor(255, 255, 255));
#endif

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