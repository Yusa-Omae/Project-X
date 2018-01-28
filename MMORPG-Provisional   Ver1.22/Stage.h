
#ifndef STAGE_H
#define STAGE_H

#include "DxLib.h"

// ステージ処理の初期化をする
//     戻り値 : 初期化に成功したかどうか
//              ( true:成功   false:失敗 )
extern bool Stage_Initialize(void);

// ステージ処理の後始末をする
extern void Stage_Terminate(void);

// ステージをセットアップする
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Stage_Setup(
	// セットアップするステージの番号
	int StageNumber
);

// ステージの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool Stage_Step(
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ステージのライトのセットアップを行う
extern void Stage_LightSetup(void);

// ステージを描画する
extern void Stage_Render(
	// シャドウマップへの描画かどうか
	bool ShadowMapDraw
);

// ステージのコリジョン用３Ｄモデルを描画する
extern void Stage_CollisionRender(void);

// ステージの空用３Ｄモデルを描画する
extern void Stage_SkyRender(void);

// ステージのディレクショナルライトの方向を取得する
//     戻り値 : ディレクショナルライトの方向
extern VECTOR Stage_GetLightDirection(void);

// 指定のカプセル形状がステージのコリジョン用３Ｄモデルのポリゴンや
// ステージオブジェクトのコリジョン用３Ｄモデルのポリゴンに当たるかどうかをチェックする
//     戻り値 : コリジョン用ポリゴンに当たるかどうか
//              ( true : 当たる   false : 当たらない )
extern bool Stage_HitCheck(
	// カプセルを形成する座標１
	VECTOR Pos1,

	// カプセルを形成する座標２
	VECTOR Pos2,

	// カプセルの幅
	float r
);

#endif

#if false
void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();


// ステージ処理の情報
typedef struct _SStageInfo
{
	// ステージのイベント処理を一度でも実行したかどうかのフラグ
	bool                 EventRunFlag[STAGE_EVENT_MAX_NUM];

	// ステージのイベントのボリュームに触れた瞬間かどうかのフラグ
	bool                 EventHitEdgeFlag[STAGE_EVENT_MAX_NUM];

	// ステージのイベントのボリュームに触れているかどうかのフラグ
	bool                 EventHitFlag[STAGE_EVENT_MAX_NUM];
} SStageInfo;


//ステージクラスで使用する構造体
typedef struct s_Stage {

	//キャラ配置情報の数
	int CharaInfoNum;
	//オブジェクト配置情報の数
	int ObjectInfoNum;
	//イベント情報の数
	int EventInfoNum;
	//ステージ開始時のプレイヤーの座標
	VECTOR PlayerPosition;
	//ステージ開始時のプレイヤーの向き
	float PlayerAngle;
	//ステージクリア条件
	int ClearCondition;
	//ステージをクリアしたか
	int Clearflg;

	//ステージデータ格納ハンドル
	int handle;
	//あたり判定用ステージデータ格納ハンドル
	int c_handle;
	//ステージ背景データ格納ハンドル
	int sky_handle;
	//オブジェクトデータ格納ハンドル
	int obj[OBJECT_NUM];
	//あたり判定用オブジェクトデータ格納ハンドル
	int c_obj[OBJECT_NUM];
	//描画用オブジェクト配列
	int d_obj[OBJECT_DRAW_MAX_NUM];
	//シャドウマップの作成用ハンドル
	int smap;


};

// ステージ処理用の整数パラメータ
typedef enum _EStage_ParamInt
{
	EStgParInt_DirLight_Diff_R,		// ディレクショナルライトのディフューズカラー(R)
	EStgParInt_DirLight_Diff_G,		// ディレクショナルライトのディフューズカラー(G)
	EStgParInt_DirLight_Diff_B,		// ディレクショナルライトのディフューズカラー(B)
	EStgParInt_DirLight_Amb_R,		// ディレクショナルライトのアンビエントカラー(R)
	EStgParInt_DirLight_Amb_G,		// ディレクショナルライトのアンビエントカラー(G)
	EStgParInt_DirLight_Amb_B,		// ディレクショナルライトのアンビエントカラー(B)
	EStgParInt_PointLight_Atten0,	// ポイントライトの距離減衰パラメータ０
	EStgParInt_PointLight_Atten1,	// ポイントライトの距離減衰パラメータ１
	EStgParInt_PointLight_Atten2,	// ポイントライトの距離減衰パラメータ２
	EStgParInt_PointLight_Diff_R,	// ポイントライトのディフューズカラー(R)
	EStgParInt_PointLight_Diff_G,	// ポイントライトのディフューズカラー(G)
	EStgParInt_PointLight_Diff_B,	// ポイントライトのディフューズカラー(B)
	EStgParInt_PointLight_Amb_R,	// ポイントライトのアンビエントカラー(R)
	EStgParInt_PointLight_Amb_G,	// ポイントライトのアンビエントカラー(G)
	EStgParInt_PointLight_Amb_B,	// ポイントライトのアンビエントカラー(B)
	EStgParInt_PointLight_Pos_X,	// ポイントライトのプレイヤーとの相対X座標
	EStgParInt_PointLight_Pos_Y,	// ポイントライトのプレイヤーとの相対Y座標
	EStgParInt_PointLight_Pos_Z,	// ポイントライトのプレイヤーとの相対Z座標

	EStgParInt_Num					// ステージ処理用の整数パラメータの数
} EStage_ParamInt;
#endif