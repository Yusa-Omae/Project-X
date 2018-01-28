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
	eGameState nowGameState;	//���̃Q�[���̏��
	eGameState nextGameState;	//���̃Q�[���̏��
	eFadeType fadeType;
	float alaph;				//�A���t�@�u�����h
	unsigned int fadeColor;		//�t�F�[�h���̐F
	int fadeCounter;			//�t�F�[�h�p�J�E���^�[
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
	�t�F�[�h
	return	true	:�t�F�[�h��
			false	:�t�F�[�h�I��
*/
static bool Fade_Proc(int fadeType) {

	//�t�F�[�h�I����
	if (fadeType == eFadeType_None) return false;
	
	//�t�F�[�h���ł��邩�H
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
	�Q�[�����C���X�V
*/
static void GameMain_Proc() {
	//�J�����̍X�V
	CameraUpdate();

	StageUpdata();

	PlayerUpdate();
	Chara_Update();

#ifdef __MY_DEBUG__
	//�V���b�v�Ɉړ�
	if (Keyboard_Press(KEY_INPUT_Q) == true) {
		GameMain_ChangeGameState(eGameState_Shop, eFadeType_CrossFade);
	}
#endif

}

/*
	�Q�[�����C���`��
*/
static void GameMain_Draw() {
	//�J�����Z�b�g
	CameraSetUp();
	StageDraw();
	DrawCube3D(VSub(Get_Player_Pos(), VGet(25.0, 25.0, 25.0)), VAdd(Get_Player_Pos(), VGet(25.0, 25.0, 25.0)), Color_ValTbl[eCol_EmeraldGreen], 0, 1);
	Chara_Draw();
	Debug_Player();
	Debug_Camera();
	Debag_Chara();


}

/*
	�Q�[�����C���̃X�e�[�g��؂�ւ���
	eGameState gameState				:���̃Q�[���X�e�[�g��ݒ�
	eFadeType fadeType = eFadeType_None	:�t�F�[�h�̃^�C�v��ݒ�@��{�̓t�F�[�h���Ȃ���ԂɂȂ��Ă���
	
		�Q�[���X�e�[�g
		eGameState_None,		//��ԂȂ�
		eGameState_Initialize,	//������
		eGameState_Fade,		//�t�F�[�h		����������̃X�e�[�g�ɐݒ肷��ƃV�[�����؂�ւ��Ȃ����Ȃ�܂�
		eGameState_Tutorial,	//�`���[�g���A��
		eGameState_MainGame,	//���C���Q�[��
		eGameState_Pouse,		//�|�[�Y���
		eGameState_Shop,		//�V���b�v

		�t�F�[�h�^�C�v
		eFadeType_None,			//��ԂȂ�
		eFadeType_In,			//�t�F�[�h�C��
		eFadeType_Out,			//�t�F�[�h�A�E�g
		eFadeType_CrossFade,	//�N���X�t�F�[�h

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
	//�v���C���[��񏉊���
	Player_Info_Initialize();
	//�J������񏉊���
	Camera_Initialize();

	Chara_Initialize();

	//�X�e�[�W������
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

		//���͂Ȃ��̂ŃQ�[�����C���Ɉڍs
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
