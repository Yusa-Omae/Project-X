#include "Chara_Needle.h"

// 針の移動速度
#define MOVE_SPEED				(1500.0f)

// 針の攻撃力
#define ATTACK_POWER			(10)

// 針が作成された際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Needle_Create(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	// 向きに関係する情報を更新する
	Chara_AngleParamRefresh( CInfo );

	// 前方方向に飛ぶ
	CInfo->Spd   = VScale( CInfo->AngleInfo.FrontDirection, MOVE_SPEED );
	CInfo->Spd.y = 10.0f;

	// ジャンプ状態にする
	CInfo->JumpState                      = true;

	// 攻撃情報をセットする
	CInfo->AtkInfo[ 0 ].Enable         = true;
	CInfo->AtkInfo[ 0 ].AtkPosIndex = 0;

	// 攻撃力をセットする
	CInfo->Atk                    = ATTACK_POWER;

	// 正常終了
	return true;
}





