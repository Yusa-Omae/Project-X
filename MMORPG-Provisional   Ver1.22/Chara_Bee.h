#ifndef CHARA_BEE_H
#define CHARA_BEE_H

#include "Chara.h"

// アニメーションイベント「その他」が発生した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Bee_AnimOtherEvent(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
);

#endif
