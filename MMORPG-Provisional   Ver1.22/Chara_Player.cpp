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

	//マウスの押下状態の取得
	//MOUSE.Input = GetMouseInput();
	//現在のマウス座標の取得
	//GetMousePoint(&MOUSE.X, &MOUSE.Y);

	Mouse_GetPositioin(&MOUSE.X, &MOUSE.Y);

	MOUSE.X -= INIT_AREA_X2 / 2;
	MOUSE.Y -= INIT_AREA_Y2 / 2;

#ifdef __MY_DEBUG__
	if (Mouse_Press(eMouseInputBotton_Left)) {
		printfDx("左\n");
	}
	if (Mouse_Press(eMouseInputBotton_Rigth)) {
		printfDx("右\n");
	}
	if (Mouse_Press(eMouseInputBotton_Wheel)) {
		printfDx("ホイール\n");
	}
#endif


#if false //とりあえずエラーになるのでコメントアウト by.Syara
	//ウィンドウ表示位置の取得
	GetWindowRect(s_SYSTEM_INFO.WinHndl, s_SYSTEM_INFO.WinAxis);

	if (s_SYSTEM_INFO.WinAxis == NULL) return;

	//マウス座標制限 //前フレームからの移動量算出した後ならがくつきが少ないかと思ったけどそうでもなかった。（座標強制変更後）(アプリ起動画面によって座標変更しなければならない。いい手はないものか。)
	if (MOUSE.Rest_Flg == true) {
		if (MOUSE.X >= s_SYSTEM_INFO.WinAxis->right) {
			MOUSE.Rest_Flg = false;						//マウス座標強制移動フラグをオンにする。
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//強制移動
			MOUSE.Back_X = MOUSE.X;						//1フレーム前座標の更新
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
		SetMousePoint(MOUSE.X, MOUSE.Y);					//マウス座標のセット
		MOUSE.Rest_Flg = true;							//マウス座標強制移動フラグをオフにする。
	}
#endif	//false 

	//マウスの移動量を算出
	MOUSE.Move_X = MOUSE.X - MOUSE.Back_X;
	MOUSE.Move_Y = MOUSE.Y - MOUSE.Back_Y;
	//マウス座標保存
	MOUSE.Back_X = MOUSE.X;
	MOUSE.Back_Y = MOUSE.Y;


	//右クリックが押されていたらカメラの向きを変更
	//if(MOUSE.Input & MOUSE_INPUT_RIGHT){
	
#if false
	//垂直角度計算
	*vAngle += CAMERA_ANGLE_SPEED * MOUSE.Move_Y;
	if (*vAngle < -(DX_PI_F / 2.0f - 0.1f)) {
		*vAngle = -(DX_PI_F / 2.0f - 0.1f);
	}
	if (*vAngle > DX_PI_F / 2.0f - 0.1f) {
		*vAngle = DX_PI_F / 2.0f - 0.1f;
	}
#endif

	//水平角度計算
	*hAngle -= CAMERA_ANGLE_SPEED * MOUSE.Move_X;
	if (*hAngle < 0.0f) {
		*hAngle += DX_PI_F * 2.0f;
	}
	else if (*hAngle >= DX_PI_F * 2.0f) {
		*hAngle -= DX_PI_F * 2.0f;
	}
	//}


	//ゲーム開始時-1.0まで自動で(カメラの制限を設けたい。)
	if (MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM) {
		//スタート時からのカウント
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI / 2 / WHEEL_CNT_INIT_NUM * MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}
	else {
		//マウスホイールの回転数を取得
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		MOUSE.Wheel_Rot = Mouse_WheelValueF();
	}
#if false
	//マウスホイールが回転されたらカメラの位置を前後に移動する。
	//負の値
	if (MOUSE.Wheel_Rot <= 0) {
		//ホイール量（注視点距離制限）//Sin処理追加予定
		if (CAMERA.Distance <= DISTANCE_MAX) {
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//正の値
	}
	else if (MOUSE.Wheel_Rot >= 0) {
		if (CAMERA.Distance >= DISTANCE_MIN) {
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}

	}
#endif


}


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

