#ifndef CHARA_ENEMY_H
#define CHARA_ENEMY_H

#include "Chara.h"

// �G���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Enemy_Create(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �G�̏�Ԑ��ڏ������s����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Enemy_Step(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
);

// �G���_���[�W���󂯂��ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Enemy_Damage(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �_���[�W
	int DamagePoint,

	// �U���������������W
	VECTOR HitPosition,

	// �U���̕���
	VECTOR AttackDirection,

	// �U����h�䂵�����ǂ�����������ϐ��̃A�h���X
	bool *Defence,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
);

// �G�����b�Z�[�W�������Ă����Ƃ��ɌĂ΂��֐�
extern bool Chara_Enemy_MessageRecv(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���b�Z�[�W�𑗂��Ă����L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *SendCInfo,

	// ���b�Z�[�W���ʗp�̒l
	int Message
);

#endif