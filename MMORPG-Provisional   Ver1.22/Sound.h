#ifndef SOUND_H
#define SOUND_H

#include "DxLib.h"

// 素材タイプ
typedef enum _EMaterialType
{
	EMaterialType_Grass,	// 草
	EMaterialType_Metal,	// 金属
	EMaterialType_Soil,		// 土
	EMaterialType_Stone,	// 石
	EMaterialType_Wood,		// 木

	EMaterialType_Num		// 素材タイプの数
} EMaterialType;

// BGM
typedef enum _EBGM
{
	EBGM_Stage0,			// ステージ１
	EBGM_Stage1,			// ステージ２
	EBGM_Title,				// タイトル
	EBGM_Boss,				// ボス
	EBGM_GameOver,			// ゲームオーバー
	EBGM_StageClear,		// ステージクリア
	EBGM_AllStageClear,		// 全ステージクリア

	EBGM_Num				// BGMの数
} EBGM;

// サウンド処理を初期化する
//     戻り値 : 初期化が成功したかどうか( true:成功  false:失敗 )
extern bool Sound_Initialize(void);

// サウンド処理の後始末をする
extern void Sound_Terminate(void);

// サウンドファイルを追加する
//     戻り値 : サウンドファイルの登録番号
extern int  Sound_AddSound(
	// 追加するサウンドファイルのパス
	const char *FilePath,

	// ３Ｄで鳴らすサウンドかどうか( true:3Dサウンド  false:2Dサウンド )
	bool Is3DSound
);

// サウンドを再生する
extern void Sound_PlaySound(
	// 再生するサウンドの登録番号
	int SoundIndex,

	// 再生タイプ( DX_PLAYTYPE_BACK など )
	int PlayType
);

// サウンドを３Ｄ再生する
extern void Sound_PlaySound3D(
	// サウンドを鳴らすワールド座標
	VECTOR Position,

	// 再生するサウンドの登録番号
	int SoundIndex,

	// 再生タイプ( DX_PLAYTYPE_BACK など )
	int PlayType
);

// ＢＧＭを再生する
extern void Sound_PlayBGM(
	// 再生するＢＧＭ
	EBGM BGM
);

// ＢＧＭの再生を止める
extern void Sound_StopBGM(void);

#endif
