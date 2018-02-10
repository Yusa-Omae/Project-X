#include"StageData.h"
#include"Define.h"
#include"BinaryFile.h"
#include"DxLib.h"
#include<malloc.h>
#include"Sound.h"

SStageData g_StageData;

// コリジョン用３Ｄモデルの指定の番号のマテリアルタイプを取得する
static EMaterialType StageData_GetCollisionModelMaterialType(
	// コリジョン用３Ｄモデルハンドル
	int CollisionModelHandle,

	// 調べるマテリアルの番号
	int MaterialIndex
);

// オブジェクトの３Ｄモデルを読み込む
//     戻り値 : 読み込みが成功したかどうか( true:成功した  false:失敗した )
static bool StageData_ObjectModel_Load(
	// 解放するオブジェクトの３Ｄモデルの番号
	int Index
);

// オブジェクトの３Ｄモデルを解放する
static void StageData_ObjectModel_Release(
	// 解放するオブジェクトの３Ｄモデルの番号
	int Index
);

// ステージデータ処理を初期化する
//     戻り値 : 初期化が成功したかどうか( true:成功した  false:失敗した )
bool StageData_Initialize(
	// オブジェクトが使用する３Ｄモデルを常にすべて読み込むかどうか
	// ( true:全て読み込む  false:必要なものだけ読み込む )
	bool ObjectModelAlwaysLoad
)
{
	int i;

	// ステージの情報が記述されているテキストファイルを読み込む
	if (!LoadTextParam(&g_StageData.TextParam, "Data\\Stage\\Param.txt"))
	{
		return false;
	}

	// 全ステージ数を取得
	g_StageData.TotalStageNum = GetTextParamInt(&g_StageData.TextParam, "StageNum");

	// 『現在読み込まれているステージ番号』を『何も読み込まれていない』にセット
	g_StageData.LoadStageNumber = -1;

	// オブジェクトの３Ｄモデルを常に全て読み込むかどうかのフラグを保存
	g_StageData.ObjectModelAlwaysLoad = ObjectModelAlwaysLoad;

	// オブジェクトの３Ｄモデルを常に全て読み込む場合はここで全て読み込む
	if (g_StageData.ObjectModelAlwaysLoad)
	{
		for (i = 0; i < OBJECT_DRAW_MAX_NUM; i++)
		{
			StageData_ObjectModel_Load(i);
		}
	}

	// 成功終了
	return true;
}

// ステージデータ処理の後始末をする
void StageData_Terminate(void)
{
	int i;

	// ステージデータの解放を行う
	StageData_Unload();

	// オブジェクト用の３Ｄモデルを全て解放する
	for (i = 0; i < STAGE_OBJECT_MODEL_MAX_NUM; i++)
	{
		StageData_ObjectModel_Release(i);
	}
}

// コリジョン用３Ｄモデルの指定の番号のマテリアルタイプを取得する
static EMaterialType StageData_GetCollisionModelMaterialType(
	// コリジョン用３Ｄモデルハンドル
	int CollisionModelHandle,

	// 調べるマテリアルの番号
	int MaterialIndex
)
{
	const char   *MaterialName;
	EMaterialType MaterialType;

	// マテリアル名を取得
	MaterialName = MV1GetMaterialName(CollisionModelHandle, MaterialIndex);

	// １文字目が『P』の場合は２文字目と３文字目をマテリアルタイプとして変換する
	if (MaterialName[0] == 'P')
	{
		MaterialType =
			(EMaterialType)((MaterialName[1] - '0') * 10
				+ (MaterialName[2] - '0'));
	}
	else
	{
		// それ以外の場合はマテリアルタイプを石にしておく
		MaterialType = EMaterialType_Stone;
	}

	return MaterialType;
}

// オブジェクトの３Ｄモデルを読み込む
//     戻り値 : 読み込みが成功したかどうか( true:成功した  false:失敗した )
static bool StageData_ObjectModel_Load(
	// 解放するオブジェクトの３Ｄモデルの番号
	int Index
)
{
	SStageObjectModelInfo *OMInfo;
	int                    MaterialNum;
	int                    i;
	char                   FilePath[MAX_PATH];

	OMInfo = &g_StageData.ObjectModelInfo[Index];

	// 読み込まれているかどうかのフラグが立っていたら何もせずに終了
	if (OMInfo->Load)
	{
		return true;
	}

	// ３Ｄモデルのファイルパスを作成
	sprintf(FilePath, "Data\\Stage\\Stage_Obj%03d.mv1", Index);

	// ３Ｄモデルが存在しなかったら失敗
	if (FileRead_size(FilePath) <= 0)
	{
		return false;
	}

	// ３Ｄモデルの読み込み
	OMInfo->ModelHandle = MV1LoadModel(FilePath);

	// ファイルが無かったら失敗終了
	if (OMInfo->ModelHandle < 0)
	{
		return false;
	}

	// コリジョン用の３Ｄモデルの読み込み
	sprintf(FilePath, "Data\\Stage\\Stage_Obj%03d_c.mv1", Index);
	if (FileRead_size(FilePath) > 0)
	{
		OMInfo->CollisionModelHandle = MV1LoadModel(FilePath);

		// コリジョン用３Ｄモデルのマテリアルタイプを調べる
		if (OMInfo->CollisionModelHandle != -1)
		{
			MaterialNum = MV1GetMaterialNum(OMInfo->CollisionModelHandle);
			if (MaterialNum >= STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM)
			{
				MaterialNum = STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM;
			}
			for (i = 0; i < MaterialNum; i++)
			{
				// マテリアルタイプを取得
				OMInfo->ColModelMatType[i] =
					StageData_GetCollisionModelMaterialType(
						OMInfo->CollisionModelHandle, i);
			}
		}
	}
	else
	{
		// コリジョン用の３Ｄモデルが無い場合は -1 を代入しておく
		OMInfo->CollisionModelHandle = -1;
	}

	// 読み込まれているかどうかのフラグを立てる
	OMInfo->Load = true;

	// 正常終了
	return true;
}

// オブジェクトの３Ｄモデルを解放する
static void StageData_ObjectModel_Release(
	// 解放するオブジェクトの３Ｄモデルの番号
	int Index
)
{
	SStageObjectModelInfo *OMInfo;

	OMInfo = &g_StageData.ObjectModelInfo[Index];

	// 読み込まれているかどうかのフラグが倒れていたら何もせずに終了
	if (!OMInfo->Load)
	{
		return;
	}

	// ３Ｄモデルハンドルの削除
	MV1DeleteModel(OMInfo->ModelHandle);
	OMInfo->ModelHandle = -1;

	// コリジョン用３Ｄモデルがある場合はそれも削除
	if (OMInfo->CollisionModelHandle != -1)
	{
		MV1DeleteModel(OMInfo->CollisionModelHandle);
		OMInfo->CollisionModelHandle = -1;
	}

	// 読み込まれているかどうかのフラグを倒す
	OMInfo->Load = false;
}

// ステージデータを読み込む
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool StageData_Load(
	// 読み込むステージの番号
	int StageNumber
)
{
	char                    FilePath[MAX_PATH];
	SStageCharaInfo        *CInfo;
	SStageObjectInfo       *OInfo;
	SStageObjectModelInfo  *OMInfo;
	SStageEventInfo        *EInfo;
	int                     i;
	int                     j;
	int                     ParamInt;
	SBinaryFileData         BinFileData;
	SStageInfoFileHeader   *Header;
	int                     MaterialNum;
	bool                    LoadSuccess;
	bool                    UseObject[STAGE_OBJECT_MODEL_MAX_NUM];

	// 既にステージが読み込まれていた場合のために解放処理を行う
	StageData_Unload();

	// 読み込みが成功したフラグを倒しておく
	LoadSuccess = false;

	// ヘッダのアドレスを代入
	Header = &g_StageData.Header;

	//20回に一度ステージデータ2、それ以外は1を読み込む
	int stageNumberBuffer = (StageNumber % 19 == 0 && StageNumber != 0 ? 1 : 0);

	// ステージデータのバイナリファイルを開く
	if (ReadBinFile_Open(&BinFileData, "Data\\Stage\\Stage%02d.dat", stageNumberBuffer))
	{
		// 最初の４文字が 『MAPD』 かどうかをチェック
		ReadBinFile_Data(&BinFileData, &Header->MagicID, 4);
		if (strncmp(Header->MagicID, STAGEFILE_ID, 4) == 0)
		{
			// ステージデータファイルのヘッダを読み込む
			ReadBinFile_Int(&BinFileData, &Header->CharaInfoNum);
			ReadBinFile_Int(&BinFileData, &Header->ObjectInfoNum);
			ReadBinFile_Int(&BinFileData, &Header->EventInfoNum);
			ReadBinFile_Vector(&BinFileData, &Header->PlayerPosition);
			ReadBinFile_Float(&BinFileData, &Header->PlayerAngle);
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Header->RenderType = (EStageRenderType)ParamInt;
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Header->StageClearCondition = (EStageClearCondition)ParamInt;
			ReadBinFile_Int(&BinFileData, &Header->KillCharaNum);
			ReadBinFile_Int(&BinFileData, &Header->KillTargetChara);

#if true
#ifdef __MY_DEBUG__

			Header->KillCharaNum = 0;

#endif
#endif

			// キャラの配置情報を読み込む
			CInfo = g_StageData.CharaInfo;
			for (i = 0; i < Header->CharaInfoNum; i++, CInfo++)
			{
				ReadBinFile_Int(&BinFileData, &ParamInt);
				CInfo->Chara = (EChara)ParamInt;
				ReadBinFile_Vector(&BinFileData, &CInfo->Position);
				ReadBinFile_Float(&BinFileData, &CInfo->Angle);
			}

			// オブジェクトの配置情報を読み込む
			OInfo = g_StageData.ObjectInfo;
			for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
			{
				ReadBinFile_Int(&BinFileData, &OInfo->ModelID);
				ReadBinFile_Vector(&BinFileData, &OInfo->Position);
				ReadBinFile_Vector(&BinFileData, &OInfo->Angle);
				ReadBinFile_Vector(&BinFileData, &OInfo->Scale);
			}

			// イベントの情報を読み込む
			EInfo = g_StageData.EventInfo;
			for (i = 0; i < Header->EventInfoNum; i++, EInfo++)
			{
				ReadBinFile_Int(&BinFileData, &ParamInt);
				EInfo->EventType = (EStageEventType)ParamInt;
				ReadBinFile_Vector(&BinFileData, &EInfo->Position);
				ReadBinFile_Vector(&BinFileData, &EInfo->AreaSize);
				ReadBinFile_Int(&BinFileData, &ParamInt);
				EInfo->Bgm = (EBGM)ParamInt;

				ReadBinFile_Int(&BinFileData, &EInfo->ObjectNum);
				for (j = 0; j < EInfo->ObjectNum; j++)
				{
					ReadBinFile_Int(&BinFileData, &EInfo->Object[j]);
				}
			}

			// 読み込みが成功したフラグを立てる
			LoadSuccess = true;
		}

		// バイナリファイルを閉じる
		ReadBinFile_Close(&BinFileData);
	}

	// ロードに失敗したらデフォルト情報をセット
	if (!LoadSuccess)
	{
		Header->CharaInfoNum = 0;
		Header->ObjectInfoNum = 0;
		Header->EventInfoNum = 0;
		Header->PlayerPosition = VGet(0.0f, 0.0f, 0.0f);
		Header->PlayerAngle = 0.0f;
		Header->RenderType = (EStageRenderType)0;
		Header->StageClearCondition = EStageClearCondition_CharaKillNum;
		Header->KillCharaNum = 0;
		Header->KillTargetChara = 0;
	}

	// オブジェクトの３Ｄモデルを常に全て読み込む指定になっていない場合は
	// ここで必要なオブジェクトの３Ｄモデルを読み込む
	if (!g_StageData.ObjectModelAlwaysLoad)
	{
		// 使用されているオブジェクトの３Ｄモデルを列挙する
		memset(UseObject, 0, sizeof(UseObject));
		OInfo = g_StageData.ObjectInfo;
		for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
		{
			UseObject[OInfo->ModelID] = true;
		}

		// 使用されていないオブジェクトの３Ｄモデルを解放して、
		// 使用されているオブジェクトの３Ｄモデルを読み込む
		for (i = 0; i < STAGE_OBJECT_MODEL_MAX_NUM; i++)
		{
			// 使用されているかどうかで処理を分岐
			if (UseObject[i])
			{
				// 使用されている場合は読み込み
				if (!StageData_ObjectModel_Load(i))
				{
					return false;
				}
			}
			else
			{
				// 使用されていない場合は解放
				StageData_ObjectModel_Release(i);
			}
		}
	}

	// オブジェクト配置用にオブジェクト用３Ｄモデルから実行用の派生３Ｄモデルを作成する
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
	{
		OMInfo = &g_StageData.ObjectModelInfo[OInfo->ModelID];

		// 実行時の描画用に派生３Ｄモデルハンドルを作成する
		OInfo->ModelHandle = MV1DuplicateModel(OMInfo->ModelHandle);
		if (OInfo->ModelHandle == -1)
		{
			return false;
		}

		// コリジョン用３Ｄモデルがある場合は派生ハンドルを作成する
		if (OMInfo->CollisionModelHandle != -1)
		{
			OInfo->CollisionModelHandle =
				MV1DuplicateModel(OMInfo->CollisionModelHandle);
			if (OInfo->CollisionModelHandle == -1)
			{
				return false;
			}
		}
		else
		{
			OInfo->CollisionModelHandle = -1;
		}

		// 位置と向きとスケールを設定
		MV1SetPosition(OInfo->ModelHandle, OInfo->Position);
		MV1SetRotationXYZ(OInfo->ModelHandle, OInfo->Angle);
		MV1SetScale(OInfo->ModelHandle, OInfo->Scale);
		if (OInfo->CollisionModelHandle != -1)
		{
			MV1SetPosition(OInfo->CollisionModelHandle, OInfo->Position);
			MV1SetRotationXYZ(OInfo->CollisionModelHandle, OInfo->Angle);
			MV1SetScale(OInfo->CollisionModelHandle, OInfo->Scale);
		}

		// 透明度が５０％以上の部分は描画しない設定にする
		MV1SetMaterialDrawAlphaTestAll(OInfo->ModelHandle, TRUE,
			DX_CMP_GREATER, 128);
	}

	// ステージの３Ｄモデルを読み込む
	sprintf(FilePath, "Data\\Stage\\Stage%02d.mv1", stageNumberBuffer);
	g_StageData.ModelHandle = MV1LoadModel(FilePath);
	if (g_StageData.ModelHandle == -1)
	{
		return false;
	}

	// ステージのコリジョン用３Ｄモデルを読み込む
	sprintf(FilePath, "Data\\Stage\\Stage%02d_c.mv1", stageNumberBuffer);
	g_StageData.CollisionModelHandle = MV1LoadModel(FilePath);
	if (g_StageData.CollisionModelHandle == -1)
	{
		return false;
	}

	// ステージの空表現用３Ｄモデルを読み込む
	sprintf(FilePath, "Data\\Stage\\Stage%02d_sky.mv1", stageNumberBuffer);
	g_StageData.SkyModelHandle = MV1LoadModel(FilePath);

	// コリジョン用３Ｄモデルのマテリアルタイプを調べる
	MaterialNum = MV1GetMaterialNum(g_StageData.CollisionModelHandle);
	if (MaterialNum >= STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM)
	{
		MaterialNum = STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM;
	}
	for (i = 0; i < MaterialNum; i++)
	{
		g_StageData.ColModelMatType[i] =
			StageData_GetCollisionModelMaterialType(
				g_StageData.CollisionModelHandle, i);
	}

	// 読み込むんだステージ番号を保存
	g_StageData.LoadStageNumber = StageNumber;

	// 正常終了
	return true;
}

// ステージデータを解放する
void StageData_Unload(void)
{
	SStageObjectInfo *OInfo;
	int               i;

	// ステージが読み込まれていない場合は何もせずに終了
	if (g_StageData.LoadStageNumber < 0)
	{
		return;
	}

	// ステージの３Ｄモデルハンドルを削除
	MV1DeleteModel(g_StageData.ModelHandle);
	g_StageData.ModelHandle = -1;

	// ステージのコリジョン用３Ｄモデルハンドルを削除
	MV1DeleteModel(g_StageData.CollisionModelHandle);
	g_StageData.CollisionModelHandle = -1;

	// ステージの空用３Ｄモデルハンドルを削除
	MV1DeleteModel(g_StageData.SkyModelHandle);
	g_StageData.SkyModelHandle = -1;

	// オブジェクト用の３Ｄモデルハンドルから作成した実行時用３Ｄモデルハンドルを削除
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		MV1DeleteModel(OInfo->ModelHandle);
		OInfo->ModelHandle = -1;

		if (OInfo->CollisionModelHandle != -1)
		{
			MV1DeleteModel(OInfo->CollisionModelHandle);
			OInfo->CollisionModelHandle = -1;
		}
	}

	// 『現在読み込まれているステージ番号』を『何も読み込まれていない』にセット
	g_StageData.LoadStageNumber = -1;
}

// ステージデータを保存する
//     戻り値 : ステージデータの保存に成功したかどうか
//              ( true:成功した  false:失敗した )
bool StageData_Save(void)
{
	SBinaryFileData        BinFileData;
	SStageCharaInfo       *CInfo;
	SStageObjectInfo      *OInfo;
	SStageEventInfo       *EInfo;
	char                   FilePath[MAX_PATH];
	int                    i;
	int                    j;
	SStageInfoFileHeader  *Header;

	// ステージが読み込まれていない場合は何もせずに終了
	if (g_StageData.LoadStageNumber < 0)
	{
		return false;
	}

	// ヘッダのアドレスを代入
	Header = &g_StageData.Header;

	// ステージデータのバイナリファイルを出力

	// 出力用のバイナリファイルを開く
	sprintf(FilePath, "Data\\Stage\\Stage%02d.dat", g_StageData.LoadStageNumber);
	if (!WriteBinFile_Open(&BinFileData, FilePath))
	{
		MessageBox(NULL, "セーブに失敗しました。", "セーブ失敗", MB_OK);
		return false;
	}

	// ステージデータのヘッダを書き出す
	strncpy(g_StageData.Header.MagicID, STAGEFILE_ID, 4);
	WriteBinFile_Data(&BinFileData, Header->MagicID, 4);

	WriteBinFile_Int(&BinFileData, Header->CharaInfoNum);
	WriteBinFile_Int(&BinFileData, Header->ObjectInfoNum);
	WriteBinFile_Int(&BinFileData, Header->EventInfoNum);
	WriteBinFile_Vector(&BinFileData, Header->PlayerPosition);
	WriteBinFile_Float(&BinFileData, Header->PlayerAngle);
	WriteBinFile_Int(&BinFileData, Header->RenderType);
	WriteBinFile_Int(&BinFileData, Header->StageClearCondition);
	WriteBinFile_Int(&BinFileData, Header->KillCharaNum);
	WriteBinFile_Int(&BinFileData, Header->KillTargetChara);

	// キャラの配置情報を書き出す
	CInfo = g_StageData.CharaInfo;
	for (i = 0; i < Header->CharaInfoNum; i++, CInfo++)
	{
		WriteBinFile_Int(&BinFileData, CInfo->Chara);
		WriteBinFile_Vector(&BinFileData, CInfo->Position);
		WriteBinFile_Float(&BinFileData, CInfo->Angle);
	}

	// オブジェクトの配置情報を書き出す
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
	{
		WriteBinFile_Int(&BinFileData, OInfo->ModelID);
		WriteBinFile_Vector(&BinFileData, OInfo->Position);
		WriteBinFile_Vector(&BinFileData, OInfo->Angle);
		WriteBinFile_Vector(&BinFileData, OInfo->Scale);
	}

	// イベントの情報を書き出す
	EInfo = g_StageData.EventInfo;
	for (i = 0; i < Header->EventInfoNum; i++, EInfo++)
	{
		WriteBinFile_Int(&BinFileData, EInfo->EventType);
		WriteBinFile_Vector(&BinFileData, EInfo->Position);
		WriteBinFile_Vector(&BinFileData, EInfo->AreaSize);
		WriteBinFile_Int(&BinFileData, EInfo->Bgm);

		WriteBinFile_Int(&BinFileData, EInfo->ObjectNum);
		for (j = 0; j < EInfo->ObjectNum; j++)
		{
			WriteBinFile_Int(&BinFileData, EInfo->Object[j]);
		}
	}

	// ファイルを閉じる
	WriteBinFile_Close(&BinFileData);

	// 成功終了
	return true;
}

// ステージの３Ｄモデルハンドルを取得する
//     戻り値 : ステージの３Ｄモデルハンドル
int StageData_GetModelHandle(void)
{
	return g_StageData.ModelHandle;
}

// ステージのコリジョン用３Ｄモデルハンドルを取得する
//     戻り値 : ステージのコリジョン用３Ｄモデルハンドル
int StageData_GetCollisionModelHandle(void)
{
	return g_StageData.CollisionModelHandle;
}

// ステージに配置されているオブジェクトの数を取得する
//     戻り値 : ステージに配置されているオブジェクトの数
int StageData_GetObjectNum(void)
{
	return g_StageData.Header.ObjectInfoNum;
}

// オブジェクトが使用するコリジョン用３Ｄモデルハンドルを取得する
//     戻り値 : コリジョン用３Ｄモデルハンドル
int StageData_GetObjectCollisionModelHandle(
	// オブジェクトが使用する３Ｄモデルの番号
	int Index
)
{
	return g_StageData.ObjectInfo[Index].CollisionModelHandle;
}


// オブジェクトのコリジョン用３Ｄモデルの指定番号のマテリアルタイプを取得する
EMaterialType StageData_GetObjectCollisionModelMaterialType(
	// オブジェクトが使用する３Ｄモデルの番号
	int Index,

	// コリジョン用３Ｄモデルのマテリアル番号
	int MaterialIndex
)
{
	SStageObjectInfo      *OInfo;
	SStageObjectModelInfo *OMInfo;

	OInfo = &g_StageData.ObjectInfo[Index];
	OMInfo = &g_StageData.ObjectModelInfo[OInfo->ModelID];

	return OMInfo->ColModelMatType[MaterialIndex];
}

// 全ステージ数を取得する
//     戻り値 : 全ステージ数
int StageData_GetTotalStageNum(void)
{
	return g_StageData.TotalStageNum;
}

// 読み込んであるステージの番号を取得する
//     戻り値 : ステージ番号
int StageData_GetLoadStageNo(void)
{
	return g_StageData.LoadStageNumber;
}

// ステージの描画タイプを取得する
//     戻り値 : 描画タイプ
EStageRenderType StageData_GetRenderType(void)
{
	return g_StageData.Header.RenderType;
}

// ステージのクリア条件を取得する
//     戻り値 : ステージのクリア条件
EStageClearCondition StageData_GetStageClearCondition(void)
{
	return g_StageData.Header.StageClearCondition;
}

// ステージをクリアするために倒さなければならない敵の数を取得する
//     戻り値 :  ステージをクリアするために倒さなければならない敵の数
int StageData_GetKillCharaNum(void)
{
	return g_StageData.Header.KillCharaNum;
}

// ステージをクリアするために倒さなければならない敵の番号を取得する
//     戻り値 : ステージをクリアするために倒さなければならない敵の番号
int StageData_GetKillTargetChara(void)
{
	return g_StageData.Header.KillTargetChara;
}

// ステージのコリジョン用３Ｄモデルの指定番号のマテリアルタイプを取得する
//     戻り値 : マテリアルタイプ
extern EMaterialType StageData_GetCollisionModelMaterialType(
	// コリジョン用３Ｄモデルのマテリアル番号
	int MaterialIndex
)
{
	return g_StageData.ColModelMatType[MaterialIndex];
}
