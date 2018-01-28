#include "Mathematics.h"

// Catmull-Rom ��Ԃ��g�p���Ďw��̂S�_�� Pos1 �` Pos2 ���Ԃ�����W���擾����
void CatmullRom(
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
)
{
	VECTOR v0;
	VECTOR v1;
	float  t2;
	float  t3;

	// Catmull-Rom ��Ԃ��g�p���āAPos1 �` Pos2 �̕�ԍ��W���Z�o����

	v0.x = ( Pos2->x - Pos0->x ) / 2.0f;
	v0.y = ( Pos2->y - Pos0->y ) / 2.0f;
	v0.z = ( Pos2->z - Pos0->z ) / 2.0f;

	v1.x = ( Pos3->x - Pos1->x ) / 2.0f;
	v1.y = ( Pos3->y - Pos1->y ) / 2.0f;
	v1.z = ( Pos3->z - Pos1->z ) / 2.0f;

	t2 = DestPosRate * DestPosRate;
	t3 = t2 * DestPosRate;

	DestPos->x = (  2 * Pos1->x - 2 * Pos2->x +     v0.x + v1.x ) * t3 +
	             ( -3 * Pos1->x + 3 * Pos2->x - 2 * v0.x - v1.x ) * t2 +
				 v0.x * DestPosRate + Pos1->x;

	DestPos->y = (  2 * Pos1->y - 2 * Pos2->y +     v0.y + v1.y ) * t3 +
	             ( -3 * Pos1->y + 3 * Pos2->y - 2 * v0.y - v1.y ) * t2 +
				 v0.y * DestPosRate + Pos1->y;

	DestPos->z = (  2 * Pos1->z - 2 * Pos2->z +     v0.z + v1.z ) * t3 +
	             ( -3 * Pos1->z + 3 * Pos2->z - 2 * v0.z - v1.z ) * t2 +
				 v0.z * DestPosRate + Pos1->z;
}

// ���W�z��̊Ԃ� Catmull-Rom ���g�p���ĕ�Ԃ������W�z����擾����
void GetCatmullRomPosList(
	// ��Ԃ��s�����W�z��̐擪�A�h���X
	VECTOR *InPos,

	// InPos �̍��W�̐�
	int PosNum,

	// ����W�Ԃ̕�ԍ��W�̐�
	int DivNum,

	// ���ʂ̍��W���i�[������W�z��̐擪�A�h���X
	VECTOR *DestPos
)
{
	VECTOR  SPos;
	VECTOR  EPos;
	VECTOR *Pos[ 4 ];
	VECTOR *dp;
	int     i;
	int     j;
	int     num;
	float   sm;

	// �O�Ԗڂ̍��W�̐���_�͂P�Ԗڂ̍��W����O�Ԗڂ̍��W�̕�����
	// �O�Ԗڂ̍��W����ړ��������W�Ƃ���
	SPos.x = InPos[ 0         ].x + (InPos[ 0         ].x - InPos[1          ].x) / 2.0f;
	SPos.y = InPos[ 0         ].y + (InPos[ 0         ].y - InPos[1          ].y) / 2.0f;
	SPos.z = InPos[ 0         ].z + (InPos[ 0         ].z - InPos[1          ].z) / 2.0f;

	// �Ō�̍��W�̐���_�͍Ō�̈�O�̍��W����Ō�̍��W�̕�����
	// �Ō�̍��W����ړ��������W�Ƃ���
	EPos.x = InPos[ PosNum -1 ].x + (InPos[ PosNum -1 ].x - InPos[ PosNum -2 ].x) / 2.0f;
	EPos.y = InPos[ PosNum -1 ].y + (InPos[ PosNum -1 ].y - InPos[ PosNum -2 ].y) / 2.0f;
	EPos.z = InPos[ PosNum -1 ].z + (InPos[ PosNum -1 ].z - InPos[ PosNum -2 ].z) / 2.0f;

	// Catmull-Rom ��Ԃ��g�p���đS���W�Ԃ̕�ԍ��W���Z�o����
	dp  = DestPos;
	num = PosNum - 1;
	sm  = 1.0f / DivNum;
	for( i = 0; i < num; i++ )
	{
		VECTOR v0;
		VECTOR v1;
		float  t2;
		float  t3;
		float  t;
		VECTOR par1;
		VECTOR par2;

		Pos[ 0 ] = i - 1 <  0      ? &SPos : &InPos[ i - 1 ];
		Pos[ 1 ] = &InPos[ i     ];
		Pos[ 2 ] = &InPos[ i + 1 ];
		Pos[ 3 ] = i + 2 >= PosNum ? &EPos : &InPos[ i + 2 ];

		v0.x     = ( Pos[ 2 ]->x - Pos[ 0 ]->x ) / 2.0f;
		v0.y     = ( Pos[ 2 ]->y - Pos[ 0 ]->y ) / 2.0f;
		v0.z     = ( Pos[ 2 ]->z - Pos[ 0 ]->z ) / 2.0f;

		v1.x     = ( Pos[ 3 ]->x - Pos[ 1 ]->x ) / 2.0f;
		v1.y     = ( Pos[ 3 ]->y - Pos[ 1 ]->y ) / 2.0f;
		v1.z     = ( Pos[ 3 ]->z - Pos[ 1 ]->z ) / 2.0f;

		par1.x   =  2 * Pos[ 1 ]->x - 2 * Pos[ 2 ]->x +     v0.x + v1.x;
		par1.y   =  2 * Pos[ 1 ]->y - 2 * Pos[ 2 ]->y +     v0.y + v1.y;
		par1.z   =  2 * Pos[ 1 ]->z - 2 * Pos[ 2 ]->z +     v0.z + v1.z;

		par2.x   = -3 * Pos[ 1 ]->x + 3 * Pos[ 2 ]->x - 2 * v0.x - v1.x;
		par2.y   = -3 * Pos[ 1 ]->y + 3 * Pos[ 2 ]->y - 2 * v0.y - v1.y;
		par2.z   = -3 * Pos[ 1 ]->z + 3 * Pos[ 2 ]->z - 2 * v0.z - v1.z;

		t = 0.0f;
		for( j = 0; j < DivNum; j++, dp++, t += sm )
		{
			t2    = t  * t;
			t3    = t2 * t;
			dp->x = par1.x * t3 + par2.x * t2 + v0.x * t + Pos[ 1 ]->x;
			dp->y = par1.y * t3 + par2.y * t2 + v0.y * t + Pos[ 1 ]->y;
			dp->z = par1.z * t3 + par2.z * t2 + v0.z * t + Pos[ 1 ]->z;
		}
	}
	*dp = InPos[ PosNum - 1 ];
}

// GetCatmullRomPosList �ɓn���uInPos �̍��W�̐��v�Ɓu����W�Ԃ̕�ԍ��W�̐��v����
// ���ʂ��i�[������W�z��ɕK�v�ȗv�f�����擾����
//     �߂�l : ���ʂ��i�[������W�z��ɕK�v�ȗv�f��
int GetCatmullRomPosNum(
	// InPos �̍��W�̐�
	int PosNum,

	// ����W�Ԃ̕�ԍ��W�̐�
	int DivNum
)
{
	// �o�͍��W�̐��́w���W�Ԃ̐��x�~�w��̍��W�Ԃ̕�ԍ��W�̐��x�{�w�Ō�̍��W�x
	return ( PosNum - 1 ) * DivNum + 1;
}

// ���������_�^�̕ϐ��̒l���w��̒l�ɋ߂Â��鏈�����s��
//     �߂�l : �ϐ��̒l���w��̒l�ɒB�������ǂ���
//              ( true : �w��̒l�ɒB����  false : �w��̒l�ɒB���Ă��Ȃ� )
bool ParamChangeFloat(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,
	
	// �ڕW�Ƃ���l
	float TargetParam,
	
	// ���ڂ����鎞��
	float StepTime,
	
	// �l�̕ω����x
	float Speed
)
{
	// �ϐ��̒l���ڕW�Ƃ���l��菬�������ǂ����ŏ����𕪊�
	if( *Param < TargetParam )
	{
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l�ɕω��l�����Z����
		*Param += Speed * StepTime;

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if( *Param >= TargetParam )
		{
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}
	else
	{
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l����ω��l�����Z����
		*Param -= Speed * StepTime;

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if( *Param <= TargetParam )
		{
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}

	// �����ɗ�����܂��ڕW�̒l�ɒB���Ă��Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
	return false;
}


// ���������_�^�̕ϐ��ɑ������Ă���p�x���w��̊p�x�ɋ߂Â��鏈�����s��
// ( �ڕW�Ƃ���p�x�ƕϐ��ɑ������Ă���p�x�̍��� 2.0f * �� �ȓ��ł���K�v������ )
//     �߂�l : �ϐ��̊p�x���w��̊p�x�ɒB�������ǂ���
//              ( true : �w��̊p�x�ɒB����  false : �w��̊p�x�ɒB���Ă��Ȃ� )
bool ParamChangeAngle(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,

	// �ڕW�Ƃ���p�x
	float TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �p�x�̕ω����x
	float Speed
)
{
	float RelateAngle;

	// �ڕW�Ƃ���p�x�ƕϐ��̊p�x�Ƃ̍����Z�o
	RelateAngle = TargetParam - *Param;

	// �p�x�̍����P�W�O�x�ȏ�̏ꍇ�͊p�x�̍������߂�������t�]������
	if( RelateAngle < -DX_PI_F )
	{
		RelateAngle += DX_TWO_PI_F;
	}
	else
	if( RelateAngle > DX_PI_F )
	{
		RelateAngle -= DX_TWO_PI_F;
	}

	// �p�x�̍����O�ɋ߂Â���
	if( ParamChangeFloat( &RelateAngle, 0.0f, StepTime, Speed ) )
	{
		// �p�x�̍����O�ɂȂ�����ڕW�Ƃ���p�x�ɂȂ�Ƃ������ƂȂ̂�
		// �ϐ��ɖڕW�̊p�x�������� true ��Ԃ�
		*Param = TargetParam;
		return true;
	}
	
	// �ڕW�̊p�x����p�x�̍������������̂�V�����p�x�Ƃ���
	*Param = TargetParam - RelateAngle;

	// �܂��ڕW�̊p�x�ɒB���Ă��Ȃ��̂� false ��Ԃ�
	return false;
}

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( ���������_�l )
//     �߂�l : �����_���Ō��肵���l
float GetRandomFloat(
	// �ő�l
	float MaxF,

	// �ŏ��l
	float MinF
)
{
	return GetRand( 10000 ) * ( MaxF - MinF ) / 10000.0f + MinF;
}

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( �����l )
//     �߂�l : �����_���Ō��肵���l
int GetRandomInt(
	// �ő�l
	int MaxI,
	
	// �ŏ��l
	int MinI
)
{
	return GetRand( MaxI - MinI ) + MinI;
}

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( ���������_�l )
//     �߂�l : �����_���Ō��肵���l
float GetRandomFloatS(
	// �ő�l�ƍŏ��l�̏�񂪓������\���̂̃A�h���X
	SMaxMinF *MaxMinF
)
{
	return GetRandomFloat( MaxMinF->MaxF, MaxMinF->MinF );
}

// �w��͈̔͂��烉���_���Ō��肵���l���擾����( �����l )
//     �߂�l : �����_���Ō��肵���l
int GetRandomIntS(
	// �ő�l�ƍŏ��l�̏�񂪓������\���̂̃A�h���X
	SMaxMinI *MaxMinI
)
{
	return GetRandomInt( MaxMinI->MaxI, MaxMinI->MinI );
}



