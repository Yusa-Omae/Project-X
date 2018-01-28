#ifndef EFFECT_SPHERELOCUS_H
#define EFFECT_SPHERELOCUS_H

#include "DxLib.h"
#include "Effect.h"

// 球の軌跡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_SphereLocus_Initialize(
	// 球の軌跡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
);

// 球の軌跡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Effect_SphereLocus_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
);

// 球の軌跡エフェクトの状態推移処理関数
extern void Effect_SphereLocus_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 球の軌跡エフェクトの描画処理関数
extern void Effect_SphereLocus_Render(
	SEffectInfo *EInfo
	// エフェクト情報構造体のアドレス
);

// 球の軌跡エフェクト処理の準備をする関数
extern void Effect_SphereLocus_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// エフェクトの色
	COLOR_U8 Color,
	
	// 球の大きさ
	float SphereSize
);

// 球の軌跡に使用する座標を追加する
extern void Effect_SphereLocus_AddPosition(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 球の中心座標
	VECTOR NewCenterPos,

	// 球の向きを決定するための座標
	VECTOR NewDirectionPos
);

#endif
