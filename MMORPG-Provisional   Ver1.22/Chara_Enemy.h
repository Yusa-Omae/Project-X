#ifndef CHARA_ENEMY_H
#define CHARA_ENEMY_H

#include "Chara.h"

// 敵が作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Enemy_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

// 敵の状態推移処理が行われる際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Enemy_Step(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
);

// 敵がダメージを受けた際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Enemy_Damage(
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
);

// 敵がメッセージが送られてきたときに呼ばれる関数
extern bool Chara_Enemy_MessageRecv(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// メッセージを送ってきたキャラクター情報構造体のアドレス
	SCharaInfo *SendCInfo,

	// メッセージ識別用の値
	int Message
);

#endif