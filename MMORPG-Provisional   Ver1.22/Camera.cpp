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
//SYSTEM_INFOは既存のライブラリ内で宣言されていたので名前を変えました by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO;

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

}

//カメラ系加藤先生or木下先生に聞く--------------------------------------------

//カメラの更新
void CameraUpdate(){


	//マウスの押下状態の取得
	MOUSE.Input = GetMouseInput();
	//現在のマウス座標の取得
	GetMousePoint(&MOUSE.X, &MOUSE.Y);

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

	//Window内にマウスを固定する。
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);


	//右クリックが押されていたらカメラの向きを変更
	if(MOUSE.Input & MOUSE_INPUT_RIGHT){
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
	}


	//ゲーム開始時-1.0まで自動で(カメラの制限を設けたい。)
	if(MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM){
		//スタート時からのカウント
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI/2/WHEEL_CNT_INIT_NUM*MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}else{
		//マウスホイールの回転数を取得
		MOUSE.Wheel_Rot = GetMouseWheelRotVol();
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

//カメラクラスのデバッグ用
void Debug_Camera(){

	DrawFormatString(10,20,Color_ValTbl[eCol_White],"マウスホイール量 = %f",MOUSE.Wheel_Rot);
	DrawFormatString(10,40,Color_ValTbl[eCol_White],"マウスホイール(最初カウント値) = %f",MOUSE.Wheel_Move_Cnt);
	DrawFormatString(10,60,Color_ValTbl[eCol_White],"カメラ座標 = ( %f , %f , %f)",CAMERA.Pos.x,CAMERA.Pos.y,CAMERA.Pos.z);
	DrawFormatString(10,80,Color_ValTbl[eCol_White],"カメラターゲット座標 = ( %f , %f , %f)",CAMERA.Target_Pos.x,CAMERA.Target_Pos.y,CAMERA.Target_Pos.z);
	DrawFormatString(10,100,Color_ValTbl[eCol_White],"マウス現在座標 X,%d, Y,%d",MOUSE.X,MOUSE.Y);
	DrawFormatString(10,120,Color_ValTbl[eCol_White],"マウス1フレーム前座標 X,%d, Y,%d",MOUSE.Back_X,MOUSE.Back_Y);
	DrawFormatString(10,140,Color_ValTbl[eCol_White],"マウス1フレーム移動座標 X,%d, Y,%d",MOUSE.Move_X,MOUSE.Move_Y);
	DrawFormatString(10,160,Color_ValTbl[eCol_White],"マウス押下状態 %d 1:左クリック 2:右クリック マウス右クリック判定用 %d",MOUSE.Input,MOUSE_INPUT_RIGHT);
	DrawFormatString(10,180,Color_ValTbl[eCol_White],"%f",CAMERA.Pos.y);
	DrawFormatString(10,200,Color_ValTbl[eCol_White],"注視点までの距離 = %f,ホイール量 = %f",CAMERA.Distance,MOUSE.Wheel_Rot);//最短170　最長1000
}

