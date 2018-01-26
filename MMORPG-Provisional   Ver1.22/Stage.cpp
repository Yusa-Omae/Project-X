#include"DxLib.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"
#include"Stage.h"

s_Stage STAGE;

//�f�[�^�ǂݍ��ݗp
char path[256];

//�X�e�[�W�N���X�̏���������
void StageInitialize(){
	
	
	/*�K�v
	���X�e�[�W�̏ڍדǂݍ���

	�E�C�x���g�����p�̃t���O
	�E�L�����N�^�[�̑S�폜�֐�
	�E�v���C���[�L�����̃X�e�[�W���ǂ��ɐݒ肷�邩�̏��
	�E
	
	*/




	//�X�e�[�W�Ŏg�����f���̓ǂݍ���
	STAGE.handle = MV1LoadModel("Data\\Stage\\Stage00.mv1");
	STAGE.c_handle = MV1LoadModel("Data\\Stage\\Stage00_c.mv1");
	STAGE.sky_handle = MV1LoadModel("Data\\Stage\\Stage00_sky.mv1");
	
	for(int i = 0;i < OBJECT_NUM;i++){

		sprintf(path,"Stage\\Stage_Obj%03d.mv1",i);
		STAGE.obj[i] = MV1LoadModel(path);

		sprintf(path,"Stage\\Stage_Obj%03d_c.mv1",i);
		STAGE.c_obj[i] = MV1LoadModel(path);
	}

	//�V���h�E�}�b�v�̍쐬
	STAGE.smap = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(STAGE.smap, VGet(1,-1,0));				//�J�����̃��C�g�ƍ��킹��Ƃ��Ƀ��C�e�B���O�̃x�N�g���������ɂ�������OK
	SetShadowMapDrawArea(STAGE.smap, SHADOWMAP_INIT1, SHADOWMAP_INIT2);

	//�ʒu�ݒ�
	MV1SetPosition(STAGE.handle, STAGE_INIT);



}

//�X�e�[�W�N���X�̍X�V����
void StageUpdata(){

	



}

//�X�e�[�W�N���X�̕`�揈��
void StageDraw(){

	//�V���h�E�}�b�v�ɕ`������
	ShadowMap_DrawSetup(STAGE.smap);
	MV1DrawModel(STAGE.handle);

	for(int i = 0;i < 100;i++){
	MV1DrawModel(STAGE.d_obj[i]);
	}

	ShadowMap_DrawEnd();
	SetUseShadowMap(0, STAGE.smap);

	//���f���`��
	MV1DrawModel(STAGE.handle);

	for (int i = 0; i < 100; i++) {
		MV1DrawModel(STAGE.d_obj[i]);
	}

	SetUseShadowMap(0,-1);

}

//�X�e�[�W�N���X�㏈��
void StageFainalize(){

}