#include"Dxlib.h"
#include"stdio.h"
#include"SceneMgr.h"
#include"Key.h"
#include"Define.h"
#include"Gamemain.h"
#include"Camera.h"
#include"Chara_Player.h"
#include"System.h"
#include"math.h"

_CAMERA CAMERA;
_MOUSE MOUSE;
//SYSTEM_INFO�͊����̃��C�u�������Ő錾����Ă����̂Ŗ��O��ς��܂��� by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO;

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

}

//�J�����n�����搶or�؉��搶�ɕ���--------------------------------------------

//�J�����̍X�V
void CameraUpdate(){


	//�}�E�X�̉�����Ԃ̎擾
	MOUSE.Input = GetMouseInput();
	//���݂̃}�E�X���W�̎擾
	GetMousePoint(&MOUSE.X, &MOUSE.Y);

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

	//Window���Ƀ}�E�X���Œ肷��B
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);


	//�E�N���b�N��������Ă�����J�����̌�����ύX
	if(MOUSE.Input & MOUSE_INPUT_RIGHT){
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
	}


	//�Q�[���J�n��-1.0�܂Ŏ�����(�J�����̐�����݂������B)
	if(MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM){
		//�X�^�[�g������̃J�E���g
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI/2/WHEEL_CNT_INIT_NUM*MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}else{
		//�}�E�X�z�C�[���̉�]�����擾
		MOUSE.Wheel_Rot = GetMouseWheelRotVol();
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

//�J�����N���X�̃f�o�b�O�p
void Debug_Camera(){

	DrawFormatString(10,20,Color_ValTbl[eCol_White],"�}�E�X�z�C�[���� = %f",MOUSE.Wheel_Rot);
	DrawFormatString(10,40,Color_ValTbl[eCol_White],"�}�E�X�z�C�[��(�ŏ��J�E���g�l) = %f",MOUSE.Wheel_Move_Cnt);
	DrawFormatString(10,60,Color_ValTbl[eCol_White],"�J�������W = ( %f , %f , %f)",CAMERA.Pos.x,CAMERA.Pos.y,CAMERA.Pos.z);
	DrawFormatString(10,80,Color_ValTbl[eCol_White],"�J�����^�[�Q�b�g���W = ( %f , %f , %f)",CAMERA.Target_Pos.x,CAMERA.Target_Pos.y,CAMERA.Target_Pos.z);
	DrawFormatString(10,100,Color_ValTbl[eCol_White],"�}�E�X���ݍ��W X,%d, Y,%d",MOUSE.X,MOUSE.Y);
	DrawFormatString(10,120,Color_ValTbl[eCol_White],"�}�E�X1�t���[���O���W X,%d, Y,%d",MOUSE.Back_X,MOUSE.Back_Y);
	DrawFormatString(10,140,Color_ValTbl[eCol_White],"�}�E�X1�t���[���ړ����W X,%d, Y,%d",MOUSE.Move_X,MOUSE.Move_Y);
	DrawFormatString(10,160,Color_ValTbl[eCol_White],"�}�E�X������� %d 1:���N���b�N 2:�E�N���b�N �}�E�X�E�N���b�N����p %d",MOUSE.Input,MOUSE_INPUT_RIGHT);
	DrawFormatString(10,180,Color_ValTbl[eCol_White],"%f",CAMERA.Pos.y);
	DrawFormatString(10,200,Color_ValTbl[eCol_White],"�����_�܂ł̋��� = %f,�z�C�[���� = %f",CAMERA.Distance,MOUSE.Wheel_Rot);//�ŒZ170�@�Œ�1000
}

