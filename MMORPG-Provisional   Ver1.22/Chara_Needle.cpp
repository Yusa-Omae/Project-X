#include "Chara_Needle.h"

// �j�̈ړ����x
#define MOVE_SPEED				(1500.0f)

// �j�̍U����
#define ATTACK_POWER			(10)

// �j���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Needle_Create(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	// �����Ɋ֌W��������X�V����
	Chara_AngleParamRefresh( CInfo );

	// �O�������ɔ��
	CInfo->MoveSpeed   = VScale( CInfo->AngleInfo.FrontDirection, MOVE_SPEED );
	CInfo->MoveSpeed.y = 10.0f;

	// �W�����v��Ԃɂ���
	CInfo->JumpState                      = true;

	// �U�������Z�b�g����
	CInfo->AttackInfo[ 0 ].Enable         = true;
	CInfo->AttackInfo[ 0 ].AttackPosIndex = 0;

	// �U���͂��Z�b�g����
	CInfo->AttackPower                    = ATTACK_POWER;

	// ����I��
	return true;
}




