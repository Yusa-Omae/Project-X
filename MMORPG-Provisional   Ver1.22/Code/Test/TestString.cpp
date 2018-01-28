
#if false
/*
				ファイル名		:
				作成者			:
				作成日時		:
				ソース説明		:
				
				
				備考
				
				
				
!*/

#ifdef __MY_DEBUG__

#include "DxLib.h"
#include "../Common/Keyboard/Keyboard.h"
#include "../Common/String/StringBase.h"
#include "../Common/Window/Scrollbar.h"
#include "../Common/Window/ScrollWindow.h"
#include "../Common/Mouse/Mouse.h"
#include "../SceneMgr.h"

#include "TestString.h"



static StringBase* s_pString;
static StringBase* s_pString2;

static SCROLLBAR_DATA_t s_Bar;
static SCROLL_WINDOW_DATA_t s_scrollWindow;

void TestString_Initialize() {
	s_pString = new StringBase();
	s_pString2 = new StringBase();

	//既存のフォントデータの作成
	s_pString->FontCreate("メイリオ", 24,3,-1);
	s_pString2->FontCreate("ＭＳ 明朝", 24,3,-1);

	Scrollbar_Initialize(&s_Bar, 1000,20);
	ScrollWindow_Initialize(&s_scrollWindow, 20, 10, 1000, 1000, 1000);

}

void TestString_Finalize() {
	if (s_pString != NULL) {
		delete s_pString;
		s_pString = NULL;
	}
}

void TestString_Update() {

	//文字列に文字列を設定設定
	s_pString->SetString("テスト文字列！(メイリオ).\nてすと\nてすとてすとてすとてすとtesttststestertrtb\nwerw文字送りのテストをしています。");

	//文字列2に文字列を設定設定
	s_pString2->SetString("これはテスト文字列です.(明朝体)\nXキーで戻る");
	//文字列2の色を赤色に設定
	s_pString2->SetColor(GetColor(255, 0, 0));

	s_pString->Update(true);

	if (Mouse_WheelValueF() > 0) {
		Scrollbar_AddValue(&s_Bar,-1);
		ScrollWindow_Scroll(&s_scrollWindow, -1);
	}
	else if(Mouse_WheelValueF() < 0){
		Scrollbar_AddValue(&s_Bar, 1);
		ScrollWindow_Scroll(&s_scrollWindow, 1);
	}

	
	if (Mouse_Press(eMouseInputBotton_Left) == true) {
		int posX;
		int posY;
		Mouse_GetPositioin(&posX, &posY);
		int ret = ScrollWindow_GetValue(s_scrollWindow, posX, posY);
		ScrolWindow_SetValue(&s_scrollWindow, ret);
		printfDx("選択したアイテム番号は%d\n",ret);
	}

	if (Keyboard_Press(KEY_INPUT_X)) {
		SceneMgr_ChangeScene(eScene_TestMemu);
	}

}

void TestString_Draw() {

	//文字列を描画
	s_pString->DrawString(200, 100);

	//文字列2を描画
	s_pString2->DrawString(600, 120);

#if false
	Scrollbar_DrawVertical(s_Bar, 20, 0);
	Scrollbar_DrawHorizontal(s_Bar, 60, 1000);
#endif

	ScrollWindow_Draw(s_scrollWindow, eScrollWindow_ScrollbarVertical);


}

#endif	//__MY_DEBUG__
#endif