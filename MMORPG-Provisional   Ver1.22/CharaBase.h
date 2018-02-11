#ifndef CHARABASE_H
#define CHARABASE_H

#include "DxLib.h"
#include "TextParam.h"
#include "Animation.h"

// キャラクターのデータファイルが格納されているフォルダ
#define CHARADATA_DIR					"Data\\Character\\"

// キャラクターアニメーションのFPS
#define CHARA_ANIM_FPS					(30)

// キャラクターに付けられるサブ３Ｄモデルの最大数
#define CHARA_MAX_SUBMODEL				(4)

// キャラクターの文字列データで使用できる最大文字数
#define CHARA_MAX_STRINGLENGTH			(64)

// キャラクターの１アニメーションで扱える攻撃判定情報の数
#define CHARA_ATTACK_MAX_NUM			(4)

// キャラクターの攻撃判定用位置情報の最大数
#define CHARA_ATTACK_POS_MAX_NUM		(4)

// キャラクタータイプ
typedef enum _ECharaType
{
	ECharaType_Player = 0,			// プレイヤー
	ECharaType_Enemy,				// 敵
	ECharaType_EnemyShot,			// 敵の弾

	ECharaType_Num					// キャラクタータイプの数
} ECharaType;

// キャラクター
typedef enum _EChara
{
	EChara_Player = 0,				// プレイヤー
	EChara_Goblin,					// ゴブリン
	EChara_Bee,						// 蜂
	EChara_Golem,					// ゴーレム
	EChara_RedGoblin,				// 赤ゴブリン
	EChara_Needle,					// 針

	EChara_Num						// キャラクターの数
} EChara;

// キャラクターのアニメーション
typedef enum _ECharaAnim
{
	ECharaAnim_Neutral = 0,			// ニュートラル
	ECharaAnim_Walk,				// 歩き
	ECharaAnim_Run,					// 走り
	ECharaAnim_Jump_In,				// ジャンプ開始
	ECharaAnim_Jump_Loop,			// ジャンプループ
	ECharaAnim_Jump_Out,			// ジャンプ終了
	ECharaAnim_Landed,				// 着地
	ECharaAnim_Attack1,				// 攻撃アニメーション１
	ECharaAnim_Attack2,				// 攻撃アニメーション２
	ECharaAnim_Attack3,				// 攻撃アニメーション３
	ECharaAnim_Guard_In,			// ガード開始
	ECharaAnim_Guard_Loop,			// ガードループ
	ECharaAnim_Guard_Impact,		// 攻撃をガード
	ECharaAnim_Guard_Out,			// ガード終了
	ECharaAnim_Damage,				// ダメージ
	ECharaAnim_Blow_In,				// 吹っ飛び開始
	ECharaAnim_Blow_Loop,			// 吹っ飛びループ
	ECharaAnim_Blow_Out,			// 吹っ飛び終了
	ECharaAnim_Down,				// ダウン
	ECharaAnim_Down_Loop,			// ダウンループ

	ECharaAnim_Num					// アニメーションの数
} ECharaAnim;

// 移動タイプ
typedef enum _ECharaMoveType
{
	ECharaMoveType_RootAnim,		// Rootフレームアニメーションによる移動
	ECharaMoveType_Program,			// プログラムによる移動

	ECharaMoveType_Num				// 移動タイプの数
} ECharaMoveType;

// キャラクターの武器
typedef enum _ECharaWeapon
{
	ECharaWeapon_Sabel,				// サーベル
	ECharaWeapon_Sword,				// 剣
	ECharaWeapon_Axe,				// 斧
	ECharaWeapon_Needle,			// 針
	ECharaWeapon_RockKnuckle,		// 岩殴り

	ECharaWeapon_Num				// キャラクターの武器の数
} ECharaWeapon;

// キャラクターの攻撃判定の形状
typedef enum _ECharaAttackFormType
{
	ECharaAttackFormType_Poly,		// ポリゴン
	ECharaAttackFormType_Sphere,	// 球

	ECharaAttackFormType_Num		// 攻撃判定の形状の数
} ECharaAttackFormType;

// キャラクター共通効果音
typedef enum _ECharaCommonSE
{
	ECharaCommonSE_Nifram,			// キャラクター消滅音

	ECharaCommonSE_Num				// キャラクター共通効果音の数
} ECharaCommonSE;

// キャラクターアニメーションファイル情報
typedef struct _SCharaAnimFileInfo
{
	// ループアニメーションかどうか
	bool                  LoopFlag;

	// ファイル名
	const char *          FileName;
} SCharaAnimFileInfo;

// キャラクターサブ３Ｄモデル情報
typedef struct _SCharaSubModelInfo
{
	// ３Ｄモデルハンドル
	int                   ModelHandle;

	// ３Ｄモデルをアタッチするフレーム番号
	int                   AttachFrameIndex;
} SCharaSubModelInfo;

// キャラクターアニメーション基本情報
typedef struct _SCharaAnimBaseInfo
{
	// アニメーション情報
	SAnimInfo             Info;

	// アニメーションの３Ｄモデルハンドル
	int                   Handle;
} SCharaAnimBaseInfo;

// キャラクターの攻撃位置情報
typedef struct _SCharaAttackPositionInfo
{
	// 起点となる３Ｄモデルフレームの番号
	int                   StartFrameIndex;

	// 攻撃判定の形状
	ECharaAttackFormType  FormType;

	// 攻撃判定の形成に使用される起点からの相対座標
	// ( FormType が ECharaAttackFormType_Poly の場合に使用 )
	VECTOR                EndLocalPosition;

	// 球の半径
	// ( FormType が ECharaAttackFormType_Sphere の場合に使用 )
	float                 SphereSize;

	// 攻撃の軌跡エフェクトの色
	COLOR_U8              EffectColor;
} SCharaAttackPositionInfo;

// キャラクターの武器の基本情報
typedef struct _SCharaWeaponBaseInfo
{
	// データファイルが存在するフォルダ名
	char                  DirectoryName[CHARA_MAX_STRINGLENGTH];

	// 武器の名前
	char                  Name[CHARA_MAX_STRINGLENGTH];

	// 武器の３Ｄモデルハンドル
	int                   ModelHandle;

	// 武器が背景の何処に当たっても鳴らす音
	int                   MaterialCommonSound;

	// 武器が背景に当たった際に鳴らす素材別の音
	int                   MaterialSound[EMaterialType_Num];
} SCharaWeaponBaseInfo;

// キャラクターの当たり判定の情報
typedef struct _SCharaHitInfo
{
	// 当たり判定の幅
	float                 Width;

	// 当たり判定の高さ
	float                 Height;

	// 当たり判定の中心座標
	VECTOR                CenterPosition;
} SCharaHitInfo;

// キャラクターの基本情報
typedef struct _SCharaBaseInfo
{
	// キャラクターデータファイルが存在するフォルダ名
	char                  DirectoryName[CHARA_MAX_STRINGLENGTH];

	// キャラクターの名前
	char                  Name[CHARA_MAX_STRINGLENGTH];

	// キャラクターのタイプ
	ECharaType            Type;

	// 体力
	int                   Health;
	/*

	Health=基本体力値
	Attack=基本攻撃値
	Def=基本防御値
	Crt=基本クリティカル率
	Speed=基本移動スピード
	Gold=所持金
	Fame=名声
	
	ItemHav[10]=アイテムは何を持っているかの確認
	Item_Db[10]=アイテム欄の耐久値
	
	Add_Health=所持アイテムの最大HPアップ合計値
	Add_Atk=装備武器アイテムの攻撃力
	Add_Def=所持アイテムの防御力合計値
	Add_Crt=所持アイテムのクリティカル率合計値
	Add_Spd=所持アイテムの移動スピード値合計
	Abp_Health=攻撃を与えた際のHealth吸収率(ダメージ*吸収率(%))
	Auto_Heal=リジェネ(1秒間に回復する量(微量))
	Gold_Per=敵を倒した際にもらえるお金の量が増える。

	*/

	// 武器による攻撃を受けたときに鳴らす音の登録番号
	int                   WeaponAttackDamageSound[ECharaWeapon_Num];

	// 素材別のアニメーション物理音の登録番号
	int                   AnimPhysicsSound[EAnimPhysicsSound_Num][EMaterialType_Num];

	// キャラクターの３Ｄモデルハンドル
	int                   ModelHandle;

	// 装備武器
	ECharaWeapon          EquipWeapon;

	// 武器をアタッチしている３Ｄモデルフレーム番号
	int                   EquipWeaponAttachFrameIndex;

	// サブ３Ｄモデルの数
	int                   SubModelNum;

	// サブ３Ｄモデルの情報
	SCharaSubModelInfo    SubModel[CHARA_MAX_SUBMODEL];

	// ルート３Ｄモデルフレーム番号
	int                   RootFrameIndex;

	// アニメーション情報
	SCharaAnimBaseInfo    AnimInfo[ECharaAnim_Num];

	// ダメージ判定情報
	SCharaHitInfo         DamageHitInfo;

	// 体力ゲージを表示する座標
	VECTOR                HealthGaugePosition;

	// 攻撃判定位置情報
	SCharaAttackPositionInfo AttackPosInfo[CHARA_ATTACK_POS_MAX_NUM];

	// 移動タイプが ECharaMoveType_Program の場合の移動速度
	float                 ProgramSpd;

	// 丸影の大きさ
	float                 ShadowSize;

	// キャラクター同士の当たり判定情報
	SCharaHitInfo         CharaHitInfo;

	// 二つあるステージとの当たり判定の下側の幅
	SCharaHitInfo         StageHitBottomInfo;

	// 二つあるステージとの当たり判定の上側の幅
	SCharaHitInfo         StageHitTopInfo;

	// キャラクタータイプ別の情報へのポインタ
	void *                SubData;
} SCharaBaseInfo;

// キャラクター別の基本情報用のコールバック関数
typedef struct _SCharaBaseFunction
{
	// キャラクターの基本情報初期化用関数のポインタ
	bool(*Initialize)(SCharaBaseInfo *CBInfo, STextParamInfo *TextParam);
} SCharaBaseFunction;

// キャラクター基本情報の初期化
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した  false : 失敗した )
extern bool CharaBase_Initialize(void);

// キャラクター基本情報の後始末
extern void CharaBase_Terminate(void);

// 指定のキャラクター基本情報のアドレスを取得する
//     戻り値 : キャラクター基本情報構造体のアドレス
extern SCharaBaseInfo * CharaBase_GetInfo(
	// キャラクター
	EChara Chara
);

// 指定の武器基本情報のアドレスを取得する
//     戻り値 : 武器基本情報のアドレス
extern const SCharaWeaponBaseInfo * CharaBase_GetWeaponInfo(
	// 武器
	ECharaWeapon Weapon
);

// 指定の素材の共通サウンドの登録番号を取得する
//     戻り値 : サウンドの登録番号
extern int CharaBase_GetWeaponCommonMaterialSound(
	// 素材タイプ
	EMaterialType MatType
);

// 指定の共通効果音の登録番号を取得する
//     戻り値 : サウンドの登録番号
extern int CharaBase_GetCommonSE(
	// キャラクター共通効果音
	ECharaCommonSE CommonSE
);

// 指定のキャラクターアニメーションのファイル情報を取得する
//     戻り値 : アニメーションのファイル情報構造体のアドレス
extern const SCharaAnimFileInfo * CharaBase_GetAnimFileInfo(
	// キャラクターアニメーション
	ECharaAnim Anim
);

// 指定の素材タイプの名前を取得する
//     戻り値 : 名前文字列のアドレス
extern const char * GetMaterialTypeName(
	// 素材タイプ
	EMaterialType MatType
);

// 当たり判定の情報から、当たり判定に使用するカプセル形状を形成する２座標を取得する
extern void CharaBase_GetHitCapsulePos(
	// 当たり判定情報のアドレス
	const SCharaHitInfo *HitInfo,

	// 当たり判定を行うキャラクターの座標のアドレス
	const VECTOR *Position,

	// 計算後のカプセルの座標を格納するアドレス
	VECTOR *CapsulePosition1,
	VECTOR *CapsulePosition2
);

#endif
