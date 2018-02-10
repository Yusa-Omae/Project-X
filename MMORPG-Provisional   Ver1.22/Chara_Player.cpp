#include "Chara_Player.h"
#include "Chara_PlayerBase.h"
#include "System.h"
#include "Input.h"
#include "Camera.h"
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
#define ATTACK_POWER						(40)

// デバッグ機能が有効な場合の攻撃力
#define DEBUG_ATTACK_POWER					(4000)


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
		CInfo->Atk = ATTACK_POWER;
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
					CHARA_DEFAULT_CHANGE_ANIM_SPEED))
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
						CHARA_DEFAULT_CHANGE_ANIM_SPEED))
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
								CHARA_DEFAULT_CHANGE_ANIM_SPEED))
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
								CHARA_DEFAULT_CHANGE_ANIM_SPEED))
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


#if false
#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"
#include"System.h"
#include"Chara.h"
#include"Camera.h"
#include "Code/Common/Mouse/Mouse.h"

#define MOVE_SPEED (-2.5f)

_PLAYER *PLAYER;
static struct _MOUSE MOUSE;

static void Player_DirectionProc(float* vAngle,float* hAngle,float* distance) {

	//マウスの押下状態の取得
	//MOUSE.Input = GetMouseInput();
	//現在のマウス座標の取得
	//GetMousePoint(&MOUSE.X, &MOUSE.Y);

	Mouse_GetPositioin(&MOUSE.X, &MOUSE.Y);
	MOUSE.Wheel_Rot = Mouse_WheelValueF();

	MOUSE.X -= INIT_AREA_X2 / 2;
	MOUSE.Y -= INIT_AREA_Y2 / 2;

#ifdef __MY_DEBUG__
	if (Mouse_Press(eMouseInputBotton_Left)) {
		printfDx("左\n");
	}
	if (Mouse_Press(eMouseInputBotton_Rigth)) {
		printfDx("右\n");
	}
	if (Mouse_Press(eMouseInputBotton_Wheel)) {
		printfDx("ホイール\n");
	}
#endif


#if false //とりあえずエラーになるのでコメントアウト by.Syara
	//ウィンドウ表示位置の取得
	GetWindowRect(s_SYSTEM_INFO.WinHndl, s_SYSTEM_INFO.WinAxis);

	if (s_SYSTEM_INFO.WinAxis == NULL) return;

	//マウス座標制限 //前フレームからの移動量算出した後ならがくつきが少ないかと思ったけどそうでもなかった。（座標強制変更後）(アプリ起動画面によって座標変更しなければならない。いい手はないものか。)
	if (MOUSE.Rest_Flg == true) {
		if (MOUSE.X >= s_SYSTEM_INFO.WinAxis->right) {
			MOUSE.Rest_Flg = false;						//マウス座標強制移動フラグをオンにする。
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//強制移動
			MOUSE.Back_X = MOUSE.X;						//1フレーム前座標の更新
		}
		else if (MOUSE.X <= s_SYSTEM_INFO.WinAxis->left) {
			MOUSE.Rest_Flg = false;
			MOUSE.X = s_SYSTEM_INFO.WinAxis->right;
			MOUSE.Back_X = MOUSE.X;
		}
		if (MOUSE.Y >= s_SYSTEM_INFO.WinAxis->bottom) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->top;
			MOUSE.Back_Y = MOUSE.Y;
		}
		else if (MOUSE.Y <= s_SYSTEM_INFO.WinAxis->top) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->bottom;
			MOUSE.Back_Y = MOUSE.Y;
		}
	}
	else {
		SetMousePoint(MOUSE.X, MOUSE.Y);					//マウス座標のセット
		MOUSE.Rest_Flg = true;							//マウス座標強制移動フラグをオフにする。
	}
#endif	//false 

	//マウスの移動量を算出
	MOUSE.Move_X = MOUSE.X - MOUSE.Back_X;
	MOUSE.Move_Y = MOUSE.Y - MOUSE.Back_Y;
	//マウス座標保存
	MOUSE.Back_X = MOUSE.X;
	MOUSE.Back_Y = MOUSE.Y;


	//右クリックが押されていたらカメラの向きを変更
	//if(MOUSE.Input & MOUSE_INPUT_RIGHT){
	
#if false
	//垂直角度計算
	*vAngle += CAMERA_ANGLE_SPEED * MOUSE.Move_Y;
	if (*vAngle < -(DX_PI_F / 2.0f - 0.1f)) {
		*vAngle = -(DX_PI_F / 2.0f - 0.1f);
	}
	if (*vAngle > DX_PI_F / 2.0f - 0.1f) {
		*vAngle = DX_PI_F / 2.0f - 0.1f;
	}
#endif

	//水平角度計算
	*hAngle -= CAMERA_ANGLE_SPEED * MOUSE.Move_X;
	if (*hAngle < 0.0f) {
		*hAngle += DX_PI_F * 2.0f;
	}
	else if (*hAngle >= DX_PI_F * 2.0f) {
		*hAngle -= DX_PI_F * 2.0f;
	}
	//}


#if false
	//ゲーム開始時-1.0まで自動で(カメラの制限を設けたい。)
	if (MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM) {
		//スタート時からのカウント
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI / 2 / WHEEL_CNT_INIT_NUM * MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}
	else {
		//マウスホイールの回転数を取得
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		
	}
#endif

	

	//マウスホイールが回転されたらカメラの位置を前後に移動する。
	//負の値
	if (MOUSE.Wheel_Rot <= 0) {
		//ホイール量（注視点距離制限）//Sin処理追加予定
		if (*distance <= DISTANCE_MAX) {
			*distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//正の値
	}
	else if (MOUSE.Wheel_Rot >= 0) {
		if (*distance >= DISTANCE_MIN) {
			*distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}

	}



}


//プレイヤーデータの初期化
void Player_Info_Initialize(){

	//メモリの確保
	_PLAYER *ix  = (_PLAYER *)malloc(sizeof(_PLAYER));

	//メモリの中身の初期化
	if(ix != NULL){
		memset(ix,0,sizeof(_PLAYER));
	}
	//メモリのグローバル化
	PLAYER = ix;

	//グローバル変数に渡したメモリ変数ixの初期化
	if(ix != NULL){
		ix = NULL;
	}

	SetMousePoint(INIT_AREA_X2 / 2, INIT_AREA_Y2 / 2);

}

void PlayerUpdate(){

	float dir = Get_Chara_Rotation(model_Player);

	float isMove = true;
	float vAngle = Camera_GetVAngle();
	float hAngle = Camera_GetHAngle();
	float distance = Camera_GetDistance();

	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);

	VECTOR playerPos = PLAYER->pos;

	if(key(KEY_INPUT_W)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Up);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_S)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Down);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_A)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Left);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_D)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Right);
		Set_Move_flg(model_Player, FALSE);
	}else{
		Set_Move_flg(model_Player, TRUE);
		isMove = false;
	}

	Player_DirectionProc(&vAngle, &hAngle,&distance);

	if (isMove) {

		VECTOR tmpVec;
		float sinParam;
		float cosParam;
		
		float angle = hAngle;/// 180.0f * DX_PI_F;
		sinParam = sin(angle);
		cosParam = cos(angle);

		tmpVec.x = moveVec.x * cosParam - moveVec.z * sinParam;
		tmpVec.y = 0.0f;
		tmpVec.z = moveVec.x * sinParam + moveVec.z * cosParam;

		playerPos = VAdd(playerPos, tmpVec);
		


	}

	Set_Chara_Rotation(model_Player, hAngle);
	PLAYER->pos = playerPos;

	Camera_SetVAngle(vAngle);
	Camera_SetHAngle(hAngle);
	Camera_SetTargetPosition(PLAYER->pos);
	Camera_SetDistance(distance);

	//木下先生に聞こう
	//PLAYER->New_Speed = VGet(0.0f,0.0f,0.0f);
	//if(key(KEY_INPUT_W)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_S)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_A)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_D)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else{
	//	//プレイヤーMoveflg更新
	//	Set_Move_flg(model_Player, TRUE);
	//}

	//if(Get_Move_flg(model_Player) == FALSE){
	//	PLAYER->New_Speed = VNorm(PLAYER->New_Speed);
	//	PLAYER->Target_Angle = atan2(PLAYER->New_Speed.x,PLAYER->New_Speed.z);
	//	//即座に
	//	if()

	//}
}

//プレイヤー座標を受け取る。
VECTOR Get_Player_Pos(){
	return PLAYER->pos;
}
//プレイヤー座標の+演算
void Add_Player_Pos(VECTOR Add_Axis){
	PLAYER->pos = VAdd(PLAYER->pos,Add_Axis);
}
//プレイヤー座標の-演算
void Sub_Player_Pos(VECTOR Sub_Axis){
	PLAYER->pos = VSub(PLAYER->pos,Sub_Axis);
}
//プレイヤーデータのクリア
void Player_Info_Fainalize(){
	//PLAYERメモリの解放
	if(PLAYER != NULL){
		free(PLAYER);
	}
}
//プレイヤークラスのデバッグ用
void Debug_Player(){
	DrawFormatString(10,0,Color_ValTbl[eCol_White],"Player_position = (%f,%f,%f)",PLAYER->pos.x,PLAYER->pos.y,PLAYER->pos.z);
}
#endif