#pragma once

void Player_Info_Initialize();
void PlayerUpdate();
VECTOR Get_Player_Pos();
void Add_Player_Pos(VECTOR Add_Axis);
void Sub_Player_Pos(VECTOR Sub_Axis);
void Player_Info_Fainalize();

void Debug_Player();

struct _PLAYER{

	//���W
	VECTOR pos;
	//�V�����X�s�[�h
	VECTOR New_Speed;
	//�V�����p�x
	float Target_Angle;
	//�������Ă���L�����̕���
	float Now_Angle;
	//�ڕW�̌����ɓ��B�������ǂ���
	bool Angle_MoveEnd;


};