#include "Chara_Golem.h"
#include "System.h"
#include "Camera.h"

// 地震攻撃のダメージ
#define EARTHQUAKE_DAMAGE			(20)

// アニメーションイベント「その他」が発生した際に呼ばれる関数
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Chara_Golem_AnimOtherEvent(
	// キャラクター情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	SCharaInfo *PCInfo;

	// カメラを揺らす
	Camera_Shake( 50.0f, 1.7f, 80.0f );

	// プレイヤーのキャラ情報構造体のアドレスを取得
	PCInfo = System_GetPlayerCharaInfo();

	// プレイヤーが存在して、且つジャンプしていなかったらプレイヤーにダメージを与える
	if( PCInfo != NULL && !PCInfo->JumpState )
	{
		if( !Chara_Damage(
				PCInfo,
				ECharaAtk_DamageType_Earthquake,
				EARTHQUAKE_DAMAGE,
				PCInfo->Position,
				VSub( PCInfo->Position, CInfo->Position ),
				NULL
			) )
		{
			return false;
		}
	}

	// 正常終了
	return true;
}



