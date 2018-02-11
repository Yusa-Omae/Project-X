#ifndef CHARA_H
#define CHARA_H

#include "CharaBase.h"
#include "CharaHealthGauge.h"
#include "Effect.h"
#include "Item.h"

// キャラのデフォルト回転速度
#define CHARA_DEFAULT_ANGLE_SPEED					(DX_PI_F * 6.0f)

// キャラのデフォルトアニメ変更速度
#define CHARA_DEFAULT_CHANGE_ANIM_SPEED				(15.0f)

// キャラのデフォルト攻撃アニメ変更速度
#define CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED		(30.0f)

// 攻撃が当たったキャラを記録できる最大数
#define CHARA_ATTACK_HIT_CHARA_MAX					(16)

// 攻撃判定用の座標の数
#define CHARA_ATTACK_POS_NUM						(2)

// キャラの体力が表示されてから消えるまでの時間
#define CHARA_HPGAUGE_HIDE_DELAY				(10.0f)

// キャラの状態
typedef enum _ECharaState
{
	ECharaState_Move = 0,						// 通常移動中
	ECharaState_Attack,							// 攻撃中
	ECharaState_Defence,						// 防御中
	ECharaState_Jump,							// ジャンプ中
	ECharaState_Damage,							// ダメージ中
	ECharaState_Blow,							// 吹っ飛び中
	ECharaState_Down,							// 倒れ中
	ECharaState_FadeOut,						// フェードアウト中
} ECharaState;

// ダメージタイプ
typedef enum _ECharaAttack_DamageType
{
	ECharaAttack_DamageType_Cut,				// 斬り攻撃によるダメージ
	ECharaAttack_DamageType_Earthquake,			// 地震によるダメージ
} ECharaAttack_DamageType;

// キャラの吹っ飛び状態
typedef enum _ECharaBlowState
{
	ECharaBlowState_In,							// 吹っ飛び状態移行アニメーション中
	ECharaBlowState_WaitLanding,				// 接地待ち中
	ECharaBlowState_Out,						// 接地してからのアニメーション中
} ECharaBlowState;

// キャラの描画モード
typedef enum _ECharaRenderMode
{
	ECharaRenderMode_Always,					// 常に描画する
	ECharaRenderMode_Distance_Less,				// 指定の距離以内であれば描画する
	ECharaRenderMode_Distance_Greater,			// 指定の距離以上であれば描画する
} ECharaRenderMode;

// 攻撃に当たったキャラやステージの情報
typedef struct _SCharaAttackHitInfo
{
	// 攻撃が当たったキャラの数
	int                   HitCharaNum;

	// 攻撃が当たったキャラへのポインタ
	struct _SCharaInfo *  HitCharaInfo[ CHARA_ATTACK_HIT_CHARA_MAX ];

	// ステージに対して当たったかどうかのフラグ( true:当たった  false:当たっていない )
	bool                  StageHit;

	// ステージに対して当たった場合に、当たった箇所の素材タイプ
	EMaterialType         StageHitMaterialType;
} SCharaAttackHitInfo;

// キャラの攻撃情報
typedef struct _SCharaAttackInfo
{
	// 情報が有効かどうかのフラグ( true:有効  false:無効 )
	bool                  Enable;

	// 攻撃判定に使用する SCharaBaseInfo 構造体のメンバー変数 AttackPosInfo の要素番号
	int                   AttackPosIndex;

	// 攻撃の軌跡エフェクトへのポインタ
	SEffectInfo *         LocusEffect;

	// 攻撃判定用の座標の有効数
	int                   ValidPositionNum;

	// 攻撃判定用のフレームに近い側の座標
	VECTOR                NearPosition[ CHARA_ATTACK_POS_NUM ];

	// 攻撃判定用のフレームから遠い側の座標
	VECTOR                FarPosition[ CHARA_ATTACK_POS_NUM ];

	// 攻撃に当たったキャラやステージの情報
	SCharaAttackHitInfo   HitInfo;
} SCharaAttackInfo;

// キャラのアニメーション情報
typedef struct _SCharaAnimInfo
{
	// 現在再生中のアニメーション
	ECharaAnim            NowAnim;

	// 一つ前に再生していたアニメーション
	ECharaAnim            PrevAnim;

	// 現在再生中にアニメーションのモデルへのアタッチインデックス
	int                   NowAttachIndex;

	// 一つ前に再生していたアニメーションのモデルへのアタッチインデックス
	int                   PrevAttachIndex;

	// 再生アニメーションの変更速度
	float                 ChangeSpeed;

	// 再生アニメーションの変更率
	float                 ChangeRate;

	// 現在再生中のアニメーションの再生時間
	float                 NowTime;

	// 一つ前に再生していたアニメーションの再生時間
	float                 PrevTime;

	// アニメーションのキャンセルイベントが発生したかどうかのフラグ
	bool                  Cancel;

	// アニメーションの再生が完了したかどうかのフラグ
	bool                  End;

	// ルートフレームの移動ベクトル
	VECTOR                Move;
} SCharaAnimInfo;

// キャラの向き情報
typedef struct _SCharaAngleInfo
{
	// 現在の向き( 単位：ラジアン )
	float                 NowAngle;

	// 目標の向き( 単位：ラジアン )
	float                 TargetAngle;

	// 目標の向きに到達したかどうか
	bool                  AngleMoveEnd;

	// 向きの変化速度
	float                 AngleSpeed;

	// 現在の向きの正面方向のベクトル
	VECTOR                FrontDirection;

	// 現在の向きの向かって左方向のベクトル
	VECTOR                LeftDirection;
} SCharaAngleInfo;

// キャラの情報
typedef struct _SCharaInfo
{
	// 構造体を使用しているかどうかのフラグ
	bool                  UseFlag;

	// ３Ｄモデルハンドル
	int                   ModelHandle;

	// キャラ
	EChara                Chara;

	// キャラ識別用番号( 特定キャラを倒したらステージクリア、という処理で使用 )
	int                   CharaNo;

	// キャラの基本情報へのポインタ
	const SCharaBaseInfo *BaseInfo;

	// キャラの状態
	ECharaState           State;

	// キャラの座標
	VECTOR                Position;

	// キャラの向き情報
	SCharaAngleInfo       AngleInfo;

	// キャラのアニメーション情報
	SCharaAnimInfo        AnimInfo;

	// 吹っ飛び状態
	ECharaBlowState       BlowState;

	// キャラが立っている場所の素材
	EMaterialType         OnMaterialType;

	// 不透明度( 0.0 ～ 1.0f )
	float                 OpacityRate;

	// ジャンプしているか( 空中にいるか )どうかのフラグ
	bool                  JumpState;

	// アニメーションの移動値ではなく、Spd による移動を行うかどうかのフラグ
	bool                  ProgramMove;

	//基礎体力
	int                   Health;

	//体力ゲージ情報
	SCharaHealthGauge     HealthGauge;

	//体力ゲージを表示するかどうかのフラグ
	bool                  HealthGaugeVisible;

	//体力ゲージを非表示にするまでの時間
	float                 HealthGaugeHideDelay;	

	//ダメージを受けた方向
	VECTOR                DamageDirection;

	//基礎攻撃力
	int                   Atk;

	//攻撃情報
	SCharaAttackInfo	  AttackInfo[ CHARA_ATTACK_MAX_NUM ];

	//基礎防御力
	int					  Def;

	//基礎クリティカル率
	float				  Crt;

	//移動速度
	VECTOR                Spd;

	//一つ前のフレームでの Spd の値
	VECTOR                PrevSpd;

	//一つ前のフレームでのルートフレームのアニメーションによる移動速度
	VECTOR                PrevAnimSpd;

	//一つ前のフレームでの最終的な移動速度
	// ( Spd にアニメーションの移動値などを加算したもの )
	VECTOR                PrevFixSpd;

	//所持金(Player:所持金 Enemy:ドロップ金)倒されたら金額ドロップ
	int					  Gold;

	//所持アイテム
	int					  ItemHav[10];

	//所持アイテムの耐久値
	int					  Item_Db[10];

	//アイテムでの追加最大HP合計値
	int					  Add_Health;

	//アイテムでの追加攻撃力合計値
	int					  Add_Atk;

	//アイテムでの追加防御力合計値
	int					  Add_Def;

	//アイテムでの追加クリティカル率合計値
	float				  Add_Crt;

	//アイテムでの追加移動速度合計値
	float				  Add_Spd;

	//アイテムでの攻撃を与えた際のHP吸収率合計値
	float				  Abp_Heal;

	//アイテムでの秒間回復速度合計値
	int					  Auto_Heal;

	//アイテムでのお金ドロップ量増加合計値(Player：敵を倒した際に獲得できる量増加率 Enemy:倒された際のゴールド落とす量の増加率)
	float				  Gold_Per;

	// キャラ別の情報へのポインタ
	void *                SubData;
} SCharaInfo;


// キャラの処理の初期化をする
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Chara_Initialize( void );

// キャラの処理の後始末をする
extern void Chara_Terminate( void );

// 全てのキャラの状態推移処理を実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_StepAll(
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 全てのキャラの描画処理を行う
extern void Chara_RenderAll(
	// 描画モード
	ECharaRenderMode RenderMode,

	// 描画モードが ECharaRenderMode_Always 以外の場合に使用される比較用距離
	float CompareDistance
);

// 全てのキャラの丸影の描画処理を行う
extern void Chara_ShadowRenderAll(
	// 描画モード
	ECharaRenderMode RenderMode,

	// 描画モードが ECharaRenderMode_Always 以外の場合に使用される比較用距離
	float CompareDistance
);

// 全てのキャラの２Ｄ描画処理を行う
extern void Chara_2D_RenderAll( void );

// 指定のタイプのキャラを倒す
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_TagetTypeKill(
	// 倒したいキャラタイプ
	ECharaType Type
);

// 新しいキャラを作成する
//     戻り値 : キャラの作成に成功したかどうか
//              ( true : 成功した   false : 失敗した )
extern bool Chara_Create(
	// 出現させるキャラ
	EChara Chara,

	// 出現させるキャラの初期位置
	VECTOR Position,

	// 出現させるキャラの初期向き
	float Angle,

	// 出現させるキャラの識別用番号
	int CharaNo
);

// 指定のキャラを削除する
extern void Chara_Delete(
	// 削除するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// 全てのキャラを削除する
extern void Chara_AllDelete( void );

// 指定キャラの周囲に居る指定タイプのキャラを探す
//     戻り値 : 検索で見つかったキャラの情報構造体のアドレス
extern SCharaInfo * Chara_SearchTarget(
	// 検索基準となるキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 探すキャラのタイプ
	ECharaType SearchCharaType,

	// 探す距離( CInfo の居る位置からどれだけの距離の範囲を検索するか )
	float SearchDistance,

	// 探す角度範囲( CInfo の向いている方向からどの範囲を検索するか )
	float SearchAngle
);

// 指定座標の周囲に居るキャラクターにメッセージを送る
extern void Chara_SendMessage(
	// メッセージを送るキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// メッセージを送る基準となる座標
	VECTOR BasePosition,

	// メッセージを送る範囲
	float SendDistance,

	// メッセージの識別番号
	int Message
);

// キャラにダメージを与える
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_Damage(
	// ダメージを喰らうキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// ダメージタイプ
	ECharaAttack_DamageType DamageType,

	// 受けるダメージ
	int DamagePoint,

	// 攻撃が当たった座標
	VECTOR HitPosition,

	// 攻撃の方向
	VECTOR AttackDirection,

	// キャラにダメージを与えられたかどうかを代入する変数のアドレス
	bool *Result
);

// キャラを指定の座標の方向に向かせる
extern void Chara_TargetAngleMove(
	// 向きを変更するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせたい座標
	VECTOR TargetPosition
);

// キャラが向くべき方向を設定する
extern void Chara_SetTargetDirection(
	// 向きを設定するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせる方向
	VECTOR Direction,

	// 徐々に向きを変えるかどうか
	// ( true : 徐々に向きを変える  false : 一瞬で向きを変える )
	bool Interp,

	// Direction の向きを反転して使用するかどうか
	// ( true : 反転して使用する  false : 反転しない )
	bool Reverse
);

// キャラが向くべき方向を設定する( Ｙ軸角度指定タイプ )
extern void Chara_SetTargetAngle(
	// 向きを設定するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// キャラに向かせる方向( Ｙ軸角度 )
	float Angle,

	// 徐々に向きを変えるかどうか
	// ( true:徐々に向きを変える  false:一瞬で向きを変える )
	bool Interp
);

// キャラの向いている方向に基づく情報を更新する
extern void Chara_AngleParamRefresh(
	// 情報を更新するキャラの情報構造体のアドレス
	SCharaInfo *CInfo
);

// キャラが再生するアニメーションを変更する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Chara_ChangeAnim(
	// アニメーションを再生するキャラの情報構造体のアドレス
	SCharaInfo *CInfo,

	// 新たに再生するアニメーション
	ECharaAnim NewAnim,

	// 現在再生しているアニメから新たに再生するアニメに移行する速度( 移行率/秒 )
	float ChangeSpeed
);

#endif