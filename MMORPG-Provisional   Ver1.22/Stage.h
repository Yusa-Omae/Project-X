
#ifndef STAGE_H
#define STAGE_H

#include "DxLib.h"

// �X�e�[�W�����̏�����������
//     �߂�l : �������ɐ����������ǂ���
//              ( true:����   false:���s )
extern bool Stage_Initialize(void);

// �X�e�[�W�����̌�n��������
extern void Stage_Terminate(void);

// �X�e�[�W���Z�b�g�A�b�v����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Stage_Setup(
	// �Z�b�g�A�b�v����X�e�[�W�̔ԍ�
	int StageNumber
);

// �X�e�[�W�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Stage_Step(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �X�e�[�W�̃��C�g�̃Z�b�g�A�b�v���s��
extern void Stage_LightSetup(void);

// �X�e�[�W��`�悷��
extern void Stage_Render(
	// �V���h�E�}�b�v�ւ̕`�悩�ǂ���
	bool ShadowMapDraw
);

// �X�e�[�W�̃R���W�����p�R�c���f����`�悷��
extern void Stage_CollisionRender(void);

// �X�e�[�W�̋�p�R�c���f����`�悷��
extern void Stage_SkyRender(void);

// �X�e�[�W�̃f�B���N�V���i�����C�g�̕������擾����
//     �߂�l : �f�B���N�V���i�����C�g�̕���
extern VECTOR Stage_GetLightDirection(void);

// �w��̃J�v�Z���`�󂪃X�e�[�W�̃R���W�����p�R�c���f���̃|���S����
// �X�e�[�W�I�u�W�F�N�g�̃R���W�����p�R�c���f���̃|���S���ɓ����邩�ǂ������`�F�b�N����
//     �߂�l : �R���W�����p�|���S���ɓ����邩�ǂ���
//              ( true : ������   false : ������Ȃ� )
extern bool Stage_HitCheck(
	// �J�v�Z�����`��������W�P
	VECTOR Pos1,

	// �J�v�Z�����`��������W�Q
	VECTOR Pos2,

	// �J�v�Z���̕�
	float r
);

#endif

#if false
void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();


// �X�e�[�W�����̏��
typedef struct _SStageInfo
{
	// �X�e�[�W�̃C�x���g��������x�ł����s�������ǂ����̃t���O
	bool                 EventRunFlag[STAGE_EVENT_MAX_NUM];

	// �X�e�[�W�̃C�x���g�̃{�����[���ɐG�ꂽ�u�Ԃ��ǂ����̃t���O
	bool                 EventHitEdgeFlag[STAGE_EVENT_MAX_NUM];

	// �X�e�[�W�̃C�x���g�̃{�����[���ɐG��Ă��邩�ǂ����̃t���O
	bool                 EventHitFlag[STAGE_EVENT_MAX_NUM];
} SStageInfo;


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

// �X�e�[�W�����p�̐����p�����[�^
typedef enum _EStage_ParamInt
{
	EStgParInt_DirLight_Diff_R,		// �f�B���N�V���i�����C�g�̃f�B�t���[�Y�J���[(R)
	EStgParInt_DirLight_Diff_G,		// �f�B���N�V���i�����C�g�̃f�B�t���[�Y�J���[(G)
	EStgParInt_DirLight_Diff_B,		// �f�B���N�V���i�����C�g�̃f�B�t���[�Y�J���[(B)
	EStgParInt_DirLight_Amb_R,		// �f�B���N�V���i�����C�g�̃A���r�G���g�J���[(R)
	EStgParInt_DirLight_Amb_G,		// �f�B���N�V���i�����C�g�̃A���r�G���g�J���[(G)
	EStgParInt_DirLight_Amb_B,		// �f�B���N�V���i�����C�g�̃A���r�G���g�J���[(B)
	EStgParInt_PointLight_Atten0,	// �|�C���g���C�g�̋��������p�����[�^�O
	EStgParInt_PointLight_Atten1,	// �|�C���g���C�g�̋��������p�����[�^�P
	EStgParInt_PointLight_Atten2,	// �|�C���g���C�g�̋��������p�����[�^�Q
	EStgParInt_PointLight_Diff_R,	// �|�C���g���C�g�̃f�B�t���[�Y�J���[(R)
	EStgParInt_PointLight_Diff_G,	// �|�C���g���C�g�̃f�B�t���[�Y�J���[(G)
	EStgParInt_PointLight_Diff_B,	// �|�C���g���C�g�̃f�B�t���[�Y�J���[(B)
	EStgParInt_PointLight_Amb_R,	// �|�C���g���C�g�̃A���r�G���g�J���[(R)
	EStgParInt_PointLight_Amb_G,	// �|�C���g���C�g�̃A���r�G���g�J���[(G)
	EStgParInt_PointLight_Amb_B,	// �|�C���g���C�g�̃A���r�G���g�J���[(B)
	EStgParInt_PointLight_Pos_X,	// �|�C���g���C�g�̃v���C���[�Ƃ̑���X���W
	EStgParInt_PointLight_Pos_Y,	// �|�C���g���C�g�̃v���C���[�Ƃ̑���Y���W
	EStgParInt_PointLight_Pos_Z,	// �|�C���g���C�g�̃v���C���[�Ƃ̑���Z���W

	EStgParInt_Num					// �X�e�[�W�����p�̐����p�����[�^�̐�
} EStage_ParamInt;
#endif