#include"DxLib.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"

//ステージクラスの初期化処理
void StageInitialize(){


}

//ステージクラスの更新処理
void StageUpdata(){

}

//ステージクラスの描画処理
void StageDraw(){

	DrawCube3D(VGet(-5000,0,-5000),VGet(5000,-50,5000),GetColor(255,255,255),GetColor(0,255,127),0);
	DrawCube3D(VGet(-5000,0,-5000),VGet(5000,-50,5000),GetColor(255,255,255),GetColor(125,125,125),1);

}

//ステージクラス後処理
void StageFainalize(){

}