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
#include "TestString.h"
#include "../SceneMgr.h"

StringBase* s_pString;
StringBase* s_pString2;


void TestString_Initialize() {
	s_pString = new StringBase();
	s_pString2 = new StringBase();

	//既存のフォントデータの作成
	s_pString->FontCreate("メイリオ", 24,3,-1);
	s_pString2->FontCreate("ＭＳ 明朝", 24,3,-1);
}

void TestString_Finalize() {
	if (s_pString != NULL) {
		delete s_pString;
		s_pString = NULL;
	}
}

void TestString_Update() {

	//文字列に文字列を設定設定
	s_pString->SetString("テスト文字列！(メイリオ)");

	//文字列2に文字列を設定設定
	s_pString2->SetString("これはテスト文字列です.(明朝体)\nXキーで戻る");
	//文字列2の色を赤色に設定
	s_pString2->SetColor(GetColor(255, 0, 0));

	if (Keyboard_Press(KEY_INPUT_X)) {
		SceneMgr_ChangeScene(eScene_TestMemu);
	}

}

void TestString_Draw() {

	//文字列を描画
	s_pString->DrawString(600, 100);

	//文字列2を描画
	s_pString2->DrawString(600, 120);
}

#endif	//__MY_DEBUG__