#ifndef EFFECT_DAMAGE_H
#define EFFECT_DAMAGE_H

#include "DxLib.h"
#include "Effect.h"

// �_���[�W�G�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_Damage_Initialize(
	// �_���[�W�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
);

// �_���[�W�G�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Effect_Damage_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// �_���[�W�G�t�F�N�g�̏�Ԑ��ڏ����֐�
extern void Effect_Damage_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �_���[�W�G�t�F�N�g�̕`�揈���֐�
extern void Effect_Damage_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// �_���[�W�G�t�F�N�g�����̏���������֐�
extern void Effect_Damage_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// �_���[�W�G�t�F�N�g�𔭐���������W
	VECTOR Position,

	// �_���[�W�G�t�F�N�g�̐F
	COLOR_U8 Color
);

#endif
