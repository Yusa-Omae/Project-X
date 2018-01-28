#ifndef EFFECT_SLASHLOCUS_H
#define EFFECT_SLASHLOCUS_H

#include "DxLib.h"
#include "Effect.h"

// 剣の軌跡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_SlashLocus_Initialize(
	// 剣の軌跡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
);

// 剣の軌跡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Effect_SlashLocus_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 剣の軌跡エフェクトの状態推移処理関数
extern void Effect_SlashLocus_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 剣の軌跡エフェクトの描画処理関数
extern void Effect_SlashLocus_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 剣の軌跡エフェクト処理の準備をする関数
extern void Effect_SlashLocus_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 剣の軌跡エフェクトの色
	COLOR_U8 Color
);

// 剣の軌跡に使用する座標を追加する
extern void Effect_SlashLocus_AddPosition(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 剣の柄側の座標
	VECTOR NewNearPos,
	
	// 剣の刃先側の座標
	VECTOR NewFarPos
);

#endif
