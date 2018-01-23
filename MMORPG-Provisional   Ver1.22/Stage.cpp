#include"DxLib.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"
#include"Stage.h"

s_Stage STAGE;


//ステージクラスの初期化処理
void StageInitialize(){
	
	//モデルの読み込み
	STAGE.handle = MV1LoadModel("Stage\\Stage00.mv1");

	/*
	for(int i = 0;i < 100;i++){

		STAGE.obj[i] = MV1LoadModel("Stage\\Stage_Obj%03d.mv1",i);
		STAGE.c_obj[i] = MV1LoadModel("Stage\\Stage_Obj%03d_c.mv1",i);

	}
	*/

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
	/*
	for(int i = 0;i < 100;i++){

	MV1DrawModel(STAGE.obj[i]);

	}
	*/
	ShadowMap_DrawEnd();
	SetUseShadowMap(0, STAGE.smap);

	//モデル描画
	MV1DrawModel(STAGE.handle);
	/*
	for(int i = 0;i < 100;i++){

	MV1DrawModel(STAGE.obj[i]);

	}
	*/
	SetUseShadowMap(0,-1);

}

//ステージクラス後処理
void StageFainalize(){

}