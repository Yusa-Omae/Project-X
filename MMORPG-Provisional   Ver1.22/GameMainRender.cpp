#include "GameMainRender.h"
#include "Stage.h"
#include "StageData.h"
#include "Chara.h"
#include "Effect.h"
#include "Camera.h"
#include "System.h"
#include "DxLib.h"

// シャドウマップの解像度
#define SHADOWMAP_RESOLUTION		(2048)

// ゲームメインの描画処理用の整数値パラメータ
typedef enum _EGameMainRender_ParamInt
{
	EGMRParInt_NearShadowDistance,	// 近距離用シャドウマップの距離
	EGMRParInt_NearShadowHeight,	// 近距離用シャドウマップの高さ

	EGMRParInt_Num,					// 整数パラメータの数
} EGameMainRender_ParamInt;

// ゲームメインの描画処理の情報
typedef struct _SGameMainRenderData
{
	// プレイヤーの座標
	VECTOR PlayerPosition;

	// ステージ３Ｄモデル用のシャドウマップハンドル
	int    StageModelShadowMapHandle;

	// 遠距離用のシャドウマップハンドル
	int    FarShadowMapHandle;

	// 近距離用のシャドウマップハンドル
	int    NearShadowMapHandle;

	// アンチエイリアスレンダリング用の描画対象にできるグラフィックハンドル
	int    AntialiasScreenHandle;
} SGameMainRenderData;

// ゲームメインの描画処理用の整数パラメータテーブル
static SParamInt g_GMRParam[EGMRParInt_Num] =
{
	{ 7,  "NearShadowDistance" },
	{ 10, "NearShadowHeight" },
};

// ゲームメインの描画処理用の情報
static SGameMainRenderData g_GMRData;

// ゲームメインの描画処理を初期化する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool GameMainRender_Initialize(void)
{
	// 動的編集用に整数パラメータを登録
	// System_SetDebugParamInt( g_GMRParam, EGMRParInt_Num );

	// 低処理負荷モードでは無い場合はシャドウマップやアンチエイリアス画面を作成する
	if (!System_GetLowSpecMode())
	{
		// シャドウマップハンドルを作成する
		g_GMRData.StageModelShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.StageModelShadowMapHandle == -1)
		{
			return false;
		}


		g_GMRData.FarShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.FarShadowMapHandle == -1)
		{
			return false;
		}

		g_GMRData.NearShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.NearShadowMapHandle == -1)
		{
			return false;
		}

		// アンチエイリアスレンダリング用の描画対象にできるグラフィックハンドルを作成する
		SetCreateDrawValidGraphMultiSample(4, 4);
		g_GMRData.AntialiasScreenHandle =
			MakeScreen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, FALSE);
		SetCreateDrawValidGraphMultiSample(0, 0);
	}

	// 正常終了
	return true;
}

// ゲームメインの描画処理の後始末をする
void GameMainRender_Terminate(void)
{
	// 低処理負荷モードではない場合はシャドウマップやアンチエイリアス画面を削除する
	if (!System_GetLowSpecMode())
	{
		// シャドウマップハンドルを削除する
		DeleteShadowMap(g_GMRData.StageModelShadowMapHandle);
		g_GMRData.StageModelShadowMapHandle = -1;

		DeleteShadowMap(g_GMRData.FarShadowMapHandle);
		g_GMRData.FarShadowMapHandle = -1;

		DeleteShadowMap(g_GMRData.NearShadowMapHandle);
		g_GMRData.NearShadowMapHandle = -1;

		// アンチエイリアスレンダリング用の描画対象にできるグラフィックハンドルを削除する
		DeleteGraph(g_GMRData.AntialiasScreenHandle);
	}
}

// ステージ開始時の描画準備を行う
void GameMainRender_StageSetup(void)
{
	VECTOR              LightDirection;
	MV1_REF_POLYGONLIST PolyList;
	bool                UseStageShadowMap;

	// 低処理負荷モードではない場合は遠距離用シャドウマップにステージを描画する
	if (!System_GetLowSpecMode())
	{
		// レンダリングモードによって処理を分岐
		switch (StageData_GetRenderType())
		{
		case EStageRenderType_Outdoors:		// 屋外
											// 屋外の場合はディレクショナルライトの方向をセット
			LightDirection = Stage_GetLightDirection();

			// ステージモデル用シャドウマップを使用するかどうかのフラグを立てる
			UseStageShadowMap = true;
			break;

		case EStageRenderType_Cave:			// 洞窟
											// 洞窟の場合は真下方向をセット
			LightDirection = VGet(0.0f, -1.0f, 0.0f);

			// ステージモデル用シャドウマップを使用するかどうかのフラグを倒す
			UseStageShadowMap = false;
			break;
		}

		// シャドウマップへのレンダリングで使用するライトの方向をセット
		SetShadowMapLightDirection(g_GMRData.StageModelShadowMapHandle, LightDirection);
		SetShadowMapLightDirection(g_GMRData.FarShadowMapHandle, LightDirection);
		SetShadowMapLightDirection(g_GMRData.NearShadowMapHandle, LightDirection);

		// ステージモデル用と遠距離用のシャドウマップにはステージ全体を収める
		PolyList = MV1GetReferenceMesh(StageData_GetModelHandle(), -1, TRUE);
		SetShadowMapDrawArea(g_GMRData.StageModelShadowMapHandle,
			PolyList.MinPosition, PolyList.MaxPosition);
		SetShadowMapDrawArea(g_GMRData.FarShadowMapHandle,
			PolyList.MinPosition, PolyList.MaxPosition);

		// ステージモデル用のシャドウマップにステージをレンダリング
		if (UseStageShadowMap)
		{
			ShadowMap_DrawSetup(g_GMRData.StageModelShadowMapHandle);
			Stage_Render(true);
			ShadowMap_DrawEnd();
		}
	}
}

// ゲームメインの描画処理を行う
void GameMainRender_Render(void)
{
	float       NearDistance;
	float       Height;
	VECTOR      MinPos;
	VECTOR      MaxPos;
	SCharaInfo *PCInfo;
	bool        SkyModelRender;
	bool        UseStageShadowMap;

	// プレイヤーの座標を更新
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo != NULL)
	{
		g_GMRData.PlayerPosition = PCInfo->Position;
	}

	// 近距離用のシャドウマップの描画範囲を算出
	NearDistance = g_GMRParam[EGMRParInt_NearShadowDistance].Param * 100.0f;
	Height = g_GMRParam[EGMRParInt_NearShadowHeight].Param * 100.0f;
	MinPos = VAdd(g_GMRData.PlayerPosition, VGet(-NearDistance, 0.0f, -NearDistance));
	MaxPos = VAdd(g_GMRData.PlayerPosition, VGet(NearDistance, 0.0f, NearDistance));
	MinPos.y = -1.0f;
	MaxPos.y = g_GMRData.PlayerPosition.y + Height;

	// 低処理負荷モードではない場合は描画先をアンチエイリアス用画面に、
	// 低処理負荷モードの場合は描画先を裏画面にする
	if (!System_GetLowSpecMode())
	{
		SetDrawScreen(g_GMRData.AntialiasScreenHandle);

		// アンチエイリアス用画面を初期化
		ClearDrawScreen();
	}
	else
	{
		SetDrawScreen(DX_SCREEN_BACK);
	}

	// カメラの設定を行う
	Camera_Setup();

	// ライトのセットアップ
	Stage_LightSetup();

	// シャドウマップへのレンダリングで使用するライトの方向をセット
	switch (StageData_GetRenderType())
	{
	case EStageRenderType_Outdoors:		// 屋外
										// 空用３Ｄモデルを描画するかどうかのフラグを立てる
		SkyModelRender = true;

		// ステージモデル用シャドウマップを使用するかどうかのフラグを立てる
		UseStageShadowMap = true;
		break;

	case EStageRenderType_Cave:			// 洞窟
										// 空用３Ｄモデルを描画するかどうかのフラグを倒す
		SkyModelRender = false;

		// ステージモデル用シャドウマップを使用するかどうかのフラグを倒す
		UseStageShadowMap = false;
		break;
	}

	// 低処理負荷モードではない場合はシャドウマップへの描画を行う
	if (!System_GetLowSpecMode())
	{
		// 遠距離用のシャドウマップにキャラクターをレンダリング
		ShadowMap_DrawSetup(g_GMRData.FarShadowMapHandle);
		Chara_RenderAll(ECharaRenderMode_Distance_Greater, NearDistance);
		ShadowMap_DrawEnd();

		// 近距離用のシャドウマップの描画範囲をセット
		SetShadowMapDrawArea(g_GMRData.NearShadowMapHandle, MinPos, MaxPos);

		// 近距離用のシャドウマップにキャラクターをレンダリング
		ShadowMap_DrawSetup(g_GMRData.NearShadowMapHandle);
		Chara_RenderAll(ECharaRenderMode_Distance_Less, NearDistance);
		ShadowMap_DrawEnd();
	}

	// 描画タイプによって処理を分岐
	switch (StageData_GetRenderType())
	{
	case EStageRenderType_Outdoors:		// 屋外
										// 空用３Ｄモデルを描画
		Stage_SkyRender();

		// 低処理負荷モードではない場合はシャドウマップを使用する
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(0, g_GMRData.StageModelShadowMapHandle);
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);
		}

		// 半透明要素の無い部分を描画
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
		Stage_Render(false);
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		// 半透明要素のある部分を描画
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
		Stage_Render(false);
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);

		// 低処理負荷モードではない場合はシャドウマップの使用を解除
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		break;

	case EStageRenderType_Cave:			// 洞窟
										// 半透明要素の無い部分を描画
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);

		// 低処理負荷モードではない場合はステージのレンダリングにシャドウマップを使用する
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);

			Stage_Render(false);

			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		else
		{
			Stage_Render(false);
		}

		// キャラクターの描画にはシャドウマップを使用しない
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		// 半透明要素のある部分を描画
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);

		// 低処理負荷モードではない場合はステージのレンダリングにシャドウマップを使用する
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);

			Stage_Render(false);

			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		else
		{
			Stage_Render(false);
		}

		// キャラクターの描画にはシャドウマップを使用しない
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
		break;
	}

	// 低処理負荷モードの場合は丸影を描画する
	if (System_GetLowSpecMode())
	{
		Chara_ShadowRenderAll(ECharaRenderMode_Always, 0.0f);
	}

	// エフェクトを描画
	Effect_RenderAll();

	// 低処理負荷モードではない場合は裏画面にアンチエイリアス用画面を描画する
	if (!System_GetLowSpecMode())
	{
		// 描画先を裏画面に設定
		SetDrawScreen(DX_SCREEN_BACK);

		// アンチエイリアス画面を裏画面に描画
		DrawGraph(0, 0, g_GMRData.AntialiasScreenHandle, FALSE);

		// SetDrawScreen を実行するとカメラの設定がリセットされてしまうので、
		// カメラの設定を再度実行
		Camera_Setup();
	}

	// ２Ｄ関係の描画処理
	Chara_2D_RenderAll();
}
