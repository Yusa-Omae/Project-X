#if false
/*
				ファイル名		:TestMenu.h
				作成者			:Syara
				作成日時		:2018/01/22
				ソース説明		:
					
					テスト用メニューデバッグ実行時はここから各シーンへ移動するようにします
					デバッグ時のみ使用できます

				備考
				
				
				
!*/

#ifdef __MY_DEBUG__

#include "DxLib.h"
#include "TestMenu.h"
#include "../Key.h"
#include "../../SceneMgr.h"
#include "../../Title.h"
#include "../../Gamemain.h"

#define SCENE_NUM (sizeof(s_SceneTbl) / sizeof(s_SceneTbl[0]))

typedef struct {
	eScene scene;	//シーン
	char name[256];	//シーン名
}SCENE_TABLE_t;

//シーンテーブル
static SCENE_TABLE_t s_SceneTbl[] = {

	{ eScene_Title,		"タイトル"			},
	{ eScene_Gamemain,	"ゲームメイン"		},
	{ eScene_Menu,		"メニュー"			},
	{ eScene_Result,	"リザルト"			},
	{ eScene_TestString,	"テスト文字列(フォント設定)" },

};


static int s_SelectScene;



void TestMenu_Initialize() {
	s_SelectScene = 0;
}

void TestMenu_Finalize() {

}

void TestMenu_Update() {

	if (key(KEY_INPUT_DOWN) == 1) {
		s_SelectScene = (s_SelectScene + 1) % SCENE_NUM;
	}
	else if (key(KEY_INPUT_UP) == 1) {
		s_SelectScene = (s_SelectScene + SCENE_NUM - 1) % SCENE_NUM;
	}

	if (key(KEY_INPUT_Z) == 1) {
		SceneMgr_ChangeScene(s_SceneTbl[s_SelectScene].scene);
	}

}

void TestMenu_Draw() {
	
	DrawString(20, 20, "テストメニュー", GetColor(255, 255, 255));

	for (int i = 0; i < SCENE_NUM; i++) {
		const char* name = s_SceneTbl[i].name;
		int drawY = 60 + i * 20;
		DrawString(20, drawY, name, GetColor(255, 255, 255));
	}

	int drawY = 60 + s_SelectScene * 20;
	DrawString(0, drawY,"◆", GetColor(255, 255, 255));

}


#endif //__MY_DEBUG__
#endif