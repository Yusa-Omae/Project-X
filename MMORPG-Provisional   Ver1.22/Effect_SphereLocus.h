#ifndef EFFECT_SPHERELOCUS_H
#define EFFECT_SPHERELOCUS_H

#include "DxLib.h"
#include "Effect.h"

// ���̋O�ՃG�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_SphereLocus_Initialize(
	// ���̋O�ՃG�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
);

// ���̋O�ՃG�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Effect_SphereLocus_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
);

// ���̋O�ՃG�t�F�N�g�̏�Ԑ��ڏ����֐�
extern void Effect_SphereLocus_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// ���̋O�ՃG�t�F�N�g�̕`�揈���֐�
extern void Effect_SphereLocus_Render(
	SEffectInfo *EInfo
	// �G�t�F�N�g���\���̂̃A�h���X
);

// ���̋O�ՃG�t�F�N�g�����̏���������֐�
extern void Effect_SphereLocus_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// �G�t�F�N�g�̐F
	COLOR_U8 Color,
	
	// ���̑傫��
	float SphereSize
);

// ���̋O�ՂɎg�p������W��ǉ�����
extern void Effect_SphereLocus_AddPosition(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̒��S���W
	VECTOR NewCenterPos,

	// ���̌��������肷�邽�߂̍��W
	VECTOR NewDirectionPos
);

#endif
