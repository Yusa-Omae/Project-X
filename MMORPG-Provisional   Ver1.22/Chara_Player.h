#ifndef CHARA_PLAYER_H
#define CHARA_PLAYER_H

#include "Chara.h"

// プレイヤーが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Player_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

// プレイヤーが削除される際に呼ばれる関数
extern void Chara_Player_Delete(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

// プレイヤーの状態推移処理が行われる際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Player_Step(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
);

// プレイヤーがダメージを受けた際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Player_Damage(
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

// プレイヤーが着地した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Player_Landed(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo,

	// デフォルトの処理を行うかどうかのフラグのアドレス
	bool *DefaultProcess
);

// アニメーションイベント「その他」が発生した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Player_AnimOtherEvent(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

/*
キャラクターにアイテムを設定する
*/
void Chara_Player_SetItem(int haveId, int itmeIdex);


/*
キャラクターにアイテムを設定する
return -1でエラー　0以上でアイテム
*/
int Chara_Player_GetItem(int haveId);


/*
キャラクターのお金を設定する
*/
void Chara_Player_SetMoney(int gold);

/*
キャラクターのお金を返却する
return 所持金額
*/
int Chara_Player_GetMoney();


#endif