#include "Effect_Dead.h"
#include "Mathematics.h"
#include <math.h>

// ポリゴンの面積に対するパーティクル作整数
#define CREATE_RATE			(0.025f)

// パーティクル上昇待ち時間の最大と最小
#define MAX_UPWAIT			(1.0f)
#define MIN_UPWAIT			(0.01f)

// パーティクル上昇速度の最大と最小
#define MAX_UPSPEED			(400.0f)
#define MIN_UPSPEED			(100.0f)

// パーティクルの上昇加速度
#define UPACCEL				(500.0f)

// パーティクル回転運動速度の最大と最小
#define MAX_ANGLESPEED		(6.0f)
#define MIN_ANGLESPEED		(1.5f)

// パーティクル座標からの水平最大距離の最大と最小
#define MAX_H_DISTANCE		(100.0f)
#define MIN_H_DISTANCE		(20.0f)

// パーティクル座標から水平方向に離れていく速度
#define H_DISTANCE_SPEED	(200.0f)

// パーティクルの最大表示時間と最小表示時間
#define MAX_VISIBLETIME		(0.5f)
#define MIN_VISIBLETIME		(0.2f)

// パーティクルの不透明度上昇速度と減少速度
#define ALPHA_UP_SPEED		(16.0f)
#define ALPHA_DOWN_SPEED	(1.0f)

// パーティクルの最大サイズと最小サイズ
#define MAX_SIZE			(16.0f)
#define MIN_SIZE			(4.0f)

// パーティクルの発生時の拡大速度
#define SIZERATE_SPEED		(3.0f)

// 計算誤差を考慮して余分に確保するパーティクル情報の数
#define ADD_PARTICLE_NUM	(10)


// 死亡エフェクトのポリゴン一つの情報
typedef struct _SEffect_Dead_PolygonInfo
{
	// ポリゴン面積
	float                      AreaSize;

	// ポリゴンの頂点０から頂点１へのベクトル
	VECTOR                     Vec1;

	// ポリゴンの頂点０から頂点２へのベクトル
	VECTOR                     Vec2;

	// ポリゴンの頂点０の座標
	VECTOR                     Pos;
} SEffect_Dead_PolygonInfo;

// 死亡エフェクトのパーティクル一つの情報
typedef struct _SEffect_Dead_ParticleInfo
{
	// 座標
	VECTOR                     Position;

	// 上昇待ち時間
	float                      UpWait;

	// 上昇最大速度
	float                      UpMaxSpeed;

	// 上昇速度
	float                      UpSpeed;

	// 角度
	float                      Angle;

	// 角速度
	float                      AngleSpeed;

	// パーティクル座標からの最大水平距離
	float                      H_DistanceMax;

	// パーティクル座標からの水平距離
	float                      H_Distance;

	// 表示時間
	float                      VisibleTime;

	// 不透明度
	float                      Alpha;

	// 大きさ
	float                      Size;

	// 大きさ率
	float                      SizeRate;
} SEffect_Dead_ParticleInfo;

// 死亡エフェクトの情報
typedef struct _SEffect_DeadInfo
{
	// エフェクトの色
	COLOR_U8                   Color;

	// パーティクルの数
	int                        ParticleNum;

	// パーティクルの情報を格納しているメモリ領域のアドレス
	SEffect_Dead_ParticleInfo *Particle;
} SEffect_DeadInfo;

static int                       g_PolyInfoBufferSize;
static SEffect_Dead_PolygonInfo *g_PolyInfoBuffer;

// 死亡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_Dead_Initialize(
	// 死亡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
)
{
	// 死亡エフェクトで使用する画像の読み込み
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\Dead.png" );

	// 画像の読み込みに失敗したら失敗終了
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// 成功終了
	return true;
}

// 死亡エフェクトの基本情報後始末関数
void Effect_Dead_Terminate(
	// 死亡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
)
{
	// ポリゴン情報格納用に確保していたメモリ領域を解放する
	if( g_PolyInfoBuffer != NULL )
	{
		free( g_PolyInfoBuffer );
		g_PolyInfoBuffer = NULL;
	}
}

// 死亡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Effect_Dead_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo *DInfo;

	// エフェクトの情報構造体を格納するメモリ領域の確保
	EInfo->SubData = malloc( sizeof( SEffect_DeadInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	DInfo = ( SEffect_DeadInfo * )EInfo->SubData;

	// パーティクルの情報を格納するメモリ領域のアドレスを保存する変数を初期化する
	DInfo->Particle    = NULL;

	// 正常終了
	return true;
}

// 死亡エフェクトが削除された際に呼ばれる関数
void Effect_Dead_Delete(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;

	// パーティクルの情報を格納していたメモリ領域を解放する
	if( DInfo->Particle != NULL )
	{
		free( DInfo->Particle );
		DInfo->Particle = NULL;
	}
}

// 死亡エフェクトの状態推移処理関数
void Effect_Dead_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	int                        ValidNum;

	// 有効なパーティクルの数を初期化
	ValidNum = 0;

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < DInfo->ParticleNum; i++, PInfo++ )
	{
		// 不透明度が０以下の場合は次のループへ
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// 有効なパーティクルの数を増やす
		ValidNum++;

		// 大きさ率が 1.0f 未満の場合は大きさ率を上げる
		if( PInfo->SizeRate < 0.99999f )
		{
			PInfo->SizeRate += StepTime * SIZERATE_SPEED;
			if( PInfo->SizeRate > 1.0f )
			{
				PInfo->SizeRate = 1.0f;
			}
		}
		else
		{
			// 角度を変更
			PInfo->Angle += StepTime * PInfo->AngleSpeed;

			// 表示間が０より大きいかどうかで処理を分岐
			if( PInfo->VisibleTime > 0.0f )
			{
				// 表示時間が０より大きい場合は表示時間を減らす
				PInfo->VisibleTime -= StepTime;
			}
			else
			{
				// 表示時間が０以下だったら不透明度を減らす
				PInfo->Alpha -= StepTime * ALPHA_DOWN_SPEED;
			}

			// 上昇待ち時間が０より大きいかどうかで処理を分岐
			if( PInfo->UpWait > 0.0f )
			{
				// 上昇待ち時間が０より大きかったら上昇待ち時間を減らす
				PInfo->UpWait -= StepTime;
			}
			else
			{
				// 上昇待ち時間が０以下の場合

				// 上昇速度を上げる
				if( PInfo->UpSpeed < PInfo->UpMaxSpeed )
				{
					PInfo->UpSpeed += StepTime * UPACCEL;
					if( PInfo->UpSpeed > PInfo->UpMaxSpeed )
					{
						PInfo->UpSpeed = PInfo->UpMaxSpeed;
					}
				}

				// パーティクルを上昇させる
				PInfo->Position.y += PInfo->UpSpeed * StepTime;

				// パーティクル座標からの水平方向の距離を加算
				if( PInfo->H_Distance < PInfo->H_DistanceMax )
				{
					PInfo->H_Distance += StepTime * H_DISTANCE_SPEED;
					if( PInfo->H_Distance > PInfo->H_DistanceMax )
					{
						PInfo->H_Distance = PInfo->H_DistanceMax;
					}
				}
			}
		}
	}

	// 終了リクエストがされていて、有効なパーティクルの数も０だったらエフェクトを削除する
	if( EInfo->EndRequest || ValidNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// 死亡エフェクトの描画処理関数
void Effect_Dead_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	VECTOR                     DrawPos;

	// Ｚバッファを使用する設定に変更する
	SetUseZBufferFlag( TRUE );

	// Ｚバッファへの書き込みは行わない
	SetWriteZBufferFlag( FALSE );

	// 描画色をセット
	SetDrawBright( DInfo->Color.r, DInfo->Color.g, DInfo->Color.b );

	// パーティクルの数だけ繰り返し
	PInfo = DInfo->Particle;
	for( i = 0; i < DInfo->ParticleNum; i++, PInfo++ )
	{
		// 不透明度が０以下のパーティクルは何もしない
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// 減算ブレンドに設定
		SetDrawBlendMode( DX_BLENDMODE_SUB, ( int )( PInfo->Alpha * 255.0f ) );

		// パーティクルを描画する座標の算出
		DrawPos = PInfo->Position;
		DrawPos.x += sin( PInfo->Angle ) * PInfo->H_Distance;
		DrawPos.z += cos( PInfo->Angle ) * PInfo->H_Distance;

		// パーティクルを描画
		DrawBillboard3D( DrawPos, 0.5f, 0.5f, PInfo->Size * PInfo->SizeRate,
									0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// 描画ブレンドモードを標準に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// 描画輝度も標準に戻す
	SetDrawBright( 255,255,255 );

	// Ｚバッファを使用しない設定に戻す
	SetUseZBufferFlag( FALSE );
}

// 死亡エフェクト処理の準備をする関数
//     戻り値 : 準備が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_Dead_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,

	// 死亡エフェクトを実行する３Ｄモデルのハンドル
	int ModelHandle,

	// 死亡エフェクトの色
	COLOR_U8 Color
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	float                      CreateNum;
	VECTOR                    *Pos2;
	VECTOR                    *Pos3;
	MV1_REF_POLYGONLIST        RefMesh;
	SEffect_Dead_PolygonInfo  *PolyInfo;
	float                      TotalAreaSize;
	int                        TempParticleNum;
	float                      Rate1;
	float                      Rate2;

	// エフェクトの色を保存
	DInfo->Color = Color;

	// エフェクトを実行する３Ｄモデルの参照用メッシュをセットアップ
	MV1RefreshReferenceMesh( ModelHandle, -1, TRUE, TRUE );

	// 参照用メッシュを取得
	RefMesh = MV1GetReferenceMesh( ModelHandle, -1, TRUE, TRUE );

	// ポリゴンの数が多くて既に確保しているポリゴン情報格納用のメモリ領域に
	// 入りきらない場合はメモリ領域を再確保する
	if( g_PolyInfoBufferSize < RefMesh.PolygonNum )
	{
		// 既に確保していたメモリ領域がある場合は解放
		if( g_PolyInfoBuffer != NULL )
		{
			free( g_PolyInfoBuffer );
			g_PolyInfoBuffer = NULL;
		}

		// ポリゴン情報格納用のメモリ領域を確保
		g_PolyInfoBuffer = ( SEffect_Dead_PolygonInfo * )malloc(
							sizeof( SEffect_Dead_PolygonInfo ) * RefMesh.PolygonNum );

		// メモリ領域の確保に失敗した場合は失敗終了
		if( g_PolyInfoBuffer == NULL )
		{
			return false;
		}

		// 確保したメモリ領域に入るポリゴン情報の数を保存
		g_PolyInfoBufferSize = RefMesh.PolygonNum;
	}

	// ポリゴンの情報を構築する
	PolyInfo   = g_PolyInfoBuffer;
	TotalAreaSize  = 0.0f;
	for( i = 0; i < RefMesh.PolygonNum; i++, PolyInfo++ )
	{
		// ポリゴンの座標を取得
		PolyInfo->Pos  = RefMesh.Vertexs[  RefMesh.Polygons[ i ].VIndex[ 0 ] ].Position;
		Pos2           = &RefMesh.Vertexs[ RefMesh.Polygons[ i ].VIndex[ 1 ] ].Position;
		Pos3           = &RefMesh.Vertexs[ RefMesh.Polygons[ i ].VIndex[ 2 ] ].Position;

		// ポリゴンの座標０から座標１や座標２へのベクトルを算出
		PolyInfo->Vec1 = VSub( *Pos2, PolyInfo->Pos );
		PolyInfo->Vec2 = VSub( *Pos3, PolyInfo->Pos );

		// ポリゴンの面積を算出
		PolyInfo->AreaSize = VSize( VCross( PolyInfo->Vec1, PolyInfo->Vec2 ) ) / 2.0f;

		// ポリゴンの面積の合計に加算
		TotalAreaSize += PolyInfo->AreaSize;
	}

	// 確保するメモリ領域に格納するパーティクル情報用の数を算出
	TempParticleNum = ( int )( TotalAreaSize * CREATE_RATE ) + ADD_PARTICLE_NUM;

	// パーティクル情報を格納するメモリ領域の確保
	DInfo->Particle    = ( SEffect_Dead_ParticleInfo * )malloc(
								TempParticleNum * sizeof( SEffect_Dead_ParticleInfo ) );

	// メモリ領域の確保に失敗したらエラー終了
	if( DInfo->Particle == NULL )
	{
		return false;
	}

	// ポリゴンの数だけ繰り返し
	CreateNum = 0.0f;
	PInfo      = DInfo->Particle;
	PolyInfo   = g_PolyInfoBuffer;
	DInfo->ParticleNum = 0;
	for( i = 0; i < RefMesh.PolygonNum; i++, PolyInfo++ )
	{
		// パーティクル作成個数カウンタにポリゴン面積あたりのパーティクルの数を加算する
		CreateNum += PolyInfo->AreaSize * CREATE_RATE;

		// パーティクル作成個数カウンタが1.0f以上ならループ
		while( CreateNum >= 1.0f )
		{
			// パーティクル作成個数カウンタから1.0fを引く
			CreateNum        -= 1.0f;

			// パーティクルの座標をポリゴン表面上からランダムで決定
			Rate1              = GetRandomFloat( 1.0f,         0.0f );
			Rate2              = GetRandomFloat( 1.0f - Rate1, 0.0f );
			PInfo->Position    = VAdd( PolyInfo->Pos,
				VAdd( VScale( PolyInfo->Vec1, Rate1 ), VScale( PolyInfo->Vec2, Rate2 )));

			// パーティクルの上昇待ち時間をランダムで決定
			PInfo->UpWait      = GetRandomFloat( MAX_UPWAIT,  MIN_UPWAIT  );

			// パーティクルの上昇最大速度をランダムで決定
			PInfo->UpMaxSpeed  = GetRandomFloat( MAX_UPSPEED, MIN_UPSPEED );

			// パーティクルの上昇速度を初期化
			PInfo->UpSpeed     = 0.0f;

			// パーティクルの初期角度をランダムで決定
			PInfo->Angle       = GetRandomFloat( DX_TWO_PI_F,    0.0f );

			// パーティクルの角速度をランダムで決定
			PInfo->AngleSpeed  = GetRandomFloat( MAX_ANGLESPEED, MIN_ANGLESPEED );

			// 半分の確立で角速度の向きを反転
			if( GetRand( 100 ) < 50 )
			{
				PInfo->AngleSpeed = -PInfo->AngleSpeed;
			}

			// パーティクル座標からの最大水平距離をランダムで決定
			PInfo->H_DistanceMax = GetRandomFloat( MAX_H_DISTANCE, MIN_H_DISTANCE );

			// パーティクル座標からの水平距離を初期化
			PInfo->H_Distance    = 0.0f;

			// パーティクルの表示時間をランダムで決定
			PInfo->VisibleTime = GetRandomFloat( MAX_VISIBLETIME, MIN_VISIBLETIME );

			// パーティクルの不透明度を初期化
			PInfo->Alpha       = 1.0f;

			// パーティクルの大きさをランダムで決定
			PInfo->Size        = GetRandomFloat( MAX_SIZE, MIN_SIZE );

			// パーティクルの大きさ率を初期化
			PInfo->SizeRate    = 0.0f;

			// ポインタのアドレスをパーティクル一つ分進める
			PInfo++;

			// パーティクルの数を一つ増やす
			DInfo->ParticleNum++;
		}
	}

	// 成功終了
	return true;
}


