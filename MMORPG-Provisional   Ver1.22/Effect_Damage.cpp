#include "Effect_Damage.h"
#include "Mathematics.h"

// パーティクルが飛んでいく最高速度と最低速度
#define MAX_SPEED			(750.0f)
#define MIN_SPEED			(200.0f)

// パーティクルの最高表示時間と最低表示時間
#define MAX_TIME			(0.1f)
#define MIN_TIME			(0.05f)

// パーティクルがフェードアウトする速度
#define ALPHA_SPEED			(16.0f)

// パーティクルの最大サイズと最小サイズ
#define MAX_SIZE			(16.0f)
#define MIN_SIZE			(4.0f)

// パーティクルの減速の速度
#define SUB_SPEED			(3000.0f)

// パーティクルの数
#define PARTICLE_NUM		(256)

// ダメージエフェクトのパーティクル一つの情報
typedef struct _SEffect_Damage_ParticleInfo
{
	// 座標
	VECTOR                      Position;

	// 進行方向
	VECTOR                      Direction;

	// スピード
	float                       Speed;

	// 大きさ
	float                       Size;

	// 表示時間
	float                       VisibleTime;

	// 不透明度
	float                       Alpha;
} SEffect_Damage_ParticleInfo;

// ダメージエフェクトの情報
typedef struct _SEffect_DamageInfo
{
	// ダメージエフェクトの色
	COLOR_U8                    Color;

	// パーティクルの情報
	SEffect_Damage_ParticleInfo Particle[ PARTICLE_NUM ];
} SEffect_DamageInfo;

// ダメージエフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_Damage_Initialize(
	// ダメージエフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
)
{
	// ダメージエフェクトで使用する画像の読み込み
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\Damage.png" );

	// 画像の読み込みに失敗したら失敗終了
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// 成功終了
	return true;
}

// ダメージエフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Effect_Damage_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	// エフェクトの情報構造体を格納するメモリ領域の確保
	EInfo->SubData = malloc( sizeof( SEffect_DamageInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// 正常終了
	return true;
}

// ダメージエフェクトの状態推移処理関数
void Effect_Damage_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int                          i;
	int                          ValidNum;

	// 有効なパーティクルの数を初期化
	ValidNum = 0;

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// 不透明度が０以下の場合は次のループへ
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// 有効なパーティクルの数を増やす
		ValidNum++;

		// 速度が０より大きいかどうかで処理を分岐
		if( PInfo->Speed > 0.0f )
		{
			// 速度が０より大きかったらパーティクルを移動する
			PInfo->Position = VAdd( PInfo->Position,
									VScale( PInfo->Direction, PInfo->Speed * StepTime) );

			// 減速処理
			PInfo->Speed -= SUB_SPEED * StepTime;

			// 速度が０以下になったら残り表示時間を０にする
			if( PInfo->Speed <= 0.0f )
			{
				PInfo->VisibleTime = 0.0f;
			}
		}

		// 表示時間が０より大きいかどうかで処理を分岐
		if( PInfo->VisibleTime > 0.0f )
		{
			// 表示時間が０より大きかったら表示時間を減らす
			PInfo->VisibleTime -= StepTime;
		}
		else
		{
			// 表示時間が０以下だったら不透明度を減らす
			PInfo->Alpha -= ALPHA_SPEED * StepTime;
		}
	}

	// 終了リクエストがされていて、有効なパーティクルの数も０だったらエフェクトを削除する
	if( EInfo->EndRequest || ValidNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// ダメージエフェクトの描画処理関数
void Effect_Damage_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int                          i;

	// Ｚバッファを使用する設定に変更する
	SetUseZBufferFlag( TRUE );

	// Ｚバッファへの書き込みは行わない
	SetWriteZBufferFlag( FALSE );

	// 描画色をセット
	SetDrawBright( DInfo->Color.r, DInfo->Color.g, DInfo->Color.b );

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// 不透明度が０以下のパーティクルは何もしない
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// 減算ブレンドでパーティクルを描画
		SetDrawBlendMode( DX_BLENDMODE_SUB, ( int )( PInfo->Alpha * 255.0f ) );
		DrawBillboard3D( PInfo->Position, 0.5f, 0.5f, PInfo->Size * PInfo->Alpha,
											0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// 不透明度が０以下のパーティクルは何もしない
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// 加算ブレンドでパーティクルを描画
		SetDrawBlendMode( DX_BLENDMODE_ADD_X4, ( int )( PInfo->Alpha * 255.0f ) );
		DrawBillboard3D( PInfo->Position, 0.5f, 0.5f, PInfo->Size * PInfo->Alpha,
											0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// 描画ブレンドモードを標準に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// 描画輝度も標準に戻す
	SetDrawBright( 255,255,255 );

	// Ｚバッファを使用しない設定に戻す
	SetUseZBufferFlag( FALSE );
}

// ダメージエフェクト処理の準備をする関数
void Effect_Damage_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// ダメージエフェクトを発生させる座標
	VECTOR Position,

	// ダメージエフェクトの色
	COLOR_U8 Color
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int i;

	// エフェクトの色を保存
	DInfo->Color = Color;

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// 座標を保存
		PInfo->Position    = Position;

		// パーティクルの飛ぶ方向をランダムで決定
		PInfo->Direction.x = GetRandomFloat( 1.0f, -1.0f );
		PInfo->Direction.y = GetRandomFloat( 1.0f, -1.0f );
		PInfo->Direction.z = GetRandomFloat( 1.0f, -1.0f );

		// パーティクルの速度をランダムで決定
		PInfo->Speed       = GetRandomFloat( MAX_SPEED, MIN_SPEED );

		// パーティクルの不透明度を初期化
		PInfo->Alpha       = 1.0f;

		// パーティクルの大きさをランダムで決定
		PInfo->Size        = GetRandomFloat( MAX_SIZE, MIN_SIZE );

		// パーティクルの表示時間をランダムで決定
		PInfo->VisibleTime = GetRandomFloat( MAX_TIME, MIN_TIME  );
	}
}


