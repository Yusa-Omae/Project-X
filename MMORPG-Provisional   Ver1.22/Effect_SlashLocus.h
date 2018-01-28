#ifndef EFFECT_SLASHLOCUS_H
#define EFFECT_SLASHLOCUS_H

#include "DxLib.h"
#include "Effect.h"

// ���̋O�ՃG�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_SlashLocus_Initialize(
	// ���̋O�ՃG�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
);

// ���̋O�ՃG�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Effect_SlashLocus_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���̋O�ՃG�t�F�N�g�̏�Ԑ��ڏ����֐�
extern void Effect_SlashLocus_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// ���̋O�ՃG�t�F�N�g�̕`�揈���֐�
extern void Effect_SlashLocus_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���̋O�ՃG�t�F�N�g�����̏���������֐�
extern void Effect_SlashLocus_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̋O�ՃG�t�F�N�g�̐F
	COLOR_U8 Color
);

// ���̋O�ՂɎg�p������W��ǉ�����
extern void Effect_SlashLocus_AddPosition(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̕����̍��W
	VECTOR NewNearPos,
	
	// ���̐n�摤�̍��W
	VECTOR NewFarPos
);

#endif
