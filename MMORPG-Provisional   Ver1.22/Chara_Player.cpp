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
#include "Code/Common/Mouse/Mouse.h"

#define MOVE_SPEED (-2.5f)

_PLAYER *PLAYER;
static struct _MOUSE MOUSE;

static void Player_DirectionProc(float* vAngle,float* hAngle) {

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
	if (MOUSE.Rest_Flg == true) {
		if (MOUSE.X >= s_SYSTEM_INFO.WinAxis->right) {
			MOUSE.Rest_Flg = false;						//�}�E�X���W�����ړ��t���O���I���ɂ���B
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//�����ړ�
			MOUSE.Back_X = MOUSE.X;						//1�t���[���O���W�̍X�V
		}
		else if (MOUSE.X <= s_SYSTEM_INFO.WinAxis->left) {
			MOUSE.Rest_Flg = false;
			MOUSE.X = s_SYSTEM_INFO.WinAxis->right;
			MOUSE.Back_X = MOUSE.X;
		}
		if (MOUSE.Y >= s_SYSTEM_INFO.WinAxis->bottom) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->top;
			MOUSE.Back_Y = MOUSE.Y;
		}
		else if (MOUSE.Y <= s_SYSTEM_INFO.WinAxis->top) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->bottom;
			MOUSE.Back_Y = MOUSE.Y;
		}
	}
	else {
		SetMousePoint(MOUSE.X, MOUSE.Y);					//�}�E�X���W�̃Z�b�g
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
	
#if false
	//�����p�x�v�Z
	*vAngle += CAMERA_ANGLE_SPEED * MOUSE.Move_Y;
	if (*vAngle < -(DX_PI_F / 2.0f - 0.1f)) {
		*vAngle = -(DX_PI_F / 2.0f - 0.1f);
	}
	if (*vAngle > DX_PI_F / 2.0f - 0.1f) {
		*vAngle = DX_PI_F / 2.0f - 0.1f;
	}
#endif

	//�����p�x�v�Z
	*hAngle -= CAMERA_ANGLE_SPEED * MOUSE.Move_X;
	if (*hAngle < 0.0f) {
		*hAngle += DX_PI_F * 2.0f;
	}
	else if (*hAngle >= DX_PI_F * 2.0f) {
		*hAngle -= DX_PI_F * 2.0f;
	}
	//}


	//�Q�[���J�n��-1.0�܂Ŏ�����(�J�����̐�����݂������B)
	if (MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM) {
		//�X�^�[�g������̃J�E���g
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI / 2 / WHEEL_CNT_INIT_NUM * MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}
	else {
		//�}�E�X�z�C�[���̉�]�����擾
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		MOUSE.Wheel_Rot = Mouse_WheelValueF();
	}
#if false
	//�}�E�X�z�C�[������]���ꂽ��J�����̈ʒu��O��Ɉړ�����B
	//���̒l
	if (MOUSE.Wheel_Rot <= 0) {
		//�z�C�[���ʁi�����_���������j//Sin�����ǉ��\��
		if (CAMERA.Distance <= DISTANCE_MAX) {
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//���̒l
	}
	else if (MOUSE.Wheel_Rot >= 0) {
		if (CAMERA.Distance >= DISTANCE_MIN) {
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}

	}
#endif


}


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

	SetMousePoint(INIT_AREA_X2 / 2, INIT_AREA_Y2 / 2);

}

void PlayerUpdate(){

	float dir = Get_Chara_Rotation(model_Player);

	float isMove = true;
	float vAngle = Camera_GetVAngle();
	float hAngle = Camera_GetHAngle();
	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);

	VECTOR playerPos = PLAYER->pos;

	if(key(KEY_INPUT_S)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Up);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_W)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Down);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_D)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Left);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_A)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Right);
		Set_Move_flg(model_Player, FALSE);
	}else{
		Set_Move_flg(model_Player, TRUE);
		isMove = false;
	}

	Player_DirectionProc(&vAngle, &hAngle);

	if (isMove) {

		VECTOR tmpVec;
		float sinParam;
		float cosParam;
		
		float angle = hAngle;/// 180.0f * DX_PI_F;
		sinParam = sin(angle);
		cosParam = cos(angle);

		tmpVec.x = moveVec.x * cosParam - moveVec.z * sinParam;
		tmpVec.y = 0.0f;
		tmpVec.z = moveVec.x * sinParam + moveVec.z * cosParam;

		playerPos = VAdd(playerPos, tmpVec);
		

	}

	PLAYER->pos = playerPos;

	Camera_SetVAngle(vAngle);
	Camera_SetHAngle(hAngle);
	Camera_SetTargetPosition(PLAYER->pos);

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

