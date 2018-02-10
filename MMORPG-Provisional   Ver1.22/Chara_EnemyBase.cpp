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
	SChara_EnemyBaseAtkInfo *AInfo;
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

	// 敵の攻撃に関する情報を取得
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

	// 成功終了
	return true;
}
