#pragma once

void Camera_Initialize();
void CameraUpdate();
void CameraSetUp();
VECTOR Get_Right_Direction();


/*
�J�����ɕ`��Ώۂɂ���I�u�W�F�N�g�̍��W��ݒ�
*/
void Camera_SetTargetPosition(VECTOR targetPos);

/*
�p�x��ݒ肷��(X������)
*/
void Camera_SetVAngle(float vAngle);

/*
�p�x��ݒ肷��(Y������)
*/
void Camera_SetHAngle(float hAngle);

/*
�p�x��ݒ肷��(Z������)
*/
void Camera_SetTAngle(float tAngle);

/*
	�`��Ώۂ܂ł̋�����ݒ肷��
*/
void Camera_SetDistance(float distance);

/*
�p�x��ԋp����(X������)
*/
float Camera_GetVAngle();
/*
�p�x��ԋp����(Y������)
*/
float Camera_GetHAngle();

/*
�p�x��ԋp����(Z������)
*/
float Camera_GetTAngle();

/*
�J�����̕`��Ώۂ܂ł̋�����ԋp����
*/
float Camera_GetDistance();

// ------------ �f�o�b�O�@�\ ----------
void Debug_Camera();


#if false
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
#endif

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