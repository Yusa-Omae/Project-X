#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include "DxLib.h"

// �w��͈̔͂��烉���_���ɒl�����肷�鏈���Ŏg�p������( ���������_�^ )
typedef struct _SMaxMinF
{
	// �ő�l
	float MaxF;

	// �ŏ��l
	float MinF;
} SMaxMinF;

// �w��͈̔͂��烉���_���ɒl�����肷�鏈���Ŏg�p������( �����^ )
typedef struct _SMaxMinI
{
	// �ő�l
	int MaxI;

	// �ŏ��l
	int MinI;
} SMaxMinI;

// Catmull-Rom ��Ԃ��g�p���Ďw��̂S�_�� Pos1 �` Pos2 ���Ԃ�����W���擾����
extern void CatmullRom(
	// Catmull-Rom ��ԂŎg�p������W�O( ����_�Ƃ��Ďg�p����� )
	VECTOR *Pos0,

	// Catmull-Rom ��ԂŎg�p������W�P
	VECTOR *Pos1,

	// Catmull-Rom ��ԂŎg�p������W�Q
	VECTOR *Pos2,

	// Catmull-Rom ��ԂŎg�p������W�R( ����_�Ƃ��Ďg�p����� )
	VECTOR *Pos3,

	// �擾������W�̈ʒu
	// ( 0.0f:p1�̍��W   0.0f��1.0f�̊�:p1��p2�̊Ԃ̕�ԍ��W   1.0f:p2�̍��W )
	float DestPosRate,

	// �v�Z���ʂ̍��W��������ϐ��̃A�h���X
	VECTOR *DestPos
);

// ���W�z��̊Ԃ� Catmull-Rom ���g�p���ĕ�Ԃ������W�z����擾����
extern void GetCatmullRomPosList(
	// ��Ԃ��s�����W�z��̐擪�A�h���X
	VECTOR *InPos,

	// InPos �̍��W�̐�
	int PosNum,

	// ����W�Ԃ̕�ԍ��W�̐�
	int DivNum,

	// ���ʂ̍��W���i�[������W�z��̐擪�A�h���X
	VECTOR *DestPos
);

// GetCatmullRomPosList �ɓn���uInPos �̍��W�̐��v�Ɓu����W�Ԃ̕�ԍ��W�̐��v����
// ���ʂ��i�[������W�z��ɕK�v�ȗv�f�����擾����
//     �߂�l : ���ʂ��i�[������W�z��ɕK�v�ȗv�f��
extern int GetCatmullRomPosNum(
	// InPos �̍��W�̐�
	int PosNum,

	// ����W�Ԃ̕�ԍ��W�̐�
	int DivNum
);

// ���������_�^�̕ϐ��̒l���w��̒l�ɋ߂Â��鏈�����s��
//     �߂�l : �ϐ��̒l���w��̒l�ɒB�������ǂ���
//              ( true : �w��̒l�ɒB����  false : �w��̒l�ɒB���Ă��Ȃ� )
extern bool ParamChangeFloat(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,
	
	// �ڕW�Ƃ���l
	float TargetParam,
	
	// ���ڂ����鎞��
	float StepTime,
	
	// �l�̕ω����x
	float Speed
);

// ���������_�^�̕ϐ��ɑ������Ă���p�x���w��̊p�x�ɋ߂Â��鏈�����s��
// ( �ڕW�Ƃ���p�x�ƕϐ��ɑ������Ă���p�x�̍��� 2.0f * �� �ȓ��ł���K�v������ )
//     �߂�l : �ϐ��̊p�x���w��̊p�x�ɒB�������ǂ���
//              ( true : �w��̊p�x�ɒB����  false : �w��̊p�x�ɒB���Ă��Ȃ� )
extern bool ParamChangeAngle(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,

	// �ڕW�Ƃ���p�x
	float TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �p�x�̕ω����x
	float Speed
);

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( ���������_�l )
//     �߂�l : �����_���Ō��肵���l
extern float GetRandomFloat(
	// �ő�l
	float MaxF,

	// �ŏ��l
	float MinF
);

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( �����l )
//     �߂�l : �����_���Ō��肵���l
extern int GetRandomInt(
	// �ő�l
	int MaxI,
	
	// �ŏ��l
	int MinI
);

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( ���������_�l )
//     �߂�l : �����_���Ō��肵���l
extern float GetRandomFloatS(
	// �ő�l�ƍŏ��l�̏�񂪓������\���̂̃A�h���X
	SMaxMinF *MaxMinF
);

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( �����l )
//     �߂�l : �����_���Ō��肵���l
extern int GetRandomIntS(
	// �ő�l�ƍŏ��l�̏�񂪓������\���̂̃A�h���X
	SMaxMinI *MaxMinI
);


#endif

