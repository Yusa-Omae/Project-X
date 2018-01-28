#ifndef STAGEDATA_H
#define STAGEDATA_H

#include "Sound.h"
#include "CharaBase.h"
#include "TextParam.h"

// ステージデータファイルの先頭に書き込む文字列
#define STAGEFILE_ID								"MAPD"

// ステージデータに含められる最大キャラ数
#define STAGE_CHARA_MAX_NUM							(256)

// ステージデータに含められる最大オブジェクト数
#define STAGE_OBJECT_MAX_NUM						(512)

// ステージデータに含められる最大オブジェクト用モデル数
#define STAGE_OBJECT_MODEL_MAX_NUM					(256)

// ステージデータに含められる最大イベント数
#define STAGE_EVENT_MAX_NUM							(256)

// 一つのイベント辺りに指定できるオブジェクトの最大数
#define STAGE_EVENT_OBJECT_MAX_NUM					(16)

// コリジョンモデルの最大マテリアル数
#define STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM		(32)

// ステージの描画タイプ
typedef enum _EStageRenderType
{
	EStageRenderType_Outdoors,				// 屋外
	EStageRenderType_Cave,					// 洞窟
} EStageRenderType;

// ステージクリア条件タイプ
typedef enum _EStageClearCondition
{
	EStageClearCondition_CharaKillNum,		// 敵を指定数倒す
	EStageClearCondition_TargetCharaKill,	// 指定の敵を倒す
} EStageClearCondition;

// ステージイベントのタイプ
typedef enum _EStageEventType
{
	EStageEventType_CharaAppear,			// キャラを出現させる
	EStageEventType_BGM,					// ＢＧＭを変更する

	EStageEventType_Num						// ステージイベントのタイプ数
} EStageEventType;

// ステージに配置するキャラの情報
typedef struct _SStageCharaInfo
{
	// キャラクター
	EChara                Chara;

	// 配置する座標
	VECTOR                Position;

	// 配置時の向き
	float                 Angle;
} SStageCharaInfo;

// オブジェクトで使用する３Ｄモデルの情報
typedef struct _SStageObjectModelInfo
{
	// 読み込まれているかどうかのフラグ
	bool                  Load;

	// ３Ｄモデルハンドル
	int                   ModelHandle;

	// コリジョン用３Ｄモデルハンドル
	int                   CollisionModelHandle;

	// コリジョン３Ｄモデルのマテリアルタイプ
	EMaterialType         ColModelMatType[STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM];
} SStageObjectModelInfo;

// ステージに配置するオブジェクトの情報
typedef struct _SStageObjectInfo
{
	// 使用する３Ｄモデル
	int                   ModelID;

	// 配置する座標
	VECTOR                Position;

	// 配置時の向き
	VECTOR                Angle;

	// 配置時のスケール
	VECTOR                Scale;

	// 実行時用の３Ｄモデルハンドル
	int                   ModelHandle;

	// 実行時用のコリジョン用３Ｄモデルハンドル
	int                   CollisionModelHandle;
} SStageObjectInfo;

// ステージ中のイベントの情報
typedef struct _SStageEventInfo
{
	// イベントタイプ
	EStageEventType       EventType;

	// 配置座標
	VECTOR                Position;

	// イベント発動判定の範囲
	VECTOR                AreaSize;

	// BGM変更イベント用のBGM情報
	EBGM                  Bgm;

	// 何らかのオブジェクトに対するイベント用の、対象のオブジェクトの数
	int                   ObjectNum;

	// 何らかのオブジェクトに対するイベント用のオブジェクト番号の配列
	int                   Object[STAGE_EVENT_OBJECT_MAX_NUM];
} SStageEventInfo;

// ステージデータファイルのヘッダー
typedef struct _SStageInfoFileHeader
{
	// ステージデータファイル確認用文字列
	char                  MagicID[4];

	// キャラ配置情報の数
	int                   CharaInfoNum;

	// オブジェクト配置情報の数
	int                   ObjectInfoNum;

	// イベント情報の数
	int                   EventInfoNum;

	// ステージ開始時のプレイヤーの座標
	VECTOR                PlayerPosition;

	// ステージ開始時のプレイヤーの向き
	float                 PlayerAngle;

	// ステージで使用する描画タイプ
	EStageRenderType      RenderType;

	// ステージクリア条件
	EStageClearCondition  StageClearCondition;

	// ステージクリアに必要な倒す敵の数
	// ( StageClearCondition が EStageClearCondition_CharaKillNum の場合に使用 )
	int                   KillCharaNum;

	// ステージクリアに必要な倒すべき敵の番号
	// ( StageClearCondition が EStageClearCondition_TargetCharaKill の場合に使用 )
	int                   KillTargetChara;
} SStageInfoFileHeader;

// ステージデータ処理の情報
typedef struct _SStageData
{
	// 全ステージ数
	int                   TotalStageNum;

	// 現在読み込んであるステージの番号
	int                   LoadStageNumber;

	// ステージデータファイルのヘッダー
	SStageInfoFileHeader  Header;

	// ステージの３Ｄモデルハンドル
	int                   ModelHandle;

	// ステージのコリジョン用３Ｄモデルハンドル
	int                   CollisionModelHandle;

	// ステージの空表現用３Ｄモデルハンドル
	int                   SkyModelHandle;

	// コリジョン３Ｄモデルのマテリアルタイプ
	EMaterialType         ColModelMatType[STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM];

	// ステージのキャラ配置情報
	SStageCharaInfo       CharaInfo[STAGE_CHARA_MAX_NUM];

	// ステージのオブジェクト配置情報
	SStageObjectInfo      ObjectInfo[STAGE_OBJECT_MAX_NUM];

	// ステージのイベント情報
	SStageEventInfo       EventInfo[STAGE_EVENT_MAX_NUM];

	// オブジェクトが使用する３Ｄモデルを常に全て読み込むかどうかのフラグ
	bool                  ObjectModelAlwaysLoad;

	// オブジェクトが使用する３Ｄモデルハンドル
	SStageObjectModelInfo ObjectModelInfo[STAGE_OBJECT_MODEL_MAX_NUM];

	// ステージ用テキストパラメータ
	STextParamInfo        TextParam;
} SStageData;

extern SStageData g_StageData;

// ステージデータ処理を初期化する
//     戻り値 : 初期化が成功したかどうか( true:成功した  false:失敗した )
extern bool StageData_Initialize(
	// オブジェクトが使用する３Ｄモデルを常にすべて読み込むかどうか
	// ( true:全て読み込む  false:必要なものだけ読み込む )
	bool ObjectModelAlwaysLoad
);

// ステージデータ処理の後始末をする
extern void StageData_Terminate(void);

// ステージデータを読み込む
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool StageData_Load(
	// 読み込むステージの番号
	int StageNumber
);

// ステージデータを解放する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern void StageData_Unload(void);

// ステージデータを保存する
//     戻り値 : ステージデータの保存に成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool StageData_Save(void);

// ステージの３Ｄモデルハンドルを取得する
//     戻り値 : ステージの３Ｄモデルハンドル
extern int StageData_GetModelHandle(void);

// ステージのコリジョン用３Ｄモデルハンドルを取得する
//     戻り値 : ステージのコリジョン用３Ｄモデルハンドル
extern int StageData_GetCollisionModelHandle(void);

// ステージに配置されているオブジェクトの数を取得する
//     戻り値 : ステージに配置されているオブジェクトの数
extern int StageData_GetObjectNum(void);

// オブジェクトが使用するコリジョン用３Ｄモデルハンドルを取得する
//     戻り値 : コリジョン用３Ｄモデルハンドル
extern int StageData_GetObjectCollisionModelHandle(
	// オブジェクトが使用する３Ｄモデルの番号
	int Index
);

// オブジェクトのコリジョン用３Ｄモデルの指定番号のマテリアルタイプを取得する
extern EMaterialType StageData_GetObjectCollisionModelMaterialType(
	// オブジェクトが使用する３Ｄモデルの番号
	int Index,

	// コリジョン用３Ｄモデルのマテリアル番号
	int MaterialIndex
);

// 全ステージ数を取得する
//     戻り値 : 全ステージ数
extern int StageData_GetTotalStageNum(void);

// 読み込んであるステージの番号を取得する
//     戻り値 : ステージ番号
extern int StageData_GetLoadStageNo(void);

// ステージの描画タイプを取得する
//     戻り値 : 描画タイプ
extern EStageRenderType StageData_GetRenderType(void);

// ステージのクリア条件を取得する
//     戻り値 : ステージのクリア条件
extern EStageClearCondition StageData_GetStageClearCondition(void);

// ステージをクリアするために倒さなければならない敵の数を取得する
//     戻り値 :  ステージをクリアするために倒さなければならない敵の数
extern int StageData_GetKillCharaNum(void);

// ステージをクリアするために倒さなければならない敵の番号を取得する
//     戻り値 : ステージをクリアするために倒さなければならない敵の番号
extern int StageData_GetKillTargetChara(void);

// ステージのコリジョン用３Ｄモデルの指定番号のマテリアルタイプを取得する
//     戻り値 : マテリアルタイプ
extern EMaterialType StageData_GetCollisionModelMaterialType(
	// コリジョン用３Ｄモデルのマテリアル番号
	int MaterialIndex
);

#endif