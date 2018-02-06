#include "CharaBase.h"

#include "Chara_PlayerBase.h"
#include "Chara_EnemyBase.h"

// 全キャラクターが共通で使用する効果音のファイルがあるフォルダ
#define CHARAWEAPON_COMMONSOUND_DIR "SE\\Weapon\\Common\\"

// 全キャラクターが共通で使用する効果音の登録番号
int                  g_CharaCommonSE[ECharaCommonSE_Num];

// 素材に合った武器の衝突音が無い場合に使用される素材別の衝突音の登録番号
int                  g_CharaWeaponCommonMaterialSound[EMaterialType_Num];

// キャラクターの武器の基本情報
SCharaWeaponBaseInfo g_CharaWeaponBaseInfo[ECharaWeapon_Num];

// キャラクターの基本情報
SCharaBaseInfo       g_CharaBaseInfo[EChara_Num];

// 各キャラクター別の基本情報用のコールバック関数
static SCharaBaseFunction g_CharaBaseFunctionTable[EChara_Num] =
{
	{ Chara_PlayerBase_Initialize },// EChara_Player
	{ Chara_EnemyBase_Initialize },	// EChara_Goblin
	{ Chara_EnemyBase_Initialize },	// EChara_Bee
	{ Chara_EnemyBase_Initialize },	// EChara_Golem
	{ Chara_EnemyBase_Initialize },	// EChara_RedGoblin
	{ NULL },						// EChara_Needle
};

// キャラクターの各アニメーションのファイル情報
static SCharaAnimFileInfo g_CharaAnimBaseInfoTable[ECharaAnim_Num] =
{
	true,	"Anim_Neutral",			// ECharaAnim_Neutral = 0,
	true,	"Anim_Walk",			// ECharaAnim_Walk,
	true,	"Anim_Run",				// ECharaAnim_Run,
	false,	"Anim_Jump_In",			// ECharaAnim_Jump_In,
	false,	"Anim_Jump_Loop",		// ECharaAnim_Jump_Loop,
	false,	"Anim_Jump_Out",		// ECharaAnim_Jump_Out,
	false,	"Anim_Landed",			// ECharaAnim_Landed,
	false,	"Anim_Atk1",			// ECharaAnim_Atk1,
	false,	"Anim_Atk2",			// ECharaAnim_Atk2,
	false,	"Anim_Atk3",			// ECharaAnim_Atk3,
	false,	"Anim_Guard_In",		// ECharaAnim_Guard_In,
	true,	"Anim_Guard_Loop",		// ECharaAnim_Guard_Loop,
	false,	"Anim_Guard_Impact",	// ECharaAnim_Guard_Impact,
	false,	"Anim_Guard_Out",		// ECharaAnim_Guard_Out,
	false,	"Anim_Damage",			// ECharaAnim_Damage,
	false,	"Anim_Blow_In",			// ECharaAnim_Blow_In,
	true,	"Anim_Blow_Loop",		// ECharaAnim_Blow_Loop,
	false,	"Anim_Blow_Out",		// ECharaAnim_Blow_Out,
	false,	"Anim_Down",			// ECharaAnim_Down,
	true,	"Anim_Down_Loop",		// ECharaAnim_Down_Loop,
};

// 素材タイプ名
static const char *g_MaterialTypeName[EMaterialType_Num] =
{
	"grass",			// EMaterialType_Grass
	"metal",			// EMaterialType_Metal
	"soil",				// EMaterialType_Soil
	"stone",			// EMaterialType_Stone
	"wood",				// EMaterialType_Wood
};

// キャラクタータイプ名
static const char *g_CharaTypeName[ECharaType_Num] =
{
	"Player",			// ECharaType_Player = 0,
	"Enemy",			// ECharaType_Enemy.
	"EnemyShot",		// ECharaType_EnemyShot.
};

// キャラクターの攻撃判定形状タイプの名前
static const char *g_CharaAtkFormTypeName[ECharaAtkFormType_Num] =
{
	"Poly",				// ECharaAtkFormType_Poly = 0,
	"Sphere",			// ECharaAtkFormType_Sphere.
};

// 全キャラクターが共通で使用する効果音の名前
static const char *g_CharaCommonSEName[ECharaCommonSE_Num] =
{
	"enemy_Nifram",
};

// キャラクターの武器の基本情報初期化
//     戻り値 : 初期化が成功したかどうか
//              ( true : 成功した   false : 失敗した )
static bool CharaBase_WeaponInitialize(void)
{
	STextParamInfo        CommonTextParam;
	STextParamInfo        TextParam;
	SCharaWeaponBaseInfo *WBInfo;
	char                  String[256];
	const char           *DirectoryName;
	const char           *ParamString;
	int                   i;
	int                   j;

	// 各武器のファイルが格納されているフォルダ名が記述されている
	// テキストファイルを読み込む
	if (!LoadTextParam(&CommonTextParam, CHARADATA_DIR "Weapon\\Param.txt"))
	{
		return false;
	}

	// 武器の数だけ繰り返し
	WBInfo = g_CharaWeaponBaseInfo;
	for (i = 0; i < ECharaWeapon_Num; i++, WBInfo++)
	{
		// 武器のファイルが格納されているフォルダ名を取得する
		DirectoryName = GetTextParamString(
			&CommonTextParam, "CharaWeapon%03d_DirectoryName", i);
		strcpy(WBInfo->DirectoryName, DirectoryName);

		// 武器のパラメータが記述されたテキストファイルを読み込む
		if (!LoadTextParam(&TextParam, CHARADATA_DIR "Weapon\\%s\\Param.txt",
			WBInfo->DirectoryName))
		{
			continue;
		}

		// 武器の名前を取得
		ParamString = GetTextParamString(&TextParam, "Name");
		strcpy(WBInfo->Name, ParamString);

		// ３Ｄモデルファイル名を取得
		ParamString = GetTextParamString(&TextParam, "ModelPath");
		if (ParamString == NULL)
		{
			WBInfo->ModelHandle = -1;
		}
		else
		{
			// ３Ｄモデルファイルを読み込み
			sprintf(String, CHARADATA_DIR "Weapon\\%s\\%s",
				WBInfo->DirectoryName, ParamString);
			WBInfo->ModelHandle = MV1LoadModel(String);
			if (WBInfo->ModelHandle == -1)
			{
				return false;
			}
		}

		// 武器が何かに衝突したときに鳴らす共通の音の登録
		sprintf(String, "SE\\Weapon\\%s\\MAT_impact_com", WBInfo->DirectoryName);
		WBInfo->MaterialCommonSound = Sound_AddSound(String, true);

		// 武器が各素材に衝突したときに鳴らす音の登録
		for (j = 0; j < EMaterialType_Num; j++)
		{
			sprintf(String, "SE\\%Weapon\\s\\MAT_impact_%s",
				WBInfo->DirectoryName, g_MaterialTypeName[i]);
			WBInfo->MaterialSound[j] = Sound_AddSound(String, true);
		}
	}

	// 成功終了
	return true;
}

// キャラクター基本情報の初期化
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した  false : 失敗した )
bool CharaBase_Initialize(void)
{
	int                   i;
	int                   j;
	int                   k;
	SCharaBaseInfo       *CBInfo;
	SCharaWeaponBaseInfo *WBInfo;
	STextParamInfo        CommonTextParam;
	STextParamInfo        TextParam;
	char                  String[256];
	const char           *DirectoryName;
	const char           *ParamString;

	// 各キャラクターのファイルが格納されているフォルダ名が記述されている
	// テキストファイルの読み込み
	if (!LoadTextParam(&CommonTextParam, CHARADATA_DIR "Param.txt"))
	{
		return false;
	}

	// 全キャラクター共通で使用する効果音の読み込み
	for (i = 0; i < ECharaCommonSE_Num; i++)
	{
		sprintf(String, "SE\\Common\\%s", g_CharaCommonSEName[i]);
		g_CharaCommonSE[i] = Sound_AddSound(String, true);
	}

	// 素材に合った武器の衝突音が無い場合に使用される素材別の衝突音の読み込み
	for (i = 0; i < EMaterialType_Num; i++)
	{
		sprintf(String, CHARAWEAPON_COMMONSOUND_DIR "MAT_impact_%s",
			g_MaterialTypeName[i]);
		g_CharaWeaponCommonMaterialSound[i] = Sound_AddSound(String, true);
	}

	// キャラクターの武器情報を初期化
	CharaBase_WeaponInitialize();

	// キャラクターの数だけ繰り返し
	CBInfo = g_CharaBaseInfo;
	for (i = 0; i < EChara_Num; i++, CBInfo++)
	{
		// キャラクターのファイルが格納されているフォルダ名の取得
		DirectoryName = GetTextParamString(
			&CommonTextParam, "Chara%03d_DirectoryName", i);
		strcpy(CBInfo->DirectoryName, DirectoryName);

		// キャラクターのパラメータが記述されたテキストファイルの読み込み
		if (!LoadTextParam(&TextParam, CHARADATA_DIR "%s\\Param.txt",
			CBInfo->DirectoryName))
		{
			continue;
		}

		// キャラクター名の取得
		ParamString = GetTextParamString(&TextParam, "Name");
		strcpy(CBInfo->Name, ParamString);

		// キャラクターのタイプを所得
		ParamString = GetTextParamString(&TextParam, "Type");
		for (j = 0; j < ECharaType_Num; j++)
		{
			if (strcmp(ParamString, g_CharaTypeName[j]) == 0)
			{
				break;
			}
		}
		CBInfo->Type = (ECharaType)j;

		// キャラクターの体力を取得
		CBInfo->Hp = GetTextParamInt(&TextParam, "Hp");

		// キャラクターの３Ｄモデルファイル名を取得＋ファイルの読み込み
		ParamString = GetTextParamString(&TextParam, "MainModelPath");
		sprintf(String, CHARADATA_DIR "%s\\%s", CBInfo->DirectoryName, ParamString);
		CBInfo->ModelHandle = MV1LoadModel(String);
		if (CBInfo->ModelHandle == -1)
		{
			return false;
		}

		// 各武器によるダメージを受けた際に鳴らす音の読み込み
		WBInfo = g_CharaWeaponBaseInfo;
		for (j = 0; j < ECharaWeapon_Num; j++, WBInfo++)
		{
			sprintf(String, "SE\\%s\\dmg_by%s", DirectoryName, WBInfo->DirectoryName);
			CBInfo->WeaponAtkDamageSound[j] = Sound_AddSound(String, true);
		}

		// 素材別のアニメーション物理音の読み込み
		for (j = 0; j < EAnimPhysicsSound_Num; j++)
		{
			for (k = 0; k < EMaterialType_Num; k++)
			{
				sprintf(String, "SE\\%s\\MAT_%s_%s", DirectoryName,
					GetAnimPhysicsSoundName((EAnimPhysicsSound)j),
					g_MaterialTypeName[k]);
				CBInfo->AnimPhysicsSound[j][k] = Sound_AddSound(String, true);
			}
		}

		// ルートフレーム名の取得と、ルーとフレーム番号を取得
		ParamString = GetTextParamString(&TextParam, "RootFrameName");
		CBInfo->RootFrameIndex = MV1SearchFrame(CBInfo->ModelHandle, ParamString);

		// アニメーションの数だけ繰り返し
		for (j = 0; j < ECharaAnim_Num; j++)
		{
			// ３Ｄモデルのアニメーションファイルを読み込み
			sprintf(String, CHARADATA_DIR "%s\\%s.mv1",
				CBInfo->DirectoryName, g_CharaAnimBaseInfoTable[j].FileName);

			// ファイルがある場合のみ読み込み処理を行う
			if (FileRead_size(String) > 0)
			{
				CBInfo->AnimInfo[j].Handle = MV1LoadModel(String);
			}

			// アニメーション情報ファイルの読み込み
			sprintf(String, CHARADATA_DIR "%s\\%s",
				CBInfo->DirectoryName, g_CharaAnimBaseInfoTable[j].FileName);
			LoadAnimInfoFile(&CBInfo->AnimInfo[j].Info, String);

			// アニメーション中に再生する音の読み込み
			LoadAnimInfoSound(&CBInfo->AnimInfo[j].Info);
		}

		// 装備武器名を取得
		ParamString = GetTextParamString(&TextParam, "EquipWeapon");

		// 装備武器があるかどうかで処理を分岐
		if (ParamString == NULL)
		{
			// 無い場合
			CBInfo->EquipWeapon = ECharaWeapon_Num;
			CBInfo->EquipWeaponAttachFrameIndex = -1;
		}
		else
		{
			// ある場合は武器名から列挙型の値に変換
			WBInfo = g_CharaWeaponBaseInfo;
			for (j = 0; j < ECharaWeapon_Num; j++, WBInfo++)
			{
				if (strcmp(ParamString, WBInfo->DirectoryName) == 0)
				{
					break;
				}
			}
			CBInfo->EquipWeapon = (ECharaWeapon)j;

			// 武器３Ｄモデルをアタッチする本体モデルのフレーム名を取得
			ParamString = GetTextParamString(&TextParam, "EquipWeapon_AttachFrame");
			if (ParamString == NULL)
			{
				CBInfo->EquipWeaponAttachFrameIndex = -1;
			}
			else
			{
				// フレーム名からフレーム番号を取得
				CBInfo->EquipWeaponAttachFrameIndex =
					MV1SearchFrame(CBInfo->ModelHandle, ParamString);
			}
		}

		// サブ３Ｄモデルの読み込み
		for (j = 0; j < CHARA_MAX_SUBMODEL; j++)
		{
			// サブ３Ｄモデルのファイルパスを取得
			ParamString = GetTextParamString(&TextParam, "SubModel%03d_Path", j);

			// 無かったらループを抜ける
			if (ParamString == NULL)
			{
				break;
			}

			// サブ３Ｄモデルの読み込み
			sprintf(String, CHARADATA_DIR "%s\\%s", CBInfo->DirectoryName, ParamString);
			CBInfo->SubModel[j].ModelHandle = MV1LoadModel(String);
			if (CBInfo->SubModel[j].ModelHandle == -1)
			{
				return false;
			}

			// サブ３Ｄモデルをアタッチする本体３Ｄモデルのフレーム名とフレーム番号を取得
			ParamString = GetTextParamString(&TextParam, "SubModel%03d_AttachFrame", j);
			CBInfo->SubModel[j].AttachFrameIndex =
				MV1SearchFrame(CBInfo->ModelHandle, ParamString);

			// サブ３Ｄモデルの数をインクリメント
			CBInfo->SubModelNum++;
		}

		// 体力ゲージを描画する相対座標を取得
		CBInfo->HpGaugePosition =
			GetTextParamVector(&TextParam, "HpGaugePosition");

		// 攻撃位置情報の取得
		for (j = 0; j < CHARA_ATTACK_POS_MAX_NUM; j++)
		{
			// 起点となる３Ｄモデルのフレーム名を取得
			ParamString = GetTextParamString(&TextParam,
				"AtkPosInfo%d_StartFrameName", j);
			if (ParamString != NULL)
			{
				// フレーム名からフレーム番号を取得
				CBInfo->AtkPosInfo[j].StartFrameIndex =
					MV1SearchFrame(CBInfo->ModelHandle, ParamString);

				// 攻撃判定の形成に使用される起点からの相対座標の取得
				CBInfo->AtkPosInfo[j].EndLocalPosition =
					GetTextParamVector(&TextParam,
						"AtkPosInfo%d_EndLocalPosition", j);

				// 攻撃判定の球の半径を取得
				CBInfo->AtkPosInfo[j].SphereSize =
					GetTextParamFloat(&TextParam, "AtkPosInfo%d_SphereSize", j);

				// 攻撃の軌跡エフェクトの色を取得
				CBInfo->AtkPosInfo[j].EffectColor =
					GetTextParamColor(&TextParam, "AtkPosInfo%d_EffectColor", j);

				// 攻撃判定の形状を取得
				ParamString =
					GetTextParamString(&TextParam, "AtkPosInfo%d_FormType", j);
				for (k = 0; k < ECharaType_Num; k++)
				{
					if (strcmp(ParamString, g_CharaAtkFormTypeName[k]) == 0)
					{
						break;
					}
				}
				CBInfo->AtkPosInfo[j].FormType = (ECharaAtkFormType)k;
			}
			else
			{
				// 無い場合はデフォルト値で初期化
				CBInfo->AtkPosInfo[j].StartFrameIndex = -1;
				CBInfo->AtkPosInfo[j].EndLocalPosition = VGet(0.0f, 0.0f, 0.0f);
				CBInfo->AtkPosInfo[j].EffectColor = GetColorU8(0, 0, 0, 0);
				CBInfo->AtkPosInfo[j].FormType = ECharaAtkFormType_Poly;
				CBInfo->AtkPosInfo[j].SphereSize = 1.0f;
			}
		}

		// プログラムで移動する場合の速度を取得
		CBInfo->ProgramSpd = GetTextParamFloat(&TextParam, "ProgramSpd");

		// 丸影のサイズを取得
		CBInfo->ShadowSize = GetTextParamFloat(&TextParam, "ShadowSize");

		// ダメージ判定の情報を取得
		CBInfo->DamageHitInfo.Width =
			GetTextParamFloat(&TextParam, "DamageHitWidth");

		CBInfo->DamageHitInfo.Height =
			GetTextParamFloat(&TextParam, "DamageHitHeight");

		CBInfo->DamageHitInfo.CenterPosition =
			GetTextParamVector(&TextParam, "DamageHitCenterPosition");

		// キャラクター同士の当たり判定の情報を取得
		CBInfo->CharaHitInfo.Width =
			GetTextParamFloat(&TextParam, "CharaHitWidth");

		CBInfo->CharaHitInfo.Height =
			GetTextParamFloat(&TextParam, "CharaHitHeight");

		CBInfo->CharaHitInfo.CenterPosition =
			GetTextParamVector(&TextParam, "CharaHitCenterPosition");

		// ステージとの当たり判定の情報を取得
		CBInfo->StageHitBottomInfo.Width =
			GetTextParamFloat(&TextParam, "StageHitBottomWidth");

		CBInfo->StageHitBottomInfo.Height =
			GetTextParamFloat(&TextParam, "StageHitBottomHeight");

		CBInfo->StageHitBottomInfo.CenterPosition =
			GetTextParamVector(&TextParam, "StageHitBottomCenterPosition");

		CBInfo->StageHitTopInfo.Width =
			GetTextParamFloat(&TextParam, "StageHitTopWidth");

		CBInfo->StageHitTopInfo.Height =
			GetTextParamFloat(&TextParam, "StageHitTopHeight");

		CBInfo->StageHitTopInfo.CenterPosition =
			GetTextParamVector(&TextParam, "StageHitTopCenterPosition");

		// キャラクター別の初期化関数がある場合は呼ぶ
		if (g_CharaBaseFunctionTable[i].Initialize != NULL)
		{
			if (!g_CharaBaseFunctionTable[i].Initialize(CBInfo, &TextParam))
			{
				return false;
			}
		}
	}

	// 成功終了
	return true;
}

// キャラクター基本情報の後始末
void CharaBase_Terminate(void)
{
	int                   i;
	int                   j;
	SCharaBaseInfo       *CBInfo;
	SCharaWeaponBaseInfo *WBInfo;

	// 武器の数だけ繰り返し
	WBInfo = g_CharaWeaponBaseInfo;
	for (i = 0; i < ECharaWeapon_Num; i++, WBInfo++)
	{
		// 武器の３Ｄモデルを削除
		MV1DeleteModel(WBInfo->ModelHandle);
		WBInfo->ModelHandle = -1;
	}

	// キャラクターの数だけ繰り返し
	CBInfo = g_CharaBaseInfo;
	for (i = 0; i < EChara_Num; i++, CBInfo++)
	{
		// 本体の３Ｄモデルを削除
		MV1DeleteModel(CBInfo->ModelHandle);
		CBInfo->ModelHandle = -1;

		// 全てのサブモデルを削除
		for (j = 0; j < CBInfo->SubModelNum; j++)
		{
			MV1DeleteModel(CBInfo->SubModel[j].ModelHandle);
			CBInfo->SubModel[j].ModelHandle = -1;
		}

		// キャラクタータイプ別の情報用にメモリを確保していた場合は解放
		if (CBInfo->SubData != NULL)
		{
			free(CBInfo->SubData);
			CBInfo->SubData = NULL;
		}
	}
}

// 指定のキャラクター基本情報のアドレスを取得する
//     戻り値 : キャラクター基本情報構造体のアドレス
SCharaBaseInfo * CharaBase_GetInfo(
	// キャラクター
	EChara Chara
)
{
	return &g_CharaBaseInfo[Chara];
}

// 指定の武器基本情報のアドレスを取得する
//     戻り値 : 武器基本情報のアドレス
const SCharaWeaponBaseInfo * CharaBase_GetWeaponInfo(
	// 武器
	ECharaWeapon Weapon
)
{
	return &g_CharaWeaponBaseInfo[Weapon];
}

// 指定の素材の共通サウンドの登録番号を取得する
//     戻り値 : サウンドの登録番号
int CharaBase_GetWeaponCommonMaterialSound(
	// 素材タイプ
	EMaterialType MatType
)
{
	return g_CharaWeaponCommonMaterialSound[MatType];
}

// 指定の共通効果音の登録番号を取得する
//     戻り値 : サウンドの登録番号
int CharaBase_GetCommonSE(
	// キャラクター共通効果音
	ECharaCommonSE CommonSE
)
{
	return g_CharaCommonSE[CommonSE];
}

// 指定のキャラクターアニメーションのファイル情報を取得する
//     戻り値 : アニメーションのファイル情報構造体のアドレス
const SCharaAnimFileInfo * CharaBase_GetAnimFileInfo(
	// キャラクターアニメーション
	ECharaAnim Anim
)
{
	return &g_CharaAnimBaseInfoTable[Anim];
}

// 指定の素材タイプの名前を取得する
//     戻り値 : 名前文字列のアドレス
const char * GetMaterialTypeName(
	// 素材タイプ
	EMaterialType MatType
)
{
	return g_MaterialTypeName[MatType];
}

// 当たり判定の情報から、当たり判定に使用するカプセル形状を形成する２座標を取得する
void CharaBase_GetHitCapsulePos(
	// 当たり判定情報のアドレス
	const SCharaHitInfo *HitInfo,

	// 当たり判定を行うキャラクターの座標のアドレス
	const VECTOR *Position,

	// 計算後のカプセルの座標を格納するアドレス
	VECTOR *CapsulePosition1,
	VECTOR *CapsulePosition2
)
{
	*CapsulePosition1 =
		*CapsulePosition2 = VAdd(*Position, HitInfo->CenterPosition);

	CapsulePosition1->y -= HitInfo->Height / 2.0f;
	CapsulePosition2->y += HitInfo->Height / 2.0f;
}