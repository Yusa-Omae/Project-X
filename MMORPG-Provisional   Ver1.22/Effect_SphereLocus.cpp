#include "Effect_SphereLocus.h"
#include "Mathematics.h"
#include <math.h>

// 軌跡を表示する時間
#define DISPLAY_TIME		(0.15f)

// 軌跡を形成する座標の間を補間関数で分割する数
#define UNITVERTNUM			(4)

// 球を表現する頂点の数
#define CIRCLEVERTNUM		(16)

// 軌跡を形成する座標の数
#define POSITIONNUM			(32)

// 球の軌跡を形成する座標の情報
typedef struct _SEffect_SphereLocus_Position
{
	// 球の中心座標
	VECTOR                       CenterPos;

	// 球の向きを決定するための座標
	VECTOR                       DirectionPos;

	// 不透明度
	float                        Alpha;
} SEffect_SphereLocus_Position;

// 球の軌跡エフェクトの情報
typedef struct _SEffect_SphereLocusInfo
{
	// エフェクトの色
	COLOR_U8                     Color; 

	// 球の大きさ
	float                        SphereSize;

	// 球の軌跡を形成する有効な座標の数
	int                          PositionNum;

	// 球の軌跡を形成する座標の情報
	SEffect_SphereLocus_Position Position[ POSITIONNUM ];
} SEffect_SphereLocusInfo;

// 球の描画で使用する Sin, Cos の値を予め計算して格納しておくための配列
static float g_EffSphereSinCos[ CIRCLEVERTNUM ][ 2 ];

// 球の軌跡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_SphereLocus_Initialize(
	// 球の軌跡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
)
{
	int i;

	// 球の軌跡エフェクトで使用する画像の読み込み
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\SphereLocus.png" );

	// 画像の読み込みに失敗したら失敗終了
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// 球の頂点座標形成に使用する Sin, Cos の値を予め算出しておく
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		g_EffSphereSinCos[ i ][ 0 ] = ( float )cos( DX_TWO_PI_F / CIRCLEVERTNUM * i );
		g_EffSphereSinCos[ i ][ 1 ] = ( float )sin( DX_TWO_PI_F / CIRCLEVERTNUM * i );
	}

	// 成功終了
	return true;
}

// 球の軌跡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Effect_SphereLocus_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	// エフェクトの情報構造体を格納するメモリ領域の確保
	EInfo->SubData = malloc( sizeof( SEffect_SphereLocusInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// 正常終了
	return true;
}

// 球の軌跡エフェクトの状態推移処理関数
void Effect_SphereLocus_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	int                      i;
	int                      PosNum;

	// 追加されている座標の数だけ繰り返し
	PosNum = SInfo->PositionNum;
	for( i = 0; i < PosNum; i++ )
	{
		// 座標の不透明度を減少させる
		SInfo->Position[ i ].Alpha -= StepTime * ( 1.0f / DISPLAY_TIME );

		// 座標の不透明度が０以下になっていたら有効な座標の数を減らす
		if( SInfo->Position[ i ].Alpha <= 0.0f )
		{
			SInfo->Position[ i ].Alpha = 0.0f;
			SInfo->PositionNum--;
		}
	}

	// 終了リクエストがされていて、有効な座標の数も０だったらエフェクトを削除する
	if( EInfo->EndRequest && SInfo->PositionNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// 球の軌跡エフェクトの描画処理関数
void Effect_SphereLocus_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	static VERTEX3D       VertexBuffer[ 
							1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) +
							( POSITIONNUM - 1 ) * ( CIRCLEVERTNUM * UNITVERTNUM ) +
							CIRCLEVERTNUM ];
	static unsigned short IndexBuffer[ 
							CIRCLEVERTNUM * 3 +
							CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) * 6 +
							( POSITIONNUM - 1 ) * ( CIRCLEVERTNUM * 6 * UNITVERTNUM ) ];
	VECTOR                CatmullRomDestPos[ 2][(POSITIONNUM - 1) * UNITVERTNUM + 1 + 1];
	VECTOR                CatmullRomSrcPos[ 2 ][ POSITIONNUM ];
	float                 ScalingSinCos[ CIRCLEVERTNUM ][ 2 ];
	VERTEX3D             *VertP;
	unsigned short       *IndexP;
	int                   i;
	int                   j;
	int                   k;
	int                   l;
	int                   m;
	COLOR_U8              DifColor;
	COLOR_U8              SpcColor;
	VECTOR                Normal;
	VECTOR                xvec;
	VECTOR                yvec;
	VECTOR                zvec;
	int                   p2ind;
	float                 alpha;
	float                 x, y, z;
	float                 xd, yd, zd;

	// 球の軌跡を形成する有効な座標が２つより少ない場合は何もせずに終了
	if( SInfo->PositionNum < 2 )
	{
		return;
	}

	// 球のサイズにスケーリングした Sin, Cos の値を算出
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		ScalingSinCos[ i ][ 0 ] = g_EffSphereSinCos[ i ][ 0 ] * SInfo->SphereSize;
		ScalingSinCos[ i ][ 1 ] = g_EffSphereSinCos[ i ][ 1 ] * SInfo->SphereSize;
	}

	// Catmull-Rom補間を使用して座標と座標の間を滑らかに繋ぐ座標を算出
	for( i = 0; i < SInfo->PositionNum; i++ )
	{
		CatmullRomSrcPos[ 0 ][ i ] = SInfo->Position[ i ].CenterPos;
		CatmullRomSrcPos[ 1 ][ i ] = SInfo->Position[ i ].DirectionPos;
	}
	GetCatmullRomPosList( CatmullRomSrcPos[ 0 ], SInfo->PositionNum, UNITVERTNUM,
														CatmullRomDestPos[ 0 ] );
	GetCatmullRomPosList( CatmullRomSrcPos[ 1 ], SInfo->PositionNum, UNITVERTNUM,
														CatmullRomDestPos[ 1 ] );

	CatmullRomDestPos[ 0 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1 ] =
							SInfo->Position[ SInfo->PositionNum - 1 ].CenterPos;
	CatmullRomDestPos[ 1 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1 ] =
							SInfo->Position[ SInfo->PositionNum - 1 ].DirectionPos;

	// 球の軌跡を形成するポリゴンの頂点データを作成
	DifColor     = SInfo->Color;
	SpcColor     = GetColorU8( 0,0,0,0 );
	VertP        = VertexBuffer;
	Normal       = VGet( 0.0f, 1.0f, 0.0f );

	zvec         = VSub( CatmullRomDestPos[ 0 ][ 1 ], CatmullRomDestPos[ 0 ][ 0 ] );
	yvec         = VSub( CatmullRomDestPos[ 1 ][ 0 ], CatmullRomDestPos[ 0 ][ 0 ] );
	xvec         = VNorm( VCross( yvec, zvec ) );
	yvec         = VNorm( VCross( zvec, xvec ) );
	zvec         = VNorm( zvec );

	// 先端の半球の頭の１頂点のデータをセット
	VertP->pos   = VAdd( CatmullRomDestPos[ 0 ][ 0 ], VScale( zvec, -1.0f ) );
	VertP->dif   = DifColor;
	VertP->dif.a = 0;
	VertP->spc   = SpcColor;
	VertP->u     = 0.0f;
	VertP->v     = 0.0f;
	VertP->su    = 0.0f;
	VertP->sv    = 0.0f;
	VertP->norm  = Normal;
	VertP++;

	// 先端の半球の頭の１頂点以外の部分のデータをセット
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		for( j = 1; j < CIRCLEVERTNUM / 4; j++ )
		{
			x            =  g_EffSphereSinCos[ j ][ 1 ];
			y            =  0.0f;
			z            = -g_EffSphereSinCos[ j ][ 0 ];

			xd           = x * g_EffSphereSinCos[i][0] - y * g_EffSphereSinCos[i][1];
			yd           = x * g_EffSphereSinCos[i][1] + y * g_EffSphereSinCos[i][0];
			zd           = z;

			VertP->pos   = CatmullRomDestPos[ 0 ][ 0 ];
			VertP->pos   = VAdd( VertP->pos, VScale( xvec, xd * SInfo->SphereSize ) );
			VertP->pos   = VAdd( VertP->pos, VScale( yvec, yd * SInfo->SphereSize ) );
			VertP->pos   = VAdd( VertP->pos, VScale( zvec, zd * SInfo->SphereSize ) );
			VertP->dif   = DifColor;
			VertP->dif.a = 0;
			VertP->spc   = SpcColor;
			VertP->u     = ( float )i / CIRCLEVERTNUM;
			VertP->v     = 0.0f;
			VertP->su    = 0.0f;
			VertP->sv    = 0.0f;
			VertP->norm  = Normal;
			VertP++;
		}
	}

	// 軌跡から形成される円筒部分の頂点データをセット
	l = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( k = 0; k < UNITVERTNUM; k++ )
		{
			alpha      = ( float )k / UNITVERTNUM;
			DifColor.a = ( ( SInfo->PositionNum - 1 ) * UNITVERTNUM - l  ) * 255
									/ ( ( SInfo->PositionNum - 1 ) * UNITVERTNUM );

			zvec = VSub( CatmullRomDestPos[ 0 ][ l + 1 ], CatmullRomDestPos[ 0 ][ l ] );
			yvec = VSub( CatmullRomDestPos[ 1 ][ l ],     CatmullRomDestPos[ 0 ][ l ] );
			xvec = VNorm( VCross( yvec, zvec ) );
			yvec = VNorm( VCross( zvec, xvec ) );

			for( m = 0; m < CIRCLEVERTNUM; m++ )
			{
				VertP->pos  = CatmullRomDestPos[ 0 ][ l ];
				VertP->pos  = VAdd( VertP->pos, VScale( xvec, ScalingSinCos[ m ][ 0 ] ));
				VertP->pos  = VAdd( VertP->pos, VScale( yvec, ScalingSinCos[ m ][ 1 ] ));
				VertP->dif  = DifColor;
				VertP->spc  = SpcColor;
				VertP->u    = ( float )m / CIRCLEVERTNUM;
				VertP->v    = 0.0f;
				VertP->su   = 0.0f;
				VertP->sv   = 0.0f;
				VertP->norm = Normal;

				VertP++;
			}

			l++;
		}
	}

	// 軌跡から形成される円筒部分の終端部分の頂点データをセット
	DifColor.a = 0;

	zvec       = VSub( CatmullRomDestPos[ 0 ][ l     ], CatmullRomDestPos[ 0 ][ l - 1 ]);
	yvec       = VSub( CatmullRomDestPos[ 1 ][ l - 1 ], CatmullRomDestPos[ 0 ][ l - 1 ]);
	xvec       = VNorm( VCross( yvec, zvec ) );
	yvec       = VNorm( VCross( zvec, xvec ) );

	for( m = 0; m < CIRCLEVERTNUM; m++ )
	{
		VertP->pos  = CatmullRomDestPos[ 0 ][ l ];
		VertP->pos  = VAdd( VertP->pos, VScale( xvec, ScalingSinCos[ m ][ 0 ] ) );
		VertP->pos  = VAdd( VertP->pos, VScale( yvec, ScalingSinCos[ m ][ 1 ] ) );
		VertP->dif  = DifColor;
		VertP->spc  = SpcColor;
		VertP->u    = ( float )m / CIRCLEVERTNUM;
		VertP->v    = 0.0f;
		VertP->su   = 0.0f;
		VertP->sv   = 0.0f;
		VertP->norm = Normal;

		VertP++;
	}

	// 球の軌跡を形成するポリゴンを描画するための頂点インデックスリストを作成

	// 先端の半球の頭の１頂点を含む部分のポリゴンを形成する頂点インデックスリストを作成
	p2ind  = 1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 );
	IndexP = IndexBuffer;
	k = 0;
	for( i = 0; i < CIRCLEVERTNUM - 1; i++ )
	{
		IndexP[ 0 ] = 0;
		IndexP[ 1 ] = k + 1 + ( CIRCLEVERTNUM / 4 - 1 );
		IndexP[ 2 ] = k + 1;
		IndexP += 3;
		k += CIRCLEVERTNUM / 4 - 1;
	}
	IndexP[ 0 ] = 0;
	IndexP[ 1 ] =     1;
	IndexP[ 2 ] = k + 1;
	IndexP += 3;

	// 先端の半球の頭の１頂点を含まない部分のポリゴンを
	// 形成する頂点インデックスリストを作成
	k = 0;
	for( i = 0; i < CIRCLEVERTNUM - 1; i++ )
	{
		for( j = 0; j < ( CIRCLEVERTNUM / 4 - 1 ) - 1; j++ )
		{
			IndexP[ 0 ] = k + 1 +   j;
			IndexP[ 1 ] = k + 1 +   j       + ( CIRCLEVERTNUM / 4 - 1 );
			IndexP[ 2 ] = k + 1 + ( j + 1 ) + ( CIRCLEVERTNUM / 4 - 1 );

			IndexP[ 3 ] = k + 1 +   j;
			IndexP[ 4 ] = k + 1 + ( j + 1 ) + ( CIRCLEVERTNUM / 4 - 1 );
			IndexP[ 5 ] = k + 1 + ( j + 1 );

			IndexP += 6;
		}
		IndexP[ 0 ] = k + 1 +   j;
		IndexP[ 1 ] = k + 1 +   j       + ( CIRCLEVERTNUM / 4 - 1 );
		IndexP[ 2 ] = p2ind + i + 1;

		IndexP[ 3 ] = k + 1 +   j;
		IndexP[ 4 ] = p2ind + i + 1;
		IndexP[ 5 ] = p2ind + i;

		IndexP += 6;
		k += ( CIRCLEVERTNUM / 4 - 1 );
	}

	// 先端の半球の頭の１頂点を含まない部分の一番外側の部分のポリゴンを
	// 形成する頂点インデックスリストを作成
	{
		for( j = 0; j < ( CIRCLEVERTNUM / 4 - 1 ) - 1; j++ )
		{
			IndexP[ 0 ] = k + 1 +   j;
			IndexP[ 1 ] =     1 +   j      ;
			IndexP[ 2 ] =     1 + ( j + 1 );

			IndexP[ 3 ] = k + 1 +   j;
			IndexP[ 4 ] =     1 + ( j + 1 );
			IndexP[ 5 ] = k + 1 + ( j + 1 );

			IndexP += 6;
		}
		IndexP[ 0 ] = k + 1 +   j;
		IndexP[ 1 ] =     1 +   j;
		IndexP[ 2 ] = p2ind;

		IndexP[ 3 ] = k + 1 +   j;
		IndexP[ 4 ] = p2ind;
		IndexP[ 5 ] = p2ind + i;

		IndexP += 6;
	}

	// 軌跡から形成される円筒部分のポリゴンを形成する頂点インデックスリストを作成
	k = p2ind;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( j = 0; j < UNITVERTNUM; j++ )
		{
			for( m = 0; m < CIRCLEVERTNUM - 1; m++ )
			{
				IndexP[ 0 ] = k + m;
				IndexP[ 1 ] = k + m + 1;
				IndexP[ 2 ] = k + m + 1 + CIRCLEVERTNUM;

				IndexP[ 3 ] = k + m;
				IndexP[ 4 ] = k + m + 1 + CIRCLEVERTNUM;
				IndexP[ 5 ] = k + m +     CIRCLEVERTNUM;

				IndexP += 6;
			}
			IndexP[ 0 ] = k + m;
			IndexP[ 1 ] = k;
			IndexP[ 2 ] = k +     CIRCLEVERTNUM;

			IndexP[ 3 ] = k + m;
			IndexP[ 4 ] = k +     CIRCLEVERTNUM;
			IndexP[ 5 ] = k + m + CIRCLEVERTNUM;

			IndexP += 6;

			k += CIRCLEVERTNUM;
		}
	}

	// Ｚバッファを使用する設定に変更する
	SetUseZBufferFlag( TRUE );

	// Ｚバッファへの書き込みは行わない
	SetWriteZBufferFlag( FALSE );

	// 加算ブレンド描画に設定
	SetDrawBlendMode( DX_BLENDMODE_ADD, 255 );

	// 軌跡のポリゴンを描画
	DrawPolygonIndexed3D(
		VertexBuffer,
		1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) +
		( SInfo->PositionNum - 1 ) * ( CIRCLEVERTNUM * UNITVERTNUM ) + CIRCLEVERTNUM,
		IndexBuffer,
		( CIRCLEVERTNUM * 3 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) * 6 +
		( SInfo->PositionNum - 1 ) * ( CIRCLEVERTNUM * 6 * UNITVERTNUM ) ) / 3,
		EInfo->BaseInfo->GraphHandle,
		TRUE
	);

	// 描画ブレンドモードを標準に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// Ｚバッファを使用しない設定に戻す
	SetUseZBufferFlag( FALSE );
}

// 球の軌跡エフェクト処理の準備をする関数
void Effect_SphereLocus_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// エフェクトの色
	COLOR_U8 Color,
	
	// 球の大きさ
	float SphereSize
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;

	// 有効な座標の数を初期化
	SInfo->PositionNum = 0;

	// 球の大きさを保存
	SInfo->SphereSize  = SphereSize;

	// エフェクトの色を保存
	SInfo->Color       = Color;
}

// 球の軌跡に使用する座標を追加する
void Effect_SphereLocus_AddPosition(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 球の中心座標
	VECTOR NewCenterPos,

	// 球の向きを決定するための座標
	VECTOR NewDirectionPos
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	int i;

	// 既に追加できる最大数に達してしまっている場合は最大数より一つ少ない数にする
	if( SInfo->PositionNum >= POSITIONNUM )
	{
		SInfo->PositionNum--;
	}

	// 既に登録されている座標を一つ分移動する
	for( i = SInfo->PositionNum - 1; i >= 0; i-- )
	{
		SInfo->Position[ i + 1 ] = SInfo->Position[ i ];
	}

	// 配列の先頭に座標を追加
	SInfo->Position[ 0 ].CenterPos     = NewCenterPos;
	SInfo->Position[ 0 ].DirectionPos  = NewDirectionPos;
	SInfo->Position[ 0 ].Alpha         = 1.0f;

	// 有効な座標の数を一つ増やす
	SInfo->PositionNum++;
}
