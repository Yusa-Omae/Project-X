#include "CharaHpGauge.h"
#include "DxLib.h"
#include "Mathematics.h"

// 不透明度の変化速度
#define ALPHA_SPEED				(3.0f)

// 遅れて本来の体力値になる表現の本来の体力値に近づく速度
#define DELAY_PARAM_SPEED		(1.0f)

// 体力ゲージの体力がある部分の色
#define GAUGE_COLOR_R			(220)
#define GAUGE_COLOR_G			(220)
#define GAUGE_COLOR_B			(0)

// 体力ゲージの体力下無くなった部分の色
#define GAUGE_BASE_COLOR_R		(200)
#define GAUGE_BASE_COLOR_G		(0)
#define GAUGE_BASE_COLOR_B		(0)

// 体力ゲージの減ってしまった部分の色
#define GAUGE_DELAY_COLOR_R		(175)
#define GAUGE_DELAY_COLOR_G		(165)
#define GAUGE_DELAY_COLOR_B		(0)

// キャラクターの体力ゲージの処理の準備を行う
void CharaHpGaugeSetup(
	// キャラクターの体力ゲージの情報構造体のアドレス
	SCharaHpGauge *GInfo,

	// 初期状態で表示するかどうか( true:表示する  false:非表示 )
	bool Visible,

	// 初期の体力値( 0.0f ～ 1.0f )
	float Hp
)
{
	// 不透明度は初期の表示状態によって１００％又は０％
	GInfo->Alpha       = Visible ? 1.0f : 0.0f;

	// 初期の体力値を保存
	GInfo->Hp      = Hp;
	GInfo->DelayHp = GInfo->Hp;
}

// キャラクターの体力ゲージの状態推移処理を行う
void CharaHpGaugeStep(
	// キャラクターの体力ゲージの情報構造体のアドレス
	SCharaHpGauge *GInfo,
	
	// 推移させる時間( 単位 : 秒 )
	float StepTime,

	// 表示するかどうか( true:表示する  false:非表示 )
	bool Visible,
	
	// 体力値( 0.0f ～ 1.0f )
	float Hp_flow
)
{
	bool MoveEnd;

	// 体力が減る場合は減る前の体力値を減る表現で使用する変数に代入する
	if( GInfo->Hp < Hp_flow)
	{
		GInfo->DelayHp = GInfo->Hp;
	}

	// 体力値の保存
	GInfo->Hp = Hp_flow;

	// 体力が減った際の減る前の体力の値を本来の体力の値に近づける
	MoveEnd = ParamChangeFloat( &GInfo->DelayHp, GInfo->Hp,
									StepTime, DELAY_PARAM_SPEED );

	// 体力の減る表現が終わっているか、表示する指定になっている場合は不透明度を変化させる
	if( MoveEnd || Visible )
	{
		ParamChangeFloat( &GInfo->Alpha, Visible ? 1.0f : 0.0f, StepTime, ALPHA_SPEED );
	}
}

// キャラクターの体力ゲージの描画処理を行う
void CharaHpGaugeDraw(
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
)
{
	int Alpha;
	int Param;
	int Delay;

	// 有効な体力ゲージの情報アドレスが渡されなかった場合は体力０として表現する
	if( GInfo == NULL )
	{
		Alpha = 255;
		Param = 0;
		Delay = 0;
	}
	else
	{
		// アルファ値に算出
		Alpha = ( int )( GInfo->Alpha * 255.0f );

		// アルファ値が０の場合は何もせずに終了
		if( Alpha <= 0 )
		{
			return;
		}

		// 体力の描画幅を算出
		Param = ( int )( DrawWidth * GInfo->Hp );

		// 以前の体力の描画幅を算出
		Delay = ( int )( DrawWidth * GInfo->DelayHp );
	}

	// アルファブレンドモードに設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, Alpha );

	// 体力のある部分を描画
	DrawBox( DrawX,         DrawY, DrawX + Param,     DrawY + DrawHeight,
		GetColor( GAUGE_COLOR_R,       GAUGE_COLOR_G,       GAUGE_COLOR_B       ), TRUE );

	// 体力が以前あった部分を描画
	DrawBox( DrawX + Param, DrawY, DrawX + Delay,     DrawY + DrawHeight,
		GetColor( GAUGE_DELAY_COLOR_R, GAUGE_DELAY_COLOR_G, GAUGE_DELAY_COLOR_B ), TRUE );

	// 体力が無い部分を描画
	DrawBox( DrawX + Delay, DrawY, DrawX + DrawWidth, DrawY + DrawHeight,
		GetColor( GAUGE_BASE_COLOR_R,  GAUGE_BASE_COLOR_G,  GAUGE_BASE_COLOR_B  ), TRUE );

	// ブレンドモードを標準に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}





