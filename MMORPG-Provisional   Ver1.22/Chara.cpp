#include "Chara.h"
#include "Stage.h"
#include "StageData.h"
#include "System.h"
#include "Sound.h"
#include "Mathematics.h"
#include "Effect.h"
#include "Effect_SlashLocus.h"
#include "Effect_Damage.h"
#include "Effect_Dead.h"
#include "Effect_SphereLocus.h"
#include <math.h>
#include <stdlib.h>

#include "Chara_Player.h"
#include "Chara_Enemy.h"
#include "Chara_Bee.h"
#include "Chara_Golem.h"
#include "Chara_Needle.h"

// キャラの最大数
#define CHARA_MAXNUM					(256)

// 処理するコリジョンポリゴンの最大数
#define CHARA_MAX_HITCOLL				(2048)

// 周囲のポリゴン検出に使用する球の初期サイズ
#define CHARA_ENUM_DEFAULT_SIZE			(200.0f)

// 足を踏み外した時のジャンプ力
#define CHARA_FALL_UP_POWER				(60.0f)

// 着地している際の床との当たり判定の座標の補正値
#define CHARA_HIT_FLOOR_Y_ADJUST_WALK	(-40.0f)

// ジャンプしている際の床との当たり判定の座標の補正値
#define CHARA_HIT_FLOOR_Y_ADJUST_JUMP	(-1.0f)

// 壁押し出し処理の最大試行回数
#define CHARA_HIT_TRYNUM				(16)

// 一度の壁押し出し処理でスライドさせる距離
#define CHARA_HIT_SLIDE_DISTANCE		(5.0f)

// キャラクター同士で当たったときの押し出される力
#define CHARA_HIT_PUSH_POWER			(12.0f)

// 重力
#define CHARA_GRAVITY					(980.0f * 2.0f)

// キャラがフェードアウトする速度
#define CHARA_DOWN_FADEOUT_SPEED		(2.0f)

// キャラが吹っ飛ぶデフォルト速度
#define CHARA_DEFAULT_BLOWSPEED			(900.0f)

// キャラが吹っ飛ぶ際の高さ
#define CHARA_DEFAULT_BLOWHEIGHT		(76.514f)

// 丸影が落ちる高さ
#define CHARA_SHADOW_HEIGHT				(700.0f)

// 一度に描画できる丸影ポリゴンの数
#define CHARA_SHADOW_POLYGON_NUM		(16)

// 体力ゲージの幅
#define HP_GAUGE_WIDTH				(64)

// 体力ゲージの高さ
#define HP_GAUGE_HEIGHT				(4)

// ステージコリジョンタイプ
typedef enum _EStageCollType
{
	EStageCollType_Wall,			// 壁
	EStageCollType_Floor,			// 床
	EStageCollType_Ceiling,			// 天井

	EStageCollType_Num				// コリジョンタイプの数
} EStageCollType;

// キャラ別の処理を分けるためのコールバック関数情報
typedef struct _SCharaFunction
{
	// 作成時に呼ばれる関数へのポインタ
	bool(*Create)(SCharaInfo *CInfo);

	// 削除時に呼ばれる関数へのポインタ
	void(*Delete)(SCharaInfo *CInfo);

	// 状態推移時に呼ばれる関数へのポインタ
	bool(*Step)(SCharaInfo *CInfo, float StepTime, bool *DefaultProcess);

	// ダメージを受けた際に呼ばれる関数へのポインタ
	bool(*Damage)(SCharaInfo *CInfo, ECharaAttack_DamageType DamageType,
		int DamagePoint, VECTOR HitPosition, VECTOR AttackDirection,
		bool *Defence, bool *DefaultProcess);

	// 着地時に呼ばれる関数へのポインタ
	bool(*Landed)(SCharaInfo *CInfo, bool *DefaultProcess);

	// アニメーションの「その他」イベントが発生した際に呼ばれる関数へのポインタ
	bool(*AnimOtherEvent)(SCharaInfo *CInfo);

	// メッセージが送られてきたときに呼ばれる関数へのポインタ
	bool(*MessageRecv)(SCharaInfo *CInfo, SCharaInfo *SendCInfo, int Message);
} SCharaFunction;

// キャラの処理全体で使用する情報
typedef struct _SCharaSystemInfo
{
	// 丸影描画用の画像ハンドル
	int  ShadowHandle;
} SCharaSystemInfo;

// キャラの処理全体で使用する情報
static SCharaSystemInfo g_CharaSystemInfo;

// キャラの情報
static SCharaInfo g_CharaInfo[CHARA_MAXNUM];

// キャラ別の処理を分けるためのコールバック関数テーブル
static SCharaFunction g_CharaFunctionTable[EChara_Num] =
{
	// EChara_Player
	{ Chara_Player_Create, Chara_Player_Delete, Chara_Player_Step,
	Chara_Player_Damage, Chara_Player_Landed, Chara_Player_AnimOtherEvent,
	NULL },

	// EChara_Goblin
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                NULL,
	Chara_Enemy_MessageRecv },

	// EChara_Bee
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                Chara_Bee_AnimOtherEvent,
	Chara_Enemy_MessageRecv },

	// EChara_Golem
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                Chara_Golem_AnimOtherEvent,
	Chara_Enemy_MessageRecv },

	// EChara_RedGoblin
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                NULL,
	Chara_Enemy_MessageRecv },

	// EChara_Needle
	{ Chara_Needle_Create, NULL,                NULL,
	NULL,                NULL,                NULL,
	NULL },

};

// キャラの状態推移処理を実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Step(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// キャラの攻撃処理を推移させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// キャラ同士の当たり判定処理を行う
static void Chara_Collision(
	// 当たり判定を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// CInfo が移動しようとしているベクトル
	VECTOR *MoveVector,

	// CInfo が MoveVector 移動する場合に当たるかどうかをチェックする
	// 対象のキャラの情報構造体のアドレス
	SCharaInfo *CheckCInfo
);

// ステージモデルとの当たり判定をしながらキャラを移動させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Move(
	// 移動させるキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 移動ベクトル
	VECTOR MoveVector
);

// キャラの移動処理を推移させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_MoveStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// キャラの描画を行う
static void Chara_Render(
	// 描画するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの丸影を描画する
static void Chara_ShadowRender(
	// 丸影を描画するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの２Ｄ描画を行う
static void Chara_2D_Render(
	// ２Ｄ描画を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの落下処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Fall(
	// 落下処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの着地処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Landed(
	// 着地処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの吹っ飛び処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Blow(
	// 吹っ飛び処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの倒れ処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Down(
	// 倒れ処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラのフェードアウト処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_FadeOut(
	// フェードアウト処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラの向き処理を初期化する
static void Chara_InitializeAngle(
	// 向き処理を初期化するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 初期の向き
	float Angle
);

// キャラの向き処理の状態推移を行う
static void Chara_AngleStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// キャラのアニメーション処理を初期化する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_InitializeAnim(
	// アニメーション処理を初期化するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラのアニメーション処理の状態推移を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AnimStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 「攻撃に当たったキャラやステージの情報」をリセットする
static void Chara_AttackHitInfo_Reset(
	// リセットする攻撃に当たったキャラの情報構造体のアドレス
	SCharaAttackHitInfo *AHInfo
);

// 「攻撃に当たったキャラやステージの情報」に新たに攻撃に当たったキャラを追加する
//     戻り値 : 追加しようとしたキャラが既に追加されていたかどうか
//              ( false : 既に追加されていた   true : 新たに追加した )
static bool Chara_AttackHitInfo_AddChara(
	// 情報を追加する「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 情報に追加するキャラの情報構造体のアドレス
	SCharaInfo *NewHitCInfo
);

// 「攻撃に当たったキャラやステージの情報」のステージと当たった場合の処理を行う
static void Chara_AttackHitInfo_ProcessStageHit(
	// 処理を行う「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 攻撃が当たった箇所のマテリアルタイプ
	EMaterialType MaterialType,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃に使用された武器
	ECharaWeapon Weapon
);

// 「攻撃に当たったキャラやステージの情報」のキャラと当たった場合の処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackHitInfo_ProcessCharaHit(
	// 処理を行う「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 攻撃が当たったキャラの情報構造体のアドレス
	SCharaInfo *HitCInfo,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃の方向
	VECTOR AttackDirection,

	// 攻撃のダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 攻撃に使用された武器
	ECharaWeapon Weapon,

	// 攻撃によって受けるダメージ
	int DamagePoint
);

// キャラの攻撃情報をリセットする
static void Chara_AttackInfo_Reset(
	// リセットするキャラの攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo
);

// キャラの攻撃情報に攻撃判定用の座標を追加する
static void Chara_AttackInfo_AddPosition(
	// 座標を追加する攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo,

	// 新たに追加する起点となるフレームに近い頂点の座標
	VECTOR NewNearPos,

	// 新たに追加する起点となるフレームから遠い頂点の座標
	VECTOR NewFarPos
);

// キャラの攻撃情報による攻撃判定を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackInfo_Check(
	// 判定に使用するキャラの攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo,

	// 当たり判定の形状
	ECharaAttackFormType FormType,

	// 攻撃を行っているキャラの情報構造体のアドレス
	SCharaInfo *AttackCInfo,

	// 攻撃対象のキャラタイプ
	ECharaType TargetType,

	// 攻撃が当たった場合のダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 攻撃に使用される武器
	ECharaWeapon Weapon,

	// 攻撃が当たった場合のダメージ
	int DamagePoint,

	// 当たり判定の形状が ECharaAttackFormType_Sphere だった場合の球の大きさ
	float SphereSize
);


// キャラの処理の初期化をする
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Chara_Initialize(void)
{
	int         i;
	SCharaInfo *CInfo;

	// キャラの基本情報の初期化
	if (!CharaBase_Initialize())
	{
		return false;
	}

	// 全てのキャラ情報構造体の『使用中かどうか』のフラグを倒す
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		CInfo->UseFlag = false;
	}

	// 丸影描画用の画像を読み込む
	g_CharaSystemInfo.ShadowHandle = LoadGraph(CHARADATA_DIR "Shadow.png");
	if (g_CharaSystemInfo.ShadowHandle == -1)
	{
		return false;
	}

	// 成功終了
	return true;
}

// キャラの処理の後始末をする
void Chara_Terminate(void)
{
	// 全てのキャラを削除する
	Chara_AllDelete();

	// キャラの基本情報の後始末
	CharaBase_Terminate();
}

// 新しいキャラを作成する
//     戻り値 : キャラの作成に成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Chara_Create(
	// 出現させるキャラ
	EChara Chara,

	// 出現させるキャラの初期位置
	VECTOR Position,

	// 出現させるキャラの初期向き
	float Angle,

	// 出現させるキャラの識別用番号
	int CharaNo
)
{
	int                   i;
	int                   j;
	SCharaInfo           *CInfo;
	const SCharaBaseInfo *CBInfo;

	// 作成するキャラの基本情報を取得する
	CBInfo = CharaBase_GetInfo(Chara);

	// 使用されていないキャラ情報構造体を探す
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (!CInfo->UseFlag)
		{
			break;
		}
	}

	// 全ての構造体が使用されてしまっていたらエラー
	if (i == CHARA_MAXNUM)
	{
		return false;
	}

	// 構造体を『使用中』にする
	CInfo->UseFlag = true;

	// 作成するキャラが使用する３Ｄモデルハンドルを作成する
	CInfo->ModelHandle = MV1DuplicateModel(CBInfo->ModelHandle);
	if (CInfo->ModelHandle == -1)
	{
		return false;
	}

	// ルートフレームの座標変換は無効にしておく
	MV1SetFrameUserLocalMatrix(CInfo->ModelHandle, CBInfo->RootFrameIndex, MGetIdent());

	// ３Ｄモデルの位置をセット
	MV1SetPosition(CInfo->ModelHandle, Position);

	// 情報を初期化
	CInfo->Chara = Chara;
	CInfo->CharaNo = CharaNo;
	CInfo->BaseInfo = CBInfo;
	CInfo->State = ECharaState_Move;
	CInfo->Position = Position;
	CInfo->OnMaterialType = EMaterialType_Grass;
	CInfo->DamageDirection = VGet(0.0f, 0.0f, 0.0f);
	CInfo->OpacityRate = 1.0f;
	CInfo->JumpState = false;
	CInfo->ProgramMove = false;
	CInfo->Spd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevAnimSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevFixSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->Health = CBInfo->Health;

	// 向き処理の初期化
	Chara_InitializeAngle(CInfo, Angle);

	// アニメーション処理の初期化
	if (!Chara_InitializeAnim(CInfo))
	{
		return false;
	}

	// 体力ゲージの表示設定を初期化
	CInfo->HealthGaugeVisible = false;
	CInfo->HealthGaugeHideDelay = 0.0f;
	CharaHealthGaugeSetup(&CInfo->HealthGauge, false, 1.0f);

	// 攻撃情報を初期化
	for (j = 0; j < CHARA_ATTACK_MAX_NUM; j++)
	{
		Chara_AttackInfo_Reset(&CInfo->AttackInfo[j]);
	}

	// キャラ別の情報へのポインタを初期化
	CInfo->SubData = NULL;

	// キャラ別の作成時に呼ぶ関数の呼び出し
	if (!g_CharaFunctionTable[Chara].Create(CInfo))
	{
		return false;
	}

	// 向きパラメータの更新
	Chara_AngleParamRefresh(CInfo);

	// 成功終了
	return true;
}

// 指定のキャラを削除する
void Chara_Delete(
	// 削除するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	int i;

	// 既に構造体が使用されていなかったら何もせずに終了
	if (!CInfo->UseFlag)
	{
		return;
	}

	// 削除時に実行する関数が存在する場合は実行する
	if (g_CharaFunctionTable[CInfo->Chara].Delete != NULL)
	{
		g_CharaFunctionTable[CInfo->Chara].Delete(CInfo);
	}

	// キャラ別の情報があった場合はメモリを解放する
	if (CInfo->SubData != NULL)
	{
		free(CInfo->SubData);
		CInfo->SubData = NULL;
	}

	// 攻撃情報を初期化
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++)
	{
		if (!CInfo->AttackInfo[i].Enable)
		{
			continue;
		}

		Chara_AttackInfo_Reset(&CInfo->AttackInfo[i]);
	}

	// ３Ｄモデルハンドルを削除する
	MV1DeleteModel(CInfo->ModelHandle);
	CInfo->ModelHandle = -1;

	// 構造体を使用しているかどうかのフラグを倒す
	CInfo->UseFlag = false;
}

// 全てのキャラを削除する
void Chara_AllDelete(void)
{
	int         i;
	SCharaInfo *CInfo;

	// 全ての構造体に対して削除処理を行う
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		Chara_Delete(CInfo);
	}
}

// キャラの描画を行う
static void Chara_Render(
	// 描画するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	int    i;
	MATRIX FrameMat;
	VECTOR CapsulePos1;
	VECTOR CapsulePos2;
	int    WeaponModelHandle;
	int    SubModelHandle;

	// 透明度がある場合はＺバッファへの書き込みを行わない
	if (CInfo->OpacityRate < 1.0f)
	{
		MV1SetWriteZBuffer(CInfo->ModelHandle, FALSE);
	}

	// ３Ｄモデルの描画透明度を設定
	MV1SetOpacityRate(CInfo->ModelHandle, CInfo->OpacityRate);

	// ３Ｄモデルの描画
	MV1DrawModel(CInfo->ModelHandle);

	// 武器を装備している場合は武器３Ｄモデルを描画する
	if (CInfo->BaseInfo->EquipWeapon != ECharaWeapon_Num &&
		CInfo->BaseInfo->EquipWeaponAttachFrameIndex != -1)
	{
		// 武器３Ｄモデルハンドルを取得
		WeaponModelHandle =
			CharaBase_GetWeaponInfo(CInfo->BaseInfo->EquipWeapon)->ModelHandle;

		// 武器をアタッチするフレームのローカル→ワールド変換行列を取得する
		FrameMat = MV1GetFrameLocalWorldMatrix(
			CInfo->ModelHandle, CInfo->BaseInfo->EquipWeaponAttachFrameIndex);

		// 透明度がある場合はＺバッファへの書き込みを行わない
		if (CInfo->OpacityRate < 1.0f)
		{
			MV1SetWriteZBuffer(WeaponModelHandle, FALSE);
		}

		// ３Ｄモデルの描画透明度を設定
		MV1SetOpacityRate(WeaponModelHandle, CInfo->OpacityRate);

		// 武器３Ｄモデルに取得した変換行列をセットする
		MV1SetMatrix(WeaponModelHandle, FrameMat);

		// 武器３Ｄモデルを描画
		MV1DrawModel(WeaponModelHandle);

		// Ｚバッファへの書き込み設定や透明度を元に戻す
		MV1SetWriteZBuffer(WeaponModelHandle, TRUE);
		MV1SetOpacityRate(WeaponModelHandle, 1.0f);
	}

	// サブ３Ｄモデルがある場合はサブ３Ｄモデルを描画する
	for (i = 0; i < CInfo->BaseInfo->SubModelNum; i++)
	{
		// サブ３Ｄモデルハンドルを取得
		SubModelHandle = CInfo->BaseInfo->SubModel[i].ModelHandle;

		// サブ３Ｄモデルをアタッチするフレームのローカル→ワールド変換行列を取得する
		FrameMat = MV1GetFrameLocalWorldMatrix(
			CInfo->ModelHandle, CInfo->BaseInfo->SubModel[i].AttachFrameIndex);

		// 透明度がある場合はＺバッファへの書き込みを行わない
		if (CInfo->OpacityRate < 1.0f)
		{
			MV1SetWriteZBuffer(SubModelHandle, FALSE);
		}

		// ３Ｄモデルの描画透明度を設定
		MV1SetOpacityRate(SubModelHandle, CInfo->OpacityRate);

		// サブ３Ｄモデルハンドルに取得した変換行列をセットする
		MV1SetMatrix(SubModelHandle, FrameMat);

		// サブ３Ｄモデルを描画
		MV1DrawModel(SubModelHandle);

		// Ｚバッファへの書き込み設定や透明度を元に戻す
		MV1SetWriteZBuffer(SubModelHandle, TRUE);
		MV1SetOpacityRate(SubModelHandle, 1.0f);
	}

	// デバッグ情報を描画するかどうかをチェック
	if (System_GetDispDebugInfo())
	{
		// デバッグ情報描画

		// Ｚバッファを使用、Ｚバッファへの書き込みあり、ライティング無しに設定
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
		SetUseLighting(FALSE);

		// ダメージ判定のカプセル形状を描画
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->DamageHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->DamageHitInfo.Width,
			10, GetColor(255, 255, 0), GetColor(0, 0, 0), FALSE);

		// キャラクター同士の当たり判定のカプセル形状を描画
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->CharaHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->CharaHitInfo.Width,
			10, GetColor(0, 255, 255), GetColor(0, 0, 0), FALSE);

		// 二つあるステージとの当たり判定の上側のカプセル形状を描画
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);

		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->StageHitTopInfo.Width,
			10, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);

		// 二つあるステージとの当たり判定の下側のカプセル形状を描画
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->StageHitBottomInfo.Width,
			10, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);

		// Ｚバッファを使用しない、Ｚバッファへの書き込みもしない、ライティングありに設定
		SetUseLighting(TRUE);
		SetWriteZBufferFlag(FALSE);
		SetUseZBufferFlag(FALSE);
	}
}

// キャラの丸影を描画する
static void Chara_ShadowRender(
	// 丸影を描画するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	int                      i;
	int                      j;
	int                      k;
	int                      StageObjectNum;
	int                      CollisionModelHandle;
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];
	int                      HitNum;
	MV1_COLL_RESULT_POLY    *HitRes;
	VERTEX3D                 Vertex[3 * CHARA_SHADOW_POLYGON_NUM];
	VERTEX3D                *VertP;
	VECTOR                   SlideVec;
	int                      PolyNum;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする
	// ( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// ステージオブジェクトの数を取得する
	StageObjectNum = StageData_GetObjectNum();

	// 当たったオブジェクトの数を初期化
	HitNum = 0;

	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitDim[HitNum] = MV1CollCheck_Capsule(
		StageData_GetCollisionModelHandle(), -1, CInfo->Position,
		VAdd(CInfo->Position, VGet(0.0f, -CHARA_SHADOW_HEIGHT, 0.0f)),
		CInfo->BaseInfo->ShadowSize
	);
	if (HitDim[HitNum].HitNum != 0)
	{
		HitNum++;
	}
	else
	{
		MV1CollResultPolyDimTerminate(HitDim[HitNum]);
	}

	// キャラクターの周囲にあるコリジョンオブジェクトのポリゴンも取得する
	for (i = 0; i < StageObjectNum; i++)
	{
		CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

		if (CollisionModelHandle != -1)
		{
			HitDim[HitNum] = MV1CollCheck_Capsule(
				CollisionModelHandle, -1, CInfo->Position,
				VAdd(CInfo->Position, VGet(0.0f, -CHARA_SHADOW_HEIGHT, 0.0f)),
				CInfo->BaseInfo->ShadowSize
			);
			if (HitDim[HitNum].HitNum != 0)
			{
				HitNum++;
			}
			else
			{
				MV1CollResultPolyDimTerminate(HitDim[HitNum]);
			}
		}
	}

	// 頂点データで変化が無い部分をセット
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	for (i = 1; i < CHARA_SHADOW_POLYGON_NUM * 3; i++)
	{
		Vertex[i] = Vertex[0];
	}

	// 描画するポリゴンの数と頂点データのアドレスを初期化
	PolyNum = 0;
	VertP = Vertex;

	// 当たり判定結果情報の数だけ繰り返し
	for (k = 0; k < HitNum; k++)
	{
		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitDim[k].Dim;
		for (i = 0; i < HitDim[k].HitNum; i++, HitRes++)
		{
			// ポリゴンが重ならないようにする為にずらすベクトルを算出
			SlideVec = VScale(HitRes->Normal, 0.5f);

			// ポリゴンの頂点の数( 3つ )だけ繰り返す
			for (j = 0; j < 3; j++)
			{
				// ポリゴンの座標は地面ポリゴンの座標
				VertP->pos = HitRes->Position[j];

				// ちょっと持ち上げて重ならないようにする
				VertP->pos = VAdd(VertP->pos, SlideVec);

				// ポリゴンの不透明度を設定する
				if (HitRes->Position[j].y > CInfo->Position.y - CHARA_SHADOW_HEIGHT)
				{
					VertP->dif.a = (BYTE)(200 * (1.0f - fabs(HitRes->Position[j].y -
						CInfo->Position.y) / CHARA_SHADOW_HEIGHT));
				}
				else
				{
					VertP->dif.a = 0;
				}

				// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
				VertP->u = (HitRes->Position[j].x - CInfo->Position.x) /
					(CInfo->BaseInfo->ShadowSize * 2.0f) + 0.5f;

				VertP->v = (HitRes->Position[j].z - CInfo->Position.z) /
					(CInfo->BaseInfo->ShadowSize * 2.0f) + 0.5f;

				// 頂点のアドレスをインクリメント
				VertP++;
			}

			// ポリゴンの数を増やす
			PolyNum++;

			// もしポリゴンの最大数になっていたらポリゴンを描画
			if (PolyNum == CHARA_SHADOW_POLYGON_NUM)
			{
				// 影ポリゴンを描画
				DrawPolygon3D(Vertex, PolyNum, g_CharaSystemInfo.ShadowHandle, TRUE);

				// カウンタとポインタをリセットする
				PolyNum = 0;
				VertP = Vertex;
			}
		}
	}

	// もし描画するポリゴンが残っていたら描画する
	if (PolyNum > 0)
	{
		// 影ポリゴンを描画
		DrawPolygon3D(Vertex, PolyNum, g_CharaSystemInfo.ShadowHandle, TRUE);
	}

	// 検出した地面ポリゴン情報の後始末
	for (k = 0; k < HitNum; k++)
	{
		MV1CollResultPolyDimTerminate(HitDim[k]);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

// キャラの２Ｄ描画を行う
static void Chara_2D_Render(
	// ２Ｄ描画を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	VECTOR ScreenPosition;
	int    DrawX;
	int    DrawY;

	// プレイヤーの場合は何もせず終了
	if (CInfo->BaseInfo->Type == ECharaType_Player)
	{
		return;
	}

	// 体力ゲージを描画する座標を取得
	ScreenPosition = ConvWorldPosToScreenPos(
		VAdd(CInfo->Position, CInfo->BaseInfo->HealthGaugePosition));

	// 画面の中に映っていなかったら何もせずに終了
	if (ScreenPosition.z < 0.0f || ScreenPosition.z > 1.0f)
	{
		return;
	}

	// 体力ゲージの描画
	DrawX = (int)(ScreenPosition.x - HP_GAUGE_WIDTH / 2);
	DrawY = (int)(ScreenPosition.y - HP_GAUGE_HEIGHT / 2);
	CharaHealthGaugeDraw(
		&CInfo->HealthGauge, DrawX, DrawY, HP_GAUGE_WIDTH, HP_GAUGE_HEIGHT);
}

// 全てのキャラの状態推移処理を実行する
bool Chara_StepAll(
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	int         i;
	SCharaInfo *CInfo;

	// 全ての有効なキャラに対して状態推移処理を行う
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			if (!Chara_Step(CInfo, StepTime))
			{
				return false;
			}
		}
	}

	// 全ての有効なキャラに対して座標移動処理を行う
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			if (!Chara_MoveStep(CInfo, StepTime))
			{
				return false;
			}
		}
	}

	// 正常終了
	return true;
}

// 全てのキャラの描画処理を行う
void Chara_RenderAll(
	// 描画モード
	ECharaRenderMode RenderMode,

	// 描画モードが ECharaRenderMode_Always 以外の場合に使用される比較用距離
	float CompareDistance
)
{
	int         i;
	SCharaInfo *CInfo;
	float       CompareDistanceSquare;
	SCharaInfo *PCInfo;
	VECTOR      PlayerPosition;

	// プレイヤーの位置を取得する
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo == NULL)
	{
		// プレイヤーが居ない場合は描画モードを『常に描画する』に設定
		RenderMode = ECharaRenderMode_Always;
	}
	else
	{
		PlayerPosition = PCInfo->Position;
	}

	// 比較用距離を二乗しておく
	CompareDistanceSquare = CompareDistance * CompareDistance;

	// 描画モードによって処理を分岐
	switch (RenderMode)
	{
	case ECharaRenderMode_Always:			// 常に描画する場合
											// 有効なキャラを無条件で描画する
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Less:	// 指定の距離以内であれば描画する場合
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// プレイヤーとの距離が指定の距離以上の場合は描画しない
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) >
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Greater:	// 指定の距離以上であれば描画する場合
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// プレイヤーとの距離が指定の距離以内の場合は描画しない
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) <=
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;
	}
}

// 全てのキャラの丸影の描画処理を行う
void Chara_ShadowRenderAll(
	// 描画モード
	ECharaRenderMode RenderMode,

	// 描画モードが ECharaRenderMode_Always 以外の場合に使用される比較用距離
	float CompareDistance
)
{
	int         i;
	SCharaInfo *CInfo;
	float       CompareDistanceSquare;
	SCharaInfo *PCInfo;
	VECTOR      PlayerPosition;

	// プレイヤーの位置を取得する
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo == NULL)
	{
		// プレイヤーが居ない場合は描画モードを『常に描画する』に設定
		RenderMode = ECharaRenderMode_Always;
	}
	else
	{
		PlayerPosition = PCInfo->Position;
	}

	// 比較用距離を二乗しておく
	CompareDistanceSquare = CompareDistance * CompareDistance;

	// 描画モードによって処理を分岐
	switch (RenderMode)
	{
	case ECharaRenderMode_Always:			// 常に描画する場合
											// 有効な全てのキャラの丸影を描画する
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (CInfo->UseFlag)
			{
				Chara_ShadowRender(CInfo);
			}
		}
		break;


	case ECharaRenderMode_Distance_Less:	// 指定の距離以内であれば描画する場合
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// プレイヤーとの距離が指定の距離以上の場合は描画しない
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) >
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_ShadowRender(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Greater:	// 指定の距離以上であれば描画する場合
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// プレイヤーとの距離が指定の距離以内の場合は描画しない
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) <=
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_ShadowRender(CInfo);
		}
		break;
	}
}

// 全てのキャラの２Ｄ描画処理を行う
void Chara_2D_RenderAll(void)
{
	int         i;
	SCharaInfo *CInfo;

	// 有効な全てのキャラの２Ｄ描画処理を行う
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			Chara_2D_Render(CInfo);
		}
	}
}

// 指定のタイプのキャラを倒す
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_TagetTypeKill(
	// 倒したいキャラタイプ
	ECharaType Type
)
{
	int         i;
	SCharaInfo *CInfo;

	// 全てのキャラに対して処理を実行
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		// 構造体が有効では無かったら何もせずに次のループへ
		if (!CInfo->UseFlag)
		{
			continue;
		}

		// 死亡処理が開始していたら何もせずに次のループへ
		if (CInfo->BaseInfo->Type != Type ||
			CInfo->State == ECharaState_Blow ||
			CInfo->State == ECharaState_Down ||
			CInfo->State == ECharaState_FadeOut)
		{
			continue;
		}

		// 体力を０にする
		CInfo->Health = 0;

		// 仮にダメージを受けた方向をセット
		CInfo->DamageDirection = VScale(CInfo->AngleInfo.FrontDirection, -1.0f);

		// 吹っ飛ばされ状態にする
		if (!Chara_Blow(CInfo))
		{
			return false;
		}

		// 体力ゲージを非表示にする
		CInfo->HealthGaugeHideDelay = 0.0f;
	}

	// 正常終了
	return true;
}

// 指定キャラの周囲に居る指定タイプのキャラを探す
//     戻り値 : 検索で見つかったキャラの情報構造体のアドレス
SCharaInfo * Chara_SearchTarget(
	// 検索基準となるキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 探すキャラのタイプ
	ECharaType SearchCharaType,

	// 探す距離( CInfo の居る位置からどれだけの距離の範囲を検索するか )
	float SearchDistance,

	// 探す角度範囲( CInfo の向いている方向からどの範囲を検索するか )
	float SearchAngle
)
{
	int         i;
	SCharaInfo *TCInfo;
	float       Distance;
	VECTOR      TargetVector;
	VECTOR      Direction;
	float       Angle;
	float       MinDistance;
	SCharaInfo *MinCInfo;

	// 見つかったキャラの情報を初期化
	MinCInfo = NULL;
	MinDistance = 0.0f;

	// キャラ情報構造体の数だけループ
	TCInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, TCInfo++)
	{
		// 検索対象の構造体が使用されていなかったら次のループへ
		if (!TCInfo->UseFlag)
		{
			continue;
		}

		// 検索対象が検索基準のキャラだった場合は次のループへ
		if (TCInfo == CInfo)
		{
			continue;
		}

		// 検索対象のキャラタイプではなかったら次のループへ
		if (TCInfo->BaseInfo->Type != SearchCharaType)
		{
			continue;
		}

		// 既に死んでいたら次のループへ
		if (TCInfo->Health <= 0)
		{
			continue;
		}

		// 検索対象への距離や角度を算出
		TargetVector = VSub(TCInfo->Position, CInfo->Position);
		TargetVector.y = 0.0f;
		Distance = VSize(TargetVector);
		Direction = VScale(TargetVector, 1.0f / Distance);
		Angle = acos(VDot(Direction, CInfo->AngleInfo.FrontDirection));

		// 検索対象への距離や角度が範囲外の場合は次のループへ
		if (SearchAngle < Angle || SearchDistance < Distance)
		{
			continue;
		}

		// 今までに条件に合うキャラが居なかったか、
		// 若しくは今まで条件に合ったキャラより距離が近い場合は保存
		if (MinCInfo == NULL || MinDistance > Distance)
		{
			MinCInfo = TCInfo;
			MinDistance = Distance;
		}
	}

	// 検索結果を返す
	return MinCInfo;
}

// 指定座標の周囲に居るキャラクターにメッセージを送る
void Chara_SendMessage(
	// メッセージを送るキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// メッセージを送る基準となる座標
	VECTOR BasePosition,

	// メッセージを送る範囲
	float SendDistance,

	// メッセージの識別番号
	int Message
)
{
	int         i;
	SCharaInfo *TCInfo;
	float       Distance;
	VECTOR      TargetVector;

	// キャラ情報構造体の数だけループ
	TCInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, TCInfo++)
	{
		// 検索対象の構造体が使用されていなかったら次のループへ
		if (!TCInfo->UseFlag)
		{
			continue;
		}

		// 検索対象が検索基準のキャラだった場合は次のループへ
		if (TCInfo == CInfo)
		{
			continue;
		}

		// 既に死んでいたら次のループへ
		if (TCInfo->Health <= 0)
		{
			continue;
		}

		// 検索対象への距離を算出
		TargetVector = VSub(TCInfo->Position, BasePosition);
		TargetVector.y = 0.0f;
		Distance = VSize(TargetVector);

		// 検索対象への距離や角度が範囲外の場合は次のループへ
		if (SendDistance < Distance)
		{
			continue;
		}

		// メッセージを受け取った際に呼び出す関数がある場合は呼び出す
		if (g_CharaFunctionTable[TCInfo->Chara].MessageRecv != NULL)
		{
			g_CharaFunctionTable[TCInfo->Chara].MessageRecv(TCInfo, CInfo, Message);
		}
	}
}

// キャラの状態推移処理を実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Step(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	VECTOR BlowMoveVec;
	bool   DefaultProcess;

	// キャラの向きの状態推移処理を行う
	Chara_AngleStep(CInfo, StepTime);

	// キャラにアニメーションの状態推移処理を行う
	if (!Chara_AnimStep(CInfo, StepTime))
	{
		return false;
	}

	// キャラ別の状態推移処理用関数がある場合は実行する
	if (g_CharaFunctionTable[CInfo->Chara].Step != NULL)
	{
		if (!g_CharaFunctionTable[CInfo->Chara].Step(CInfo, StepTime, &DefaultProcess))
		{
			return false;
		}
	}
	else
	{
		DefaultProcess = true;
	}

	// デフォルトの処理を行うかどうかで分岐
	if (DefaultProcess)
	{
		// キャラの状態に応じて処理を分岐
		switch (CInfo->State)
		{
		case ECharaState_Damage:			// ダメージ中
											// ダメージアニメーションが終了していなかったら何もせず終了
			if (!CInfo->AnimInfo.End)
			{
				break;
			}

			// 通常移動中に移行
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}
			CInfo->State = ECharaState_Move;
			break;

		case ECharaState_Blow:				// 吹っ飛び中
											// 吹っ飛び状態によって処理を分岐
			switch (CInfo->BlowState)
			{
			case ECharaBlowState_In:		// 吹っ飛び状態移行アニメーション中
											// アニメーションが終了していなかったら何もせず終了
				if (!CInfo->AnimInfo.End)
				{
					break;
				}

				// キャラクターをジャンプ状態にする
				CInfo->Position.y += CHARA_DEFAULT_BLOWHEIGHT;
				CInfo->JumpState = true;

				// 吹っ飛びベクトルを算出
				BlowMoveVec = VScale(CInfo->DamageDirection, CHARA_DEFAULT_BLOWSPEED);
				CInfo->Spd.x = BlowMoveVec.x;
				CInfo->Spd.z = BlowMoveVec.z;

				// 接地待ち中に移行する
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_Loop, -1.0f))
				{
					return false;
				}
				CInfo->BlowState = ECharaBlowState_WaitLanding;
				break;

			case ECharaBlowState_Out:		// 接地してからのアニメーション中
											// アニメーションが終了していなかったら何もせず終了
				if (!CInfo->AnimInfo.End)
				{
					break;
				}

				// フェードアウト状態に移行
				if (!Chara_FadeOut(CInfo))
				{
					return false;
				}
				break;
			}
			break;

		case ECharaState_Down:				// 倒れ中
											// 倒れアニメーションが終了していなかったら何もせずに終了
			if (!CInfo->AnimInfo.End)
			{
				break;
			}

			// フェードアウト状態に移行
			if (!Chara_FadeOut(CInfo))
			{
				return false;
			}
			break;

		case ECharaState_FadeOut:			// フェードアウト中
											// 不透明度を下げる
			CInfo->OpacityRate -= CHARA_DOWN_FADEOUT_SPEED * StepTime;

			// 完全に透明になったらキャラを削除する
			if (CInfo->OpacityRate < 0.0f)
			{
				CInfo->OpacityRate = 0.0f;
				Chara_Delete(CInfo);
				return true;
			}
			break;
		}
	}

	// 攻撃情報の状態推移処理を行う
	if (!Chara_AttackStep(CInfo, StepTime))
	{
		return false;
	}

	// 体力ゲージ表示フラグが倒れている場合は『非表示にするのを待つ時間』を減らす
	if (!CInfo->HealthGaugeVisible)
	{
		ParamChangeFloat(&CInfo->HealthGaugeHideDelay, 0.0f, StepTime, 1.0f);
	}

	// 体力ゲージの状態推移処理を行う
	CharaHealthGaugeStep(
		&CInfo->HealthGauge,
		StepTime,
		CInfo->HealthGaugeVisible || CInfo->HealthGaugeHideDelay > 0.0f,
		(float)CInfo->Health / CInfo->BaseInfo->Health
	);

	// 正常終了
	return true;
}

// キャラの攻撃処理を推移させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaAttackInfo               *AInfo;
	const SCharaAttackPositionInfo *AFInfo;
	int                             i;
	VECTOR                          NewNearPos;
	VECTOR                          NewFarPos;
	MATRIX                          AttackPosMatrix;

	// 攻撃情報構造体の数だけループ
	AInfo = CInfo->AttackInfo;
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++, AInfo++)
	{
		// 構造体が使用されていなかったら何もせずに次のループへ
		if (!AInfo->Enable)
		{
			continue;
		}

		// 攻撃位置情報の取得
		AFInfo = &CInfo->BaseInfo->AttackPosInfo[AInfo->AttackPosIndex];

		// 攻撃判定の起点となる３Ｄモデル中のフレームのローカル→ワールド変換行列を取得
		AttackPosMatrix =
			MV1GetFrameLocalWorldMatrix(CInfo->ModelHandle, AFInfo->StartFrameIndex);

		// 攻撃判定で使用する２点を算出
		NewNearPos = VTransform(VGet(0.0f, 0.0f, 0.0f), AttackPosMatrix);
		NewFarPos = VTransform(AFInfo->EndLocalPosition, AttackPosMatrix);

		// 攻撃情報に新しい２点を追加する
		Chara_AttackInfo_AddPosition(AInfo, NewNearPos, NewFarPos);

		// 攻撃判定の実行
		if (!Chara_AttackInfo_Check(
			AInfo, AFInfo->FormType, CInfo,
			CInfo->BaseInfo->Type == ECharaType_Player ?
			ECharaType_Enemy : ECharaType_Player,
			ECharaAttack_DamageType_Cut,
			CInfo->BaseInfo->EquipWeapon,
			CInfo->Atk,
			AFInfo->SphereSize
		))
		{
			return false;
		}

		// 軌跡エフェクトを発生させていなかったら発生させる
		if (AInfo->LocusEffect == NULL)
		{
			// 攻撃判定の形状によって処理を分岐
			switch (AFInfo->FormType)
			{
			case ECharaAttackFormType_Poly:		// ポリゴン
												// ポリゴン軌跡エフェクトを作成
				AInfo->LocusEffect = Effect_Create(EEffect_SlashLocus);
				if (AInfo->LocusEffect == NULL)
				{
					return false;
				}
				Effect_SlashLocus_Setup(AInfo->LocusEffect, AFInfo->EffectColor);
				break;

			case ECharaAttackFormType_Sphere:	// 球
												// 球軌跡エフェクトを作成
				AInfo->LocusEffect = Effect_Create(EEffect_SphereLocus);
				if (AInfo->LocusEffect == NULL)
				{
					return false;
				}
				Effect_SphereLocus_Setup(
					AInfo->LocusEffect, AFInfo->EffectColor, AFInfo->SphereSize);
				break;
			}
		}

		// 攻撃判定の形状に応じた軌跡用座標の追加を行う
		switch (AFInfo->FormType)
		{
		case ECharaAttackFormType_Poly:
			Effect_SlashLocus_AddPosition(AInfo->LocusEffect, NewNearPos, NewFarPos);
			break;

		case ECharaAttackFormType_Sphere:
			Effect_SphereLocus_AddPosition(AInfo->LocusEffect, NewNearPos, NewFarPos);
			break;
		}
	}

	// 無効になった攻撃情報の後始末を行う
	AInfo = CInfo->AttackInfo;
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++, AInfo++)
	{
		// 構造体が有効だった場合は何もせず次のループへ
		if (AInfo->Enable)
		{
			continue;
		}

		// 攻撃情報をリセットする
		Chara_AttackInfo_Reset(AInfo);
	}

	// 正常終了
	return true;
}

// キャラ同士の当たり判定処理を行う
static void Chara_Collision(
	// 当たり判定を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// CInfo が移動しようとしているベクトル
	VECTOR *MoveVector,

	// CInfo が MoveVector 移動する場合に当たるかどうかをチェックする
	// 対象のキャラの情報構造体のアドレス
	SCharaInfo *CheckCInfo
)
{
	VECTOR ChkChToChVec;
	VECTOR PushVec;
	VECTOR ChPosition;
	float  Distance;
	float  TotalWidth;
	float  TempY;
	VECTOR CapsulePos1;
	VECTOR CapsulePos2;
	VECTOR CheckCapsulePos1;
	VECTOR CheckCapsulePos2;

	// キャラ同士の幅の合計を算出
	TotalWidth =
		CInfo->BaseInfo->CharaHitInfo.Width + CheckCInfo->BaseInfo->CharaHitInfo.Width;

	// 移動後のキャラ CInfo の座標を算出
	ChPosition = VAdd(CInfo->Position, *MoveVector);

	// 各キャラの当たり判定用のカプセル座標を算出
	CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->CharaHitInfo,
		&ChPosition, &CapsulePos1, &CapsulePos2);
	CharaBase_GetHitCapsulePos(&CheckCInfo->BaseInfo->CharaHitInfo,
		&CheckCInfo->Position, &CheckCapsulePos1, &CheckCapsulePos2);

	// 当たっているか判定
	if (HitCheck_Capsule_Capsule(
		CapsulePos1, CapsulePos2,
		CInfo->BaseInfo->CharaHitInfo.Width,
		CheckCapsulePos1, CheckCapsulePos2,
		CheckCInfo->BaseInfo->CharaHitInfo.Width) == TRUE)
	{
		// 当たっていたら CInfo が CheckCInfo から離れる処理をする

		// CheckCInfo から CInfo へのベクトルを算出
		ChkChToChVec = VSub(ChPosition, CheckCInfo->Position);

		// Ｙ軸は見ない
		ChkChToChVec.y = 0.0f;

		// 二人の距離を算出
		Distance = VSize(ChkChToChVec);

		// CheckCInfo から CInfo へのベクトルを正規化
		PushVec = VScale(ChkChToChVec, 1.0f / Distance);

		// 押し出す距離を算出、もし二人の距離から二人の大きさを引いた値に押し出し力を
		// 足して離れてしまう場合は、ぴったりくっつく距離に移動する
		if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f)
		{
			TempY = ChPosition.y;
			ChPosition = VAdd(CheckCInfo->Position, VScale(PushVec, TotalWidth));

			// Ｙ座標は変化させない
			ChPosition.y = TempY;
		}
		else
		{
			// 押し出し
			ChPosition = VAdd(ChPosition, VScale(PushVec, CHARA_HIT_PUSH_POWER));
		}
	}

	// 当たり判定処理後の移動ベクトルをセット
	*MoveVector = VSub(ChPosition, CInfo->Position);
}

// ステージモデルとの当たり判定をしながらキャラを移動させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Move(
	// 移動させるキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 移動ベクトル
	VECTOR MoveVector
)
{
	int                      i;
	int                      j;
	int                      k;

	// 水平方向に移動したかどうかのフラグ
	// ( false : 移動していない  true:移動した )
	bool                     MoveFlag;

	// ポリゴンに当たったかどうかを記憶しておく変数
	// ( false : 当たっていない  true : 当たった )
	int                      HitFlag;

	// キャラクターの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];

	// 当たったオブジェクトの数
	int                      HitNum;

	// 当たったポリゴンのタイプ別の数
	int                      HitPolyNum[EStageCollType_Num];

	// 当たったタイプ別の各ポリゴンへのポインタ
	MV1_COLL_RESULT_POLY    *HitPoly[EStageCollType_Num][CHARA_MAX_HITCOLL];

	// ポリゴンの構造体にアクセスするために使用するポインタ
	MV1_COLL_RESULT_POLY    *Poly;

	// 線分とポリゴンとの当たり判定の結果を代入する構造体
	HITRESULT_LINE           LineRes;

	// 移動前の座標	
	VECTOR                   OldPos;

	// 移動後の座標
	VECTOR                   NextPos;

	VECTOR                   BottomPos1;
	VECTOR                   BottomPos2;
	VECTOR                   TopPos1;
	VECTOR                   TopPos2;
	VECTOR                   LineTopPos;
	VECTOR                   LineBottomPos;
	VECTOR                  *PolyNormal;
	float                    TotalMoveDistance;
	float                    CompMoveDistance;
	float                    OneMoveMaxDistance;
	float                    OneMoveDistance;
	float                    SearchDistance;
	VECTOR                   MoveDirection;
	VECTOR                   TempMoveVector;
	EStageCollType           CollType;
	int                      StageObjectNum;
	const SCharaBaseInfo    *CBInfo;
	VECTOR                   SlideVec;
	VECTOR                   TempYVector;
	VECTOR                   PolyXZNormal;
	int                      CollisionModelHandle;

	CBInfo = CInfo->BaseInfo;

	// 移動総距離を取得
	TotalMoveDistance = VSize(MoveVector);

	// 移動距離が０の場合は仮にＸ軸方向に移動したことにする
	if (TotalMoveDistance < 0.000001f)
	{
		MoveDirection = VGet(1.0f, 0.0f, 0.0f);
	}
	else
	{
		MoveDirection = VScale(MoveVector, 1.0f / TotalMoveDistance);
	}

	// ステージオブジェクトの数を取得する
	StageObjectNum = StageData_GetObjectNum();

	// 一度に移動する距離を算出
	OneMoveMaxDistance = CBInfo->StageHitTopInfo.Width * 2.0f;

	// 一度に移動する距離より移動距離が長い場合は複数回に分けて移動する
	CompMoveDistance = 0.0f;
	do
	{
		// 移動する距離を決定
		if (CompMoveDistance + OneMoveMaxDistance > TotalMoveDistance)
		{
			OneMoveDistance = TotalMoveDistance - CompMoveDistance;
		}
		else
		{
			OneMoveDistance = OneMoveMaxDistance;
		}

		// 移動ベクトルの算出
		TempMoveVector = VScale(MoveDirection, OneMoveDistance);

		// キャラクターの周囲にあるポリゴンを検索する際の検索距離を算出
		SearchDistance = CHARA_ENUM_DEFAULT_SIZE + OneMoveDistance;

		// 移動する距離を足す
		CompMoveDistance += OneMoveDistance;

		// 移動前の座標を保存
		OldPos = CInfo->Position;

		// 移動後の座標を算出
		NextPos = VAdd(CInfo->Position, TempMoveVector);

		// 当たり判定に使用する座標を計算する
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
			&NextPos, &TopPos1, &TopPos2);

		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
			&NextPos, &BottomPos1, &BottomPos2);

		// 当たり判定結果情報の数を初期化
		HitNum = 0;

		// キャラクターの周囲にあるステージポリゴンを取得する
		HitDim[HitNum] = MV1CollCheck_Sphere(
			StageData_GetCollisionModelHandle(), -1,
			CInfo->Position, SearchDistance);
		if (HitDim[HitNum].HitNum != 0)
		{
			HitNum++;
		}
		else
		{
			MV1CollResultPolyDimTerminate(HitDim[HitNum]);
		}

		// キャラクターの周囲にあるコリジョンオブジェクトのポリゴンも取得する
		for (i = 0; i < StageObjectNum; i++)
		{
			CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

			if (CollisionModelHandle != -1)
			{
				HitDim[HitNum] = MV1CollCheck_Sphere(CollisionModelHandle, -1,
					CInfo->Position, SearchDistance);
				if (HitDim[HitNum].HitNum != 0)
				{
					HitNum++;
				}
				else
				{
					MV1CollResultPolyDimTerminate(HitDim[HitNum]);
				}
			}
		}

		// x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを立てる
		MoveFlag = fabs(TempMoveVector.x) > 0.01f || fabs(TempMoveVector.z) > 0.01f;

		// 壁ポリゴンと床ポリゴンと天井ポリゴンの数を初期化する
		for (i = 0; i < EStageCollType_Num; i++)
		{
			HitPolyNum[i] = 0;
		}

		// 当たり判定結果情報の数だけ繰り返し
		for (j = 0; j < HitNum; j++)
		{
			// 検出されたポリゴンの数だけ繰り返し
			for (i = 0; i < HitDim[j].HitNum; i++)
			{
				// ポリゴンの法線のアドレスを取得
				PolyNormal = &HitDim[j].Dim[i].Normal;

				// 一定以上の傾斜は壁ポリゴンとして処理する
				if (PolyNormal->x * PolyNormal->x + PolyNormal->z * PolyNormal->z > 0.5f)
				{
					CollType = EStageCollType_Wall;
				}
				else
					// 法線が下向きかどうかで処理を分岐
					if (PolyNormal->y <= 0.0f)
					{
						// ジャンプ中かつ上昇中の場合は天井ポリゴンとして処理し、
						// そうではない場合は壁ポリゴンとして処理する
						if (CInfo->JumpState && CInfo->Spd.y > 0.0f)
						{
							CollType = EStageCollType_Ceiling;
						}
						else
						{
							CollType = EStageCollType_Wall;
						}
					}
					else
					{
						// それ以外の場合は床ポリゴンとする
						CollType = EStageCollType_Floor;
					}

				// ポリゴンの数が上限に達していない場合はタイプ別のポリゴン配列に登録する
				if (HitPolyNum[CollType] < CHARA_MAX_HITCOLL)
				{
					HitPoly[CollType][HitPolyNum[CollType]] = &HitDim[j].Dim[i];
					HitPolyNum[CollType] ++;
				}
			}
		}

		// 壁ポリゴンとの当たり判定処理
		if (HitPolyNum[EStageCollType_Wall] != 0)
		{
			// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
			HitFlag = false;

			// 移動したかどうかで処理を分岐
			if (MoveFlag)
			{
				// 移動していた場合は衝突した壁ポリゴンに対してスライドする処理を行う

				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					Poly = HitPoly[EStageCollType_Wall][i];

					// ポリゴンとキャラクターが当たっていなかったら次のループへ
					if (HitCheck_Capsule_Triangle(
						BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
						Poly->Position[0], Poly->Position[1],
						Poly->Position[2]) == FALSE &&
						HitCheck_Capsule_Triangle(
							TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == FALSE) continue;

					// キャラクラータイプが敵弾だった場合はキャラを削除して終了
					if (CBInfo->Type == ECharaType_EnemyShot)
					{
						Chara_Delete(CInfo);
						for (k = 0; k < HitNum; k++)
						{
							MV1CollResultPolyDimTerminate(HitDim[k]);
						}
						return true;
					}

					// ここにきたらポリゴンとキャラクターが当たっているということなので、
					// ポリゴンに当たったフラグを立てる
					HitFlag = true;

					// 壁ポリゴンに当たったら壁に遮られない移動分だけ移動する

					// 法線のＸＺ値だけを抽出
					PolyXZNormal = Poly->Normal;
					PolyXZNormal.y = 0.0f;

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					TempYVector = VCross(TempMoveVector, PolyXZNormal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(PolyXZNormal, TempYVector);

					// Ｙ軸方向にはスライドしない
					SlideVec.y = 0.0f;

					// それを移動前の座標に足したものを新たな座標とする
					NextPos = VAdd(OldPos, SlideVec);

					// Ｙ座標には元の移動値を加算する
					NextPos.y += TempMoveVector.y;

					// 当たり判定に使用する座標を更新する
					CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
						&NextPos, &TopPos1, &TopPos2);

					CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
						&NextPos, &BottomPos1, &BottomPos2);

					// 新たな移動後座標で壁ポリゴンと当たるかどうかを判定する
					for (j = 0; j < HitPolyNum[EStageCollType_Wall]; j++)
					{
						// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						Poly = HitPoly[EStageCollType_Wall][j];

						// 当たっていたらループから抜ける
						if (HitCheck_Capsule_Triangle(
							BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == TRUE ||
							HitCheck_Capsule_Triangle(
								TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == TRUE) break;
					}

					// j が HitPolyNum[ EStageCollType_Wall ] だった場合は
					// どのポリゴンとも当たらなかったということなので
					// 『壁に当たったかどうか』のフラグを倒した上でループから抜ける
					if (j == HitPolyNum[EStageCollType_Wall])
					{
						HitFlag = false;
						break;
					}
				}
			}
			else
			{
				// 移動していない場合は当たっているポリゴンがあるかどうかだけ確認

				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					Poly = HitPoly[EStageCollType_Wall][i];

					// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
					if (HitCheck_Capsule_Triangle(
						BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
						Poly->Position[0], Poly->Position[1],
						Poly->Position[2]) == TRUE ||
						HitCheck_Capsule_Triangle(
							TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == TRUE)
					{
						// キャラクラータイプが敵弾だった場合はキャラを削除して終了
						if (CBInfo->Type == ECharaType_EnemyShot)
						{
							Chara_Delete(CInfo);
							for (k = 0; k < HitNum; k++)
							{
								MV1CollResultPolyDimTerminate(HitDim[k]);
							}
							return true;
						}

						HitFlag = true;
						break;
					}
				}
			}

			// 壁に当たっていたら壁から押し出す処理を行う
			if (HitFlag)
			{
				// 壁からの押し出し処理を試みる最大数だけ繰り返し
				for (k = 0; k < CHARA_HIT_TRYNUM; k++)
				{
					// 壁ポリゴンの数だけ繰り返し
					for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
					{
						VECTOR PolyXZNormal;

						// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						Poly = HitPoly[EStageCollType_Wall][i];

						// キャラクターと当たっているかを判定
						if (HitCheck_Capsule_Triangle(
							BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == FALSE &&
							HitCheck_Capsule_Triangle(
								TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == FALSE) continue;

						// 当たっていたらキャラクターを壁の法線方向に一定距離移動させる
						PolyXZNormal = Poly->Normal;
						PolyXZNormal.y = 0.0f;
						NextPos = VAdd(NextPos, VScale(PolyXZNormal,
							CHARA_HIT_SLIDE_DISTANCE));

						// 当たり判定に使用する座標を更新する
						CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
							&NextPos, &TopPos1, &TopPos2);

						CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
							&NextPos, &BottomPos1, &BottomPos2);

						// 移動した上で壁ポリゴンと当たるかどうかを判定
						for (j = 0; j < HitPolyNum[EStageCollType_Wall]; j++)
						{
							// 当たっていたらループを抜ける
							Poly = HitPoly[EStageCollType_Wall][j];
							if (HitCheck_Capsule_Triangle(
								BottomPos1, BottomPos2,
								CBInfo->StageHitBottomInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == TRUE ||
								HitCheck_Capsule_Triangle(
									TopPos1, TopPos2,
									CBInfo->StageHitTopInfo.Width,
									Poly->Position[0], Poly->Position[1],
									Poly->Position[2]) == TRUE) break;
						}

						// 全てのポリゴンと当たっていなかったらここでループ終了
						if (j == HitPolyNum[EStageCollType_Wall])
						{
							break;
						}
					}

					// i が HitPolyNum[ EStageCollType_Wall ] ではない場合は
					// 全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと
					// 接触しなくなったということなのでループから抜ける
					if (i != HitPolyNum[EStageCollType_Wall])
					{
						break;
					}
				}
			}
		}

		// 天井ポリゴンとの当たり判定
		if (HitPolyNum[EStageCollType_Ceiling] != 0)
		{
			float MinY;

			// 天井に頭をぶつける処理を行う

			// 一番低い天井にぶつける為の判定用変数を初期化
			MinY = 0.0f;

			// 当たったかどうかのフラグを倒す
			HitFlag = false;

			// 当たり判定用の線分の座標を算出
			LineTopPos = VAdd(NextPos, CBInfo->StageHitTopInfo.CenterPosition);
			LineTopPos.y += CBInfo->StageHitTopInfo.Height / 2.0f
				+ CBInfo->StageHitTopInfo.Width;

			LineBottomPos = VAdd(NextPos, CBInfo->StageHitBottomInfo.CenterPosition);
			LineBottomPos.y -= CBInfo->StageHitBottomInfo.Height / 2.0f
				+ CBInfo->StageHitBottomInfo.Width;

			// 天井ポリゴンの数だけ繰り返し
			for (i = 0; i < HitPolyNum[EStageCollType_Ceiling]; i++)
			{
				// i番目の天井ポリゴンのアドレスを天井ポリゴンポインタ配列から取得
				Poly = HitPoly[EStageCollType_Ceiling][i];

				// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
				LineRes = HitCheck_Line_Triangle(LineBottomPos, LineTopPos,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// 接触していなかったら何もしない
				if (LineRes.HitFlag == FALSE)
				{
					continue;
				}

				// キャラクラータイプが敵弾だった場合は削除して終了
				if (CBInfo->Type == ECharaType_EnemyShot)
				{
					Chara_Delete(CInfo);
					for (k = 0; k < HitNum; k++)
					{
						MV1CollResultPolyDimTerminate(HitDim[k]);
					}
					return true;
				}

				// 既にポリゴンに当たっていて、且つ今まで検出した
				// 天井ポリゴンより高い場合は何もしない
				if (HitFlag && MinY < LineRes.Position.y)
				{
					continue;
				}

				// ポリゴンに当たったフラグを立てる
				HitFlag = true;

				// 接触したＹ座標を保存する
				MinY = LineRes.Position.y;
			}

			// 接触したポリゴンがあったかどうかで処理を分岐
			if (HitFlag)
			{
				// 接触した場合はキャラクターのＹ座標を接触座標を元に更新
				NextPos.y = MinY - (LineTopPos.y - LineBottomPos.y);

				// Ｙ軸方向の速度は反転
				CInfo->Spd.y = -CInfo->Spd.y;
			}
		}

		// 床ポリゴンとの当たり判定
		if (HitPolyNum[EStageCollType_Floor] != 0)
		{
			float MaxY;
			int   MaxIndex;

			// ポリゴンに当たったかどうかのフラグを倒しておく
			HitFlag = false;

			// 一番高い床ポリゴンに接地させる為の判定用変数を初期化
			MaxY = 0.0f;
			MaxIndex = -1;

			// 当たり判定の線分の座標をセット
			LineTopPos = VAdd(NextPos, CBInfo->StageHitTopInfo.CenterPosition);
			LineTopPos.y += CBInfo->StageHitTopInfo.Height / 2.0f
				+ CBInfo->StageHitTopInfo.Width;

			LineBottomPos = VAdd(NextPos, CBInfo->StageHitBottomInfo.CenterPosition);
			LineBottomPos.y -= CBInfo->StageHitBottomInfo.Height / 2.0f
				+ CBInfo->StageHitBottomInfo.Width;

			// 下端の値を床判定用に補正
			if (CInfo->JumpState)
			{
				LineBottomPos.y += CHARA_HIT_FLOOR_Y_ADJUST_JUMP;
			}
			else
			{
				LineBottomPos.y += CHARA_HIT_FLOOR_Y_ADJUST_WALK;
			}

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < HitPolyNum[EStageCollType_Floor]; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				Poly = HitPoly[EStageCollType_Floor][i];

				// 線分とポリゴンの当たり判定
				LineRes = HitCheck_Line_Triangle(LineTopPos, LineBottomPos,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// 当たっていなかったら次のループへ
				if (LineRes.HitFlag == FALSE)
				{
					continue;
				}

				// キャラクラータイプが敵弾だった場合は削除して終了
				if (CBInfo->Type == ECharaType_EnemyShot)
				{
					Chara_Delete(CInfo);
					for (k = 0; k < HitNum; k++)
					{
						MV1CollResultPolyDimTerminate(HitDim[k]);
					}
					return true;
				}

				// 既に当たったポリゴンがあり、
				// 且つ今まで検出した床ポリゴンより低い場合は次のループへ
				if (HitFlag && MaxY > LineRes.Position.y)
				{
					continue;
				}

				// ポリゴンに当たったフラグを立てる
				HitFlag = true;

				// 接触したＹ座標を保存する
				MaxY = LineRes.Position.y;

				// 接触したポリゴンのインデックスを保存する
				MaxIndex = i;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (HitFlag)
			{
				// 当たった場合

				// 接触したポリゴンで一番高いＹ座標をキャラクターのＹ座標にする
				NextPos.y = MaxY;

				// Ｙ軸方向の移動速度は０に
				CInfo->Spd.y = 0.0f;

				// 接触したポリゴンのマテリアルタイプを取得する
				CInfo->OnMaterialType = StageData_GetCollisionModelMaterialType(
					HitPoly[EStageCollType_Floor][MaxIndex]->MaterialIndex);

				// もしジャンプ中だった場合は着地状態に移行する
				if (CInfo->JumpState)
				{
					if (!Chara_Landed(CInfo))
					{
						return false;
					}
					CInfo->JumpState = false;
				}
			}
			else
			{
				// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
				// ジャンプ状態に移行する
				if (!CInfo->JumpState)
				{
					CInfo->JumpState = true;
					if (!Chara_Fall(CInfo))
					{
						return false;
					}
				}
			}
		}

		// 検出したキャラクターの周囲のポリゴン情報を開放する
		for (i = 0; i < HitNum; i++)
		{
			MV1CollResultPolyDimTerminate(HitDim[i]);
		}

		// 新しい座標を保存する
		CInfo->Position = NextPos;

		// 全ての移動が完了していない場合はループ
	} while (CompMoveDistance + 0.001f < TotalMoveDistance);

	// キャラクターの３Ｄモデルの座標を更新する
	MV1SetPosition(CInfo->ModelHandle, CInfo->Position);

	// 正常終了
	return true;
}

// キャラの移動処理を推移させる
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_MoveStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaInfo *ChkCInfo;
	VECTOR      MoveVector;
	VECTOR      AnimMoveVector;
	int         i;

	// ジャンプ中の場合は重力処理を行う
	if (CInfo->JumpState)
	{
		CInfo->Spd.y -= CHARA_GRAVITY * StepTime;
	}

	// 移動ベクトルの算出
	MoveVector = VScale(VAdd(CInfo->PrevSpd, CInfo->Spd), StepTime / 2.0f);

	// 今の移動速度を保存しておく
	CInfo->PrevSpd = CInfo->Spd;

	// ルートフレームのアニメーションによる移動値の算出
	AnimMoveVector = VScale(CInfo->AngleInfo.FrontDirection, -CInfo->AnimInfo.Move.z);
	AnimMoveVector = VAdd(AnimMoveVector,
		VScale(CInfo->AngleInfo.LeftDirection, -CInfo->AnimInfo.Move.x));

	// ルートフレームのアニメーションによる移動速度を保存しておく
	if (StepTime < 0.0000001f)
	{
		CInfo->PrevAnimSpd = VGet(0.0f, 0.0f, 0.0f);
	}
	else
	{
		CInfo->PrevAnimSpd = VScale(AnimMoveVector, 1.0f / StepTime);
	}

	// アニメーションによる移動値を移動ベクトルに加算
	MoveVector = VAdd(MoveVector, AnimMoveVector);

	// プログラムによる移動を行う場合はプログラムで設定した移動速度による移動値も加算する	
	if (CInfo->ProgramMove)
	{
		MoveVector = VAdd(MoveVector, VScale(CInfo->AngleInfo.FrontDirection,
			CInfo->BaseInfo->ProgramSpd * StepTime));
	}

	// 移動ベクトルから移動速度を算出
	if (StepTime < 0.0000001f)
	{
		CInfo->PrevFixSpd = VGet(0.0f, 0.0f, 0.0f);
	}
	else
	{
		CInfo->PrevFixSpd = VScale(MoveVector, 1.0f / StepTime);
	}

	// 敵の弾ではない場合はキャラクター同士の当たり判定を行う
	if (CInfo->BaseInfo->Type != ECharaType_EnemyShot)
	{
		// キャラクターの数だけ繰り返し
		ChkCInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, ChkCInfo++)
		{
			// 使用されていない構造体の場合は次のループへ
			if (ChkCInfo->UseFlag == FALSE)
			{
				continue;
			}

			// チェックしようとしているキャラが
			// 移動しようとしているキャラだったら次のループへ
			if (ChkCInfo == CInfo)
			{
				continue;
			}

			// チェックしようとしているキャラが敵の弾だった場合は
			// 敵の弾はすり抜けるので次のループへ
			if (ChkCInfo->BaseInfo->Type == ECharaType_EnemyShot)
			{
				continue;
			}

			// キャラ同士の当たり判定
			Chara_Collision(CInfo, &MoveVector, ChkCInfo);
		}
	}

	// ステージの３Ｄモデルとの当たり判定を伴った移動処理を行う
	if (!Chara_Move(CInfo, MoveVector))
	{
		return false;
	}

	// ジャンプ中ではない場合は移動速度を０にする
	if (!CInfo->JumpState)
	{
		CInfo->Spd = VGet(0.0f, 0.0f, 0.0f);
	}

	// 正常終了
	return true;
}

// キャラの向き処理を初期化する
static void Chara_InitializeAngle(
	// 向き処理を初期化するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 初期の向き
	float Angle
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	CAngInfo->NowAngle = Angle;
	CAngInfo->TargetAngle = Angle;
	CAngInfo->AngleSpeed = CHARA_DEFAULT_ANGLE_SPEED;
	CAngInfo->AngleMoveEnd = false;
}

// キャラを指定の座標の方向に向かせる
void Chara_TargetAngleMove(
	// 向きを変更するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせたい座標
	VECTOR TargetPosition
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;
	VECTOR TargetVector;

	// キャラの座標から指定の座標へのベクトルを算出
	TargetVector = VSub(TargetPosition, CInfo->Position);

	// atan2 を使用して方向ベクトルから角度を求める
	Chara_SetTargetAngle(CInfo, atan2(TargetVector.x, TargetVector.z), true);
}

// キャラが向くべき方向を設定する
void Chara_SetTargetDirection(
	// 向きを設定するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせる方向
	VECTOR Direction,

	// 徐々に向きを変えるかどうか
	// ( true : 徐々に向きを変える  false : 一瞬で向きを変える )
	bool Interp,

	// Direction の向きを反転して使用するかどうか
	// ( true : 反転して使用する  false : 反転しない )
	bool Reverse
)
{
	// 向きを反転して使用する場合はここでベクトルを反転させる
	if (Reverse)
	{
		Direction = VScale(Direction, -1.0f);
	}

	// atan2 を使用して角度を求める
	Chara_SetTargetAngle(CInfo, atan2(Direction.x, Direction.z), Interp);
}

// キャラが向くべき方向を設定する( Ｙ軸角度指定タイプ )
void Chara_SetTargetAngle(
	// 向きを設定するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせる方向( Ｙ軸角度 )
	float Angle,

	// 徐々に向きを変えるかどうか
	// ( true:徐々に向きを変える  false:一瞬で向きを変える )
	bool Interp
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	// 目標とする角度をセット
	CAngInfo->TargetAngle = Angle;

	// 補間を行うかどうかで処理を分岐
	if (!Interp)
	{
		// 補間を行わない場合は即座に向きを反映する
		CAngInfo->NowAngle = Angle;
		CAngInfo->AngleMoveEnd = true;
		Chara_AngleParamRefresh(CInfo);
	}
	else
	{
		// 補間を行う場合は向きの変更が終了しているかどうかのフラグを倒す
		CAngInfo->AngleMoveEnd = false;
	}
}

// キャラの向き処理の状態推移を行う
static void Chara_AngleStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	// 向きの変更処理を行う
	if (ParamChangeAngle(&CAngInfo->NowAngle, CAngInfo->TargetAngle,
		StepTime, CAngInfo->AngleSpeed))
	{
		// 変更が完了したら向きの変更が終了しているかどうかのフラグを立てる
		CAngInfo->AngleMoveEnd = true;

	}

	// 向きに付随するパラメータを更新する
	Chara_AngleParamRefresh(CInfo);
}

// キャラの向いている方向に基づく情報を更新する
void Chara_AngleParamRefresh(
	// 情報を更新するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;
	float            fSin;
	float            fCos;

	// ３Ｄモデルの向きを設定する
	MV1SetRotationXYZ(CInfo->ModelHandle, VGet(0.0f, CAngInfo->NowAngle + DX_PI_F, 0.0f));

	// ３Ｄモデルが向いている方向のベクトルを算出
	fSin = (float)sin(CAngInfo->NowAngle);
	fCos = (float)cos(CAngInfo->NowAngle);
	CAngInfo->FrontDirection.x = fSin;
	CAngInfo->FrontDirection.y = 0.0f;
	CAngInfo->FrontDirection.z = fCos;

	// ３Ｄモデルから見て左方向のベクトルを算出
	fSin = (float)sin(CAngInfo->NowAngle + DX_PI * 0.5f);
	fCos = (float)cos(CAngInfo->NowAngle + DX_PI * 0.5f);
	CAngInfo->LeftDirection.x = fSin;
	CAngInfo->LeftDirection.y = 0.0f;
	CAngInfo->LeftDirection.z = fCos;
}

// キャラのアニメーション処理を初期化する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_InitializeAnim(
	// アニメーション処理を初期化するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SCharaAnimInfo * CAInfo = &CInfo->AnimInfo;

	// 情報の初期化
	CAInfo->NowAnim = ECharaAnim_Num;
	CAInfo->PrevAnim = ECharaAnim_Num;
	CAInfo->NowAttachIndex = -1;
	CAInfo->PrevAttachIndex = -1;
	CAInfo->ChangeSpeed = 1.0f;
	CAInfo->ChangeRate = 1.0f;
	CAInfo->NowTime = 0.0f;
	CAInfo->PrevTime = 0.0f;

	CAInfo->Cancel = false;
	CAInfo->End = false;
	CAInfo->Move = VGet(0.0f, 0.0f, 0.0f);

	// 初期状態ではニュートラルアニメーションを再生する
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, -1.0f))
	{
		return false;
	}

	// 正常終了
	return true;
}

// キャラのアニメーション処理の状態推移を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AnimStep(
	// 状態推移を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaAnimInfo       *CAInfo = &CInfo->AnimInfo;
	SAnimEventCheck       AECheck;
	const SAnimEventInfo *AEInfo;
	float                 AnimTotalTime;
	VECTOR                NowAnimMoveVector;
	VECTOR                PrevAnimMoveVector;
	VECTOR                PrevAnimPosition;
	VECTOR                NowAnimPosition;
	float                 StartTime;
	bool                  EndTimeFlag;
	int                   i;
	int                   RootFrameIndex;

	// アニメーションによる移動ベクトルを初期化
	NowAnimMoveVector = VGet(0.0f, 0.0f, 0.0f);
	PrevAnimMoveVector = VGet(0.0f, 0.0f, 0.0f);

	// ルートフレームの番号を取得
	RootFrameIndex = CInfo->BaseInfo->RootFrameIndex;

	// 前回再生していたアニメーションがある場合はその処理をする
	if (CAInfo->PrevAttachIndex >= 0)
	{
		// 前回再生していたアニメーションのルートフレームのローカル座標を取得
		PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
			CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

		// 前回再生していたアニメーションの総再生時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(
			CInfo->ModelHandle, CAInfo->PrevAttachIndex);

		// 前回再生していたアニメーションの再生時間を進める
		CAInfo->PrevTime += StepTime * CHARA_ANIM_FPS;

		// 再生時間が総再生時間を越えたかどうかで処理を分岐
		if (CAInfo->PrevTime >= AnimTotalTime)
		{
			// 超えた場合はループ再生するかどうかで処理を分岐
			if (CharaBase_GetAnimFileInfo(CAInfo->PrevAnim)->LoopFlag)
			{
				// ループする場合は再生時間から総再生時間引く
				CAInfo->PrevTime -= AnimTotalTime;

				// アニメーションによる移動ベクトルとして『再生時間を進める前の
				// ルートフレームからアニメーションの最後の位置でのルートフレーム
				// の座標の差分』と『アニメーションの最初の位置でのルートフレーム
				// の座標から再生時間を進めた後のルートフレームの座標の差分』を使用する

				// 再生時間をアニメーションの終端に変更
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, AnimTotalTime);

				// アニメーション終端でのルートフレームのローカル座標を取得
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// 再生時間を進める前のルートフレームの位置と、アニメーション終端での
				// ルートフレームの座標の差分を算出
				PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);

				// 再生時間をアニメーションの最初に変更
				MV1SetAttachAnimTime(CInfo->ModelHandle, CAInfo->PrevAttachIndex, 0);

				// アニメーションの最初の位置でのルートフレームのローカル座標を取得
				PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// 再生時間を進めた後の位置に変更
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

				// 再生時間を進めた後のルートフレームのローカル座標を取得
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// アニメーションの最初の位置でのルートフレームのローカル座標と
				// 再生時間を進めた後のルートフレームのローカル座標の差分を
				// 再生時間を進める前のルートフレームの位置と、アニメーション終端での
				// ルートフレームの座標の差分に加算
				PrevAnimMoveVector = VAdd(VSub(NowAnimPosition, PrevAnimPosition),
					PrevAnimMoveVector);
			}
			else
			{
				// ループしない場合は再生時間を終端の時間にする
				CAInfo->PrevTime = AnimTotalTime;

				// 再生時間を進める前のルートフレームのローカル座標と
				// アニメーションの最後の位置でのルートフレームのローカル座標の差分を取得
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
			}
		}
		else
		{
			// 総再生時間を超えていない場合は再生時間を進めた後のルートフレームの
			// ローカル座標と再生時間を進める前のルートフレームのローカル座標の差分を取得
			MV1SetAttachAnimTime(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

			NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

			PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
		}
	}

	// 現在再生しているアニメーションがある場合はその処理をする
	if (CAInfo->NowAttachIndex >= 0)
	{
		// 再生しているアニメーションのルートフレームのローカル座標を取得
		PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
			CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

		// 再生しているアニメーションの総再生時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(
			CInfo->ModelHandle, CAInfo->NowAttachIndex);

		// 進める前の再生時間を保持しておく
		StartTime = CAInfo->NowTime;

		// 再生時間がアニメーションの終端かどうかのフラグを倒しておく
		EndTimeFlag = false;

		// 再生時間を進める
		CAInfo->NowTime += StepTime * CHARA_ANIM_FPS;

		// 再生時間が総再生時間を越えたかどうかで処理を分岐
		if (CAInfo->NowTime >= AnimTotalTime)
		{
			// 超えていた場合はループ再生などの処理を行う

			// アニメーションの再生が終了したかどうかのフラグを立てる
			CAInfo->End = true;

			// アニメーションをループするかどうかで処理を分岐
			if (CharaBase_GetAnimFileInfo(CAInfo->NowAnim)->LoopFlag)
			{
				// ループする場合は再生時間から総再生時間引く
				CAInfo->NowTime -= AnimTotalTime;

				// アニメーションによる移動ベクトルとして『再生時間を進める前の
				// ルートフレームからアニメーションの最後の位置でのルートフレーム
				// の座標の差分』と『アニメーションの最初の位置でのルートフレーム
				// の座標から再生時間を進めた後のルートフレームの座標の差分』を使用する

				// 再生時間をアニメーションの終端に変更
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, AnimTotalTime);

				// アニメーション終端でのルートフレームのローカル座標を取得
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// 再生時間を進める前のルートフレームの位置と、アニメーション終端での
				// ルートフレームの座標の差分を算出
				NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);

				// 再生時間をアニメーションの最初に変更
				MV1SetAttachAnimTime(CInfo->ModelHandle, CAInfo->NowAttachIndex, 0);

				// アニメーションの最初の位置でのルートフレームのローカル座標を取得
				PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// 再生時間を進めた後の位置に変更
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

				// 再生時間を進めた後のルートフレームのローカル座標を取得
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// アニメーションの最初の位置でのルートフレームのローカル座標と
				// 再生時間を進めた後のルートフレームのローカル座標の差分を
				// 再生時間を進める前のルートフレームの位置と、アニメーション終端での
				// ルートフレームの座標の差分に加算
				NowAnimMoveVector = VAdd(VSub(NowAnimPosition, PrevAnimPosition),
					NowAnimMoveVector);
			}
			else
			{
				// ループしない場合は再生時間を終端の時間にする
				CAInfo->NowTime = AnimTotalTime;

				// 再生時間がアニメーションの終端かどうかのフラグを立てる
				EndTimeFlag = true;

				// 再生時間を進める前のルートフレームのローカル座標と
				// アニメーションの最後の位置でのルートフレームのローカル座標の差分を取得
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
			}
		}
		else
		{
			// 総再生時間を超えていない場合は再生時間を進めた後のルートフレームの
			// ローカル座標と再生時間を進める前のルートフレームのローカル座標の差分を取得
			MV1SetAttachAnimTime(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

			NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

			NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
		}

		// 進める前の再生時間と、進めた後の再生時間の間に存在する
		// アニメーションイベントを取得
		AnimInfoEventCheck(&CInfo->BaseInfo->AnimInfo[CAInfo->NowAnim].Info,
			&AECheck, StartTime, EndTimeFlag ? -1.0f : CAInfo->NowTime);

		// アニメーションイベントの数だけ繰り返し
		for (i = 0; i < AECheck.EventNum; i++)
		{
			AEInfo = AECheck.Event[i];

			// アニメーションイベントタイプによって処理を分岐
			switch (AEInfo->Type)
			{
			case EAnimEventType_Sound:			// 音を再生
												// 音を再生する
				Sound_PlaySound3D(CInfo->Position, AEInfo->SoundHandle, DX_PLAYTYPE_BACK);
				break;

			case EAnimEventType_PhysicsSound:	// 物理音を再生
												// 現在立っている場所の素材にあった物理音がある場合は再生
				if (CInfo->BaseInfo->AnimPhysicsSound[AEInfo->PhysicsSound]
					[CInfo->OnMaterialType] != -1)
				{
					Sound_PlaySound3D(CInfo->Position,
						CInfo->BaseInfo->AnimPhysicsSound[AEInfo->PhysicsSound]
						[CInfo->OnMaterialType], DX_PLAYTYPE_BACK);
				}
				break;

			case EAnimEventType_AnimCancel:		// アニメーションキャンセル
												// アニメーションキャンセルイベントが発生したかどうかのフラグを立てる
				CAInfo->Cancel = true;
				break;

			case EAnimEventType_AttackStart:	// 攻撃判定開始
												// 攻撃情報を有効にする
				CInfo->AttackInfo[AEInfo->AttackNo].Enable = true;
				CInfo->AttackInfo[AEInfo->AttackNo].AttackPosIndex =
					AEInfo->AttackPosIndex;
				break;

			case EAnimEventType_AttackEnd:		// 攻撃判定終了
												// 攻撃情報を無効にする
				CInfo->AttackInfo[AEInfo->AttackNo].Enable = false;
				break;

			case EAnimEventType_Other:			// その他
												// その他イベントが発生した際に実行する関数が存在する場合は実行する
				if (g_CharaFunctionTable[CInfo->Chara].AnimOtherEvent != NULL)
				{
					if (!g_CharaFunctionTable[CInfo->Chara].AnimOtherEvent(CInfo))
					{
						return false;
					}
				}
				break;
			}
		}
	}

	// 再生するアニメーションの変更を行っているかどうかで処理を分岐
	if (CAInfo->PrevAttachIndex >= 0)
	{
		// アニメーションの変更を行っている場合

		// 変更率を進める
		CAInfo->ChangeRate += CAInfo->ChangeSpeed * StepTime;

		// 変更が完了したかどうかで処理を分岐
		if (CAInfo->ChangeRate >= 1.0f)
		{
			CAInfo->ChangeRate = 1.0f;

			// 変更が完了した場合は前回再生していたアニメーションをデタッチする
			MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
			CAInfo->PrevAttachIndex = -1;

			// 現在再生しているアニメーションの影響率を１００％にする
			MV1SetAttachAnimBlendRate(CInfo->ModelHandle, CAInfo->NowAttachIndex, 1.0f);

			// アニメーションによる移動ベクトルとして現在再生しているアニメーションの
			// ルートフレームの移動値を設定する
			CAInfo->Move = NowAnimMoveVector;
		}
		else
		{
			// 変更が完了していない場合は変更率に応じた
			// 二つのアニメーションのブレンドを行う
			MV1SetAttachAnimBlendRate(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->ChangeRate);
			MV1SetAttachAnimBlendRate(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, 1.0f - CAInfo->ChangeRate);

			// アニメーションによる移動ベクトルも二つのアニメーションの
			// ルートフレームの移動値をブレンドする
			CAInfo->Move = VScale(PrevAnimMoveVector, 1.0f - CAInfo->ChangeRate);
			CAInfo->Move = VAdd(CAInfo->Move, VScale(NowAnimMoveVector,
				CAInfo->ChangeRate));
		}
	}
	else
	{
		// アニメーションの変更を行っていない場合

		// アニメーションによる移動ベクトルは現在再生しているアニメーションの
		// ルートフレームの移動値を設定する
		CAInfo->Move = NowAnimMoveVector;
	}

	// 正常終了
	return true;
}

// キャラが再生するアニメーションを変更する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_ChangeAnim(
	// アニメーションを再生するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 新たに再生するアニメーション
	ECharaAnim NewAnim,

	// 現在再生しているアニメから新たに再生するアニメに移行する速度( 移行率/秒 )
	float ChangeSpeed
)
{
	SCharaAnimInfo *CAInfo = &CInfo->AnimInfo;
	int             i;

	// 指定のアニメーションが無かった場合は何もせずに終了
	if (CInfo->BaseInfo->AnimInfo[NewAnim].Handle < 0)
	{
		return true;
	}

	// 前回再生していたアニメーションとの変更中だった場合は強制的に変更処理を終了
	if (CAInfo->PrevAttachIndex >= 0)
	{
		MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
		CAInfo->PrevAnim = ECharaAnim_Num;
		CAInfo->PrevAttachIndex = -1;
	}

	// 現在再生しているアニメーションを前回再生していたアニメーションにする
	CAInfo->PrevAnim = CAInfo->NowAnim;

	// 現在再生しているアニメーションを変更
	CAInfo->NowAnim = NewAnim;

	// 前回再生していたアニメーションの再生時間として
	// 今まで再生していたアニメーションの再生時間を代入
	CAInfo->PrevTime = CAInfo->NowTime;

	// 現在再生しているアニメーションの再生時間を初期化
	CAInfo->NowTime = 0.0f;

	// 現在再生しているアニメーションのアタッチ番号を
	// 前回再生していたアニメーションのアタッチ番号にする
	CAInfo->PrevAttachIndex = CAInfo->NowAttachIndex;

	// 新しいアニメーションを３Ｄモデルにアタッチする
	CAInfo->NowAttachIndex =
		MV1AttachAnim(CInfo->ModelHandle, 0, CInfo->BaseInfo->AnimInfo[NewAnim].Handle);

	// ChangeSpeed がマイナスの場合は『徐々にアニメーションを変更する』処理を行わない
	if (ChangeSpeed < 0.0f)
	{
		// アニメーションの変更率を１００％にする
		CAInfo->ChangeRate = 1.0f;

		// 今まで再生していたアニメーションをデタッチする
		CAInfo->PrevAnim = ECharaAnim_Num;
		if (CAInfo->PrevAttachIndex >= 0)
		{
			MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
			CAInfo->PrevAttachIndex = -1;
		}
	}
	else
	{
		// アニメーションの変更率を初期化
		CAInfo->ChangeRate = 0.0f;

		// アニメーションの変更速度を保存
		CAInfo->ChangeSpeed = ChangeSpeed;
	}

	// アニメーションの再生が完了したかどうかのフラグを倒す
	CAInfo->End = false;

	// アニメーションキャンセルイベントが発生したかどうかのフラグを倒す
	CAInfo->Cancel = false;

	// 攻撃情報をリセット
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++)
	{
		CInfo->AttackInfo[i].Enable = false;
	}

	// アニメーションに付随する情報を更新するために推移時間０で状態推移処理を行う
	if (!Chara_AnimStep(CInfo, 0.0f))
	{
		return false;
	}

	// 正常終了
	return true;
}

// キャラの吹っ飛び処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Blow(
	// 吹っ飛び処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	Chara_SetTargetDirection(CInfo, CInfo->DamageDirection, false, true);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_In, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Blow;
	CInfo->BlowState = ECharaBlowState_In;

	// 正常終了
	return true;
}

// キャラの倒れ処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Down(
	// 倒れ処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	Chara_SetTargetDirection(CInfo, CInfo->DamageDirection, false, true);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Down, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Down;

	// 正常終了
	return true;
}

// キャラのフェードアウト処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_FadeOut(
	// フェードアウト処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SEffectInfo *EInfo;

	// 死亡エフェクトを開始
	EInfo = Effect_Create(EEffect_Dead);
	if (EInfo == NULL)
	{
		return false;
	}
	if (!Effect_Dead_Setup(EInfo, CInfo->ModelHandle, GetColorU8(255, 255, 255, 255)))
	{
		return false;
	}

	// 消滅音の再生
	Sound_PlaySound3D(
		CInfo->Position, CharaBase_GetCommonSE(ECharaCommonSE_Nifram), DX_PLAYTYPE_BACK);

	// フェードアウト状態に移行
	CInfo->State = ECharaState_FadeOut;

	// 正常終了
	return true;
}

// キャラにダメージを与える
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Damage(
	// ダメージを喰らうキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// ダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 受けるダメージ
	int DamagePoint,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃の方向
	VECTOR AttackDirection,

	// キャラにダメージを与えられたかどうかを代入する変数のアドレス
	bool *Result
)
{
	bool         DefaultProcess;
	bool         Defence;
	SEffectInfo *EInfo;

	// ダメージを受けた際に実行する関数が存在しない場合はダメージを与えられない
	if (g_CharaFunctionTable[CInfo->Chara].Damage == NULL)
	{
		// ダメージを与えられなかったことを示す false を代入する
		*Result = false;

		// 正常終了
		return true;
	}

	// ダメージを受けた際に実行する関数を実行する
	if (!g_CharaFunctionTable[CInfo->Chara].Damage(
		CInfo, DamageType, DamagePoint,
		HitPosition, AttackDirection, &Defence, &DefaultProcess))
	{
		return false;
	}

	// 防御されず、且つデフォルトの処理を行うフラグが
	// 立っている場合のみデフォルトの処理を行う
	if (!Defence && DefaultProcess)
	{
		// 斬り攻撃だった場合はダメージエフェクトを発生させる
		if (DamageType == ECharaAttack_DamageType_Cut)
		{
			EInfo = Effect_Create(EEffect_Damage);
			if (EInfo == NULL)
			{
				return false;
			}
			Effect_Damage_Setup(EInfo, HitPosition, GetColorU8(128, 16, 16, 255));
		}

		// ダメージを受けた方向を保存
		CInfo->DamageDirection = AttackDirection;

		// 体力を減らす
		CInfo->Health -= DamagePoint;

		// 体力がなくなったかどうかで処理を分岐
		if (CInfo->Health <= 0)
		{
			// 体力がなくなったときの処理

			// 体力を０にする
			CInfo->Health = 0;

			// 吹っ飛びアニメーションがある場合は吹っ飛び状態に、
			// 無い場合はダウン状態にする
			if (CInfo->BaseInfo->AnimInfo[ECharaAnim_Blow_In].Handle > 0)
			{
				if (!Chara_Blow(CInfo))
				{
					return false;
				}
			}
			else
			{
				if (!Chara_Down(CInfo))
				{
					return false;
				}
			}

			// 体力ゲージを非表示にする
			CInfo->HealthGaugeHideDelay = 0.0f;
		}
		else
		{
			// 体力が残っている場合

			// ダメージアニメーションがある場合はダメージ状態にする
			if (CInfo->BaseInfo->AnimInfo[ECharaAnim_Damage].Handle > 0)
			{
				Chara_SetTargetDirection(CInfo, AttackDirection, false, true);

				if (!Chara_ChangeAnim(
					CInfo, ECharaAnim_Damage, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
				{
					return false;
				}
				CInfo->State = ECharaState_Damage;
			}

			// ダメージを受けたら体力ゲージを表示する
			CInfo->HealthGaugeHideDelay = CHARA_HPGAUGE_HIDE_DELAY;
		}
	}

	// ダメージを受けたかどうかを代入する
	if (Result != NULL)
	{
		*Result = !Defence;
	}

	// 正常終了
	return true;
}

// キャラの落下処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Fall(
	// 落下処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	// 死亡処理が開始していたら何もせずに終了
	if (CInfo->State == ECharaState_Blow ||
		CInfo->State == ECharaState_Down ||
		CInfo->State == ECharaState_FadeOut)
	{
		return true;
	}

	// ジャンプ状態に移行する
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Jump;

	// 前回のフレームの移動速度をセットする
	CInfo->Spd = VScale(CInfo->PrevFixSpd, 0.5f);

	// ちょっとだけジャンプする
	CInfo->Spd.y = CHARA_FALL_UP_POWER;

	// 正常終了
	return true;
}

// キャラの着地処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_Landed(
	// 着地処理を行うキャラの情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	bool DefaultProcess;

	// 着地時に実行する関数が存在する場合は実行する
	if (g_CharaFunctionTable[CInfo->Chara].Landed != NULL)
	{
		if (!g_CharaFunctionTable[CInfo->Chara].Landed(CInfo, &DefaultProcess))
		{
			return false;
		}
	}
	else
	{
		DefaultProcess = true;
	}

	// デフォルトの処理を行うかどうかで処理を分岐
	if (DefaultProcess)
	{
		// キャラの状態が吹っ飛び状態かどうかで処理を分岐
		if (CInfo->State == ECharaState_Blow)
		{
			// 吹っ飛び中だった場合は吹っ飛び終了状態に移行する
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_Out,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}
			CInfo->BlowState = ECharaBlowState_Out;
		}
		else
		{
			// 吹っ飛び状態ではなかったらニュートラルアニメーションを再生
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}

			// 空中に居る際に必ずしもジャンプ中とは限らないので、
			// 状態がジャンプ中の場合のみ移動状態に移行する
			if (CInfo->State == ECharaState_Jump)
			{
				CInfo->State = ECharaState_Move;
			}
		}
	}

	// 正常終了
	return true;
}

// 「攻撃に当たったキャラやステージの情報」をリセットする
static void Chara_AttackHitInfo_Reset(
	// リセットする攻撃に当たったキャラの情報構造体のアドレス
	SCharaAttackHitInfo *AHInfo
)
{
	AHInfo->HitCharaNum = 0;
	AHInfo->StageHit = false;
}

// 「攻撃に当たったキャラやステージの情報」に新たに攻撃に当たったキャラを追加する
//     戻り値 : 追加しようとしたキャラが既に追加されていたかどうか
//              ( false : 既に追加されていた   true : 新たに追加した )
static bool Chara_AttackHitInfo_AddChara(
	// 情報を追加する「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 情報に追加するキャラの情報構造体のアドレス
	SCharaInfo *NewHitCInfo
)
{
	int i;

	// 追加しようとしているキャラが既に登録されているかチェック
	for (i = 0; i < AHInfo->HitCharaNum; i++)
	{
		if (AHInfo->HitCharaInfo[i] == NewHitCInfo)
		{
			// 登録されていたら何もせずに終了
			return false;
		}
	}

	// 登録できるキャラの数が既に最大数だった場合は何もせずに終了
	if (AHInfo->HitCharaNum >= CHARA_ATTACK_HIT_CHARA_MAX)
	{
		return false;
	}

	// 配列に追加
	AHInfo->HitCharaInfo[AHInfo->HitCharaNum] = NewHitCInfo;
	AHInfo->HitCharaNum++;

	// 成功終了
	return true;
}

// 「攻撃に当たったキャラやステージの情報」のステージと当たった場合の処理を行う
static void Chara_AttackHitInfo_ProcessStageHit(
	// 処理を行う「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 攻撃が当たった箇所のマテリアルタイプ
	EMaterialType MaterialType,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃に使用された武器
	ECharaWeapon Weapon
)
{
	int                         SoundIndex;
	const SCharaWeaponBaseInfo *WBInfo;

	// ステージに当たったかどうかのフラグを立てる
	AHInfo->StageHit = true;

	// あたった箇所のマテリアルタイプを保存
	AHInfo->StageHitMaterialType = MaterialType;

	// 攻撃に使用された武器の情報を取得
	WBInfo = CharaBase_GetWeaponInfo(Weapon);

	// 武器が何かに衝突した場合に必ず鳴らす音がある場合は再生
	if (WBInfo->MaterialCommonSound >= 0)
	{
		Sound_PlaySound3D(HitPosition, WBInfo->MaterialCommonSound, DX_PLAYTYPE_BACK);
	}

	// 当たった箇所の素材に合った音があるかどうかで処理を分岐
	SoundIndex = -1;
	if (WBInfo->MaterialSound[AHInfo->StageHitMaterialType] >= 0)
	{
		SoundIndex = WBInfo->MaterialSound[AHInfo->StageHitMaterialType];
	}
	else
	{
		// 無い場合は全武器共通で鳴らす音を取得
		SoundIndex = CharaBase_GetWeaponCommonMaterialSound(AHInfo->StageHitMaterialType);
	}

	// 有効な音が存在したら音を鳴らす
	if (SoundIndex >= 0)
	{
		Sound_PlaySound3D(HitPosition, SoundIndex, DX_PLAYTYPE_BACK);
	}
}

// 「攻撃に当たったキャラやステージの情報」のキャラと当たった場合の処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackHitInfo_ProcessCharaHit(
	// 処理を行う「攻撃に当たったキャラやステージの情報」構造体のアドレス
	SCharaAttackHitInfo *AHInfo,

	// 攻撃が当たったキャラの情報構造体のアドレス
	SCharaInfo *HitCInfo,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃の方向
	VECTOR AttackDirection,

	// 攻撃のダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 攻撃に使用された武器
	ECharaWeapon Weapon,

	// 攻撃によって受けるダメージ
	int DamagePoint
)
{
	bool DamageResult;

	// キャラにダメージを与える
	if (!Chara_Damage(HitCInfo, DamageType, DamagePoint, HitPosition,
		AttackDirection, &DamageResult))
	{
		return false;
	}

	// ダメージを与えることに成功したら武器で攻撃された際の音を再生する
	if (DamageResult)
	{
		if (Weapon != ECharaWeapon_Num &&
			HitCInfo->BaseInfo->WeaponAttackDamageSound[Weapon] != -1)
		{
			Sound_PlaySound3D(HitPosition,
				HitCInfo->BaseInfo->WeaponAttackDamageSound[Weapon], DX_PLAYTYPE_BACK);
		}
	}

	// 正常終了
	return true;
}

// キャラの攻撃情報をリセットする
static void Chara_AttackInfo_Reset(
	// リセットするキャラの攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo
)
{
	// 情報が有効かどうかのフラグを倒す
	AInfo->Enable = false;

	// 判定の起点となるSCharaBaseInfo構造体のメンバー変数AttackPosInfoの要素番号を初期化
	AInfo->AttackPosIndex = 0;

	// 「攻撃が当たったキャラやステージの情報」をリセット
	Chara_AttackHitInfo_Reset(&AInfo->HitInfo);

	// 攻撃判定用の座標の有効数も初期化
	AInfo->ValidPositionNum = 0;

	// 攻撃の軌跡エフェクトを出していた場合は終了処理を行う
	if (AInfo->LocusEffect != NULL)
	{
		Effect_EndRequest(AInfo->LocusEffect);
		AInfo->LocusEffect = NULL;
	}
}

// キャラの攻撃情報に攻撃判定用の座標を追加する
static void Chara_AttackInfo_AddPosition(
	// 座標を追加する攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo,

	// 新たに追加する起点となるフレームに近い頂点の座標
	VECTOR NewNearPos,

	// 新たに追加する起点となるフレームから遠い頂点の座標
	VECTOR NewFarPos
)
{
	int i;

	// 既に追加できる最大数に達してしまっている場合は最大数より一つ少ない数にする
	if (AInfo->ValidPositionNum >= CHARA_ATTACK_POS_NUM)
	{
		AInfo->ValidPositionNum = CHARA_ATTACK_POS_NUM - 1;
	}

	// 既に登録されている座標を一つ分移動する
	for (i = AInfo->ValidPositionNum - 1; i >= 0; i--)
	{
		AInfo->NearPosition[i + 1] = AInfo->NearPosition[i];
		AInfo->FarPosition[i + 1] = AInfo->FarPosition[i];
	}

	// 配列の先頭に座標を追加
	AInfo->NearPosition[0] = NewNearPos;
	AInfo->FarPosition[0] = NewFarPos;

	// 有効な座標の数を一つ増やす
	AInfo->ValidPositionNum++;
}

// キャラの攻撃情報による攻撃判定を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Chara_AttackInfo_Check(
	// 判定に使用するキャラの攻撃情報構造体のアドレス
	SCharaAttackInfo *AInfo,

	// 当たり判定の形状
	ECharaAttackFormType FormType,

	// 攻撃を行っているキャラの情報構造体のアドレス
	SCharaInfo *AttackCInfo,

	// 攻撃対象のキャラタイプ
	ECharaType TargetType,

	// 攻撃が当たった場合のダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 攻撃に使用される武器
	ECharaWeapon Weapon,

	// 攻撃が当たった場合のダメージ
	int DamagePoint,

	// 当たり判定の形状が ECharaAttackFormType_Sphere だった場合の球の大きさ
	float SphereSize
)
{
	int                      i;
	VECTOR                   CapsulePos1;
	VECTOR                   CapsulePos2;
	VECTOR                   HitPosition;
	VECTOR                   AttackDirection;
	int                      HitResult;
	SCharaInfo              *CInfo;
	MV1_COLL_RESULT_POLY_DIM HitDim;
	EMaterialType            HitMaterialType;
	int                      StageObjectNum;
	int                      CollisionModelHandle;

	// 攻撃判定用の座標の有効数が２つ未満の場合は何もせず終了
	if (AInfo->ValidPositionNum < 2)
	{
		return true;
	}

	// 攻撃がまだステージに当たっていない場合はステージとの当たり判定を行う
	if (!AInfo->HitInfo.StageHit)
	{
		// ステージのコリジョン用３Ｄモデルハンドルを取得
		CollisionModelHandle = StageData_GetCollisionModelHandle();

		// ステージオブジェクトの数を取得する
		StageObjectNum = StageData_GetObjectNum();

		// 当たり判定の形状によって処理を分岐
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// ポリゴン
											// 攻撃判定用座標を使用したポリゴンとステージとの当たり判定を行う
			HitDim = MV1CollCheck_Triangle(
				CollisionModelHandle,
				-1,
				AInfo->FarPosition[0],
				AInfo->FarPosition[1],
				AInfo->NearPosition[0]
			);
			if (HitDim.HitNum == 0)
			{
				MV1CollResultPolyDimTerminate(HitDim);

				HitDim = MV1CollCheck_Triangle(
					CollisionModelHandle,
					-1,
					AInfo->NearPosition[0],
					AInfo->FarPosition[1],
					AInfo->NearPosition[1]
				);
			}

			// ステージと当たっていたら当たった箇所のマテリアルタイプを取得する
			if (HitDim.HitNum != 0)
			{
				HitMaterialType =
					StageData_GetCollisionModelMaterialType(
						HitDim.Dim[0].MaterialIndex
					);
			}
			else
			{
				// ステージと当たっていなかったらオブジェクトとの当たり判定を行う
				MV1CollResultPolyDimTerminate(HitDim);

				for (i = 0; i < StageObjectNum; i++)
				{
					CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

					if (CollisionModelHandle == -1)
					{
						continue;
					}

					HitDim = MV1CollCheck_Triangle(
						CollisionModelHandle,
						-1,
						AInfo->FarPosition[0],
						AInfo->FarPosition[1],
						AInfo->NearPosition[0]
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);

					HitDim = MV1CollCheck_Triangle(
						CollisionModelHandle,
						-1,
						AInfo->NearPosition[0],
						AInfo->FarPosition[1],
						AInfo->NearPosition[1]
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);
				}

				// オブジェクトと当たっていたら当たった箇所のマテリアルタイプを取得する
				if (HitDim.HitNum != 0)
				{
					HitMaterialType =
						StageData_GetObjectCollisionModelMaterialType(
							i, HitDim.Dim[0].MaterialIndex
						);
				}
			}

			// 当たっていたら当たった座標を算出
			if (HitDim.HitNum != 0)
			{
				HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
				HitPosition = VAdd(HitPosition, AInfo->FarPosition[0]);
				HitPosition = VAdd(HitPosition, AInfo->FarPosition[1]);
				HitPosition = VScale(HitPosition, 1.0f / 4.0f);
			}
			break;

		case ECharaAttackFormType_Sphere:	// 球
											// 攻撃判定用座標を使用したカプセル形状とステージとの当たり判定を行う
			HitDim = MV1CollCheck_Capsule(
				CollisionModelHandle,
				-1,
				AInfo->NearPosition[0],
				AInfo->NearPosition[1],
				SphereSize
			);

			// ステージと当たっていたら当たった箇所のマテリアルタイプを取得する
			if (HitDim.HitNum != 0)
			{
				HitMaterialType =
					StageData_GetCollisionModelMaterialType(
						HitDim.Dim[0].MaterialIndex
					);
			}
			else
			{
				// ステージと当たっていなかったらオブジェクトとの当たり判定を行う
				MV1CollResultPolyDimTerminate(HitDim);

				for (i = 0; i < StageObjectNum; i++)
				{
					CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

					if (CollisionModelHandle == -1)
					{
						continue;
					}

					HitDim = MV1CollCheck_Capsule(
						CollisionModelHandle,
						-1,
						AInfo->NearPosition[0],
						AInfo->NearPosition[1],
						SphereSize
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);
				}

				// オブジェクトと当たっていたら当たった箇所のマテリアルタイプを取得する
				if (HitDim.HitNum != 0)
				{
					HitMaterialType =
						StageData_GetObjectCollisionModelMaterialType(
							i, HitDim.Dim[0].MaterialIndex
						);
				}
			}

			// 当たっていたら当たった座標を算出
			if (HitDim.HitNum != 0)
			{
				HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
				HitPosition = VScale(HitPosition, 1.0f / 2.0f);
			}
			break;
		}

		// ステージと当たっていたら、ステージと当たった場合の処理を実行する
		if (HitDim.HitNum != 0)
		{
			Chara_AttackHitInfo_ProcessStageHit(
				&AInfo->HitInfo, HitMaterialType, HitPosition, Weapon);
		}

		// 当たり判定結果の後始末
		MV1CollResultPolyDimTerminate(HitDim);
	}

	// すべてのキャラとの当たり判定
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		// 攻撃しているキャラか、構造体が使用されていないか、
		// 既に死んでいる場合は次のループへ
		if (AttackCInfo == CInfo || !CInfo->UseFlag || CInfo->Health <= 0)
		{
			continue;
		}

		// ターゲットのキャラタイプではない場合は次のループへ
		if (CInfo->BaseInfo->Type != TargetType)
		{
			continue;
		}

		// キャラの当たり判定に使用する座標の算出
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->DamageHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);

		// 当たり判定の形状によって処理を分岐
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// ポリゴン
											// 攻撃判定用座標を使用したポリゴンとキャラとの当たり判定を行う
			HitResult = HitCheck_Capsule_Triangle(
				CapsulePos1,
				CapsulePos2,
				CInfo->BaseInfo->DamageHitInfo.Width,
				AInfo->FarPosition[0],
				AInfo->FarPosition[1],
				AInfo->NearPosition[0]
			);

			if (HitResult == FALSE)
			{
				HitResult = HitCheck_Capsule_Triangle(
					CapsulePos1,
					CapsulePos2,
					CInfo->BaseInfo->DamageHitInfo.Width,
					AInfo->NearPosition[0],
					AInfo->FarPosition[1],
					AInfo->NearPosition[1]
				);
			}
			break;

		case ECharaAttackFormType_Sphere:	// 球
											// 攻撃判定用座標を使用した球とキャラとの当たり判定を行う
			HitResult = HitCheck_Capsule_Capsule(
				CapsulePos1,
				CapsulePos2,
				CInfo->BaseInfo->DamageHitInfo.Width,
				AInfo->NearPosition[0],
				AInfo->NearPosition[1],
				SphereSize
			);
			break;
		}

		// 当たっていなかったら次のループへ
		if (HitResult == FALSE)
		{
			continue;
		}

		// 既に当たっていたら次のループへ
		if (!Chara_AttackHitInfo_AddChara(&AInfo->HitInfo, CInfo))
		{
			continue;
		}

		// 攻撃の方向を算出
		AttackDirection = VSub(CInfo->Position, AttackCInfo->Position);
		AttackDirection.y = 0.0f;
		AttackDirection = VNorm(AttackDirection);

		// 攻撃が当たった座標を算出
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// ポリゴン
			HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
			HitPosition = VAdd(HitPosition, AInfo->FarPosition[0]);
			HitPosition = VAdd(HitPosition, AInfo->FarPosition[1]);
			HitPosition = VScale(HitPosition, 1.0f / 4.0f);
			break;

		case ECharaAttackFormType_Sphere:	// 球
			HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
			HitPosition = VScale(HitPosition, 1.0f / 2.0f);
			break;
		}

		// キャラに攻撃が当たった場合の処理を行う
		if (!Chara_AttackHitInfo_ProcessCharaHit(&AInfo->HitInfo, CInfo, HitPosition,
			AttackDirection, DamageType, Weapon, DamagePoint))
		{
			return false;
		}
	}

	// デバッグ情報を描画するかどうかをチェック
	if (System_GetDispDebugInfo())
	{
		// デバッグ情報描画

		// Ｚバッファを使用、Ｚバッファへの書き込みあり、ライティング無しに設定
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
		SetUseLighting(FALSE);

		// 当たり判定の形状を描画
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:
			DrawTriangle3D(AInfo->FarPosition[0], AInfo->FarPosition[1],
				AInfo->NearPosition[0], GetColor(0, 255, 0), TRUE);
			DrawTriangle3D(AInfo->NearPosition[0], AInfo->FarPosition[1],
				AInfo->NearPosition[1], GetColor(0, 255, 0), TRUE);
			break;

		case ECharaAttackFormType_Sphere:
			DrawCapsule3D(AInfo->NearPosition[0], AInfo->NearPosition[1],
				SphereSize, 12, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);
			break;
		}

		// Ｚバッファを使用しない、Ｚバッファへの書き込みもしない、ライティングありに設定
		SetUseLighting(TRUE);
		SetWriteZBufferFlag(FALSE);
		SetUseZBufferFlag(FALSE);
	}

	// 正常終了
	return true;
}
/*
//戦闘データのやり取りなど。
#include"Chara.h"
#include"DxLib.h"
#include"Chara_Player.h"
#include"Key.h"
#include"Camera.h"
#include"System.h"


_CHARA_MGR CHARA_MGR[100];

int Model_Kind[model_Max];
int Model_Anim[model_Max][anim_Max];
int Attach_Model;
float Total_Anim_time;
float Play_Anim_time;
bool Anim_Flg[10];
int Root_flm;




float RATETION_TBL[] = {

	0.0f,		//direction_Down
	90.0f,		//direction_Left
	180.0f,		//direction_Up
	270.0f,		//direction_Right

};
*/
/*

	アニメーションを設定する
	int modelHandle			:モデルハンドル
	int *attachModleHandle	:アタッチモデルハンドル
	int ChangeAnimHandle	:切り替えるアニメハンドル
	int animIdex = 0		:アニメーション番号 何も入力がなければ0が設定される
	return	アニメーションの総時間
*/
/*
static int Change_AttachAnim(int modelHandle,int *attachModleHandle,int ChangeAnimHandle,int animIdex = 0) {

	MV1DetachAnim(modelHandle, *attachModleHandle);
	*attachModleHandle = MV1AttachAnim(modelHandle, animIdex, ChangeAnimHandle);
	return MV1GetAttachAnimTotalTime(modelHandle, *attachModleHandle);
}

//初期化
void Chara_Initialize(){

	memset(Anim_Flg,0,10);
	CHARA_MGR[model_Player].Move = true;
	//プレイヤー
	Model_Kind[model_Player] = MV1LoadModel("Data/Charactor/Player/PC.mv1");
	//エネミー
	Model_Kind[model_Goblin] = MV1LoadModel("Data/Charactor/Goblin/Goblin.mv1");
	//プレイヤーアニメーション
	Model_Anim[model_Player][anim_Neutral] = MV1LoadModel("Data/Charactor/Player/Anim_Neutral.mv1");
	Model_Anim[model_Player][anim_Run] = MV1LoadModel("Data/Charactor/Player/Anim_Run.mv1");
	Model_Anim[model_Player][anim_Attack] = MV1LoadModel("Data/Charactor/Player/Anim_Attack1.mv1");
	//エネミーアニメーション
	Model_Anim[model_Goblin][anim_Neutral] = MV1LoadModel("Data/Charactor/Goblin/Anim_Neutral.mv1");
	Model_Anim[model_Goblin][anim_Run] = MV1LoadModel("Data/Charactor/Goblin/Anim_Run.mv1");
	//プレイヤー初期アニメーション設定
	Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);

}

void Chara_Update(){

	//アニメーション進行
	Play_Anim_time += 0.5f;
	if(Play_Anim_time > Total_Anim_time){
		Play_Anim_time = 0.0f;
	}
	MV1SetAttachAnimTime(Model_Kind[model_Player],Attach_Model,Play_Anim_time);

	//プレイヤーモデル設定(走る、idol 切り替え)
	if(CHARA_MGR[model_Player].Move == true){
		if(Anim_Flg[anim_Run] == true){
			Anim_Flg[anim_Run] = false;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Neutral]);
#if false
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}else{
		if(Anim_Flg[anim_Run] == false){
			Anim_Flg[anim_Run] = true;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Run]);

#if false	//関数を作成したのでコメントアウト By.Syara
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Run]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}
	
	

	//モデルのフレームを検索
	Root_flm = MV1SearchFrame(Model_Kind[model_Player],"root");
	MV1SetFrameUserLocalMatrix(Model_Kind[model_Player],Root_flm,MGetIdent());

}

void Chara_Draw(){
	
	float angle = RATETION_TBL[CHARA_MGR[model_Player].Direction] / 180.0f * DX_PI_F;
		angle += CHARA_MGR[model_Player].Angle;

	//キャラの回転
	MV1SetRotationXYZ(Model_Kind[model_Player],VGet(0.0f,angle/*CHARA_MGR[model_Player].Direction*90.0f+GetRad_Direction()*//*,0.0f));
	//キャラの移動
	MV1SetPosition(Model_Kind[model_Player],Get_Player_Pos());
	//キャラの描画
	MV1DrawModel(Model_Kind[model_Player]);

	MV1DrawModel(Model_Kind[model_Goblin]);

}


void Chara_Fainalize(){
	
	for(int i = 0; i > model_Max-1;i++){
		MV1DeleteModel(Model_Kind[i]);
	}

}

void Set_Chara_Direction(int charanum,int direction){
	CHARA_MGR[charanum].Direction = direction;
}

int Get_Chara_Direction(int charanum){
	return CHARA_MGR[charanum].Direction;
}

/*
	角度を設定する
*/
/*
void Set_Chara_Rotation(int charanum, float angle) {
	CHARA_MGR[charanum].Angle = angle;
}
*/
/*
	角度を返却する
*/
/*
float Get_Chara_Rotation(int charanum) {
	return CHARA_MGR[charanum].Angle;
}

void Set_Anim_Flg(int anim_Kind,bool flg){
	Anim_Flg[anim_Kind] = flg;
}

void Set_Move_flg(int model_Kind, bool flg){
	CHARA_MGR[model_Kind].Move = flg;
}

bool Get_Move_flg(int model_Kind){
	return	CHARA_MGR[model_Kind].Move;
}


void Debag_Chara(){

}
*/