#ifndef CHARA_PLAYERBASE_H
#define CHARA_PLAYERBASE_H

#include "TextParam.h"

// プレイヤーの基本情報
typedef struct _SChara_PlayerBaseInfo
{
	// 攻撃を防御したときに鳴らす音の登録番号
	int DefenceSuccessSound;
} SChara_PlayerBaseInfo;

// プレイヤーの基本情報の初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した  false : 失敗した )
extern bool Chara_PlayerBase_Initialize(
	// キャラクターの基本情報構造体のアドレス
	SCharaBaseInfo *CBInfo,

	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TextParam
);

#endif
