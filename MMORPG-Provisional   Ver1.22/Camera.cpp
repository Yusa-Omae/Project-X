#include "Camera.h"
#include "System.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"
#include <math.h>

// カメラの当たり判定サイズ
#define COLLISION_SIZE				(15.0f)

// カメラの角度変更速度
#define ANGLE_SPEED					(3.0f)

// カメラとプレイヤーの距離
#define PLAYER_DISTANCE				(500.0f)

// カメラが注視点とするプレイヤーの座標のＹ座標に足す値
#define PLAYER_OFFSET_Y				(120.0f)

// カメラの初期水平角度
#define DEFAULT_H_ANGLE				(DX_PI_F)

// カメラの初期垂直角度
#define DEFAULT_V_ANGLE				(0.4f)

// カメラの画角
#define CAMERA_FOV					(40.0f)

// カメラの座標を目標座標に近づける計算で除算に使用する値
#define MOVE_DIV_NUM				(0.17f)

// カメラの情報
typedef struct _SCameraInfo
{
	// カメラの座標
	VECTOR Position;

	// カメラの注視点座標
	VECTOR LookAtPosition;

	// カメラ揺らし座標を無視したカメラの座標
	VECTOR OrigPosition;

	// カメラ揺らし座標を無視したカメラの注視点座標
	VECTOR OrigLookAtPosition;

	// カメラの目標座標
	VECTOR TargetPosition;

	// カメラの目標注視点座標
	VECTOR TargetLookAtPosition;

	// カメラの注視点に対する水平角度
	float  HAngle;

	// カメラの注視点に対する垂直角度
	float  VAngle;

	// カメラの座標からカメラの注視点座標へのベクトル
	VECTOR Direction;

	// カメラから見て右方向のベクトル
	VECTOR RightDirection;

	// カメラの正面方向のＹ成分を抜いたベクトル
	VECTOR FrontDirection;

	// カメラを揺らす処理を行っているかどうか
	bool   Shake;

	// カメラを揺らす幅
	float  ShakeWidth;

	// カメラを揺らす時間
	float  ShakeTime;

	// カメラを揺らす処理の経過時間計測用変数
	float  ShakeTimeCounter;

	// カメラを揺らす処理で使用する sin に渡す角度
	float  ShakeAngle;

	// カメラを揺らす処理で使用する sin に渡す角度の変化速度
	float  ShakeAngleSpeed;

	// カメラを揺らす処理で算出したカメラ座標に足す座標
	VECTOR ShakePosition;
} SCameraInfo;

static SCameraInfo g_CamInfo;

// カメラの処理を初期化する
void Camera_Initialize(void)
{
	// 各パラメータに初期値をセットする
	g_CamInfo.HAngle = DEFAULT_H_ANGLE;
	g_CamInfo.VAngle = DEFAULT_V_ANGLE;
	g_CamInfo.Shake = false;
	g_CamInfo.ShakePosition = VGet(0.0f, 0.0f, 0.0f);
}

// カメラの状態推移処理を実行する
void Camera_Step(
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	SCharaInfo *PCInfo;
	VECTOR      CameraStickInput;
	float       MoveScale;
	VECTOR      TempV;
	float       NotHitDistance;
	float       HitDistance;
	float       TestDistance;
	VECTOR      TestPosition;
	VECTOR      Direction;

	// プレイヤーのキャラ情報構造体のアドレスを取得する
	PCInfo = System_GetPlayerCharaInfo();

	// プレイヤーが注視点になるので、プレイヤーが存在しない場合は何もせず終了
	if (PCInfo == NULL)
	{
		return;
	}

	// カメラの操作入力を取得する
	CameraStickInput = GetCameraStickInput();

	// 水平方向の角度変更入力の処理
	g_CamInfo.HAngle += -CameraStickInput.x * ANGLE_SPEED * StepTime;
	if (g_CamInfo.HAngle < -DX_PI_F)
	{
		g_CamInfo.HAngle += DX_TWO_PI_F;
	}
	if (g_CamInfo.HAngle > DX_PI_F)
	{
		g_CamInfo.HAngle -= DX_TWO_PI_F;
	}

	// 垂直方向の角度変更入力の処理
	g_CamInfo.VAngle += CameraStickInput.y * ANGLE_SPEED * StepTime;
	if (g_CamInfo.VAngle < -DX_PI_F / 2.0f + 0.6f)
	{
		g_CamInfo.VAngle = -DX_PI_F / 2.0f + 0.6f;
	}
	if (g_CamInfo.VAngle > DX_PI_F / 2.0f - 0.6f)
	{
		g_CamInfo.VAngle = DX_PI_F / 2.0f - 0.6f;
	}

	// カメラを揺らす処理を行うかどうかのフラグが立っていたらカメラを揺らす処理を行う
	if (g_CamInfo.Shake)
	{
		// sin を使用した揺らし座標の算出
		g_CamInfo.ShakePosition.y =
			sin(g_CamInfo.ShakeAngle) * (1.0f - (g_CamInfo.ShakeTimeCounter /
				g_CamInfo.ShakeTime)) * g_CamInfo.ShakeWidth;
		g_CamInfo.ShakePosition.x = 0.0f;
		g_CamInfo.ShakePosition.z = 0.0f;

		// 揺らし処理に使用する sin に渡す角度の変更処理
		g_CamInfo.ShakeAngle += g_CamInfo.ShakeAngleSpeed * StepTime;

		// 揺らす時間が経過したら揺らし処理を終了する
		g_CamInfo.ShakeTimeCounter += StepTime;
		if (g_CamInfo.ShakeTimeCounter >= g_CamInfo.ShakeTime)
		{
			g_CamInfo.Shake = false;
		}
	}
	else
	{
		// 揺らされていない場合は揺らし処理による加算座標を０にする
		g_CamInfo.ShakePosition = VGet(0.0f, 0.0f, 0.0f);
	}

	// 目標注視点座標の算出
	g_CamInfo.TargetLookAtPosition = PCInfo->Position;
	g_CamInfo.TargetLookAtPosition.y += PLAYER_OFFSET_Y;

	// 目標座標の算出
	g_CamInfo.TargetPosition.x = 0.0f;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.VAngle) * PLAYER_DISTANCE;
	g_CamInfo.TargetPosition.y = sin(g_CamInfo.VAngle) * PLAYER_DISTANCE;

	g_CamInfo.TargetPosition.x = -sin(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;

	g_CamInfo.TargetPosition = VAdd(g_CamInfo.TargetPosition,
		g_CamInfo.TargetLookAtPosition);

	// カメラの揺らし処理を無視したカメラの座標、注視点座標をそれぞれの目標座標に近づける
	MoveScale = StepTime / MOVE_DIV_NUM;

	TempV = VSub(g_CamInfo.TargetPosition, g_CamInfo.OrigPosition);
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigPosition = VAdd(g_CamInfo.OrigPosition, TempV);

	TempV = VSub(g_CamInfo.TargetLookAtPosition, g_CamInfo.OrigLookAtPosition);
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigLookAtPosition = VAdd(g_CamInfo.OrigLookAtPosition, TempV);

	// カメラの揺らし処理を無視したカメラの座標、注視点座標にカメラの揺らし処理による
	// 加算座標を足した座標を現在のカメラの座標、注視点座標とする
	g_CamInfo.Position =
		VAdd(g_CamInfo.OrigPosition, g_CamInfo.ShakePosition);

	g_CamInfo.LookAtPosition =
		VAdd(g_CamInfo.OrigLookAtPosition, g_CamInfo.ShakePosition);

	// カメラの座標からカメラの注視点座標へのベクトルを算出
	g_CamInfo.Direction = VNorm(VSub(g_CamInfo.LookAtPosition, g_CamInfo.Position));

	// カメラから見て右方向のベクトルを算出
	g_CamInfo.RightDirection =
		VNorm(VCross(g_CamInfo.Direction, VGet(0.0f, -1.0f, 0.0f)));

	// カメラの正面方向のＹ成分を抜いたベクトルを算出
	g_CamInfo.FrontDirection =
		VNorm(VCross(VGet(0.0f, -1.0f, 0.0f), g_CamInfo.RightDirection));

	// カメラ座標から注視点座標の間にコリジョン用ポリゴンが存在するかチェック
	if (Stage_HitCheck(g_CamInfo.Position, g_CamInfo.LookAtPosition, COLLISION_SIZE))
	{
		// コリジョン用ポリゴンが存在しない座標までカメラ座標を注視点に近づける

		// 注視点座標からカメラ座標へのベクトルを算出
		Direction = VNorm(VSub(g_CamInfo.Position, g_CamInfo.LookAtPosition));

		// ポリゴンに当たらない距離をセット
		NotHitDistance = 0.0f;

		// ポリゴンに当たる距離をセット
		HitDistance = PLAYER_DISTANCE;
		do
		{
			// 当たるかどうかテストする距離を算出( 当たらない距離と当たる距離の中間 )
			TestDistance = NotHitDistance + (HitDistance - NotHitDistance) / 2.0f;

			// テスト用のカメラ座標を算出
			TestPosition =
				VAdd(g_CamInfo.LookAtPosition, VScale(Direction, TestDistance));

			// 新しい座標でコリジョン用ポリゴンに当たるかテスト
			if (Stage_HitCheck(TestPosition, g_CamInfo.LookAtPosition, COLLISION_SIZE))
			{
				// 当たったら当たる距離を TestDistance に変更する
				HitDistance = TestDistance;
			}
			else
			{
				// 当たらなかったら当たらない距離を TestDistance に変更する
				NotHitDistance = TestDistance;
			}

			// HitDistance と NoHitDistance が十分に近づいていなかったらループ
		} while (HitDistance - NotHitDistance > 1.0f);

		// カメラの座標をセット
		g_CamInfo.Position = TestPosition;
		g_CamInfo.OrigPosition = TestPosition;
	}

	// リスナーの位置を変更
	Set3DSoundListenerPosAndFrontPos_UpVecY(
		g_CamInfo.Position, g_CamInfo.TargetLookAtPosition);
}

// カメラの設定を行う
void Camera_Setup(void)
{
	// カメラの写す範囲を設定
	SetupCamera_Perspective(CAMERA_FOV * DX_PI_F / 180.0f);
	SetCameraNearFar(CAMERA_NEAR_Z, CAMERA_FAR_Z);
	SetCameraPositionAndTarget_UpVecY(g_CamInfo.Position, g_CamInfo.LookAtPosition);
}

// カメラを揺らす処理を開始する
void Camera_Shake(
	// 揺れる幅
	float ShakeWidth,

	// 揺れる時間
	float ShakeTime,

	// 揺れの速度
	float ShakeAngleSpeed
)
{
	g_CamInfo.Shake = true;
	g_CamInfo.ShakeWidth = ShakeWidth;
	g_CamInfo.ShakeTimeCounter = 0.0f;
	g_CamInfo.ShakeTime = ShakeTime;
	g_CamInfo.ShakeAngle = 0.0f;
	g_CamInfo.ShakeAngleSpeed = ShakeAngleSpeed;
}

// カメラの向いている方向を取得する
//     戻り値 : カメラの向いている方向のベクトル
VECTOR Camera_FrontDirection(void)
{
	return g_CamInfo.FrontDirection;
}

// カメラの向いている方向から見て右方向を取得する
//     戻り値 : カメラの向いている方向から見て右方向のベクトルを取得する
VECTOR Camera_RightDirection(void)
{
	return g_CamInfo.RightDirection;
}


#if false
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
#endif