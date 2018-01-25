#include"Dxlib.h"
#include"Define.h"
#include"Camera.h"
#include"System.h"
#include<math.h>




struct _CAMERA {

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
	//Z�������̊p�x
	float TAngle;
	//�J��������
	VECTOR Direction;
	//���ʕ����̃x�N�g��
	VECTOR Front_Direction;
	//�J��������(�E����)
	VECTOR Right_Direction;
	//�J�����̍���
	float height;


};

static _CAMERA s_Camera;
_MOUSE MOUSE;
//SYSTEM_INFO�͊����̃��C�u�������Ő錾����Ă����̂Ŗ��O��ς��܂��� by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO; 



void Camera_Initialize() {


	//�J���������p�x�̏����ݒ�
	s_Camera.HAngle = DX_PI_F;
	//�J���������p�x�̏����ݒ�
	s_Camera.VAngle = 0.05f;
	//�����_�܂ł̋��������ݒ�
	s_Camera.Distance = 270.0f;
	
	//�f�B���N�g�����C�g�̐ݒ�
	ChangeLightTypeDir(VGet(0.57735026f, 0.57735026f, -0.57735026f));
	//�F�ʐݒ�
	COLOR_F DifColor = GetColorF(1, 1, 1, 1.0);
	COLOR_F AmbColor = GetColorF(1, 1, 1, 1.0);
	SetLightDifColor(DifColor);
	SetLightAmbColor(AmbColor);

	//�J�����̃N���b�v�����ݒ�
	SetCameraNearFar(CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP);

	s_Camera.height = 100.0f;
	s_Camera.Distance = 1000.0f;

}


/*
	�J�����̏�Ԃ��X�V����
*/
void CameraUpdate() {

	float sinParam = 0.0f;
	float cosParam = 0.0f;
	VECTOR tmpPos1;				//�ꎞ�ۑ��p�ϐ�
	VECTOR tmpPos2;				//
	VECTOR cameraLockAtPos;		//�J���������Ă�����W


	cameraLockAtPos = s_Camera.Target_Pos;
	cameraLockAtPos.y += 50.0f;

	//���������̊p�x���v�Z
	float vAngle = s_Camera.VAngle;// / 180.0f * DX_PI_F;

	sinParam = sin(vAngle);
	cosParam = cos(vAngle);

	tmpPos1.x = 0.0f;
	tmpPos1.y = sinParam * s_Camera.height;
	tmpPos1.z = cosParam * s_Camera.Distance;

	float hAngle = s_Camera.HAngle;// / 180.0f * DX_PI_F;

	sinParam = sin(hAngle);
	cosParam = cos(hAngle);

	tmpPos2.x = cosParam * tmpPos1.x - sinParam * tmpPos1.z;
	tmpPos2.y = tmpPos1.y;
	tmpPos2.z = sinParam * tmpPos1.x - cosParam * tmpPos1.z;

	//�J�����̍��W��ݒ�
	s_Camera.Pos = VAdd(tmpPos2, cameraLockAtPos);

	SetCameraPositionAndTarget_UpVecY(s_Camera.Pos, cameraLockAtPos);

}

void CameraSetUp() {}

/*
	�J�����ɕ`��Ώۂɂ���I�u�W�F�N�g�̍��W��ݒ�
*/
void Camera_SetTargetPosition(VECTOR targetPos) {
	s_Camera.Target_Pos = targetPos;
}

/*
	�p�x��ݒ肷��(X������)
*/
void Camera_SetVAngle(float vAngle) {
	s_Camera.VAngle = vAngle;
}

/*
�p�x��ݒ肷��(Y������)
*/
void Camera_SetHAngle(float hAngle) {
	s_Camera.HAngle = hAngle;
}

/*
�p�x��ݒ肷��(Z������)
*/
void Camera_SetTAngle(float tAngle) {
	s_Camera.TAngle = tAngle;
}

/*
�`��Ώۂ܂ł̋�����ݒ肷��
*/
void Camera_SetDistance(float distance) {
	s_Camera.Distance = distance;
}

/*
	�p�x��ԋp����(X������)
*/
float Camera_GetVAngle() {
	return s_Camera.VAngle;
}

/*
�p�x��ԋp����(Y������)
*/
float Camera_GetHAngle() {
	return s_Camera.HAngle;
}

/*
�p�x��ԋp����(Z������)
*/
float Camera_GetTAngle() {
	return s_Camera.TAngle;
}

/*
	�J�����̕`��Ώۂ܂ł̋�����ԋp����
*/
float Camera_GetDistance() {
	return s_Camera.Distance;
}

#if false

//�f�B�t���[�Y�J���[�ݒ�p
COLOR_F DifColor;
//�A���r�G���g�J���[�ݒ�p
COLOR_F AmbColor;
//sin�v�Z�p�ϐ�
float Sin;
//cos�v�Z�p�ϐ�
float Cos;

//�J�����̏����ݒ�
void Camera_Initialize(){

	//Window���Ƀ}�E�X���Œ肷��B
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);

	//�J�����̈ʒu�ݒ�
	CAMERA.Pos = VAdd(Get_Player_Pos(),VGet(0.0,-500.0,-500.0));//�@�\���ĂȂ�
	//�J�����̏㉺�����ݒ�
	CAMERA.Vec_UP = VGet(0,1,0);
	//�J���������p�x�̏����ݒ�
	CAMERA.HAngle = DX_PI_F;
	//�J���������p�x�̏����ݒ�
	CAMERA.VAngle = 0.05f;
	//�����_�܂ł̋��������ݒ�
	CAMERA.Distance = 270.0f;
	//�f�B���N�g�����C�g�̐ݒ�
	ChangeLightTypeDir(VGet(0.57735026f,0.57735026f,-0.57735026f));
	//�J�����̃N���b�v�����ݒ�
	SetCameraNearFar(CAMERA_NEAR_CLIP,CAMERA_FAR_CLIP);
	//�F�ʐݒ�
	DifColor = GetColorF(1,1,1,1.0);
	AmbColor = GetColorF(1,1,1,1.0);
	SetLightDifColor(DifColor);
	SetLightAmbColor(AmbColor);
	//�}�E�X�̋����ړ��t���O���I�t��
	MOUSE.Rest_Flg = true;

	//�}�E�X�J�[�\���̈ʒu����ʒ����ɐݒ�
	SetMousePoint(INIT_AREA_X2/2, INIT_AREA_Y2/2);


}

//�J�����n�����搶or�؉��搶�ɕ���--------------------------------------------

//�J�����̍X�V
void CameraUpdate(){


	//�}�E�X�̉�����Ԃ̎擾
	//MOUSE.Input = GetMouseInput();
	//���݂̃}�E�X���W�̎擾
	//GetMousePoint(&MOUSE.X, &MOUSE.Y);

	Mouse_GetPositioin(&MOUSE.X, &MOUSE.Y);

	MOUSE.X -= INIT_AREA_X2 / 2;
	MOUSE.Y -= INIT_AREA_Y2 / 2;

#ifdef __MY_DEBUG__
	if (Mouse_Press(eMouseInputBotton_Left)) {
		printfDx("��\n");
	}
	if (Mouse_Press(eMouseInputBotton_Rigth)) {
		printfDx("�E\n");
	}
	if (Mouse_Press(eMouseInputBotton_Wheel)) {
		printfDx("�z�C�[��\n");
	}
#endif


#if false //�Ƃ肠�����G���[�ɂȂ�̂ŃR�����g�A�E�g by.Syara
	//�E�B���h�E�\���ʒu�̎擾
	GetWindowRect(s_SYSTEM_INFO.WinHndl, s_SYSTEM_INFO.WinAxis);

	if (s_SYSTEM_INFO.WinAxis == NULL) return;

	//�}�E�X���W���� //�O�t���[������̈ړ��ʎZ�o������Ȃ炪���������Ȃ����Ǝv�������ǂ����ł��Ȃ������B�i���W�����ύX��j(�A�v���N����ʂɂ���č��W�ύX���Ȃ���΂Ȃ�Ȃ��B������͂Ȃ����̂��B)
	if(MOUSE.Rest_Flg == true){
		if(MOUSE.X >= s_SYSTEM_INFO.WinAxis->right){
			MOUSE.Rest_Flg = false;						//�}�E�X���W�����ړ��t���O���I���ɂ���B
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//�����ړ�
			MOUSE.Back_X = MOUSE.X;						//1�t���[���O���W�̍X�V
		}else if(MOUSE.X <= s_SYSTEM_INFO.WinAxis->left){
			MOUSE.Rest_Flg = false;
			MOUSE.X = s_SYSTEM_INFO.WinAxis->right;
			MOUSE.Back_X = MOUSE.X;
		}
		if(MOUSE.Y >= s_SYSTEM_INFO.WinAxis->bottom){
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->top;
			MOUSE.Back_Y = MOUSE.Y;
		}else if(MOUSE.Y <= s_SYSTEM_INFO.WinAxis->top){
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->bottom;
			MOUSE.Back_Y = MOUSE.Y;
		}
	}else{
		SetMousePoint(MOUSE.X,MOUSE.Y);					//�}�E�X���W�̃Z�b�g
		MOUSE.Rest_Flg = true;							//�}�E�X���W�����ړ��t���O���I�t�ɂ���B
	}
#endif	//false 

	//�}�E�X�̈ړ��ʂ��Z�o
	MOUSE.Move_X = MOUSE.X - MOUSE.Back_X;
	MOUSE.Move_Y = MOUSE.Y - MOUSE.Back_Y;
	//�}�E�X���W�ۑ�
	MOUSE.Back_X = MOUSE.X;
	MOUSE.Back_Y = MOUSE.Y;


	//�E�N���b�N��������Ă�����J�����̌�����ύX
	//if(MOUSE.Input & MOUSE_INPUT_RIGHT){
		//�����p�x�v�Z
		CAMERA.VAngle += CAMERA_ANGLE_SPEED*MOUSE.Move_Y;
		if(CAMERA.VAngle < -(DX_PI_F / 2.0f - 0.1f)){
			CAMERA.VAngle = -(DX_PI_F / 2.0f - 0.1f);
		}
		if(CAMERA.VAngle > DX_PI_F / 2.0f - 0.1f){
			CAMERA.VAngle = DX_PI_F / 2.0f - 0.1f;
		}
		//�����p�x�v�Z
		CAMERA.HAngle -= CAMERA_ANGLE_SPEED*MOUSE.Move_X;
		if(CAMERA.HAngle < 0.0f){
			CAMERA.HAngle += DX_PI_F*2.0f;
		}else if(CAMERA.HAngle >= DX_PI_F*2.0f){
			CAMERA.HAngle -= DX_PI_F*2.0f;
		}
	//}


	//�Q�[���J�n��-1.0�܂Ŏ�����(�J�����̐�����݂������B)
	if(MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM){
		//�X�^�[�g������̃J�E���g
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI/2/WHEEL_CNT_INIT_NUM*MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}else{
		//�}�E�X�z�C�[���̉�]�����擾
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		MOUSE.Wheel_Rot = Mouse_WheelValueF();
	}
	//�}�E�X�z�C�[������]���ꂽ��J�����̈ʒu��O��Ɉړ�����B
	//���̒l
	if(MOUSE.Wheel_Rot <= 0){
		//�z�C�[���ʁi�����_���������j//Sin�����ǉ��\��
		if(CAMERA.Distance <= DISTANCE_MAX){
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//���̒l
	}else if(MOUSE.Wheel_Rot >= 0){
		if(CAMERA.Distance >= DISTANCE_MIN){
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}
		
	}


}



//�J�����̃Z�b�g
void CameraSetUp(){

	//�J�����̍��W���Z�o
	//�����p�x
	Sin = sin(CAMERA.VAngle);
	Cos = cos(CAMERA.VAngle);
	CAMERA.Pos.x = 0.0f;
	CAMERA.Pos.y = Sin * CAMERA.Distance;
	CAMERA.Pos.z = Cos * CAMERA.Distance;

	//�����p�x
	Sin = sin(CAMERA.HAngle);
	Cos = cos(CAMERA.HAngle);
	CAMERA.Pos.x = -Sin * CAMERA.Pos.z;
	CAMERA.Pos.z = Cos * CAMERA.Pos.z;

	CAMERA.Pos = VAdd(CAMERA.Pos,CAMERA.Target_Pos);

	//�J�����������Ă�������̃x�N�g�����Z�o
	CAMERA.Direction = VNorm(VSub(CAMERA.Target_Pos,CAMERA.Pos));
	//�J�������猩���E�������Z�o
	CAMERA.Right_Direction = VNorm(VCross(CAMERA.Direction,VGet(0.0f,-1.0f,0.0f)));
	//���ʕ����̃x�N�g�����Z�o
	CAMERA.Front_Direction = VNorm(VCross(VGet(0.0f,-1.0f,0.0f),CAMERA.Right_Direction));

	//�J�����ʒu�ݒ�
	CAMERA.Target_Pos = VAdd(Get_Player_Pos(),VGet(0.0f,120.0f,0.0f));
	SetCameraPositionAndTargetAndUpVec(CAMERA.Pos,CAMERA.Target_Pos,CAMERA.Vec_UP);
	//3D�T�E���h�̃��X�i�[�ʒu�ƕ����̐ݒ�
	Set3DSoundListenerPosAndFrontPosAndUpVec(CAMERA.Pos,CAMERA.Target_Pos,CAMERA.Vec_UP);
}
//�J�����̌����Ă���������猩�ĉE�����̃x�N�g�����擾����B
VECTOR Get_Right_Direction(){
	return CAMERA.Right_Direction;
}

#endif

//�J�����N���X�̃f�o�b�O�p
void Debug_Camera(){
#ifdef __MY_DEBUG__
	DrawFormatString(10,20,Color_ValTbl[eCol_White],"�}�E�X�z�C�[���� = %f",MOUSE.Wheel_Rot);
	DrawFormatString(10,40,Color_ValTbl[eCol_White],"�}�E�X�z�C�[��(�ŏ��J�E���g�l) = %f",MOUSE.Wheel_Move_Cnt);
	DrawFormatString(10,60,Color_ValTbl[eCol_White],"�J�������W = ( %f , %f , %f)",s_Camera.Pos.x, s_Camera.Pos.y, s_Camera.Pos.z);
	DrawFormatString(10,80,Color_ValTbl[eCol_White],"�J�����^�[�Q�b�g���W = ( %f , %f , %f)", s_Camera.Target_Pos.x, s_Camera.Target_Pos.y, s_Camera.Target_Pos.z);
	DrawFormatString(10,100,Color_ValTbl[eCol_White],"�}�E�X���ݍ��W X,%d, Y,%d",MOUSE.X,MOUSE.Y);
	DrawFormatString(10,120,Color_ValTbl[eCol_White],"�}�E�X1�t���[���O���W X,%d, Y,%d",MOUSE.Back_X,MOUSE.Back_Y);
	DrawFormatString(10,140,Color_ValTbl[eCol_White],"�}�E�X1�t���[���ړ����W X,%d, Y,%d",MOUSE.Move_X,MOUSE.Move_Y);
	DrawFormatString(10,160,Color_ValTbl[eCol_White],"�}�E�X������� %d 1:���N���b�N 2:�E�N���b�N �}�E�X�E�N���b�N����p %d",MOUSE.Input,MOUSE_INPUT_RIGHT);
	DrawFormatString(10,180,Color_ValTbl[eCol_White],"%f", s_Camera.Pos.y);
	DrawFormatString(10,200,Color_ValTbl[eCol_White],"�����_�܂ł̋��� = %f,�z�C�[���� = %f", s_Camera.Distance,MOUSE.Wheel_Rot);//�ŒZ170�@�Œ�1000
#endif	//__MY_DEBUG__
}
