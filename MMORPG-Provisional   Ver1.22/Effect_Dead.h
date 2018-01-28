#ifndef EFFECT_DEAD_H
#define EFFECT_DEAD_H

#include "DxLib.h"
#include "Effect.h"

// 死亡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_Dead_Initialize(
	// 死亡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
);

// 死亡エフェクトの基本情報後始末関数
extern void Effect_Dead_Terminate(
	// 死亡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
);

// 死亡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Effect_Dead_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 死亡エフェクトが削除された際に呼ばれる関数
extern void Effect_Dead_Delete(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 死亡エフェクトの状態推移処理関数
extern void Effect_Dead_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 死亡エフェクトの描画処理関数
extern void Effect_Dead_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 死亡エフェクト処理の準備をする関数
//     戻り値 : 準備が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_Dead_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// 死亡エフェクトを実行する３Ｄモデルのハンドル
	int ModelHandle,

	// 死亡エフェクトの色
	COLOR_U8 Color
);

#endif
