#pragma once

void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();

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