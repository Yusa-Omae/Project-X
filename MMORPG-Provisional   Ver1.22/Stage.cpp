#include"DxLib.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"

//�X�e�[�W�N���X�̏���������
void StageInitialize(){


}

//�X�e�[�W�N���X�̍X�V����
void StageUpdata(){

}

//�X�e�[�W�N���X�̕`�揈��
void StageDraw(){

	DrawCube3D(VGet(-5000,0,-5000),VGet(5000,-50,5000),GetColor(255,255,255),GetColor(0,255,127),0);
	DrawCube3D(VGet(-5000,0,-5000),VGet(5000,-50,5000),GetColor(255,255,255),GetColor(125,125,125),1);

}

//�X�e�[�W�N���X�㏈��
void StageFainalize(){

}