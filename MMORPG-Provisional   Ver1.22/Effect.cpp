#include "Effect.h"
#include "Effect_SlashLocus.h"
#include "Effect_Damage.h"
#include "Effect_Dead.h"
#include "Effect_SphereLocus.h"

// 発生させることのできるエフェクトの最大数
#define EFFECT_MAXNUM			(256)

// エフェクトの基本情報
static SEffectBaseInfo g_EffectBaseInfo[ EEffect_Num ];

// エフェクトの情報
static SEffectInfo     g_EffectInfo[ EFFECT_MAXNUM ];

// エフェクト別の処理を分けるためのコールバック関数情報
typedef struct _SEffectFunction
{
	// 初期化時に呼ばれる関数へのポインタ
	bool ( *Initialize )( SEffectBaseInfo *EBInfo );

	// 後始末時に呼ばれる関数へのポインタ
	void ( *Terminate  )( SEffectBaseInfo *EBInfo );

	// 作成時に呼ばれる関数へのポインタ
	bool ( *Create     )( SEffectInfo *    EInfo );

	// 削除時に呼ばれる関数へのポインタ
	void ( *Delete     )( SEffectInfo *    EInfo );

	// 状態推移時に呼ばれる関数へのポインタ
	void ( *Step       )( SEffectInfo *    EInfo, float StepTime );

	// 描画時に呼ばれる関数へのポインタ
	void ( *Render     )( SEffectInfo *    EInfo );
} SEffectFunction;

// エフェクト別の処理を分けるためのコールバック関数テーブル
static SEffectFunction g_EffectFunctionTable[ EEffect_Num ] =
{
	//EEffect_SlashLocus
	{ Effect_SlashLocus_Initialize,  NULL,                    Effect_SlashLocus_Create,
	  NULL,                          Effect_SlashLocus_Step,  Effect_SlashLocus_Render },

	// EEffect_Damage
	{ Effect_Damage_Initialize,      NULL,                    Effect_Damage_Create,
	  NULL,                          Effect_Damage_Step,      Effect_Damage_Render     },

	// EEffect_Dead
	{ Effect_Dead_Initialize,        Effect_Dead_Terminate,   Effect_Dead_Create,
	  Effect_Dead_Delete,            Effect_Dead_Step,        Effect_Dead_Render       },

	// EEffect_SphereLocus
	{ Effect_SphereLocus_Initialize, NULL,                    Effect_SphereLocus_Create,
	  NULL,                          Effect_SphereLocus_Step, Effect_SphereLocus_Render},
};

// エフェクト処理の初期化をする
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した   false : 失敗した )
bool Effect_Initialize( void )
{
	int              i;
	SEffectInfo     *EInfo;
	SEffectBaseInfo *EBInfo;

	// エフェクトの数だけ繰り返し
	EBInfo = g_EffectBaseInfo;
	for( i = 0; i < EEffect_Num; i++, EBInfo++ )
	{
		// エフェクトで使用する画像ハンドルの値を初期化
		EBInfo->GraphHandle = -1;

		// エフェクト別の初期化関数がある場合は呼ぶ
		if( g_EffectFunctionTable[ i ].Initialize != NULL )
		{
			if( !g_EffectFunctionTable[ i ].Initialize( EBInfo ) )
			{
				return false;
			}
		}
	}

	// 全てのエフェクト情報構造体の『使用中かどうか』のフラグを倒す
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		EInfo->UseFlag = false;
	}

	// 成功終了
	return true;
}

// エフェクト処理の後始末をする
void Effect_Terminate( void )
{
	int              i;
	SEffectBaseInfo *EBInfo;

	// エフェクトを全て削除する
	Effect_DeleteAll();

	// エフェクトの数だけ繰り返し
	EBInfo = g_EffectBaseInfo;
	for( i = 0; i < EEffect_Num; i++, EBInfo++ )
	{
		// エフェクト別の後始末関数がある場合は呼ぶ
		if( g_EffectFunctionTable[ i ].Terminate != NULL )
		{
			g_EffectFunctionTable[ i ].Terminate( EBInfo );
		}

		// エフェクトで使用していた画像ハンドルを削除
		if( EBInfo->GraphHandle != -1 )
		{
			DeleteGraph( EBInfo->GraphHandle );
			EBInfo->GraphHandle = -1;
		}
	}
}

// エフェクトを作成する
//     戻り値 : 作成したエフェクトの情報構造体のアドレス
SEffectInfo *Effect_Create(
	// 作成するエフェクト
	EEffect Effect
)
{
	int          i;
	SEffectInfo *EInfo;

	// 使用されていないエフェクト情報構造体を探す
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			break;
		}
	}

	// 全ての構造体が使用されてしまっていたらエラー
	if( i == EFFECT_MAXNUM )
	{
		return NULL;
	}

	// 構造体を『使用中』にする
	EInfo->UseFlag    = true;

	// 終了リクエストがされているかどうかのフラグを倒す
	EInfo->EndRequest = false;

	// 何のエフェクトかを保存
	EInfo->Effect   = Effect;

	// エフェクトの基本情報のアドレスをセット
	EInfo->BaseInfo   = &g_EffectBaseInfo[ Effect ];

	// エフェクト別のデータを格納しているメモリ領域のアドレスを保存するポインタを初期化
	EInfo->SubData    = NULL;

	// エフェクト別の作成時に呼ぶ関数の呼び出し
	if( !g_EffectFunctionTable[ Effect ].Create( EInfo ) )
	{
		return NULL;
	}

	// 成功終了
	return EInfo;
}

// エフェクトに対して終了リクエストをする
void Effect_EndRequest(
	// エフェクトの情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	// 終了リクエストがされているかどうかのフラグを立てる
	EInfo->EndRequest = true;
}

// エフェクトを削除する
void Effect_Delete(
	// エフェクトの情報構造体のアドレス
	SEffectInfo *EInfo
)
{
	// 既に構造体が使用されていなかったら何もせずに終了
	if( !EInfo->UseFlag )
	{
		return;
	}

	// 削除時に実行する関数が存在する場合は実行する
	if( g_EffectFunctionTable[ EInfo->Effect ].Delete != NULL )
	{
		g_EffectFunctionTable[ EInfo->Effect ].Delete( EInfo );
	}

	// エフェクト別の情報があった場合はメモリを解放する
	if( EInfo->SubData != NULL )
	{
		free( EInfo->SubData );
		EInfo->SubData = NULL;
	}

	// 構造体を使用しているかどうかのフラグを倒す
	EInfo->UseFlag = false;
}

// 全てのエフェクトを削除する
void Effect_DeleteAll( void )
{
	int          i;
	SEffectInfo *EInfo;

	// 全ての構造体に対して削除処理を行う
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		Effect_Delete( EInfo );
	}
}

// 全てのエフェクトの状態推移処理を実行する
void Effect_StepAll(
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	int          i;
	SEffectInfo *EInfo;

	// 全ての有効なエフェクトに対して状態推移処理を行う
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			continue;
		}

		g_EffectFunctionTable[ EInfo->Effect ].Step( EInfo, StepTime );
	}
}

// 全てのエフェクトの描画処理を実行する
void Effect_RenderAll( void )
{
	int          i;
	SEffectInfo *EInfo;

	// 全ての有効なエフェクトに対して状描画処理を行う
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			continue;
		}

		g_EffectFunctionTable[ EInfo->Effect ].Render( EInfo );
	}
}
