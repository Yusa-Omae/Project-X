#pragma once

void StageInitialize();
void StageUpdata();
void StageDraw();
void StageFainalize();

//ステージクラスで使用する構造体
typedef struct s_Stage {

	//ステージデータ格納ハンドル
	int handle;

	//オブジェクト格納ハンドルは仮で100個配列をつくっています。
	//オブジェクトデータ格納ハンドル
	int obj[100];

	//あたり判定用オブジェクトデータ格納ハンドル
	int c_obj[100];


	//シャドウマップの作成用ハンドル
	int smap;


};