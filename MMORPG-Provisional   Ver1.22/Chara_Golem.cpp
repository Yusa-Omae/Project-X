#include "Chara_Golem.h"
#include "System.h"
#include "Camera.h"

// �n�k�U���̃_���[�W
#define EARTHQUAKE_DAMAGE			(20)

// �A�j���[�V�����C�x���g�u���̑��v�����������ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Golem_AnimOtherEvent(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SCharaInfo *PCInfo;

	// �J������h�炷
	Camera_Shake( 50.0f, 1.7f, 80.0f );

	// �v���C���[�̃L�������\���̂̃A�h���X���擾
	PCInfo = System_GetPlayerCharaInfo();

	// �v���C���[�����݂��āA���W�����v���Ă��Ȃ�������v���C���[�Ƀ_���[�W��^����
	if( PCInfo != NULL && !PCInfo->JumpState )
	{
		if( !Chara_Damage(
				PCInfo,
				ECharaAtk_DamageType_Earthquake,
				EARTHQUAKE_DAMAGE,
				PCInfo->Position,
				VSub( PCInfo->Position, CInfo->Position ),
				NULL
			) )
		{
			return false;
		}
	}

	// ����I��
	return true;
}



