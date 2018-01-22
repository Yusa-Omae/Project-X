#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"
#include"System.h"
#include"Chara.h"
#include"Camera.h"


_PLAYER *PLAYER;


//プレイヤーデータの初期化
void Player_Info_Initialize(){

	//メモリの確保
	_PLAYER *ix  = (_PLAYER *)malloc(sizeof(_PLAYER));

	//メモリの中身の初期化
	if(ix != NULL){
		memset(ix,0,sizeof(_PLAYER));
	}

	//メモリのグローバル化
	PLAYER = ix;

	//グローバル変数に渡したメモリ変数ixの初期化
	if(ix != NULL){
		ix = NULL;
	}
}

void PlayerUpdate(){

	if(key(KEY_INPUT_W)){
		PLAYER->pos.x += sin(1.57f*Get_Chara_Direction(model_Player))*-0.1f;
		PLAYER->pos.z += cos(1.57f*Get_Chara_Direction(model_Player))*-0.1f;
		Set_Chara_Direction(model_Player,direction_Up);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_S)){
		PLAYER->pos.x += sin(1.57f*Get_Chara_Direction(model_Player))*0.1f;
		PLAYER->pos.z += cos(1.57f*Get_Chara_Direction(model_Player))*0.1f;
		Set_Chara_Direction(model_Player,direction_Down);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_A)){
		PLAYER->pos.x += sin(1.57f*Get_Chara_Direction(model_Player))*0.1f;
		Set_Chara_Direction(model_Player,direction_Left);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_D)){
		PLAYER->pos.x += sin(1.57f*Get_Chara_Direction(model_Player))*0.1f;
		Set_Chara_Direction(model_Player,direction_Right);
		Set_Move_flg(model_Player, FALSE);
	}else{
		Set_Move_flg(model_Player, TRUE);
	}

	//木下先生に聞こう
	//PLAYER->New_Speed = VGet(0.0f,0.0f,0.0f);
	//if(key(KEY_INPUT_W)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_S)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_A)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_D)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else{
	//	//プレイヤーMoveflg更新
	//	Set_Move_flg(model_Player, TRUE);
	//}

	//if(Get_Move_flg(model_Player) == FALSE){
	//	PLAYER->New_Speed = VNorm(PLAYER->New_Speed);
	//	PLAYER->Target_Angle = atan2(PLAYER->New_Speed.x,PLAYER->New_Speed.z);
	//	//即座に
	//	if()

	//}
}

//プレイヤー座標を受け取る。
VECTOR Get_Player_Pos(){
	return PLAYER->pos;
}
//プレイヤー座標の+演算
void Add_Player_Pos(VECTOR Add_Axis){
	PLAYER->pos = VAdd(PLAYER->pos,Add_Axis);
}
//プレイヤー座標の-演算
void Sub_Player_Pos(VECTOR Sub_Axis){
	PLAYER->pos = VSub(PLAYER->pos,Sub_Axis);
}
//プレイヤーデータのクリア
void Player_Info_Fainalize(){
	//PLAYERメモリの解放
	if(PLAYER != NULL){
		free(PLAYER);
	}
}
//プレイヤークラスのデバッグ用
void Debug_Player(){
	DrawFormatString(10,0,Color_ValTbl[eCol_White],"Player_position = (%f,%f,%f)",PLAYER->pos.x,PLAYER->pos.y,PLAYER->pos.z);
}

