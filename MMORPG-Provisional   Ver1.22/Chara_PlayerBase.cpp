#include "Chara.h"
#include "Chara_PlayerBase.h"

// �v���C���[�̊�{���̏������֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������  false : ���s���� )
bool Chara_PlayerBase_Initialize(
	// �L�����N�^�[�̊�{���\���̂̃A�h���X
	SCharaBaseInfo *CBInfo,

	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TextParam
)
{
	SChara_PlayerBaseInfo *PBInfo;

	// �v���C���[�̊�{�����i�[���郁�����̈�̊m��
	CBInfo->SubData = malloc( sizeof( SChara_PlayerBaseInfo ) );
	if( CBInfo->SubData == NULL )
	{
		return false;
	}
	PBInfo = ( SChara_PlayerBaseInfo * )CBInfo->SubData;

	// �U����h�䂵���Ƃ��ɖ炷���̓ǂݍ���
	PBInfo->DefenceSuccessSound = Sound_AddSound( "SE\\Player\\guard_success", true );

	// �����I��
	return true;
}
