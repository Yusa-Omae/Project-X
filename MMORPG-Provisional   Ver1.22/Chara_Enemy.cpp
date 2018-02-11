#include "Chara_Enemy.h"
#include "DxLib.h"
#include "Chara_EnemyBase.h"
#include "Chara_Player.h"
#include "System.h"
#include "Stage.h"
#include "StageData.h"
#include "Task_GameMain.h"
#include <math.h>

// デフォルトの敵のアニメーション変更速度
#define DEFAULT_CHANGE_ANIM_SPEED			(5.0f)

// プレイヤーの存在を気付かせるためのメッセージの識別番号
#define PLAYER_NEAR_DISTANCE_MESSAGE		(1)

// ダメージを受けた際に周囲に居る仲間にプレイヤーの存在を気付かせる距離
#define PLAYER_NEAR_MESSAGE_SEND_DISTANCE	(2000.0f)

#define GET_MIN_Gold 200

// 敵の状態
typedef enum _EChara_EnemyState
{
	EChara_EnemyState_Idle,						// 特に何もしていない状態
	EChara_EnemyState_IdleMove,					// 特に何もせず移動中
	EChara_EnemyState_IdleAngleMove_BeginWait,	// 特に何もせず向き変更開始前
	EChara_EnemyState_IdleAngleMove_AngleMove,	// 特に何もせず向き変更中
	EChara_EnemyState_IdleAngleMove_AfterWait,	// 特に何もせず向き変更後
	EChara_EnemyState_AttackMoveWait,			// 攻撃移動開始前
	EChara_EnemyState_AttackMove,				// 攻撃移動中
	EChara_EnemyState_Attack,					// 攻撃中
	EChara_EnemyState_AttackAfter,				// 攻撃後
	EChara_EnemyState_Damage,					// ダメージリアクション中
	EChara_EnemyState_DamageAfter,				// ダメージリアクション後
} EChara_EnemyState;

// 敵の攻撃情報
typedef struct _SChara_EnemyAttackInfo
{
	// 攻撃を行う距離の範囲にプレイヤーが居るかどうか
	bool                   AttackDistance;

	// 攻撃を行う距離の範囲にプレイヤーが居るかどうか( 水平方向のみ考慮 )
	bool                   AttackDistance_XZ;
} SChara_EnemyAttackInfo;

// 敵の情報
typedef struct _SChara_EnemyInfo
{
	// 敵の基本情報構造体のアドレス
	SChara_EnemyBaseInfo  *BaseInfo;

	// プレイヤーに気付く距離になってからの経過時間計測用変数
	float                  PlayerNoticeDistanceCounter;

	// プレイヤーに気付く距離になってからの経過時間計測用変数( 水平方向のみ考慮 )
	float                  PlayerNoticeDistanceCounter_XZ;

	// プレイヤーが周囲に居るというメッセージが届いたかどうか
	bool                   PlayerNearDistanceMessage;

	// プレイヤーが見えているかどうか
	bool                   SeePlayer;

	// プレイヤーが見えている時間計測用変数
	float                  SeePlayerCounter;

	// プレイヤーに気付いているかどうか
	bool                   NoticePlayer;

	// プレイヤーに気付いているかどうか( 攻撃中用 )
	bool                   NoticePlayer_AttackMode;

	// プレイヤーの居る方向
	VECTOR                 PlayerDirection;

	// プレイヤーの居る方向( 水平方向のみ考慮 )
	VECTOR                 PlayerDirection_XZ;

	// 攻撃タイプ
	int                    AttackType;

	// 各攻撃タイプの情報
	SChara_EnemyAttackInfo AttackInfo[ENEMY_ATTACK_MAX_NUM];

	// 敵の状態
	EChara_EnemyState      EnemyState;

	// 時間計測用変数
	float                  TimeCounter;

	// 待つ時間
	float                  WaitTime;

	// 目標とする向き
	float                  TargetAngle;

	// 倒された場合にシステムに倒されたことを伝えたかどうか
	bool                   KillCounterAdd;
} SChara_EnemyInfo;

// 敵の処理に必要な情報のセットアップを行う
static void Chara_Enemy_Info_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 敵の「特に何もしていない状態」の共通処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Idle_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 状態変更が発生したかどうかを保存するフラグのアドレス
	bool *ChangeState
);

// 敵の「攻撃移動状態」の共通処理を行う
//     戻り値 : 状態変更が発生したかどうか
//              ( true : 状態変更が発生した  false : 状態変更は発生していない )
static bool Chara_Enemy_State_AttackMove_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 状態変更が発生したかどうかを保存するフラグのアドレス
	bool *ChangeState
);

// 敵の「攻撃状態」の共通処理を行う
static void Chara_Enemy_State_Attack_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「特に何もしていない状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Idle_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「特に何もせず移動中」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_IdleMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「特に何もせず向き変更状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_IdleAngleMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「攻撃移動開始前状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackMoveWait_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「攻撃移動中状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「攻撃中状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Attack_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「攻撃後状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackAfter_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);

// 敵を「ダメージ後状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_DamageAfter_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
);



// 敵が作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Enemy_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SChara_EnemyInfo *EInfo;

	CInfo->Gold = GetRand(250)+ GET_MIN_Gold;
	// 敵の情報構造体を格納するメモリ領域の確保
	CInfo->SubData = malloc(sizeof(SChara_EnemyInfo));
	if (CInfo->SubData == NULL)
	{
		return false;
	}

	EInfo = (SChara_EnemyInfo *)CInfo->SubData;

	// 零初期化
	memset(EInfo, 0, sizeof(SChara_EnemyInfo));

	// 敵の基本情報構造体のアドレスをセット
	EInfo->BaseInfo = (SChara_EnemyBaseInfo *)CInfo->BaseInfo->SubData;

	// 最初は「特に何もしていない状態」にする
	if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵の処理に必要な情報のセットアップを行う
static void Chara_Enemy_Info_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaInfo           *PCInfo;
	VECTOR                EnemyToPlayerVec;
	VECTOR                EnemyToPlayerVec_XZ;
	int                   StageCollModel;
	MV1_COLL_RESULT_POLY  CollPoly;
	int                   i;
	VECTOR                PlayerDirection;
	VECTOR                PlayerDirection_XZ;
	float                 PlayerDistance;
	float                 PlayerDistance_XZ;
	VECTOR                PlayerHeadPosition;
	VECTOR                HeadPosition;
	int                   StageObjectNum;
	int                   ObjectCollModel;

	// ステージ用コリジョン３Ｄモデルのハンドルを取得
	StageCollModel = StageData_GetCollisionModelHandle();

	// ステージオブジェクトの数を取得する
	StageObjectNum = StageData_GetObjectNum();

	// プレイヤーのキャラ情報構造体のアドレスを取得
	PCInfo = System_GetPlayerCharaInfo();

	// プレイヤーが居るかどうかで処理を分岐
	if (PCInfo == NULL)
	{
		// プレイヤーが居ない場合は『プレイヤーを見つけたかどうか』などの情報を初期化する
		EInfo->PlayerNoticeDistanceCounter = 0.0f;
		EInfo->PlayerNoticeDistanceCounter_XZ = 0.0f;
		EInfo->SeePlayer = false;
		EInfo->SeePlayerCounter = 0.0f;
		EInfo->NoticePlayer = false;
		EInfo->NoticePlayer_AttackMode = false;
	}
	else
	{
		// 敵からプレイヤーへのベクトルを算出
		EnemyToPlayerVec = VSub(PCInfo->Position, CInfo->Position);
		EnemyToPlayerVec_XZ = EnemyToPlayerVec;
		EnemyToPlayerVec_XZ.y = 0.0f;

		// 敵からプレイヤーへの正規化ベクトルを算出
		PlayerDirection = VNorm(EnemyToPlayerVec);
		PlayerDirection_XZ = VNorm(EnemyToPlayerVec_XZ);

		// 敵からプレイヤーまでの距離を算出
		PlayerDistance = VSize(EnemyToPlayerVec);
		PlayerDistance_XZ = VSize(EnemyToPlayerVec_XZ);

		// プレイヤーの居る方向を保存
		EInfo->PlayerDirection = PlayerDirection;
		EInfo->PlayerDirection_XZ = PlayerDirection_XZ;

		// プレイヤーに気付く距離に居る場合はプレイヤーに
		// 気付く距離になってからの時間を進める
		if (PlayerDistance < EInfo->BaseInfo->NoticeDistance)
		{
			EInfo->PlayerNoticeDistanceCounter += StepTime;
		}
		else
		{
			EInfo->PlayerNoticeDistanceCounter = 0.0f;
		}

		// プレイヤーに気付く距離に居る場合はプレイヤーに
		// 気付く距離になってからの時間を進める( 水平方向のみ考慮 )
		if (PlayerDistance_XZ < EInfo->BaseInfo->NoticeDistance)
		{
			EInfo->PlayerNoticeDistanceCounter_XZ += StepTime;
		}
		else
		{
			EInfo->PlayerNoticeDistanceCounter_XZ = 0.0f;
		}

		// 敵の視界の範囲にプレイヤーが居るかを調べる
		if (cos(EInfo->BaseInfo->SeeAngleRange) <
			VDot(CInfo->AngleInfo.FrontDirection, PlayerDirection_XZ))
		{
			// 敵の視界の範囲にプレイヤーが居る場合は間に遮蔽物が無いかを調べる

			// プレイヤーと敵の頭の位置を取得
			PlayerHeadPosition = MV1GetFramePosition(PCInfo->ModelHandle,
				MV1SearchFrame(PCInfo->ModelHandle, "head"));
			HeadPosition = MV1GetFramePosition(CInfo->ModelHandle,
				MV1SearchFrame(CInfo->ModelHandle, "head"));

			// お互いの頭部の間に遮るものがあるかどうかを調べる
			CollPoly = MV1CollCheck_Line(StageCollModel, -1, HeadPosition,
				PlayerHeadPosition);
			if (CollPoly.HitFlag == FALSE)
			{
				for (i = 0; i < StageObjectNum; i++)
				{
					ObjectCollModel = StageData_GetObjectCollisionModelHandle(i);

					if (ObjectCollModel == -1)
					{
						continue;
					}

					CollPoly = MV1CollCheck_Line(ObjectCollModel, -1, HeadPosition,
						PlayerHeadPosition);

					if (CollPoly.HitFlag)
					{
						break;
					}
				}
			}

			// 無い場合は敵からプレイヤーが見えるということ
			EInfo->SeePlayer = CollPoly.HitFlag == FALSE;
		}
		else
		{
			EInfo->SeePlayer = false;
		}

		// 敵からプレイヤーが見えている場合は見えるようになってからの時間を進める
		if (EInfo->SeePlayer)
		{
			EInfo->SeePlayerCounter += StepTime;
		}
		else
		{
			EInfo->SeePlayerCounter = 0.0f;
		}

		// 攻撃モードではない場合にプレイヤーの存在に気付く条件が揃っているかをチェック
		if ((EInfo->SeePlayerCounter            > EInfo->BaseInfo->NoticeTime_See &&
			PlayerDistance_XZ                  < EInfo->BaseInfo->NoticeDistance_See) ||
			EInfo->PlayerNoticeDistanceCounter    > EInfo->BaseInfo->NoticeTime_Distance ||
			EInfo->PlayerNoticeDistanceCounter_XZ > EInfo->BaseInfo->NoticeTime_Distance)
		{
			// 揃っている場合はプレイヤーに気付いているかどうかのフラグを立てる
			EInfo->NoticePlayer = true;
		}
		else
		{
			EInfo->NoticePlayer = false;
		}

		// 戦闘モードでプレイヤーの存在に気付く条件が揃っているかをチェック
		if (EInfo->SeePlayer ||
			PlayerDistance    < EInfo->BaseInfo->NoticeDistance_AttackMode ||
			PlayerDistance_XZ < EInfo->BaseInfo->NoticeDistance_AttackMode)
		{
			// 揃っている場合はプレイヤーに気付いているかどうかのフラグを立てる
			EInfo->NoticePlayer_AttackMode = true;
		}
		else
		{
			EInfo->NoticePlayer_AttackMode = false;
		}

		// プレイヤーが周囲に居るというメッセージが届いていたら無条件で
		// プレイヤーに気付いていることにする
		if (EInfo->PlayerNearDistanceMessage)
		{
			EInfo->NoticePlayer = true;
			EInfo->NoticePlayer_AttackMode = true;

			// メッセージが届いたことを示すフラグを倒す
			EInfo->PlayerNearDistanceMessage = false;
		}

		// 各攻撃タイプの「攻撃を行う距離に居るかどうか」のフラグを更新
		for (i = 0; i < EInfo->BaseInfo->AttackNum; i++)
		{
			EInfo->AttackInfo[i].AttackDistance =
				PlayerDistance    < EInfo->BaseInfo->AttackInfo[i].AttackDistance;

			EInfo->AttackInfo[i].AttackDistance_XZ =
				PlayerDistance_XZ < EInfo->BaseInfo->AttackInfo[i].AttackDistance;
		}
	}
}

// 敵の状態推移処理が行われる際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Enemy_Step(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
)
{
	SChara_EnemyInfo *EInfo = (SChara_EnemyInfo *)CInfo->SubData;
	bool              ChangeState;

	// 敵の処理に必要な情報のセットアップを行う
	Chara_Enemy_Info_Setup(CInfo, EInfo, StepTime);

	// キャラの状態が移動中かどうかで処理を分岐
	if (CInfo->State != ECharaState_Move)
	{
		// 移動中ではない場合
		switch (CInfo->State)
		{
		case ECharaState_Damage:	// ダメージ中
									// 敵の状態もダメージ中にする
			EInfo->EnemyState = EChara_EnemyState_Damage;
			break;

		case ECharaState_FadeOut:	// フェードアウト中
									// まだシステムに倒されたことを伝えていない場合は伝える
			Chara_Player_AddMoney(CInfo->Gold*(1 + CInfo->Gold_Per));
			if (!EInfo->KillCounterAdd)
			{
				EInfo->KillCounterAdd = true;
				Task_GameMain_AddKillEnemy(
					System_GetGameMainTaskInfo(), CInfo->CharaNo);
			}
			break;

		default:					// それ以外の場合
									// 敵の状態は「特に何もしていない」にする
			EInfo->EnemyState = EChara_EnemyState_Idle;
			break;
		}

		// 移動中以外の場合は汎用の時間計測用変数を初期化する
		EInfo->TimeCounter = 0.0f;
	}
	else
	{
		// 汎用の時間計測用変数の時間を進める
		EInfo->TimeCounter += StepTime;

		// 敵の状態によって処理を分岐
		switch (EInfo->EnemyState)
		{
		case EChara_EnemyState_Idle:	// 特に何もしていない状態
										// 特に何もしていない状態の共通処理を実行する
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// 一定時間が経過したら「特に何もせず移動中」に移行
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_IdleMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_IdleMove:	// 特に何もせず移動中
											// 特に何もしていない状態の共通処理を実行する
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// 一定時間が経過して且つアニメーションが終了したら
			// 「特に難易もせず向き変更開始前」に移行
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (CInfo->AnimInfo.End)
				{
					if (!Chara_Enemy_State_IdleAngleMove_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
			}
			else
			{
				// 一定時間が経過するまでアニメーションが終了しても無視する
				CInfo->AnimInfo.End = false;
			}
			break;

		case EChara_EnemyState_IdleAngleMove_BeginWait:		// 特に何もせず向き変更開始前
															// 特に何もしていない状態の共通処理を実行する
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// 一定時間が経過したら「特に何もせず向き変更中」に移行
			if (EInfo->TimeCounter > EInfo->BaseInfo->IdleAngleMoveBeginWait)
			{
				EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_AngleMove;

				Chara_SetTargetAngle(CInfo, EInfo->TargetAngle, true);
			}
			break;

		case EChara_EnemyState_IdleAngleMove_AngleMove:		// 特に何もせず向き変更中
															// 向き変更が完了したら「特に何もせず向き変更後」に移行
			if (CInfo->AngleInfo.AngleMoveEnd)
			{
				EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_AfterWait;
				EInfo->TimeCounter = 0.0f;
			}
			break;

		case EChara_EnemyState_IdleAngleMove_AfterWait:		// 特に何もせず向き変更後
															// 特に何もしていない状態の共通処理を実行する
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// 一定時間経過したら「特に何もしていない状態」に移行
			if (EInfo->TimeCounter > EInfo->BaseInfo->IdleAngleMoveAfterWait)
			{
				if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackMoveWait:		// 攻撃移動開始前
													// 戦闘移動状態の共通処理を行う
			if (!Chara_Enemy_State_AttackMove_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// 一定時間経過したら「攻撃移動中」に移行する
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackMove:			// 攻撃移動中
													// 戦闘移動状態の共通処理を行う
			if (!Chara_Enemy_State_AttackMove_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// プレイヤーが攻撃できる距離に来たら「攻撃中」に移行する
			if (EInfo->AttackInfo[EInfo->AttackType].AttackDistance_XZ)
			{
				if (!Chara_Enemy_State_Attack_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_Attack:				// 攻撃中
													// 攻撃状態の共通処理を行う
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// 攻撃アニメーションが終了したら「攻撃後」に移行する
			if (CInfo->AnimInfo.End)
			{
				if (!Chara_Enemy_State_AttackAfter_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackAfter:			// 攻撃後
													// 攻撃状態の共通処理を行う
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// 一定時間が経過したら処理を分岐
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				// プレイヤーが敵の気付く位置に居る場合は「攻撃移動中」に、
				// 居ない場合は「特に何もしていない状態」に移行する
				if (EInfo->NoticePlayer_AttackMode)
				{
					if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
				else
				{
					if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
			}
			break;

		case EChara_EnemyState_Damage:				// ダメージリアクション中
													// ここにくる時はダメージ処理が終わっているので、
													// ダメージリアクション後に移行する
			if (!Chara_Enemy_State_DamageAfter_Setup(CInfo, EInfo))
			{
				return false;
			}
			break;

		case EChara_EnemyState_DamageAfter:			// ダメージリアクション後
													// 攻撃状態の共通処理を行う
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// 一定時間経過していたら「攻撃移動中」に移行する
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;
		}
	}

	// プログラムによる移動を行うかどうかで処理を分岐
	if (EInfo->BaseInfo->ProgramMove)
	{
		// 「特に何もせず移動中」又は「攻撃移動中」の場合はプログラムによる移動を行う
		if (EInfo->EnemyState == EChara_EnemyState_IdleMove ||
			EInfo->EnemyState == EChara_EnemyState_AttackMove)
		{
			CInfo->ProgramMove = true;
		}
		else
		{
			CInfo->ProgramMove = false;
		}
	}

	// 基本的にデフォルトの処理を行う
	*DefaultProcess = true;

	// 正常終了
	return true;
}

// 敵がダメージを受けた際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Enemy_Damage(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// ダメージタイプ
	ECharaAttack_DamageType DamageType,

	// ダメージ
	int DamagePoint,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃の方向
	VECTOR AttackDirection,

	// 攻撃を防御したかどうかを代入する変数のアドレス
	bool *Defence,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
)
{
	// 攻撃は必ず当たる
	*Defence = false;

	// デフォルトの処理を行う
	*DefaultProcess = true;

	// 周囲にプレイヤーに気付いていない敵が居たら気付かせる
	Chara_SendMessage(CInfo, CInfo->Position,
		PLAYER_NEAR_MESSAGE_SEND_DISTANCE, PLAYER_NEAR_DISTANCE_MESSAGE);

	// 正常終了
	return true;
}


// 敵がメッセージが送られてきたときに呼ばれる関数
bool Chara_Enemy_MessageRecv(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// メッセージを送ってきたキャラクター情報構造体のアドレス
	SCharaInfo *SendCInfo,

	// メッセージ識別用の値
	int Message
)
{
	SChara_EnemyInfo *EInfo = (SChara_EnemyInfo *)CInfo->SubData;

	// プレイヤーが近くに居ることを知らせるメッセージだった場合は
	// メッセージが届いたかどうかのフラグを立てる
	if (Message == PLAYER_NEAR_DISTANCE_MESSAGE)
	{
		EInfo->PlayerNearDistanceMessage = true;
	}

	// 正常終了
	return true;
}

// 敵の「特に何もしていない状態」の共通処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Idle_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 状態変更が発生したかどうかを保存するフラグのアドレス
	bool *ChangeState
)
{
	// プレイヤーに気付いたら「攻撃移動開始前」に移行する
	if (EInfo->NoticePlayer)
	{
		if (!Chara_Enemy_State_AttackMoveWait_Setup(CInfo, EInfo))
		{
			return false;
		}

		// 状態変更が発生したので true を代入する
		*ChangeState = true;
	}
	else
	{
		// 状態変更が発生しなかったので false を代入する
		*ChangeState = false;
	}

	// 正常終了
	return true;
}

// 敵の「攻撃移動状態」の共通処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackMove_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo,

	// 状態変更が発生したかどうかを保存するフラグのアドレス
	bool *ChangeState
)
{
	// プレイヤーを見失ったら「特に何もしていない状態」に移行する
	if (!EInfo->NoticePlayer_AttackMode)
	{
		if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
		{
			return false;
		}

		// 状態変更が発生したので true を代入する
		*ChangeState = true;
	}
	else
	{
		// プレイヤーの居る方向に向きを変更する
		Chara_SetTargetDirection(CInfo, EInfo->PlayerDirection_XZ, true, false);
		CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;

		// 状態変更が発生していないので false を代入する
		*ChangeState = false;
	}

	// 正常終了
	return true;
}

// 敵の「攻撃状態」の共通処理を行う
static void Chara_Enemy_State_Attack_Common(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	// 「攻撃中も向いている方向を変化させるかどうか」のフラグが倒れている場合は何もしない
	if (!EInfo->BaseInfo->AttackInfo[EInfo->AttackType].IsAttackAngleChange)
	{
		return;
	}

	// プレイヤーの居る方向に向きを変更する
	Chara_SetTargetDirection(CInfo, EInfo->PlayerDirection_XZ, true, false);
	CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;
}

// 敵を「特に何もしていない状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Idle_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_Idle;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->IdleMoveStartWait);
	if (CInfo->AnimInfo.NowAnim != ECharaAnim_Neutral)
	{
		if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
		{
			return false;
		}
	}

	// 正常終了
	return true;
}

// 敵を「特に何もせず移動中」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_IdleMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_IdleMove;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->IdleMoveTime);
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Walk, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵を「特に何もせず向き変更状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_IdleAngleMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	float MoveAngle;

	EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_BeginWait;
	EInfo->TimeCounter = 0.0f;

	// 変更後の向きはランダム
	MoveAngle = GetRandomFloat(DX_PI_F, 1.0f);
	if (GetRand(1000) > 500)
	{
		MoveAngle = -MoveAngle;
	}

	EInfo->TargetAngle = CInfo->AngleInfo.NowAngle + MoveAngle;
	CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵を「攻撃移動開始前状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackMoveWait_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackMoveWait;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->SeeAttackMoveStartWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵を「攻撃移動中状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackMove_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackMove;
	EInfo->TimeCounter = 0.0f;

	// どの攻撃タイプになるかはランダム
	EInfo->AttackType = GetRand(EInfo->BaseInfo->AttackNum - 1);

	// 攻撃力は攻撃タイプ毎に設定されている値にする
	CInfo->Atk = EInfo->BaseInfo->AttackInfo[EInfo->AttackType].Atk;

	if (!Chara_ChangeAnim(
		CInfo, EInfo->BaseInfo->AttackInfo[EInfo->AttackType].IsAttackMoveWalk ?
		ECharaAnim_Walk : ECharaAnim_Run, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵を「攻撃中状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_Attack_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	static ECharaAnim AttackAnimTable[] =
	{
		ECharaAnim_Attack1,
		ECharaAnim_Attack2,
		ECharaAnim_Attack3
	};

	EInfo->EnemyState = EChara_EnemyState_Attack;

	// 攻撃タイプによって再生するアニメーションを変更
	if (!Chara_ChangeAnim(CInfo, AttackAnimTable[EInfo->AttackType],
		DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;

	}

	// 正常終了
	return true;
}

// 敵を「攻撃後状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_AttackAfter_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackAfter;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->AttackRepeatWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 敵を「ダメージ後状態」に移行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Enemy_State_DamageAfter_Setup(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 敵の情報構造体のアドレス
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_DamageAfter;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->DamageAfterMoveWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// 正常終了
	return true;
}


#if false
//エネミーの攻撃、移動、AI、視野角など
#include"DxLib.h"







//エネミーパラメータ設定
void Enemy_Initialize(){


}

//エネミーAI設定
void Enemy_Ai(){


}
#endif