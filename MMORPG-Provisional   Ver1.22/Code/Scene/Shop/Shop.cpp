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
#define ONELETTER_DISP_INTERVAL (20)							//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (1200)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (INIT_AREA_Y2 - 20 * 7)			//文字列描画座標Y

enum eImage{

	eImage_BG,		//背景

	eImage_Num,
};

typedef struct {

	SCROLL_WINDOW_DATA_t scrollWindow;
	StringBase* stringBase;
	int imgHandle[eImage_Num];	//画像ハンドル
	
}WORK_OBJ_t;

static WORK_OBJ_t s_Work;


void Shop_Intialize() {

	//リソース読み込み

	s_Work.stringBase = new StringBase();
	s_Work.stringBase->FontCreate("ＭＳ 明朝", 24, 1, -1);
	s_Work.stringBase->SetColor(GetColor(255, 255, 255));
	

	ScrollWindow_Initialize(&s_Work.scrollWindow, 20, 20, 1000, 1000, 1000);


}

void Shop_Finalize() {

	//リソース解放
	delete (s_Work.stringBase);
	s_Work.stringBase = NULL;
}

void Shop_Update() {

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
		ScrolWindow_SetValue(&s_Work.scrollWindow, ret);
		printfDx("選択したアイテム番号は%d\n", ret);
	}


	if (Keyboard_Press(KEY_INPUT_X) == true) {
		GameMain_ChangeGameState(eGameState_MainGame, eFadeType_CrossFade);
	}

	s_Work.stringBase->SetString("いらっしゃい！\n今日はどうするんだい？");
	s_Work.stringBase->Update(true, STRING_LINE_LENGTH_MAX,ONELETTER_DISP_INTERVAL);

}

void Shop_Draw() {

#ifdef __MY_DEBUG__
	DrawString(0, 0, "GAME_STATE_SHOP", GetColor(0, 255, 0));
	DrawString(0, 20, "Xキーでゲームメインへ移動", GetColor(255, 255, 255));
#endif



	ScrollWindow_Draw(s_Work.scrollWindow, eScrollWindow_ScrollbarVertical);
	
#if false
	/*
	アイテムデータ一覧
	*/
	int itemNum = ItemData_GetItemDataNum();
	ITEM_DATA_t itemData;
	for (int i = 0; i < itemNum; i++) {
		ItemData_GetItemData(i, &itemData);
		DrawString(20, 60 + i * 20, itemData.name, GetColor(255, 255, 255));
	}
#endif

	s_Work.stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y);

}