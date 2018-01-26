#pragma once

void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();

//�X�e�[�W�N���X�Ŏg�p����\����
typedef struct s_Stage {

	//�L�����z�u���̐�
	int CharaInfoNum;
	//�I�u�W�F�N�g�z�u���̐�
	int ObjectInfoNum;
	//�C�x���g���̐�
	int EventInfoNum;
	//�X�e�[�W�J�n���̃v���C���[�̍��W
	VECTOR PlayerPosition;
	//�X�e�[�W�J�n���̃v���C���[�̌���
	float PlayerAngle;
	//�X�e�[�W�N���A����
	int ClearCondition;
	//�X�e�[�W���N���A������
	int Clearflg;

	//�X�e�[�W�f�[�^�i�[�n���h��
	int handle;
	//�����蔻��p�X�e�[�W�f�[�^�i�[�n���h��
	int c_handle;
	//�X�e�[�W�w�i�f�[�^�i�[�n���h��
	int sky_handle;
	//�I�u�W�F�N�g�f�[�^�i�[�n���h��
	int obj[OBJECT_NUM];
	//�����蔻��p�I�u�W�F�N�g�f�[�^�i�[�n���h��
	int c_obj[OBJECT_NUM];
	//�`��p�I�u�W�F�N�g�z��
	int d_obj[OBJECT_DRAW_MAX_NUM];
	//�V���h�E�}�b�v�̍쐬�p�n���h��
	int smap;


};