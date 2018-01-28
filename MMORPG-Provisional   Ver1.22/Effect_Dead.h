#ifndef EFFECT_DEAD_H
#define EFFECT_DEAD_H

#include "DxLib.h"
#include "Effect.h"

// ���S�G�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_Dead_Initialize(
	// ���S�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
);

// ���S�G�t�F�N�g�̊�{����n���֐�
extern void Effect_Dead_Terminate(
	// ���S�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
);

// ���S�G�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Effect_Dead_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���S�G�t�F�N�g���폜���ꂽ�ۂɌĂ΂��֐�
extern void Effect_Dead_Delete(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���S�G�t�F�N�g�̏�Ԑ��ڏ����֐�
extern void Effect_Dead_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// ���S�G�t�F�N�g�̕`�揈���֐�
extern void Effect_Dead_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���S�G�t�F�N�g�����̏���������֐�
//     �߂�l : �����������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_Dead_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// ���S�G�t�F�N�g�����s����R�c���f���̃n���h��
	int ModelHandle,

	// ���S�G�t�F�N�g�̐F
	COLOR_U8 Color
);

#endif
