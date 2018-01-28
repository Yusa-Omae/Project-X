#include "Chara_Bee.h"

// アニメーションイベント「その他」が発生した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Bee_AnimOtherEvent(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	VECTOR NeedlePosition;

	// 針を発射する座標の取得
	NeedlePosition = MV1GetFramePosition(
							CInfo->ModelHandle,
							MV1SearchFrame( CInfo->ModelHandle, "point" )
					);

	// 針を発射する
	if( !Chara_Create( EChara_Needle, NeedlePosition, CInfo->AngleInfo.NowAngle, -1 ) )
	{
		return false;
	}

	// 正常終了
	return true;
}



