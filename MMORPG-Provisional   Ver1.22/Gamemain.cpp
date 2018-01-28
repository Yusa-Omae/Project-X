#if false
#include"DxLib.h"
#include"Code/Common/Keyboard/Keyboard.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Camera.h"
#include"Chara_Player.h"
#include"Stage.h"
#include"Chara.h"

#include "Code\Scene\Shop\Shop.h"
#include "Code/Scene/Adventure/Adventure.h"
#include "Code/Scene/Tutorial/Tutorial.h"
#include "Code/Scene/Result/Result.h"

#include "Code/AppData/SaveData/SaveData.h"

#define SAVE_DATA_FILENAME ("saveData.sav")
#define FADE_TIME (60)



typedef struct {
	eGameState nowGameState;	//今のゲームの状態
	eGameState nextGameState;	//次のゲームの状態
	eFadeType fadeType;
	float alaph;				//アルファブレンド
	unsigned int fadeColor;		//フェード時の色
	int fadeCounter;			//フェード用カウンター
}WORK_OBJ_t;

static WORK_OBJ_t s_Work;


static bool FadeIn_Proc() {

	if (s_Work.fadeCounter >= FADE_TIME && s_Work.alaph < 0.0f) {
		s_Work.alaph = 0.0f;
		return false;
	}

	s_Work.alaph = 255.0f - 255.0f * (float)s_Work.fadeCounter / (float)FADE_TIME;

	return true;
}

static bool FadeOut_Proc() {

	if (s_Work.fadeCounter >= FADE_TIME && s_Work.alaph > 255.0f) {
		s_Work.alaph = 255.0f;
		return false;
	}

	s_Work.alaph = 255.0f * (float)s_Work.fadeCounter / (float)FADE_TIME;

	return true;

}

static bool FadeCross_Proc() {

	if (s_Work.fadeCounter >= FADE_TIME && s_Work.alaph < 0.0f) {
		s_Work.alaph = 0.0f;
		return false;
	}

	float fadeTime = (float)FADE_TIME / 2.0f;

	if (s_Work.fadeCounter < fadeTime) {

		s_Work.alaph = 255.0f * (float)s_Work.fadeCounter / fadeTime;
	}
	else {
		s_Work.alaph = 255.0f - 255.0f * (float)(s_Work.fadeCounter - fadeTime) / fadeTime;
	}
	return true;
}

/*
	フェード
	return	true	:フェード中
			false	:フェード終了
*/
static bool Fade_Proc(int fadeType) {

	//フェード終了中
	if (fadeType == eFadeType_None) return false;
	
	//フェード中であるか？
	bool isFadePlay = true;


	if (fadeType == eFadeType_In) {
		isFadePlay = FadeIn_Proc();
	}
	else if (fadeType == eFadeType_Out) {
		isFadePlay = FadeOut_Proc();
	}
	else {
		isFadePlay = FadeCross_Proc();
	}

	s_Work.fadeCounter++;

	return isFadePlay;
}

static void Fade_Draw() {

	int alpha = s_Work.alaph;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,alpha);
	DrawBox(0, 0, INIT_AREA_X2, INIT_AREA_Y2,s_Work.fadeColor,TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

/*
	ゲームメイン更新
*/
static void GameMain_Proc() {
	//カメラの更新
	CameraUpdate();

	StageUpdata();

	PlayerUpdate();
	Chara_Update();

#ifdef __MY_DEBUG__
	//ショップに移動
	if (Keyboard_Press(KEY_INPUT_Q) == true) {
		GameMain_ChangeGameState(eGameState_Shop, eFadeType_CrossFade);
	}
#endif

}

/*
	ゲームメイン描画
*/
static void GameMain_Draw() {
	//カメラセット
	CameraSetUp();
	StageDraw();
	DrawCube3D(VSub(Get_Player_Pos(), VGet(25.0, 25.0, 25.0)), VAdd(Get_Player_Pos(), VGet(25.0, 25.0, 25.0)), Color_ValTbl[eCol_EmeraldGreen], 0, 1);
	Chara_Draw();
	Debug_Player();
	Debug_Camera();
	Debag_Chara();


}

/*
	ゲームメインのステートを切り替える
	eGameState gameState				:次のゲームステートを設定
	eFadeType fadeType = eFadeType_None	:フェードのタイプを設定　基本はフェードしない状態になっている
	
		ゲームステート
		eGameState_None,		//状態なし
		eGameState_Initialize,	//初期化
		eGameState_Fade,		//フェード		※これを次のステートに設定するとシーンが切り替わらないくなります
		eGameState_Tutorial,	//チュートリアル
		eGameState_MainGame,	//メインゲーム
		eGameState_Pouse,		//ポーズ画面
		eGameState_Shop,		//ショップ

		フェードタイプ
		eFadeType_None,			//状態なし
		eFadeType_In,			//フェードイン
		eFadeType_Out,			//フェードアウト
		eFadeType_CrossFade,	//クロスフェード

*/
void GameMain_ChangeGameState(eGameState gameState, eFadeType fadeType/* = eFadeType_None*/) {

	if (fadeType == eFadeType_None) {
		s_Work.nowGameState = gameState;
	}
	else {
		s_Work.nowGameState = eGameState_Fade;
		s_Work.nextGameState = gameState;
		s_Work.fadeType = fadeType;

	}


}


void GameMainInitialize(){
	//プレイヤー情報初期化
	Player_Info_Initialize();
	//カメラ情報初期化
	Camera_Initialize();

	Chara_Initialize();

	//ステージ初期化
	StageInitialize();

	Shop_Intialize();

	Adeventure_Initialize();

	s_Work.nowGameState = eGameState_Initialize;

	if (SaveData_Exits(SAVE_DATA_FILENAME) == true) {
		s_Work.nextGameState = eGameState_MainGame;
	}
	else {
		s_Work.nextGameState = eGameState_Adventure;
	}

	s_Work.fadeCounter = 0;
	s_Work.alaph = 0.0f;
	s_Work.fadeColor = GetColor(255, 255, 255);


}
void GameMainupdate(){

	eGameState state = s_Work.nowGameState;

	switch (state) {
	case eGameState_Initialize:

		s_Work.nowGameState = eGameState_Fade;
		s_Work.fadeType = eFadeType_In;
		break;
	case eGameState_Fade:
		if (Fade_Proc(s_Work.fadeType) == false) {
			s_Work.nowGameState = s_Work.nextGameState;
			s_Work.nextGameState = eGameState_None;
			s_Work.fadeCounter = 0;
		}

		break;
	case eGameState_Adventure:
		Adeventure_Update();
		break;
	case eGameState_Tutorial:

		//今はないのでゲームメインに移行
		s_Work.nowGameState = eGameState_MainGame;
		break;
	case eGameState_MainGame:
		GameMain_Proc();
		break;
	case eGameState_Pouse:

		break;
	case eGameState_Shop:
		Shop_Update();
		break;
	}




}
void GameMainDraw(){
	
	eGameState state = s_Work.nowGameState;

	switch (state) {
	case eGameState_Initialize:

		break;
	case eGameState_Fade:
		Fade_Draw();
		break;
	case eGameState_Adventure:
		Adeventure_Draw();
		break;
	case eGameState_Tutorial:

		break;
	case eGameState_MainGame:
		GameMain_Draw();
		break;
	case eGameState_Pouse:

		break;
	case eGameState_Shop:
		Shop_Draw();
		break;
	}

	
}

void GameMainFainalize(){

	Player_Info_Fainalize();
	Chara_Fainalize();
	StageFainalize();

	Shop_Finalize();

	Adeventure_Finalize();

}
#endif
