#ifndef CHARA_ENEMYBASE_H
#define CHARA_ENEMYBASE_H

#include "DxLib.h"
#include "TextParam.h"
#include "Mathematics.h"
#include "CharaBase.h"

// 敵１体辺りの攻撃パターンの最大数
#define ENEMY_ATTACK_MAX_NUM			(3)

// 攻撃パターン情報
typedef struct _SChara_EnemyBaseAtkInfo
{
	// 攻撃アニメーションを開始する距離
	float    AtkDistance;

	// 攻撃アニメーションを開始する相手と自分の向いている方向との角度範囲
	float    AtkAngleRange;

	// 攻撃力
	int      Atk;

	// 攻撃の為に相手に接近する際に歩くかどうか
	// ( true : 歩く  false : 走る )
	bool     IsAtkMoveWalk;

	// 攻撃中も向いている方向を変化させるかどうか
	// ( true : 変化させる  false : 変化させない )
	bool     IsAtkAngleChange;
} SChara_EnemyBaseAtkInfo;

// 敵の基本情報
typedef struct _SChara_EnemyBaseInfo
{
	// 視界の角度範囲
	float    SeeAngleRange;

	// 向きを変える速度
	float    AngleChangeSpeed;

	// 相手が視界に入っている状態で存在に気付く距離
	float    NoticeDistance_See;

	// 相手が視界に入っていなくても存在に気付く距離
	float    NoticeDistance;

	// 相手が視界に入っていなくても存在に気付く距離( 臨戦態勢時 )
	float    NoticeDistance_AtkMode;

	// 相手が視界に入っている場合に相手の存在に気付くまで時間
	float    NoticeTime_See;

	// 相手が存在に気付く距離に居る場合に実際に気付くまでの時間
	float    NoticeTime_Distance;

	// 相手が視界に入っている場合に実際に攻撃行動に移るまでの時間
	SMaxMinF SeeAtkMoveStartWait;

	// 一度攻撃をしてから再度攻撃行動に移るまでの時間
	SMaxMinF AtkRepeatWait;

	// ダメージを受けてから再度行動を開始するまでの時間
	SMaxMinF DamageAfterMoveWait;

	// 臨戦態勢ではないときに徘徊を開始するまでの時間
	SMaxMinF IdleMoveStartWait;

	// 臨戦態勢ではないときに一度の徘徊で移動する時間
	SMaxMinF IdleMoveTime;

	// 臨戦態勢ではないときに向きを変更する場合に、向きを変更を開始するまでの時間
	float    IdleAngleMoveBeginWait;

	// 臨戦態勢ではないときに向きの変更をした後に次の行動を開始するまでの時間
	float    IdleAngleMoveAfterWait;

	// プログラムによる移動を行うかどうか
	bool     ProgramMove;

	// 攻撃情報の数
	int      AtkNum;

	// 攻撃情報
	SChara_EnemyBaseAtkInfo AtkInfo[ ENEMY_ATTACK_MAX_NUM ];
} SChara_EnemyBaseInfo;

// 敵の基本情報の初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した  false : 失敗した )
extern bool Chara_EnemyBase_Initialize(
	// キャラクターの基本情報構造体のアドレス
	SCharaBaseInfo *CBInfo,

	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TextParam
);

#endif
