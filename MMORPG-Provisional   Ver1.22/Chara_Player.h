#ifndef CHARA_PLAYER_H
#define CHARA_PLAYER_H

#include "Chara.h"

// �v���C���[���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Player_Create(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �v���C���[���폜�����ۂɌĂ΂��֐�
extern void Chara_Player_Delete(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �v���C���[�̏�Ԑ��ڏ������s����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Player_Step(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
);

// �v���C���[���_���[�W���󂯂��ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Player_Damage(
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

// �v���C���[�����n�����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Player_Landed(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
);

// �A�j���[�V�����C�x���g�u���̑��v�����������ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Player_AnimOtherEvent(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
);

/*
�L�����N�^�[�ɃA�C�e����ݒ肷��
*/
void Chara_Player_SetItem(int haveId, int itmeIdex);


/*
�L�����N�^�[�ɃA�C�e����ݒ肷��
return -1�ŃG���[�@0�ȏ�ŃA�C�e��
*/
int Chara_Player_GetItem(int haveId);


/*
�L�����N�^�[�̂�����ݒ肷��
*/
void Chara_Player_SetMoney(int gold);

/*
�L�����N�^�[�̂�����ԋp����
return �������z
*/
int Chara_Player_GetMoney();


#endif