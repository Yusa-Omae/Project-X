#ifndef EFFECT_H
#define EFFECT_H

// エフェクト
typedef enum _EEffect
{
	EEffect_SlashLocus = 0,		// 剣の軌跡
	EEffect_Damage,				// ダメージ
	EEffect_Dead,				// 死亡
	EEffect_SphereLocus,		// 球の軌跡

	EEffect_Num					// エフェクトの数
} EEffect;

// エフェクトの基本情報
typedef struct _SEffectBaseInfo
{
	// エフェクトが使用する画像のハンドル
	int              GraphHandle;
} SEffectBaseInfo;

// エフェクトの情報
typedef struct _SEffectInfo
{
	// この構造体を使用しているかどうかのフラグ
	bool             UseFlag;

	// エフェクトの終了リクエストがされているかどうかのフラグ
	bool             EndRequest;

	// エフェクト
	EEffect          Effect;

	// エフェクトの基本情報構造体のアドレス
	SEffectBaseInfo *BaseInfo;

	// エフェクト別のデータを格納しているメモリ領域のアドレス
	void *           SubData;
} SEffectInfo;

// エフェクト処理の初期化をする
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Effect_Initialize( void );

// エフェクト処理の後始末をする
extern void Effect_Terminate( void );

// エフェクトを作成する
//     戻り値 : 作成したエフェクトの情報構造体のアドレス
extern SEffectInfo *Effect_Create(
	// 作成するエフェクト
	EEffect Effect
);

// エフェクトに対して終了リクエストをする
extern void Effect_EndRequest(
	// エフェクトの情報構造体のアドレス
	SEffectInfo *EInfo
);

// エフェクトを削除する
extern void Effect_Delete(
	// エフェクトの情報構造体のアドレス
	SEffectInfo *EInfo
);

// 全てのエフェクトを削除する
extern void Effect_DeleteAll( void );

// 全てのエフェクトの状態推移処理を実行する
extern void Effect_StepAll(
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 全てのエフェクトの描画処理を実行する
extern void Effect_RenderAll( void );


#endif
