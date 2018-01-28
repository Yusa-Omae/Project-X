#ifndef CHARA_NEEDLE_H
#define CHARA_NEEDLE_H

#include "Chara.h"

// 針が作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Needle_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

#endif
