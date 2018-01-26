/*
				ファイル名		:Adventure.cpp
				作成者			:Syara
				作成日時		:2018/01/26
				ソース説明		:
					
						会話シーン
				
				備考
				
				
				
!*/

#include "DxLib.h""

#include "../Define.h"
#include "../Gamemain.h"

#include "../Code/Common/String/StringBase.h"
#include "../Code/Common/Mouse/Mouse.h"

#include "Adventure.h"

#define STRING_LINE_LENGTH_MAX (60)								//一行に表示する最大文字数
#define ONELETTER_DISP_INTERVAL (20)							//一文字表示するまでの間隔
#define STRING_DRAW_POSITION_X (120)							//文字列描画座標X
#define STRING_DRAW_POSITION_Y (INIT_AREA_Y2 - 20 * 7)			//文字列描画座標Y

typedef struct {
	StringBase* stringBase;
	bool isOneletter;				//一文字ずつ描画するか？
	int OneletterDispInterval;		//一文字表示するまでの間隔
	bool isDisp;					//表示するか？
	int num;
}WORK_OBJ_t;

const char* STRING_TBL[] = {
	{"あいうえおかきくけこ。"},
	{ "test test test あぁあぁあぁーーーー" },
	{ "ああああああああああいいいいいいいいいいううううううううううええええええええええおおおおおおおおおおかかかかかかかかかかかここから改行されるはず" },
	{ "昔々あるところに、一つの小さな国がありました"},
	{"その国は行商人が行きかう、小さいけれど栄えた町でした。"},
	{"ある日、1人の行商人が魔王の土地に・・・"},
	{"なんと！足を踏み入れてしまいました！！"},
	{"行商人であるプレイヤーは魔王の土地から" },
	{"無事抜け出せることができるのでしょうか？！" },
};

static WORK_OBJ_t s_Work;

void Adeventure_Initialize() {

	
	s_Work.stringBase = new StringBase();
	s_Work.stringBase->FontCreate("ＭＳ 明朝",24,1,-1);
	s_Work.stringBase->SetColor(GetColor(255, 255, 255));
	s_Work.isOneletter = true;
	s_Work.OneletterDispInterval = ONELETTER_DISP_INTERVAL;
	s_Work.isDisp = true;

	s_Work.num = 0;

}

void Adeventure_Finalize() {

	delete (s_Work.stringBase);
	s_Work.stringBase = NULL;

}

void Adeventure_Update() {
	
	s_Work.stringBase->SetString(STRING_TBL[s_Work.num]);
	
	int result = s_Work.stringBase->Update(s_Work.isOneletter, STRING_LINE_LENGTH_MAX, s_Work.OneletterDispInterval);

	//描画終了していたら次の文字列を設定する
	if (result == 1) {
		if (Mouse_Press(eMouseInputBotton_Left)) {
			s_Work.num++;
			if (s_Work.num >= 9) {
				s_Work.num = 0;
				s_Work.isDisp = false;
				GameMain_ChangeGameState(eGameState_Tutorial,eFadeType_CrossFade);
			}
		}
	}
	//描画中の場合
	else if(result == 0){

		if (Mouse_Repeat(eMouseInputBotton_Left)) {
			s_Work.OneletterDispInterval = 2;
		}
		else {
			s_Work.OneletterDispInterval = ONELETTER_DISP_INTERVAL;
		}

	}

}

void Adeventure_Draw() {


	s_Work.stringBase->DrawString(STRING_DRAW_POSITION_X, STRING_DRAW_POSITION_Y, s_Work.isDisp);


}