#pragma once

void Camera_Initialize();
void CameraUpdate();
void CameraSetUp();
VECTOR Get_Right_Direction();


void Debug_Camera();



struct _CAMERA{
	//�J�����̈ʒu
	VECTOR Pos;
	//�J�����̒����_�ʒu
	VECTOR Target_Pos;
	//�����_�܂ł̋���
	float Distance;
	//�J�����̏㉺����
	VECTOR Vec_UP;
	//�J�����̃Y�[���C���E�A�E�g �����J�����{���Œ�
	VECTOR Zoom_Base;
	//�J�����{���ύX�� AS=(Add/Sub)
	MATRIX Zoom_AS;
	//�����p�x
	float HAngle;
	//�����p�x
	float VAngle;
	//�J��������
	VECTOR Direction;
	//���ʕ����̃x�N�g��
	VECTOR Front_Direction;
	//�J��������(�E����)
	VECTOR Right_Direction;

};

struct _MOUSE{
	//�}�E�X�̌��ݍ��W
	int X;
	int Y;
	//1�t���[���O�̃}�E�X���W
	int Back_X;
	int Back_Y;
	//1�t���[���O���瓮�����}�E�X��
	int Move_X;
	int Move_Y;
	//�}�E�X�̋������W�ϊ����s������
	bool Rest_Flg;
	//�}�E�X�z�C�[���ړ���
	float Wheel_Rot;
	//�}�E�X�z�C�[���ړ��ʂ������ēK�؂Ȓl�ɂ�������
	float Wheel_Move_Per;
	//�Q�[���X�^�[�g���K��Y�[���l�ύX�֐�
	float Wheel_Move_Cnt;
	//�}�E�X�̉������
	int Input;

};