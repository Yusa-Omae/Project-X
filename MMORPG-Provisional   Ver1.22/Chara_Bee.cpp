#include "Chara_Bee.h"

// �A�j���[�V�����C�x���g�u���̑��v�����������ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Bee_AnimOtherEvent(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	VECTOR NeedlePosition;

	// �j�𔭎˂�����W�̎擾
	NeedlePosition = MV1GetFramePosition(
							CInfo->ModelHandle,
							MV1SearchFrame( CInfo->ModelHandle, "point" )
					);

	// �j�𔭎˂���
	if( !Chara_Create( EChara_Needle, NeedlePosition, CInfo->AngleInfo.NowAngle, -1 ) )
	{
		return false;
	}

	// ����I��
	return true;
}



