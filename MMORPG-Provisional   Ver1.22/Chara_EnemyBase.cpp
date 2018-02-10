#include "Chara_EnemyBase.h"

// �G�̊�{���̏������֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������  false : ���s���� )
bool Chara_EnemyBase_Initialize(
	// �L�����N�^�[�̊�{���\���̂̃A�h���X
	SCharaBaseInfo *CBInfo,

	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TextParam
)
{
	SChara_EnemyBaseInfo       *EBInfo;
	SChara_EnemyBaseAtkInfo *AInfo;
	int                         i;

	// �G�̊�{�����i�[���郁�����̈�̊m��
	CBInfo->SubData = malloc( sizeof( SChara_EnemyBaseInfo ) );
	if( CBInfo->SubData == NULL )
	{
		return false;
	}
	EBInfo = ( SChara_EnemyBaseInfo * )CBInfo->SubData;

	// �G�̍s���Ɋւ���p�����[�^���擾
	EBInfo->SeeAngleRange				=
		GetTextParamFloat( TextParam, "Enemy_SeeAngleRange" );
	EBInfo->SeeAngleRange = EBInfo->SeeAngleRange * DX_PI_F / 180.0f;

	EBInfo->AngleChangeSpeed			=
		GetTextParamFloat( TextParam, "Enemy_AngleChangeSpeed" );

	EBInfo->NoticeDistance_See			=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance_See" );

	EBInfo->NoticeDistance				=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance" );

	EBInfo->NoticeDistance_AtkMode	=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance_AtkMode" );

	EBInfo->NoticeTime_See				=
		GetTextParamFloat( TextParam, "Enemy_NoticeTime_See" );

	EBInfo->NoticeTime_Distance			=
		GetTextParamFloat( TextParam, "Enemy_NoticeTime_Distance" );

	EBInfo->SeeAtkMoveStartWait.MaxF	=
		GetTextParamFloat( TextParam, "Enemy_SeeAtkMoveStartWait_Max" );

	EBInfo->SeeAtkMoveStartWait.MinF	=
		GetTextParamFloat( TextParam, "Enemy_SeeAtkMoveStartWait_Min" );

	EBInfo->AtkRepeatWait.MaxF		=
		GetTextParamFloat( TextParam, "Enemy_AtkRepeatWait_Max" );

	EBInfo->AtkRepeatWait.MinF		=
		GetTextParamFloat( TextParam, "Enemy_AtkRepeatWait_Min" );

	EBInfo->DamageAfterMoveWait.MaxF	=
		GetTextParamFloat( TextParam, "Enemy_DamageAfterMoveWait_Max" );

	EBInfo->DamageAfterMoveWait.MinF	=
		GetTextParamFloat( TextParam, "Enemy_DamageAfterMoveWait_Min" );

	EBInfo->IdleMoveStartWait.MaxF		=
		GetTextParamFloat( TextParam, "Enemy_IdleMoveStartWait_Max" );

	EBInfo->IdleMoveStartWait.MinF		=
		GetTextParamFloat( TextParam, "Enemy_IdleMoveStartWait_Min" );

	EBInfo->IdleMoveTime.MaxF			=
		GetTextParamFloat( TextParam, "Enemy_IdleMoveTime_Max" );

	EBInfo->IdleMoveTime.MinF			=
		GetTextParamFloat( TextParam, "Enemy_IdleMoveTime_Min" );

	EBInfo->IdleAngleMoveBeginWait		=
		GetTextParamFloat( TextParam, "Enemy_IdleAngleMoveBeginWait" );

	EBInfo->IdleAngleMoveAfterWait		=
		GetTextParamFloat( TextParam, "Enemy_IdleAngleMoveAfterWait" );

	EBInfo->ProgramMove                 =
		GetTextParamBool(  TextParam, "Enemy_ProgramMove" );

	// �G�̍U���Ɋւ�������擾
	EBInfo->AtkNum = 0;
	AInfo = EBInfo->AtkInfo;
	for( i = 0; i < ENEMY_ATTACK_MAX_NUM; i++, AInfo++ )
	{
		if( SearchTextParam( TextParam, "Enemy_Atk%d_AtkDistance", i ) < 0 )
		{
			break;
		}

		AInfo->AtkDistance      =
			GetTextParamFloat( TextParam, "Enemy_Atk%d_AtkDistance",      i );

		AInfo->AtkAngleRange    =
			GetTextParamFloat( TextParam, "Enemy_Atk%d_AtkAngleRange",    i );

		AInfo->Atk         =
			GetTextParamInt(   TextParam, "Enemy_Atk%d_Atk",         i );

		AInfo->IsAtkMoveWalk    =
			GetTextParamBool(  TextParam, "Enemy_Atk%d_IsAtkMoveWalk",    i );

		AInfo->IsAtkAngleChange =
			GetTextParamBool(  TextParam, "Enemy_Atk%d_IsAtkAngleChange", i );

		EBInfo->AtkNum++;
	}

	// �����I��
	return true;
}
