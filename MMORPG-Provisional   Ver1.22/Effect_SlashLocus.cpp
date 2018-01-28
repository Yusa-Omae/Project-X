#include "Effect_SlashLocus.h"
#include "Mathematics.h"

// 軌跡を表示する時間
#define DISPLAY_TIME		(0.1f)

// 軌跡を形成する座標の間を補間関数で分割する数
#define UNITVERTNUM			(8)

// 軌跡を形成する座標の数
#define POSITIONNUM			(16)

// 剣の軌跡を形成する座標の情報
typedef struct _SEffect_SlashLocus_Position
{
	// 剣の柄側の座標
	VECTOR                      NearPos;

	// 剣の刃先側の座標
	VECTOR                      FarPos;

	// 不透明度
	float                       Alpha;
} SEffect_SlashLocus_Position;

// 剣の軌跡エフェクトの情報
typedef struct _SEffect_SlashLocusInfo
{
	// エフェクトの色
	COLOR_U8                    Color;

	// 剣の軌跡を形成する有効な座標の数
	int                         PositionNum;

	// 剣の軌跡を形成する座標の情報
	SEffect_SlashLocus_Position Position[ POSITIONNUM ];
} SEffect_SlashLocusInfo;


// 剣の軌跡エフェクトの基本情報初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_SlashLocus_Initialize(
	// 剣の軌跡エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *EBInfo
)
{
	// 剣の軌跡エフェクトで使用する画像の読み込み
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\SlashLocus.png" );

	// 画像の読み込みに失敗したら失敗終了
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// 成功終了
	return true;
}

// 剣の軌跡エフェクトが作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Effect_SlashLocus_Create(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	// エフェクトの情報構造体を格納するメモリ領域の確保
	EInfo->SubData = malloc( sizeof( SEffect_SlashLocusInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// 正常終了
	return true;
}

// 剣の軌跡エフェクトの状態推移処理関数
void Effect_SlashLocus_Step(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	int                     i;
	int                     PosNum;

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

// 剣の軌跡エフェクトの描画処理関数
void Effect_SlashLocus_Render(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	static VERTEX3D       VertexBuffer[ ( POSITIONNUM - 1 ) * ( 2 * UNITVERTNUM ) + 2 ];
	static unsigned short IndexBuffer[ ( POSITIONNUM - 1 ) * ( UNITVERTNUM * 6 ) ];
	VECTOR                CatmullRomDestPos[ 2 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1];
	VECTOR                CatmullRomSrcPos[ 2 ][ POSITIONNUM ];
	VERTEX3D             *VertP;
	unsigned short       *IndexP;
	int                   i;
	int                   j;
	int                   k;
	int                   l;
	COLOR_U8              SpcColor;
	VECTOR                Normal;
	float                 alpha;

	// 剣の軌跡を形成する有効な座標が２つより少ない場合は何もせずに終了
	if( SInfo->PositionNum < 2 )
	{
		return;
	}

	// Catmull-Rom補間を使用して座標と座標の間を滑らかに繋ぐ座標を算出
	for( i = 0; i < SInfo->PositionNum; i++ )
	{
		CatmullRomSrcPos[ 0 ][ i ] = SInfo->Position[ i ].NearPos;
		CatmullRomSrcPos[ 1 ][ i ] = SInfo->Position[ i ].FarPos;
	}
	GetCatmullRomPosList( CatmullRomSrcPos[ 0 ], SInfo->PositionNum, UNITVERTNUM,
															CatmullRomDestPos[ 0 ] );
	GetCatmullRomPosList( CatmullRomSrcPos[ 1 ], SInfo->PositionNum, UNITVERTNUM,
															CatmullRomDestPos[ 1 ] );

	// 剣の軌跡を形成するポリゴンの頂点データを作成
	SpcColor = GetColorU8( 0,0,0,0 );
	VertP    = VertexBuffer;
	Normal   = VGet( 0.0f, 1.0f, 0.0f );
	l        = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( k = 0; k < UNITVERTNUM; k++ )
		{
			alpha = ( float )k / UNITVERTNUM;

			VertP[ 0 ].pos   = CatmullRomDestPos[ 0 ][ l ];
			VertP[ 1 ].pos   = CatmullRomDestPos[ 1 ][ l ];

			VertP[ 0 ].dif   = SInfo->Color;
			VertP[ 0 ].dif.a = ( BYTE )( ( alpha * SInfo->Position[ i + 1 ].Alpha +
							( 1.0f - alpha ) * SInfo->Position[ i ].Alpha ) * 255.0f );
			VertP[ 1 ].dif   = VertP[ 0 ].dif;
			VertP[ 0 ].spc   = SpcColor;
			VertP[ 1 ].spc   = SpcColor;
			VertP[ 0 ].u     = 0.0f;
			VertP[ 1 ].u     = 0.0f;
			VertP[ 0 ].v     = 1.0f;
			VertP[ 1 ].v     = 0.0f;
			VertP[ 0 ].su    = 0.0f;
			VertP[ 1 ].su    = 0.0f;
			VertP[ 0 ].norm  = Normal;
			VertP[ 1 ].norm  = Normal;

			VertP += 2;
			l++;
		}
	}
	VertP[ 0 ].pos   = SInfo->Position[ i ].NearPos;
	VertP[ 1 ].pos   = SInfo->Position[ i ].FarPos;
	VertP[ 0 ].dif   = SInfo->Color;
	VertP[ 0 ].dif.a = ( BYTE )( SInfo->Position[ i ].Alpha * 255.0f );
	VertP[ 1 ].dif   = VertP[ 0 ].dif;
	VertP[ 0 ].spc   = SpcColor;
	VertP[ 1 ].spc   = SpcColor;
	VertP[ 0 ].u     = 0.0f;
	VertP[ 1 ].u     = 0.0f;
	VertP[ 0 ].v     = 1.0f;
	VertP[ 1 ].v     = 0.0f;
	VertP[ 0 ].su    = 0.0f;
	VertP[ 1 ].su    = 0.0f;
	VertP[ 0 ].norm  = Normal;
	VertP[ 1 ].norm  = Normal;
	VertP += 2;

	// 剣の軌跡を形成するポリゴンを描画するための頂点インデックスリストを作成
	IndexP = IndexBuffer;
	k = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( j = 0; j < UNITVERTNUM; j++ )
		{
			IndexP[ 0 ] = k;
			IndexP[ 1 ] = k + 2;
			IndexP[ 2 ] = k + 1;
			IndexP[ 3 ] = k + 1;
			IndexP[ 4 ] = k + 2;
			IndexP[ 5 ] = k + 3;

			k += 2;
			IndexP += 6;
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
		VertexBuffer, (   SInfo->PositionNum - 1 ) * ( 2 * UNITVERTNUM ) + 2,
		IndexBuffer,  ( ( SInfo->PositionNum - 1 ) * ( UNITVERTNUM * 6 ) ) / 3,
		EInfo->BaseInfo->GraphHandle,
		TRUE
	);

	// 描画ブレンドモードを標準に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// Ｚバッファを使用しない設定に戻す
	SetUseZBufferFlag( FALSE );
}

// 剣の軌跡エフェクト処理の準備をする関数
void Effect_SlashLocus_Setup(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 剣の軌跡エフェクトの色
	COLOR_U8 Color
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;

	// 剣の軌跡を形成する有効な座標の数を初期化
	SInfo->PositionNum = 0;

	// エフェクトの色を保存
	SInfo->Color = Color;
}

// 剣の軌跡に使用する座標を追加する
void Effect_SlashLocus_AddPosition(
	// エフェクト情報構造体のアドレス
	SEffectInfo *EInfo,
	
	// 剣の柄側の座標
	VECTOR NewNearPos,
	
	// 剣の刃先側の座標
	VECTOR NewFarPos
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	int                     i;

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
	SInfo->Position[ 0 ].NearPos = NewNearPos;
	SInfo->Position[ 0 ].FarPos  = NewFarPos;
	SInfo->Position[ 0 ].Alpha   = 1.0f;

	// 有効な座標の数を一つ増やす
	SInfo->PositionNum++;
}

