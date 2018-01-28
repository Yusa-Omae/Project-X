#include "Chara.h"
#include "Chara_PlayerBase.h"

// プレイヤーの基本情報の初期化関数
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した  false : 失敗した )
bool Chara_PlayerBase_Initialize(
	// キャラクターの基本情報構造体のアドレス
	SCharaBaseInfo *CBInfo,

	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TextParam
)
{
	SChara_PlayerBaseInfo *PBInfo;

	// プレイヤーの基本情報を格納するメモリ領域の確保
	CBInfo->SubData = malloc( sizeof( SChara_PlayerBaseInfo ) );
	if( CBInfo->SubData == NULL )
	{
		return false;
	}
	PBInfo = ( SChara_PlayerBaseInfo * )CBInfo->SubData;

	// 攻撃を防御したときに鳴らす音の読み込み
	PBInfo->DefenceSuccessSound = Sound_AddSound( "SE\\Player\\guard_success", true );

	// 成功終了
	return true;
}
