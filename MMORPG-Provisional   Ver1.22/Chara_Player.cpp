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


//�v���C���[�f�[�^�̏�����
void Player_Info_Initialize(){

	//�������̊m��
	_PLAYER *ix  = (_PLAYER *)malloc(sizeof(_PLAYER));

	//�������̒��g�̏�����
	if(ix != NULL){
		memset(ix,0,sizeof(_PLAYER));
	}

	//�������̃O���[�o����
	PLAYER = ix;

	//�O���[�o���ϐ��ɓn�����������ϐ�ix�̏�����
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

	//�؉��搶�ɕ�����
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
	//	//�v���C���[Moveflg�X�V
	//	Set_Move_flg(model_Player, TRUE);
	//}

	//if(Get_Move_flg(model_Player) == FALSE){
	//	PLAYER->New_Speed = VNorm(PLAYER->New_Speed);
	//	PLAYER->Target_Angle = atan2(PLAYER->New_Speed.x,PLAYER->New_Speed.z);
	//	//������
	//	if()

	//}
}

//�v���C���[���W���󂯎��B
VECTOR Get_Player_Pos(){
	return PLAYER->pos;
}
//�v���C���[���W��+���Z
void Add_Player_Pos(VECTOR Add_Axis){
	PLAYER->pos = VAdd(PLAYER->pos,Add_Axis);
}
//�v���C���[���W��-���Z
void Sub_Player_Pos(VECTOR Sub_Axis){
	PLAYER->pos = VSub(PLAYER->pos,Sub_Axis);
}
//�v���C���[�f�[�^�̃N���A
void Player_Info_Fainalize(){
	//PLAYER�������̉��
	if(PLAYER != NULL){
		free(PLAYER);
	}
}
//�v���C���[�N���X�̃f�o�b�O�p
void Debug_Player(){
	DrawFormatString(10,0,Color_ValTbl[eCol_White],"Player_position = (%f,%f,%f)",PLAYER->pos.x,PLAYER->pos.y,PLAYER->pos.z);
}

