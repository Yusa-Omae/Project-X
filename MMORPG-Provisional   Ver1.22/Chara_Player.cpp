#include "Chara_Player.h"
#include "Chara_PlayerBase.h"
#include "System.h"
#include "Input.h"
#include "Camera.h"
#include "Code/AppData/Item/ItemData.h"
#include <math.h>

// 攻撃１コンボ目の、２コンボ目の攻撃の入力受付開始時間
#define ATTACK1_NEXT_INPUT_START_TIME		(4.0f)

// 攻撃２コンボ目の、３コンボ目の攻撃の入力受付開始時間
#define ATTACK2_NEXT_INPUT_START_TIME		(4.0f)

// ロックオン距離
#define LOCKON_DISTANCE						(1000.0f)

// ロックオン角度範囲
#define LOCKON_ANGLE						(1.0f)

// ジャンプ力
#define JUMP_POWER							(770.0f)

// デバッグ機能が有効な場合のジャンプ力
#define DEBUG_JUMP_POWER					(1400.0f)

// ジャンプ中の速度
#define JUMPMOVE_SPEED						(460.0f)

// 攻撃力
#define ATTACK_POWER						(15)

// デバッグ機能が有効な場合の攻撃力
#define DEBUG_ATTACK_POWER					(4000)

int Save_Item[10];
int Save_Gold;


// プレイヤーの状態
typedef enum _EChara_PlayerState
{
	EChara_PlayerState_None,			// 特に無し
	EChara_PlayerState_Attack1,			// 攻撃１コンボ目
	EChara_PlayerState_Attack2,			// 攻撃２コンボ目
	EChara_PlayerState_Attack3,			// 攻撃３コンボ目

	EChara_PlayerState_GuardIn,			// ガード開始
	EChara_PlayerState_GuardLoop,		// ガード中
	EChara_PlayerState_GuardOut,		// ガード終了
	EChara_PlayerState_GuardImpact,		// ガードで攻撃を受け止め中

	EChara_PlayerState_JumpIn,			// ジャンプ開始
	EChara_PlayerState_JumpLoop,		// ジャンプ中
	EChara_PlayerState_JumpOut,			// ジャンプ終了
} EChara_PlayerState;


// プレイヤーの情報
typedef struct _SChara_PlayerInfo
{
	// プレイヤーの状態
	EChara_PlayerState PlayerState;

	// 次の攻撃が予約済みかどうかのフラグ
	bool               NextAttackRequest;

	// アニメーションのキャンセルイベントをチェック済みかどうかのフラグ
	bool               AnimCancelCheck;

	// プレイヤーが攻撃対象としているキャラの情報構造体へのポインタ
	SCharaInfo *       LockOnTarget;

	// ジャンプを開始した時点での速度
	VECTOR             JumpSpd;
} SChara_PlayerInfo;

SCharaInfo* s_PlayerInfo;



// プレイヤーが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Player_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SChara_PlayerInfo *PInfo;

	// システムにプレイヤーのキャラ情報構造体のアドレスをセット
	System_SetPlayerCharaInfo(CInfo);

	// プレイヤーの情報構造体を格納するメモリ領域の確保
	CInfo->SubData = malloc(sizeof(SChara_PlayerInfo));
	if (CInfo->SubData == NULL)
	{
		return false;
	}

	PInfo = (SChara_PlayerInfo *)CInfo->SubData;

	// プレイヤーの情報を初期化
	PInfo->NextAttackRequest = false;
	PInfo->AnimCancelCheck = false;
	PInfo->LockOnTarget = NULL;

	// 体力ゲージの情報を初期化
	CharaHealthGaugeSetup(&CInfo->HealthGauge, true, 1.0f);

	// プレイヤーは常に体力ゲージを表示する
	CInfo->HealthGaugeVisible = true;

	// 攻撃力をセット
	CInfo->Atk = ATTACK_POWER;

	CInfo->Gold = 9999999999;

	//アイテムの初期化処理
	for (int i = 0; i < 10; i++) {
		CInfo->Item_Db[i] = -1;
		CInfo->ItemHav[i] = -1;
	}

	//キャラクター構造体を設定
	s_PlayerInfo = CInfo;

	// 正常終了
	return true;
}

// プレイヤーが削除される際に呼ばれる関数
void Chara_Player_Delete(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	// システムに登録したプレイヤーのキャラ情報構造体のアドレスを無効にする
	System_SetPlayerCharaInfo(NULL);
}

// プレイヤーの状態推移処理が行われる際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Player_Step(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;
	int    EdgeInput;
	int    Input;
	bool   MoveInputFlag;
	bool   InputLeft;
	bool   InputRight;
	bool   InputUp;
	bool   InputDown;
	bool   InputDefence;
	bool   InputJump;
	bool   InputAttack;
	VECTOR NewSpeed;
	float  NewAngle;

	// デバッグ機能が有効になっているかどうかで攻撃力を変化させる
	if (System_GetDispDebugInfo())
	{
		CInfo->Atk = DEBUG_ATTACK_POWER;
	}
	else
	{
		int Crt_Decision = GetRand(100);
		//クリティカルが入れば1.25倍する
		if (Crt_Decision <= (CInfo->Crt + CInfo->Add_Crt) * 100) {
			CInfo->Atk = (ATTACK_POWER + CInfo->Add_Atk)*1.25;
		}else{
			CInfo->Atk = ATTACK_POWER + CInfo->Add_Atk;
		}
	}

	// 入力状態を取得
	EdgeInput = GetEdgeInput();
	Input = GetInput();

	InputLeft = (Input     & (1 << EInputType_Left)) != 0;
	InputRight = (Input     & (1 << EInputType_Right)) != 0;
	InputUp = (Input     & (1 << EInputType_Up)) != 0;
	InputDown = (Input     & (1 << EInputType_Down)) != 0;
	InputAttack = (EdgeInput & (1 << EInputType_Attack)) != 0;
	InputDefence = (Input     & (1 << EInputType_Defence)) != 0;
	InputJump = (EdgeInput & (1 << EInputType_Jump)) != 0;

	// キャラクターが移動する入力を行っているかどうかを取得
	MoveInputFlag = InputLeft || InputRight || InputUp || InputDown;

	// 移動する入力によって新しい速度を決定
	NewSpeed = VGet(0.0f, 0.0f, 0.0f);
	if (InputLeft)
	{
		NewSpeed = VAdd(NewSpeed, VScale(Camera_RightDirection(), -1.0f));
	}
	else
		if (InputRight)
		{
			NewSpeed = VAdd(NewSpeed, Camera_RightDirection());
		}
	if (InputUp)
	{
		NewSpeed = VAdd(NewSpeed, Camera_FrontDirection());
	}
	else
		if (InputDown)
		{
			NewSpeed = VAdd(NewSpeed, VScale(Camera_FrontDirection(), -1.0f));
		}

	// プレイヤーが敵をロックオンしている場合はその方向にプレイヤーの向きを変化させる
	if (PInfo->LockOnTarget != NULL)
	{
		Chara_TargetAngleMove(CInfo, PInfo->LockOnTarget->Position);
	}

STATE_PROCESS:

	// キャラの状態によって処理を分岐
	switch (CInfo->State)
	{
	case ECharaState_Move:		// 移動中
								// 移動中の場合はロックオンは解除する
		PInfo->LockOnTarget = NULL;

		// ジャンプ中の場合は特に何もせず終了
		if (CInfo->JumpState)
		{
			break;
		}

		// 攻撃ボタンが押されていたら攻撃状態に移行する
		if (InputAttack)
		{
			CInfo->State = ECharaState_Attack;
			PInfo->PlayerState = EChara_PlayerState_Attack1;
			PInfo->AnimCancelCheck = false;

			// ロックオン対象となる敵が居るか検索する
			PInfo->LockOnTarget = Chara_SearchTarget(CInfo, ECharaType_Enemy,
				LOCKON_DISTANCE, LOCKON_ANGLE);

			// 攻撃１コンボ目のアニメーションを再生
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack1,
				CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
			{
				return false;
			}
		}
		else
			// ガードボタンが押されていたらガード状態に移行する
			if (InputDefence)
			{
				CInfo->State = ECharaState_Defence;
				PInfo->PlayerState = EChara_PlayerState_GuardIn;

				// ガード開始アニメーションを再生
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_In,
					CHARA_DEFAULT_CHANGE_ANIM_SPEED  * (1 + CInfo->Add_Spd)))
				{
					return false;
				}
			}
			else
				// ジャンプボタンが押されていたらジャンプ状態に移行する
				if (InputJump)
				{
					CInfo->State = ECharaState_Jump;
					PInfo->PlayerState = EChara_PlayerState_JumpIn;

					// ジャンプを開始した時点での速度を保存しておく
					PInfo->JumpSpd = CInfo->PrevAnimSpd;

					// ジャンプ開始アニメーションを再生
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_In,
						CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
					{
						return false;
					}
				}
				else
					// 何かしらの移動入力がされている場合は向きを変更し、走りアニメーションを再生する
					if (MoveInputFlag)
					{
						NewSpeed = VNorm(NewSpeed);
						NewAngle = atan2(NewSpeed.x, NewSpeed.z);
						Chara_SetTargetAngle(CInfo, NewAngle, true);

						if (CInfo->AnimInfo.NowAnim != ECharaAnim_Run)
						{
							if (!Chara_ChangeAnim(CInfo, ECharaAnim_Run,
								CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
							{
								return false;
							}
						}
					}
					else
						// 何も入力がされていない場合はニュートラルアニメーションを再生する
					{
						if (CInfo->AnimInfo.NowAnim != ECharaAnim_Neutral)
						{
							if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
								CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
							{
								return false;
							}
						}
					}
		break;

	case ECharaState_Attack:		// 攻撃中
									// プレイヤーの状態によって処理を分岐
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_Attack1:	 // 攻撃１コンボ目
											 // 攻撃２コンボ目の入力が可能になる時間を経過している状態で攻撃入力が
											 // されたら、『次の攻撃が予約済みかどうかのフラグ』を立てる
			if (InputAttack && CInfo->AnimInfo.NowTime > ATTACK1_NEXT_INPUT_START_TIME)
			{
				PInfo->NextAttackRequest = true;
			}

			// アニメーションのキャンセルイベントが発生していて、且つキャンセルイベントの
			// チェックをまだ行っていない場合は処理を分岐
			if (CInfo->AnimInfo.Cancel && !PInfo->AnimCancelCheck)
			{
				// キャンセルイベントのチェックを行ったかどうかのフラグを立てる
				PInfo->AnimCancelCheck = true;

				// 攻撃２コンボ目を発動させる予約がされていたら攻撃２コンボ目を開始する
				if (PInfo->NextAttackRequest)
				{
					PInfo->AnimCancelCheck = false;
					PInfo->NextAttackRequest = false;
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack2,
						CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
					{
						return false;
					}

					PInfo->PlayerState = EChara_PlayerState_Attack2;
				}
			}
			else
				// アニメーションが終了していたら移動状態に移行する
				if (CInfo->AnimInfo.End)
				{
					CInfo->State = ECharaState_Move;
					PInfo->PlayerState = EChara_PlayerState_None;

					// 移動状態を１フレーム分実行する
					goto STATE_PROCESS;
				}
			break;

		case EChara_PlayerState_Attack2:	// 攻撃２コンボ目
											// 攻撃３コンボ目の入力が可能になる時間を経過している状態で攻撃入力が
											// されたら、『次の攻撃が予約済みかどうかのフラグ』を立てる
			if (InputAttack && CInfo->AnimInfo.NowTime > ATTACK2_NEXT_INPUT_START_TIME)
			{
				PInfo->NextAttackRequest = true;
			}

			// アニメーションのキャンセルイベントが発生していて、且つキャンセルイベントの
			// チェックをまだ行っていない場合は処理を分岐
			if (CInfo->AnimInfo.Cancel && !PInfo->AnimCancelCheck)
			{
				// キャンセルイベントのチェックを行ったかどうかのフラグを立てる
				PInfo->AnimCancelCheck = true;

				// 攻撃３コンボ目を発動させる予約がされていたら攻撃３コンボ目を開始する
				if (PInfo->NextAttackRequest)
				{
					PInfo->AnimCancelCheck = false;
					PInfo->NextAttackRequest = false;
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack3,
						CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
					{
						return false;
					}

					PInfo->PlayerState = EChara_PlayerState_Attack3;
				}
			}
			else
				// アニメーションが終了していたら移動状態に移行する
				if (CInfo->AnimInfo.End)
				{
					CInfo->State = ECharaState_Move;
					PInfo->PlayerState = EChara_PlayerState_None;

					// 移動状態を１フレーム分実行する
					goto STATE_PROCESS;
				}
			break;

		case EChara_PlayerState_Attack3:	// 攻撃３コンボ目
											// アニメーションが終了していたら移動状態に移行する
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// 移動状態を１フレーム分実行する
				goto STATE_PROCESS;
			}
			break;
		}
		break;

	case ECharaState_Defence:		// 防御中
									// プレイヤーの状態によって処理を分岐
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_GuardIn:		// ガード開始
												// ガード開始アニメーションが終了したらガード中状態に移行する
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_GuardLoop:		// ガード中
												// 入力ボタンが離されたらガード終了状態に移行する
			if (!InputDefence)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardOut;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Out,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_GuardOut:		// ガード終了
												// ガード終了アニメーションが終了したら移動状態に移行する
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// 移動状態を１フレーム分実行する
				goto STATE_PROCESS;
			}
			break;

		case EChara_PlayerState_GuardImpact:	// ガードで攻撃を受け止め中
												// ガードで攻撃を受け止めるアニメーションが完了したらガード中状態に移行する
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;
		}
		break;

	case ECharaState_Jump:		// ジャンプ中
								// プレイヤーの状態によって処理を分岐
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_JumpIn:		// ジャンプ開始
											// ジャンプ開始アニメーションが終了したらジャンプ中状態に移行する
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_JumpLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_JumpOut:	// ジャンプ終了
											// ジャンプ終了アニメーションが終了したら移動状態に移行する
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// 移動状態を１フレーム分実行する
				goto STATE_PROCESS;
			}
			break;
		}
		break;

		// 吹っ飛び、ダウン、フェードアウトはデフォルトのキャラクター処理を行う
	case ECharaState_Blow:
	case ECharaState_Down:
	case ECharaState_FadeOut:
		*DefaultProcess = true;
		return true;

	case ECharaState_Damage:	// ダメージ中
								// ジャンプ中にダメージアニメーションが終了した場合はジャンプ中状態に移行する
		if (CInfo->AnimInfo.End && CInfo->JumpState)
		{
			CInfo->State = ECharaState_Jump;
			PInfo->PlayerState = EChara_PlayerState_JumpLoop;

			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop,
				CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
			{
				return false;
			}
		}
		else
		{
			// それ以外の場合はデフォルトの処理を行う
			*DefaultProcess = true;
			return true;
		}
		break;
	}

	// ここに来た場合はデフォルトの処理は実行しない
	*DefaultProcess = false;

	// 正常終了
	return true;
}

// プレイヤーがダメージを受けた際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Player_Damage(
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
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;
	float ACos;

	// デバッグ機能が有効になっていたらダメージを受けない
	if (System_GetDispDebugInfo())
	{
		*Defence = true;
		*DefaultProcess = false;

		return true;
	}

	// ガード中ではない場合はダメージを受ける
	if (CInfo->State != ECharaState_Defence)
	{
		*Defence = false;
		*DefaultProcess = true;

		return true;
	}

	// ガードをしている場合でも背後側から攻撃されたらダメージを受ける
	ACos = VDot(AttackDirection, CInfo->AngleInfo.FrontDirection);
	if (ACos >= 0.0f)
	{
		*Defence = false;
		*DefaultProcess = true;

		return true;
	}

	// ガード音を再生する
	Sound_PlaySound3D(HitPosition, PBInfo->DefenceSuccessSound, DX_PLAYTYPE_BACK);

	// ガードで攻撃を受け止めた状態に移行する
	PInfo->PlayerState = EChara_PlayerState_GuardImpact;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Impact,
		CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ガードをしたかどうかのフラグを立てる
	*Defence = true;

	// ダメージを受けなかった場合はデフォルトの動作を行わない
	*DefaultProcess = false;

	// 正常終了
	return true;
}

// プレイヤーが着地した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Player_Landed(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;

	// 吹っ飛び中やダウン中の場合はデフォルトの動作を行う
	if (CInfo->State == ECharaState_Blow ||
		CInfo->State == ECharaState_Down ||
		CInfo->State == ECharaState_FadeOut)
	{
		*DefaultProcess = true;
		return true;
	}

	// 横方向の移動速度を０にする
	CInfo->Spd.x = 0.0f;
	CInfo->Spd.z = 0.0f;

	// ジャンプ終了状態に移行する
	CInfo->State = ECharaState_Jump;
	PInfo->PlayerState = EChara_PlayerState_JumpOut;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Out,
		CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ここに来た場合はデフォルトの動作は行わない
	*DefaultProcess = false;

	// 正常終了
	return true;
}

// アニメーションイベント「その他」が発生した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Player_AnimOtherEvent(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;

	// ジャンプ中の速度をセット
	CInfo->Spd = PInfo->JumpSpd;

	// デバッグ機能が有効な場合は大ジャンプをする
	if (System_GetDispDebugInfo())
	{
		CInfo->Spd.y = DEBUG_JUMP_POWER;
	}
	else
	{
		CInfo->Spd.y = JUMP_POWER;
	}

	// ジャンプ状態かどうかのフラグを立てる
	CInfo->JumpState = true;

	// 正常終了
	return true;
}


/*
キャラクターにアイテムを設定する
*/
void Chara_Player_SetItem(int haveId, int itmeIdex) {
	if (s_PlayerInfo == NULL) return;
	if (haveId < 0 || haveId >= 10) return;

	s_PlayerInfo->ItemHav[haveId] = itmeIdex;

}


/*
キャラクターにアイテムを設定する
return -1でエラー　0以上でアイテム
*/
int Chara_Player_GetItem(int haveId) {
	if (s_PlayerInfo == NULL) return -1;
	if (haveId < 0 || haveId >= 10) return -1;

	return s_PlayerInfo->ItemHav[haveId];

}


/*
キャラクターのお金を設定する
*/
void Chara_Player_SetMoney(int gold) {
	if (s_PlayerInfo == NULL) return;

	s_PlayerInfo->Gold = gold;
}

//キャラクターにお金を加算する。
void Chara_Player_AddMoney(int gold) {
	if (s_PlayerInfo == NULL) return;

	s_PlayerInfo->Gold += gold;
}

/*
キャラクターのお金を返却する
return 所持金額
*/
int Chara_Player_GetMoney() {
	if (s_PlayerInfo == NULL) return 0;

	return s_PlayerInfo->Gold;
}

//アイテムのパラメータを合計する
void Player_Add_Item_sum() {
	if (s_PlayerInfo == NULL) return;

	ITEM_PARAM_DATA_t ItemData;

	for (int i = 0; i < 10; i++) {
		int ItemHave = s_PlayerInfo->ItemHav[i];

		ItemData_GetItemData(ItemHave, &ItemData);

		if (ItemHave >= 0 && ItemHave < ITEM_PARAM_DATA_NUM){

			s_PlayerInfo->Add_Health += ItemData.Health;
			s_PlayerInfo->Add_Atk += ItemData.Attack;
			s_PlayerInfo->Add_Def += ItemData.Def;
			s_PlayerInfo->Add_Crt += ItemData.Critical;
			s_PlayerInfo->Add_Spd += ItemData.Spd;
			s_PlayerInfo->Abp_Heal += ItemData.Absorption;
			s_PlayerInfo->Auto_Heal += ItemData.AutoHeal;
			s_PlayerInfo->Gold_Per += ItemData.GoldPrice;

		}
	}
}

//アイテムの耐久値を設定(仮)
int Set_Player_Item_Durable(int BuyItem) {
	if (s_PlayerInfo == NULL) return -1;

	ITEM_PARAM_DATA_t ItemData,NewItemData;
	bool Item_Load_flg = true;

	for (int i = 0; i < 10; i++) {

		int ItemHave = s_PlayerInfo->ItemHav[i];

		ItemData_GetItemData(ItemHave, &ItemData);
		ItemData_GetItemData(BuyItem, &NewItemData);
		
		if (ItemHave >= 0 && ItemHave < ITEM_PARAM_DATA_NUM) {
			for (int j = 0; j < 4; j++) {
				if (ItemData.Evol[j] == BuyItem) {
					//既存の武器より進化した場合
					s_PlayerInfo->Item_Db[i] = NewItemData.Durable - ItemData.Durable + s_PlayerInfo->Item_Db[i];
					s_PlayerInfo->ItemHav[i] = BuyItem;
					Item_Load_flg = true;
					break;

				}else{
					Item_Load_flg = false;
				}
			}

		}
		if (Item_Load_flg == false){
			//新しく買った場合
			s_PlayerInfo->Item_Db[i] = NewItemData.Durable;
			s_PlayerInfo->ItemHav[i] = BuyItem;
			Item_Load_flg = true;
			break;
		}else if (ItemHave == -1) {
			//新しく買った場合
			s_PlayerInfo->Item_Db[i] = NewItemData.Durable;
			s_PlayerInfo->ItemHav[i] = BuyItem;
			return -1;
			break;
		}
	}

}