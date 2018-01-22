#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Camera.h"
#include"Chara_Player.h"
#include"Stage.h"
#include"Chara.h"




void GameMainInitialize(){
	//�v���C���[��񏉊���
	Player_Info_Initialize();
	//�J������񏉊���
	Camera_Initialize();

	Chara_Initialize();
}
void GameMainupdate(){

	//�J�����̍X�V
	CameraUpdate();

	PlayerUpdate();
	Chara_Update();


}
void GameMainDraw(){
	
	//�J�����Z�b�g
	CameraSetUp();
	StageDraw();
	DrawCube3D(VSub(Get_Player_Pos(),VGet(25.0,25.0,25.0)),VAdd(Get_Player_Pos(),VGet(25.0,25.0,25.0)),Color_ValTbl[eCol_EmeraldGreen],0,1);
	Chara_Draw();
	Debug_Player();
	Debug_Camera();
	Debag_Chara();

}
void GameMainFainalize(){

	Player_Info_Fainalize();
	Chara_Fainalize();

}
