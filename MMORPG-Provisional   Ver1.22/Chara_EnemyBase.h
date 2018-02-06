#ifndef CHARA_ENEMYBASE_H
#define CHARA_ENEMYBASE_H

#include "DxLib.h"
#include "TextParam.h"
#include "Mathematics.h"
#include "CharaBase.h"

// �G�P�̕ӂ�̍U���p�^�[���̍ő吔
#define ENEMY_ATTACK_MAX_NUM			(3)

// �U���p�^�[�����
typedef struct _SChara_EnemyBaseAtkInfo
{
	// �U���A�j���[�V�������J�n���鋗��
	float    AtkDistance;

	// �U���A�j���[�V�������J�n���鑊��Ǝ����̌����Ă�������Ƃ̊p�x�͈�
	float    AtkAngleRange;

	// �U����
	int      Atk;

	// �U���ׂ̈ɑ���ɐڋ߂���ۂɕ������ǂ���
	// ( true : ����  false : ���� )
	bool     IsAtkMoveWalk;

	// �U�����������Ă��������ω������邩�ǂ���
	// ( true : �ω�������  false : �ω������Ȃ� )
	bool     IsAtkAngleChange;
} SChara_EnemyBaseAtkInfo;

// �G�̊�{���
typedef struct _SChara_EnemyBaseInfo
{
	// ���E�̊p�x�͈�
	float    SeeAngleRange;

	// ������ς��鑬�x
	float    AngleChangeSpeed;

	// ���肪���E�ɓ����Ă����Ԃő��݂ɋC�t������
	float    NoticeDistance_See;

	// ���肪���E�ɓ����Ă��Ȃ��Ă����݂ɋC�t������
	float    NoticeDistance;

	// ���肪���E�ɓ����Ă��Ȃ��Ă����݂ɋC�t������( �Ր�Ԑ��� )
	float    NoticeDistance_AtkMode;

	// ���肪���E�ɓ����Ă���ꍇ�ɑ���̑��݂ɋC�t���܂Ŏ���
	float    NoticeTime_See;

	// ���肪���݂ɋC�t�������ɋ���ꍇ�Ɏ��ۂɋC�t���܂ł̎���
	float    NoticeTime_Distance;

	// ���肪���E�ɓ����Ă���ꍇ�Ɏ��ۂɍU���s���Ɉڂ�܂ł̎���
	SMaxMinF SeeAtkMoveStartWait;

	// ��x�U�������Ă���ēx�U���s���Ɉڂ�܂ł̎���
	SMaxMinF AtkRepeatWait;

	// �_���[�W���󂯂Ă���ēx�s�����J�n����܂ł̎���
	SMaxMinF DamageAfterMoveWait;

	// �Ր�Ԑ��ł͂Ȃ��Ƃ��ɜp�j���J�n����܂ł̎���
	SMaxMinF IdleMoveStartWait;

	// �Ր�Ԑ��ł͂Ȃ��Ƃ��Ɉ�x�̜p�j�ňړ����鎞��
	SMaxMinF IdleMoveTime;

	// �Ր�Ԑ��ł͂Ȃ��Ƃ��Ɍ�����ύX����ꍇ�ɁA������ύX���J�n����܂ł̎���
	float    IdleAngleMoveBeginWait;

	// �Ր�Ԑ��ł͂Ȃ��Ƃ��Ɍ����̕ύX��������Ɏ��̍s�����J�n����܂ł̎���
	float    IdleAngleMoveAfterWait;

	// �v���O�����ɂ��ړ����s�����ǂ���
	bool     ProgramMove;

	// �U�����̐�
	int      AtkNum;

	// �U�����
	SChara_EnemyBaseAtkInfo AtkInfo[ ENEMY_ATTACK_MAX_NUM ];
} SChara_EnemyBaseInfo;

// �G�̊�{���̏������֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������  false : ���s���� )
extern bool Chara_EnemyBase_Initialize(
	// �L�����N�^�[�̊�{���\���̂̃A�h���X
	SCharaBaseInfo *CBInfo,

	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TextParam
);

#endif
