#include "Stage.h"
#include "StageData.h"
#include "Camera.h"
#include "Chara.h"
#include "GameMainRender.h"
#include "TextParam.h"
#include "System.h"
#include <stdlib.h>
#include <malloc.h>

s_Stage STAGE;
SStageInfo g_StageInfo;
//データ読み込み用
char path[256];

// ステージ処理用の整数パラメータテーブル
static SParamInt g_Stage_ParamInt[EStgParInt_Num] =
{
	{ 0, "DirLight_Diff_R" },
	{ 0, "DirLight_Diff_G" },
	{ 0, "DirLight_Diff_B" },
	{ 0, "DirLight_Amb_R" },
	{ 0, "DirLight_Amb_G" },
	{ 0, "DirLight_Amb_B" },
	{ 0, "PointLight_Atten0" },
	{ 0, "PointLight_Atten1" },
	{ 0, "PointLight_Atten2" },
	{ 0, "PointLight_Diff_R" },
	{ 0, "PointLight_Diff_G" },
	{ 0, "PointLight_Diff_B" },
	{ 0, "PointLight_Amb_R" },
	{ 0, "PointLight_Amb_G" },
	{ 0, "PointLight_Amb_B" },
	{ 0, "PointLight_Pos_X" },
	{ 0, "PointLight_Pos_Y" },
	{ 0, "PointLight_Pos_Z" },
};

// ステージ処理の初期化をする
//     戻り値 : 初期化に成功したかどうか
//              ( true:成功   false:失敗 )
bool Stage_Initialize(void)
{
	int i;

	// ステージデータ処理の初期化を行う
	if (!StageData_Initialize(false))
	{
		return false;
	}

	// ライトに関するパラメータをテキストファイルから読み込む
	for (i = 0; i < EStgParInt_Num; i++)
	{
		g_Stage_ParamInt[i].Param =
			GetTextParamInt(&g_StageData.TextParam, g_Stage_ParamInt[i].Name);
	}

	// 成功終了
	return true;
}

// ステージ処理の後始末をする
void Stage_Terminate(void)
{
	// ステージデータ処理の後始末を行う
	StageData_Terminate();
}

// ステージをセットアップする
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Stage_Setup(
	// セットアップするステージの番号
	int StageNumber
)
{
	// イベント処理用のフラグを初期化する
	memset(g_StageInfo.EventRunFlag, 0, sizeof(g_StageInfo.EventRunFlag));
	memset(g_StageInfo.EventHitEdgeFlag, 0, sizeof(g_StageInfo.EventHitEdgeFlag));
	memset(g_StageInfo.EventHitFlag, 0, sizeof(g_StageInfo.EventHitFlag));

	// キャラを全部削除する
	Chara_AllDelete();

	// ステージデータを読み込む
	if (!StageData_Load(StageNumber))
	{
		return false;
	}

	// プレイヤーキャラをステージ開始位置に作成する
	if (!Chara_Create(
		EChara_Player,
		g_StageData.Header.PlayerPosition,
		g_StageData.Header.PlayerAngle,
		-1
	))
	{
		return false;
	}

	// 描画の準備を行う
	GameMainRender_StageSetup();

	// 正常終了
	return true;
}

// ステージの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Stage_Step(
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	int                   i;
	int                   j;
	SStageCharaInfo      *AInfo;
	SStageEventInfo      *EInfo;
	SCharaInfo           *PCInfo;
	VECTOR                Distance;
	bool                  EventHitFlagOld;

	// プレイヤーの情報を取得する
	PCInfo = System_GetPlayerCharaInfo();

	// プレイヤーが存在する場合はイベントの処理を行う
	if (PCInfo != NULL)
	{
		// イベントの数だけ繰り返し
		EInfo = g_StageData.EventInfo;
		for (i = 0; i < g_StageData.Header.EventInfoNum; i++, EInfo++)
		{
			EventHitFlagOld = g_StageInfo.EventHitFlag[i];
			g_StageInfo.EventHitFlag[i] = false;

			// プレイヤーがイベントのボリュームに触れているかどうかをチェックする
			Distance = VSub(PCInfo->Position, EInfo->Position);
			if (Distance.x < 0.0f) Distance.x = -Distance.x;
			if (Distance.y < 0.0f) Distance.y = -Distance.y;
			if (Distance.z < 0.0f) Distance.z = -Distance.z;

			if (Distance.x <= EInfo->AreaSize.x / 2.0f &&
				Distance.y <= EInfo->AreaSize.y / 2.0f &&
				Distance.z <= EInfo->AreaSize.z / 2.0f)
			{
				g_StageInfo.EventHitFlag[i] = true;
				g_StageInfo.EventHitEdgeFlag[i] = !EventHitFlagOld &&
					g_StageInfo.EventHitFlag[i];

				// 触れていた場合は、イベントのタイプに応じて処理を行う
				switch (EInfo->EventType)
				{
				case EStageEventType_CharaAppear:			// キャラを出現させる

															// 一度でもイベントを実行している場合は何もせずに終了
					if (g_StageInfo.EventRunFlag[i])
					{
						continue;
					}

					// イベントに関連付けられているキャラを出現させる
					for (j = 0; j < EInfo->ObjectNum; j++)
					{
						AInfo = &g_StageData.CharaInfo[EInfo->Object[j]];

						if (!Chara_Create(AInfo->Chara, AInfo->Position,
							AInfo->Angle, EInfo->Object[j]))
						{
							return false;
						}
					}
					break;

				case EStageEventType_BGM:		// ＢＧＭを変更する
												// ＢＧＭ変更を一度でも実行している場合は何もせずに終了
					if (g_StageInfo.EventRunFlag[i])
					{
						continue;
					}

					// イベントに設定されているＢＧＭの再生を開始する
					Sound_PlayBGM(EInfo->Bgm);
					break;
				}

				// イベントを実行したかどうかのフラグを立てる
				g_StageInfo.EventRunFlag[i] = true;
			}
			else
			{
				// イベントに触れていなかった場合は、触れた瞬間かどうかのフラグを倒す
				g_StageInfo.EventHitEdgeFlag[i] = false;
			}
		}
	}

	// 正常終了
	return true;
}

// ステージのライトのセットアップを行う
void Stage_LightSetup(void)
{
	SCharaInfo *PCInfo;
	COLOR_F     DifColor;
	COLOR_F     AmbColor;
	VECTOR      Position;

	System_SetDebugParamInt(g_Stage_ParamInt, EStgParInt_Num);

	// 描画タイプによって処理を分岐
	switch (g_StageData.Header.RenderType)
	{
	case EStageRenderType_Outdoors:	// 屋外
									// ディレクショナルライトの設定を行う
		ChangeLightTypeDir(Stage_GetLightDirection());
		DifColor = GetColorF(
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_R].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_G].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_B].Param * 0.01f,
			1.0f
		);
		AmbColor = GetColorF(
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_R].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_G].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_B].Param * 0.01f,
			1.0f
		);
		SetLightDifColor(DifColor);
		SetLightAmbColor(AmbColor);
		break;

	case EStageRenderType_Cave:		// 洞窟
		PCInfo = System_GetPlayerCharaInfo();
		if (PCInfo != NULL)
		{
			// プレイヤーの居る位置にポイントライトを配置する
			Position =
				VScale(
					PCInfo->AngleInfo.LeftDirection,
					g_Stage_ParamInt[EStgParInt_PointLight_Pos_X].Param * 10.0f
				);

			Position.y =
				g_Stage_ParamInt[EStgParInt_PointLight_Pos_Y].Param * 10.0f;

			Position =
				VAdd(Position, VScale(
					PCInfo->AngleInfo.FrontDirection,
					g_Stage_ParamInt[EStgParInt_PointLight_Pos_Z].Param * 10.0f
				));
			ChangeLightTypePoint(
				VAdd(PCInfo->Position, Position),
				POINTLIGHT_RANGE,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten0].Param * 0.1f,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten1].Param * 0.0001f,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten2].Param * 0.0000001f
			);
			DifColor = GetColorF(
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_R].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_G].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_B].Param * 0.01f,
				1.0f
			);
			AmbColor = GetColorF(
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_R].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_G].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_B].Param * 0.01f,
				1.0f
			);

			SetLightDifColor(DifColor);
			SetLightAmbColor(AmbColor);
		}
		break;
	}
}

// ステージを描画する
void Stage_Render(
	// シャドウマップへの描画かどうか
	bool ShadowMapDraw
)
{
	int               i;
	SStageObjectInfo *OInfo;
	bool              LowSpecMode;

	// ステージ３Ｄモデルを描画
	MV1DrawModel(g_StageData.ModelHandle);

	// 低処理負荷モードかどうかを取得しておく
	LowSpecMode = System_GetLowSpecMode();

	// ステージに配置されているオブジェクトの３Ｄモデルを描画
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		// 低処理負荷モードの場合はコリジョン用３Ｄモデルの無いオブジェクトは描画しない
		if (LowSpecMode && OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		// ３Ｄモデルを描画
		MV1DrawModel(OInfo->ModelHandle);
	}
}

// ステージのコリジョン用３Ｄモデルを描画する
void Stage_CollisionRender(void)
{
	int               i;
	SStageObjectInfo *OInfo;

	// ステージのコリジョン用３Ｄモデルをワイヤーフレーム描画
	MV1DrawModelDebug(
		g_StageData.CollisionModelHandle,
		GetColor(0, 255, 0), FALSE, 1.0f, TRUE, FALSE);

	// ステージに配置されているオブジェクトのコリジョン用３Ｄモデルをワイヤーフレーム描画
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		if (OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		MV1DrawModelDebug(
			OInfo->CollisionModelHandle,
			GetColor(0, 255, 0), FALSE, 1.0f, TRUE, FALSE);
	}
}

// ステージの空用３Ｄモデルを描画する
void Stage_SkyRender(void)
{
	SCharaInfo *PCInfo;

	// プレイヤーが居る場合はプレイヤーの座標に空用３Ｄモデルの位置を合わせる
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo != NULL)
	{
		MV1SetPosition(g_StageData.SkyModelHandle, PCInfo->Position);
	}

	// 空用３Ｄモデル描画用のカメラ設定を行う
	SetCameraNearFar(SKYMODEL_CAMERA_NEAR_Z, SKYMODEL_CAMERA_FAR_Z);

	// 空用３Ｄモデルの描画にＺバッファは使用しない
	MV1SetUseZBuffer(g_StageData.SkyModelHandle, FALSE);

	// 空用３Ｄモデルを描画する
	MV1DrawModel(g_StageData.SkyModelHandle);

	// 変更したカメラ設定を元に戻す
	SetCameraNearFar(CAMERA_NEAR_Z, CAMERA_FAR_Z);
}

// ステージのディレクショナルライトの方向を取得する
//     戻り値 : ディレクショナルライトの方向
VECTOR Stage_GetLightDirection(void)
{
	return VGet(DIRLIGHT_DIRECTION_X, DIRLIGHT_DIRECTION_Y, DIRLIGHT_DIRECTION_Z);
}

// 指定のカプセル形状がステージのコリジョン用３Ｄモデルのポリゴンや
// ステージオブジェクトのコリジョン用３Ｄモデルのポリゴンに当たるかどうかをチェックする
//     戻り値 : コリジョン用ポリゴンに当たるかどうか
//              ( true : 当たる   false : 当たらない )
bool Stage_HitCheck(
	// カプセルを形成する座標１
	VECTOR Pos1,

	// カプセルを形成する座標２
	VECTOR Pos2,

	// カプセルの幅
	float r
)
{
	MV1_COLL_RESULT_POLY_DIM  HitDim;
	int                       HitNum;
	int                       i;
	SStageObjectInfo         *OInfo;

	// ステージ用コリジョン３Ｄモデルに当たるかをチェック
	HitDim = MV1CollCheck_Capsule(g_StageData.CollisionModelHandle, -1, Pos1, Pos2, r);
	HitNum = HitDim.HitNum;
	MV1CollResultPolyDimTerminate(HitDim);

	// 当たっていたらここで終了
	if (HitNum > 0)
	{
		return true;
	}

	// ステージのコリジョン用ポリゴンに当たっていなかったら
	// ステージオブジェクトのコリジョン用ポリゴンにも当たっていないかチェック
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		if (OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		HitDim = MV1CollCheck_Capsule(OInfo->CollisionModelHandle, -1, Pos1, Pos2, r);
		HitNum = HitDim.HitNum;
		MV1CollResultPolyDimTerminate(HitDim);

		// 当たっていた時点で終了
		if (HitNum > 0)
		{
			return true;
		}
	}

	// ここに来たら当たっていないということ
	return false;
}
//ステージクラスの初期化処理
void StageInitialize(){
	
	
	/*必要
	○ステージの詳細読み込み

	・イベント処理用のフラグ
	・キャラクターの全削除関数
	・プレイヤーキャラのステージ内どこに設定するかの情報
	・
	
	*/




	//ステージで使うモデルの読み込み
	STAGE.handle = MV1LoadModel("Data\\Stage\\Stage00.mv1");
	STAGE.c_handle = MV1LoadModel("Data\\Stage\\Stage00_c.mv1");
	STAGE.sky_handle = MV1LoadModel("Data\\Stage\\Stage00_sky.mv1");
	
	for(int i = 0;i < OBJECT_NUM;i++){

		sprintf(path,"Stage\\Stage_Obj%03d.mv1",i);
		STAGE.obj[i] = MV1LoadModel(path);

		sprintf(path,"Stage\\Stage_Obj%03d_c.mv1",i);
		STAGE.c_obj[i] = MV1LoadModel(path);
	}

	//シャドウマップの作成
	STAGE.smap = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(STAGE.smap, VGet(1,-1,0));				//カメラのライトと合わせるときにライティングのベクトルをここにもいれるとOK
	SetShadowMapDrawArea(STAGE.smap, SHADOWMAP_INIT1, SHADOWMAP_INIT2);

	//位置設定
	MV1SetPosition(STAGE.handle, STAGE_INIT);



}

//ステージクラスの更新処理
void StageUpdata(){

	



}

//ステージクラスの描画処理
void StageDraw(){

	//シャドウマップに描きこみ
	ShadowMap_DrawSetup(STAGE.smap);
	MV1DrawModel(STAGE.handle);

	for(int i = 0;i < 100;i++){
	MV1DrawModel(STAGE.d_obj[i]);
	}

	ShadowMap_DrawEnd();
	SetUseShadowMap(0, STAGE.smap);

	//モデル描画
	MV1DrawModel(STAGE.handle);

	for (int i = 0; i < 100; i++) {
		MV1DrawModel(STAGE.d_obj[i]);
	}

	SetUseShadowMap(0,-1);

}

//ステージクラス後処理
void StageFainalize(){

}