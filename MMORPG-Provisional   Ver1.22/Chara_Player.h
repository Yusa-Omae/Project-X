#pragma once

void Player_Info_Initialize();
void PlayerUpdate();
VECTOR Get_Player_Pos();
void Add_Player_Pos(VECTOR Add_Axis);
void Sub_Player_Pos(VECTOR Sub_Axis);
void Player_Info_Fainalize();

void Debug_Player();

struct _PLAYER{

	//座標
	VECTOR pos;
	//新しいスピード
	VECTOR New_Speed;
	//新しい角度
	float Target_Angle;
	//今向いているキャラの方向
	float Now_Angle;
	//目標の向きに到達したかどうか
	bool Angle_MoveEnd;


};