#ifndef EFFECT_DAMAGE_H
#define EFFECT_DAMAGE_H

#include "DxLib.h"
#include "Effect.h"

// ダメージエフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_Damage_Initialize(
	// ダメージエフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
);

// ダメージエフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Effect_Damage_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// ダメージエフェクトの状態推移処理関数
extern void Effect_Damage_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ダメージエフェクトの描画処理関数
extern void Effect_Damage_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// ダメージエフェクト処理の準備をする関数
extern void Effect_Damage_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// ダメージエフェクトを発生させる座標
	VECTOR Position,

	// ダメージエフェクトの色
	COLOR_U8 Color
);

#endif
