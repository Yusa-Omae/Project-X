#pragma once

void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();

//�X�e�[�W�N���X�Ŏg�p����\����
typedef struct s_Stage {

	//�X�e�[�W�f�[�^�i�[�n���h��
	int handle;

	//�I�u�W�F�N�g�i�[�n���h���͉���100�z��������Ă��܂��B
	//�I�u�W�F�N�g�f�[�^�i�[�n���h��
	int obj[100];

	//�����蔻��p�I�u�W�F�N�g�f�[�^�i�[�n���h��
	int c_obj[100];


	//�V���h�E�}�b�v�̍쐬�p�n���h��
	int smap;


};