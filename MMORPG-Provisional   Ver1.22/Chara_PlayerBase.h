#ifndef CHARA_PLAYERBASE_H
#define CHARA_PLAYERBASE_H

#include "TextParam.h"

// �v���C���[�̊�{���
typedef struct _SChara_PlayerBaseInfo
{
	// �U����h�䂵���Ƃ��ɖ炷���̓o�^�ԍ�
	int DefenceSuccessSound;
} SChara_PlayerBaseInfo;

// �v���C���[�̊�{���̏������֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������  false : ���s���� )
extern bool Chara_PlayerBase_Initialize(
	// �L�����N�^�[�̊�{���\���̂̃A�h���X
	SCharaBaseInfo *CBInfo,

	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TextParam
);

#endif
