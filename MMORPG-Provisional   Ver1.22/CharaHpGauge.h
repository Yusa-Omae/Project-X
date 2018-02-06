#ifndef CHARAHPGAUGE_H
#define CHARAHPGAUGE_H

// キャラクターの体力ゲージの情報
typedef struct _SCharaHpGauge
{
	// 不透明度
	float Alpha;

	// 体力値( 0.0f ～ 1.0f )
	float Hp;

	// 減った際に使用する以前の体力値
	float DelayHp;
} SCharaHpGauge;

// キャラクターの体力ゲージの処理の準備を行う
extern void CharaHpGaugeSetup(
	// キャラクターの体力ゲージの情報構造体のアドレス
	SCharaHpGauge *GInfo,

	// 初期状態で表示するかどうか( true:表示する  false:非表示 )
	bool Visible,

	// 初期の体力値( 0.0f ～ 1.0f )
	float Hp
);

// キャラクターの体力ゲージの状態推移処理を行う
extern void CharaHpGaugeStep(
	// キャラクターの体力ゲージの情報構造体のアドレス
	SCharaHpGauge *GInfo,
	
	// 推移させる時間( 単位 : 秒 )
	float StepTime,

	// 表示するかどうか( true:表示する  false:非表示 )
	bool Visible,
	
	// 体力値( 0.0f ～ 1.0f )
	float Hp
);

// キャラクターの体力ゲージの描画処理を行う
extern void CharaHpGaugeDraw(
	// キャラクターの体力ゲージの情報構造体のアドレス
	SCharaHpGauge *GInfo,

	// 描画Ｘ座標
	int DrawX,
	
	// 描画Ｙ座標
	int DrawY,

	// 描画するゲージの幅
	int DrawWidth,
	
	// 描画するゲージの高さ
	int DrawHeight
);

#endif
