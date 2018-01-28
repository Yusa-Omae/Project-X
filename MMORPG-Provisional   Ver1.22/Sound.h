#ifndef SOUND_H
#define SOUND_H

#include "DxLib.h"

// �f�ރ^�C�v
typedef enum _EMaterialType
{
	EMaterialType_Grass,	// ��
	EMaterialType_Metal,	// ����
	EMaterialType_Soil,		// �y
	EMaterialType_Stone,	// ��
	EMaterialType_Wood,		// ��

	EMaterialType_Num		// �f�ރ^�C�v�̐�
} EMaterialType;

// BGM
typedef enum _EBGM
{
	EBGM_Stage0,			// �X�e�[�W�P
	EBGM_Stage1,			// �X�e�[�W�Q
	EBGM_Title,				// �^�C�g��
	EBGM_Boss,				// �{�X
	EBGM_GameOver,			// �Q�[���I�[�o�[
	EBGM_StageClear,		// �X�e�[�W�N���A
	EBGM_AllStageClear,		// �S�X�e�[�W�N���A

	EBGM_Num				// BGM�̐�
} EBGM;

// �T�E���h����������������
//     �߂�l : �������������������ǂ���( true:����  false:���s )
extern bool Sound_Initialize(void);

// �T�E���h�����̌�n��������
extern void Sound_Terminate(void);

// �T�E���h�t�@�C����ǉ�����
//     �߂�l : �T�E���h�t�@�C���̓o�^�ԍ�
extern int  Sound_AddSound(
	// �ǉ�����T�E���h�t�@�C���̃p�X
	const char *FilePath,

	// �R�c�Ŗ炷�T�E���h���ǂ���( true:3D�T�E���h  false:2D�T�E���h )
	bool Is3DSound
);

// �T�E���h���Đ�����
extern void Sound_PlaySound(
	// �Đ�����T�E���h�̓o�^�ԍ�
	int SoundIndex,

	// �Đ��^�C�v( DX_PLAYTYPE_BACK �Ȃ� )
	int PlayType
);

// �T�E���h���R�c�Đ�����
extern void Sound_PlaySound3D(
	// �T�E���h��炷���[���h���W
	VECTOR Position,

	// �Đ�����T�E���h�̓o�^�ԍ�
	int SoundIndex,

	// �Đ��^�C�v( DX_PLAYTYPE_BACK �Ȃ� )
	int PlayType
);

// �a�f�l���Đ�����
extern void Sound_PlayBGM(
	// �Đ�����a�f�l
	EBGM BGM
);

// �a�f�l�̍Đ����~�߂�
extern void Sound_StopBGM(void);

#endif
