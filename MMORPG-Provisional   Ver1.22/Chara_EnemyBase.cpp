#include "Chara_EnemyBase.h"

// 敵の基本情報の初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した  false : 失敗した )
bool Chara_EnemyBase_Initialize(
	// キャラクターの基本情報構造体のアドレス
	SCharaBaseInfo *CBInfo,

	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TextParam
)
{
	SChara_EnemyBaseInfo       *EBInfo;
	SChara_EnemyBaseAttackInfo *AInfo;
	int                         i;

	// 敵の基本情報を格納するメモリ領域の確保
	CBInfo->SubData = malloc( sizeof( SChara_EnemyBaseInfo ) );
	if( CBInfo->SubData == NULL )
	{
		return false;
	}
	EBInfo = ( SChara_EnemyBaseInfo * )CBInfo->SubData;

	// 敵の行動に関するパラメータを取得
	EBInfo->SeeAngleRange				=
		GetTextParamFloat( TextParam, "Enemy_SeeAngleRange" );
	EBInfo->SeeAngleRange = EBInfo->SeeAngleRange * DX_PI_F / 180.0f;

	EBInfo->AngleChangeSpeed			=
		GetTextParamFloat( TextParam, "Enemy_AngleChangeSpeed" );

	EBInfo->NoticeDistance_See			=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance_See" );

	EBInfo->NoticeDistance				=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance" );

	EBInfo->NoticeDistance_AttackMode	=
		GetTextParamFloat( TextParam, "Enemy_NoticeDistance_AttackMode" );

	EBInfo->NoticeTime_See				=
		GetTextParamFloat( TextParam, "Enemy_NoticeTime_See" );

	EBInfo->NoticeTime_Distance			=
		GetTextParamFloat( TextParam, "Enemy_NoticeTime_Distance" );

	EBInfo->SeeAttackMoveStartWait.MaxF	=
		GetTextParamFloat( TextParam, "Enemy_SeeAttackMoveStartWait_Max" );

	EBInfo->SeeAttackMoveStartWait.MinF	=
		GetTextParamFloat( TextParam, "Enemy_SeeAttackMoveStartWait_Min" );

	EBInfo->AttackRepeatWait.MaxF		=
		GetTextParamFloat( TextParam, "Enemy_AttackRepeatWait_Max" );

	EBInfo->AttackRepeatWait.MinF		=
		GetTextParamFloat( TextParam, "Enemy_AttackRepeatWait_Min" );

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

	// 敵の攻撃に関する情報を取得
	EBInfo->AttackNum = 0;
	AInfo = EBInfo->AttackInfo;
	for( i = 0; i < ENEMY_ATTACK_MAX_NUM; i++, AInfo++ )
	{
		if( SearchTextParam( TextParam, "Enemy_Attack%d_AttackDistance", i ) < 0 )
		{
			break;
		}

		AInfo->AttackDistance      =
			GetTextParamFloat( TextParam, "Enemy_Attack%d_AttackDistance",      i );

		AInfo->AttackAngleRange    =
			GetTextParamFloat( TextParam, "Enemy_Attack%d_AttackAngleRange",    i );

		AInfo->Attack         =
			GetTextParamInt(   TextParam, "Enemy_Attack%d_Attack",         i );

		AInfo->IsAttackMoveWalk    =
			GetTextParamBool(  TextParam, "Enemy_Attack%d_IsAttackMoveWalk",    i );

		AInfo->IsAttackAngleChange =
			GetTextParamBool(  TextParam, "Enemy_Attack%d_IsAttackAngleChange", i );

		EBInfo->AttackNum++;
	}

	// 成功終了
	return true;
}
