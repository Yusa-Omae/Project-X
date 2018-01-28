#pragma once

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
