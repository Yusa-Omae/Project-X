#pragma once

void Camera_Initialize();
void CameraUpdate();
void CameraSetUp();
VECTOR Get_Right_Direction();


void Debug_Camera();



struct _CAMERA{
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
	//カメラ向き
	VECTOR Direction;
	//正面方向のベクトル
	VECTOR Front_Direction;
	//カメラ向き(右方向)
	VECTOR Right_Direction;

};

struct _MOUSE{
	//マウスの現在座標
	int X;
	int Y;
	//1フレーム前のマウス座標
	int Back_X;
	int Back_Y;
	//1フレーム前から動いたマウス量
	int Move_X;
	int Move_Y;
	//マウスの強制座標変換を行ったか
	bool Rest_Flg;
	//マウスホイール移動量
	float Wheel_Rot;
	//マウスホイール移動量を割って適切な値にしたもの
	float Wheel_Move_Per;
	//ゲームスタート時規定ズーム値変更関数
	float Wheel_Move_Cnt;
	//マウスの押下状態
	int Input;

};