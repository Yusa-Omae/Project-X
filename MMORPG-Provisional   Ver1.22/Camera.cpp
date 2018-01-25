#include"Dxlib.h"
#include"Define.h"
#include"Camera.h"
#include"System.h"
#include<math.h>




struct _CAMERA {

	//カメラの位置
	VECTOR Pos;
	//カメラの注視点位置
	VECTOR Target_Pos;
	//注視点までの距離
	float Distance;
	//カメラの上下方向
	VECTOR Vec_UP;
	//カメラのズームイン・アウト 初期カメラ倍率固定
	VECTOR Zoom_Base;
	//カメラ倍率変更分 AS=(Add/Sub)
	MATRIX Zoom_AS;
	//水平角度
	float HAngle;
	//垂直角度
	float VAngle;
	//Z軸方向の角度
	float TAngle;
	//カメラ向き
	VECTOR Direction;
	//正面方向のベクトル
	VECTOR Front_Direction;
	//カメラ向き(右方向)
	VECTOR Right_Direction;
	//カメラの高さ
	float height;


};

static _CAMERA s_Camera;
_MOUSE MOUSE;
//SYSTEM_INFOは既存のライブラリ内で宣言されていたので名前を変えました by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO; 



void Camera_Initialize() {


	//カメラ水平角度の初期設定
	s_Camera.HAngle = DX_PI_F;
	//カメラ垂直角度の初期設定
	s_Camera.VAngle = 0.05f;
	//注視点までの距離初期設定
	s_Camera.Distance = 270.0f;
	
	//ディレクトリライトの設定
	ChangeLightTypeDir(VGet(0.57735026f, 0.57735026f, -0.57735026f));
	//色彩設定
	COLOR_F DifColor = GetColorF(1, 1, 1, 1.0);
	COLOR_F AmbColor = GetColorF(1, 1, 1, 1.0);
	SetLightDifColor(DifColor);
	SetLightAmbColor(AmbColor);

	//カメラのクリップ距離設定
	SetCameraNearFar(CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP);

	s_Camera.height = 100.0f;
	s_Camera.Distance = 1000.0f;

}


/*
	カメラの状態を更新する
*/
void CameraUpdate() {

	float sinParam = 0.0f;
	float cosParam = 0.0f;
	VECTOR tmpPos1;				//一時保存用変数
	VECTOR tmpPos2;				//
	VECTOR cameraLockAtPos;		//カメラが見ている座標


	cameraLockAtPos = s_Camera.Target_Pos;
	cameraLockAtPos.y += 50.0f;

	//水平方向の角度を計算
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

	//カメラの座標を設定
	s_Camera.Pos = VAdd(tmpPos2, cameraLockAtPos);

	SetCameraPositionAndTarget_UpVecY(s_Camera.Pos, cameraLockAtPos);

}

void CameraSetUp() {}

/*
	カメラに描画対象にするオブジェクトの座標を設定
*/
void Camera_SetTargetPosition(VECTOR targetPos) {
	s_Camera.Target_Pos = targetPos;
}

/*
	角度を設定する(X軸方向)
*/
void Camera_SetVAngle(float vAngle) {
	s_Camera.VAngle = vAngle;
}

/*
角度を設定する(Y軸方向)
*/
void Camera_SetHAngle(float hAngle) {
	s_Camera.HAngle = hAngle;
}

/*
角度を設定する(Z軸方向)
*/
void Camera_SetTAngle(float tAngle) {
	s_Camera.TAngle = tAngle;
}

/*
描画対象までの距離を設定する
*/
void Camera_SetDistance(float distance) {
	s_Camera.Distance = distance;
}

/*
	角度を返却する(X軸方向)
*/
float Camera_GetVAngle() {
	return s_Camera.VAngle;
}

/*
角度を返却する(Y軸方向)
*/
float Camera_GetHAngle() {
	return s_Camera.HAngle;
}

/*
角度を返却する(Z軸方向)
*/
float Camera_GetTAngle() {
	return s_Camera.TAngle;
}

/*
	カメラの描画対象までの距離を返却する
*/
float Camera_GetDistance() {
	return s_Camera.Distance;
}

#if false

//ディフューズカラー設定用
COLOR_F DifColor;
//アンビエントカラー設定用
COLOR_F AmbColor;
//sin計算用変数
float Sin;
//cos計算用変数
float Cos;

//カメラの初期設定
void Camera_Initialize(){

	//Window内にマウスを固定する。
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);

	//カメラの位置設定
	CAMERA.Pos = VAdd(Get_Player_Pos(),VGet(0.0,-500.0,-500.0));//機能してない
	//カメラの上下向き設定
	CAMERA.Vec_UP = VGet(0,1,0);
	//カメラ水平角度の初期設定
	CAMERA.HAngle = DX_PI_F;
	//カメラ垂直角度の初期設定
	CAMERA.VAngle = 0.05f;
	//注視点までの距離初期設定
	CAMERA.Distance = 270.0f;
	//ディレクトリライトの設定
	ChangeLightTypeDir(VGet(0.57735026f,0.57735026f,-0.57735026f));
	//カメラのクリップ距離設定
	SetCameraNearFar(CAMERA_NEAR_CLIP,CAMERA_FAR_CLIP);
	//色彩設定
	DifColor = GetColorF(1,1,1,1.0);
	AmbColor = GetColorF(1,1,1,1.0);
	SetLightDifColor(DifColor);
	SetLightAmbColor(AmbColor);
	//マウスの強制移動フラグをオフに
	MOUSE.Rest_Flg = true;

	//マウスカーソルの位置を画面中央に設定
	SetMousePoint(INIT_AREA_X2/2, INIT_AREA_Y2/2);


}

//カメラ系加藤先生or木下先生に聞く--------------------------------------------

//カメラの更新
void CameraUpdate(){


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
	if(MOUSE.Rest_Flg == true){
		if(MOUSE.X >= s_SYSTEM_INFO.WinAxis->right){
			MOUSE.Rest_Flg = false;						//マウス座標強制移動フラグをオンにする。
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//強制移動
			MOUSE.Back_X = MOUSE.X;						//1フレーム前座標の更新
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
		SetMousePoint(MOUSE.X,MOUSE.Y);					//マウス座標のセット
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
		//垂直角度計算
		CAMERA.VAngle += CAMERA_ANGLE_SPEED*MOUSE.Move_Y;
		if(CAMERA.VAngle < -(DX_PI_F / 2.0f - 0.1f)){
			CAMERA.VAngle = -(DX_PI_F / 2.0f - 0.1f);
		}
		if(CAMERA.VAngle > DX_PI_F / 2.0f - 0.1f){
			CAMERA.VAngle = DX_PI_F / 2.0f - 0.1f;
		}
		//水平角度計算
		CAMERA.HAngle -= CAMERA_ANGLE_SPEED*MOUSE.Move_X;
		if(CAMERA.HAngle < 0.0f){
			CAMERA.HAngle += DX_PI_F*2.0f;
		}else if(CAMERA.HAngle >= DX_PI_F*2.0f){
			CAMERA.HAngle -= DX_PI_F*2.0f;
		}
	//}


	//ゲーム開始時-1.0まで自動で(カメラの制限を設けたい。)
	if(MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM){
		//スタート時からのカウント
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI/2/WHEEL_CNT_INIT_NUM*MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}else{
		//マウスホイールの回転数を取得
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		MOUSE.Wheel_Rot = Mouse_WheelValueF();
	}
	//マウスホイールが回転されたらカメラの位置を前後に移動する。
	//負の値
	if(MOUSE.Wheel_Rot <= 0){
		//ホイール量（注視点距離制限）//Sin処理追加予定
		if(CAMERA.Distance <= DISTANCE_MAX){
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//正の値
	}else if(MOUSE.Wheel_Rot >= 0){
		if(CAMERA.Distance >= DISTANCE_MIN){
			CAMERA.Distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}
		
	}


}



//カメラのセット
void CameraSetUp(){

	//カメラの座標を算出
	//垂直角度
	Sin = sin(CAMERA.VAngle);
	Cos = cos(CAMERA.VAngle);
	CAMERA.Pos.x = 0.0f;
	CAMERA.Pos.y = Sin * CAMERA.Distance;
	CAMERA.Pos.z = Cos * CAMERA.Distance;

	//水平角度
	Sin = sin(CAMERA.HAngle);
	Cos = cos(CAMERA.HAngle);
	CAMERA.Pos.x = -Sin * CAMERA.Pos.z;
	CAMERA.Pos.z = Cos * CAMERA.Pos.z;

	CAMERA.Pos = VAdd(CAMERA.Pos,CAMERA.Target_Pos);

	//カメラが向いている方向のベクトルを算出
	CAMERA.Direction = VNorm(VSub(CAMERA.Target_Pos,CAMERA.Pos));
	//カメラから見た右方向を算出
	CAMERA.Right_Direction = VNorm(VCross(CAMERA.Direction,VGet(0.0f,-1.0f,0.0f)));
	//正面方向のベクトルを算出
	CAMERA.Front_Direction = VNorm(VCross(VGet(0.0f,-1.0f,0.0f),CAMERA.Right_Direction));

	//カメラ位置設定
	CAMERA.Target_Pos = VAdd(Get_Player_Pos(),VGet(0.0f,120.0f,0.0f));
	SetCameraPositionAndTargetAndUpVec(CAMERA.Pos,CAMERA.Target_Pos,CAMERA.Vec_UP);
	//3Dサウンドのリスナー位置と方向の設定
	Set3DSoundListenerPosAndFrontPosAndUpVec(CAMERA.Pos,CAMERA.Target_Pos,CAMERA.Vec_UP);
}
//カメラの向いている方向から見て右方向のベクトルを取得する。
VECTOR Get_Right_Direction(){
	return CAMERA.Right_Direction;
}

#endif

//カメラクラスのデバッグ用
void Debug_Camera(){
#ifdef __MY_DEBUG__
	DrawFormatString(10,20,Color_ValTbl[eCol_White],"マウスホイール量 = %f",MOUSE.Wheel_Rot);
	DrawFormatString(10,40,Color_ValTbl[eCol_White],"マウスホイール(最初カウント値) = %f",MOUSE.Wheel_Move_Cnt);
	DrawFormatString(10,60,Color_ValTbl[eCol_White],"カメラ座標 = ( %f , %f , %f)",s_Camera.Pos.x, s_Camera.Pos.y, s_Camera.Pos.z);
	DrawFormatString(10,80,Color_ValTbl[eCol_White],"カメラターゲット座標 = ( %f , %f , %f)", s_Camera.Target_Pos.x, s_Camera.Target_Pos.y, s_Camera.Target_Pos.z);
	DrawFormatString(10,100,Color_ValTbl[eCol_White],"マウス現在座標 X,%d, Y,%d",MOUSE.X,MOUSE.Y);
	DrawFormatString(10,120,Color_ValTbl[eCol_White],"マウス1フレーム前座標 X,%d, Y,%d",MOUSE.Back_X,MOUSE.Back_Y);
	DrawFormatString(10,140,Color_ValTbl[eCol_White],"マウス1フレーム移動座標 X,%d, Y,%d",MOUSE.Move_X,MOUSE.Move_Y);
	DrawFormatString(10,160,Color_ValTbl[eCol_White],"マウス押下状態 %d 1:左クリック 2:右クリック マウス右クリック判定用 %d",MOUSE.Input,MOUSE_INPUT_RIGHT);
	DrawFormatString(10,180,Color_ValTbl[eCol_White],"%f", s_Camera.Pos.y);
	DrawFormatString(10,200,Color_ValTbl[eCol_White],"注視点までの距離 = %f,ホイール量 = %f", s_Camera.Distance,MOUSE.Wheel_Rot);//最短170　最長1000
#endif	//__MY_DEBUG__
}
