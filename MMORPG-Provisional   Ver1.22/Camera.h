#ifndef CAMERA_H
#define CAMERA_H

#include "DxLib.h"

// カメラの手前クリップ距離
#define CAMERA_NEAR_Z		(20.0f)

// カメラの奥クリップ距離
#define CAMERA_FAR_Z		(25000.0f)

// カメラの処理を初期化する
extern void Camera_Initialize(void);

// カメラの状態推移処理を実行する
extern void Camera_Step(
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// カメラの設定を行う
extern void Camera_Setup(void);

// カメラを揺らす処理を開始する
extern void Camera_Shake(
	// 揺れる幅
	float ShakeWidth,

	// 揺れる時間
	float ShakeTime,

	// 揺れの速度
	float ShakeAngleSpeed
);

// カメラの向いている方向を取得する
//     戻り値 : カメラの向いている方向のベクトル
extern VECTOR Camera_FrontDirection(void);

// カメラの向いている方向から見て右方向を取得する
//     戻り値 : カメラの向いている方向から見て右方向のベクトルを取得する
extern VECTOR Camera_RightDirection(void);

#endif


#if false
#pragma once

void Camera_Initialize();
void CameraUpdate();
void CameraSetUp();
VECTOR Get_Right_Direction();


/*
カメラに描画対象にするオブジェクトの座標を設定
*/
void Camera_SetTargetPosition(VECTOR targetPos);

/*
角度を設定する(X軸方向)
*/
void Camera_SetVAngle(float vAngle);

/*
角度を設定する(Y軸方向)
*/
void Camera_SetHAngle(float hAngle);

/*
角度を設定する(Z軸方向)
*/
void Camera_SetTAngle(float tAngle);

/*
	描画対象までの距離を設定する
*/
void Camera_SetDistance(float distance);

/*
角度を返却する(X軸方向)
*/
float Camera_GetVAngle();
/*
角度を返却する(Y軸方向)
*/
float Camera_GetHAngle();

/*
角度を返却する(Z軸方向)
*/
float Camera_GetTAngle();

/*
カメラの描画対象までの距離を返却する
*/
float Camera_GetDistance();

// ------------ デバッグ機能 ----------
void Debug_Camera();


#if false
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
#endif

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
#endif