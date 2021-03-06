#pragma once

//------------------------------------------------------------------------------------------------ゲームモード-----------------------------------------------------------------------------------------------

//#define SET_DEBAG_FLAG 0
#define SET_TIMER_FLAG 0
#define SET_STAGE_COM_FLAG 1


//-------------------------------------------------------------------------------------------------カメラ関係------------------------------------------------------------------------------------------------

//カメラの手前クリップ距離
#define CAMERA_NEAR_CLIP 50.0f
//カメラの奥クリップ距離
#define CAMERA_FAR_CLIP 5000.0f
//マウスホイールを動かしたときのカメラ移動量
#define WHEEL_SPEED 60.0f
//マウスホイール初期値
#define WHEEL_INIT_NUM -1.0f
//カメラズームカウント初期値
#define WHEEL_CNT_INIT_NUM -0.5f
//ホイールの初期カウント値
#define WHEEL_MOVE_CNT 0.05f
//ズーム最大値
#define WHEEL_MAX -0.5f
//ズーム最小値
#define WHEEL_MIN -3.0f
//カメラアングルの移動量
#define CAMERA_ANGLE_SPEED 0.005f
//注視点までの最大距離
#define DISTANCE_MAX 1000.0f
//注視点までの最小距離
#define DISTANCE_MIN 170.0f
//マウス移動制限座標 左上X座標		//
#define MOUSE_AREA_X1 50			//全て余裕をもって
//マウス移動制限座標 右下X座標		//
#define MOUSE_AREA_X2 1870			//フルスクリーンより
//マウス移動制限座標 左上Y座標		//
#define MOUSE_AREA_Y1 50			//+-50の値にしてある。
//マウス移動制限座標 右下Y座標		//
#define MOUSE_AREA_Y2 1030			//
//-----------------------------------------------------------------------------------------------座標関係-------------------------------------------------------------------------------------------------

//描画初期座標
//初期座標 左上X座標
#define INIT_AREA_X1 0
//初期座標 右下X座標
#define INIT_AREA_X2 1920
//初期座標 左上Y座標
#define INIT_AREA_Y1 0
//初期座標 右下Y座標
#define INIT_AREA_Y2 1080

//タイトル画面
//タイトル画面背景表示左上X座標
#define TITLE_BACKIMAGE_POS_X -40
//タイトル画面背景表示左上Y座標
#define TITLE_BACKIMAGE_POS_Y -120
//タイトル表示左上X座標
#define TITLE_POS_X 360
//タイトル表示左上Y座標
#define TITLE_POS_Y 200
//回転するマーク表示左上X座標
#define MARK_POS_X 960
//回転するマーク表示左上Y座標
#define MARK_POS_Y 500
//LOGIN表示左上X座標
#define LOGIN_POS_X 760
//LOGIN表示左上Y座標
#define LOGIN_POS_Y 640
//KEYCONFIG表示左上X座標
#define KEYCONFIG_POS_X 810
//KEYCONFIG表示左上Y座標
#define	KEYCONFIG_POS_Y 800
//EXIT表示左上X座標
#define EXIT_POS_X 910
//EXIT表示左上Y座標
#define	EXIT_POS_Y 900
//LOGIN選択カーソル表示X座標
#define CORSOL_POINT_X1 LOGIN_POS_X-75		//カーソルの場所設定
//LOGIN選択カーソル表示Y座標				//
#define CORSOL_POINT_Y1 LOGIN_POS_Y+25		//-75を文字座標から引いて求める
//KEYCONFIG選択カーソル表示X座標			//
#define CORSOL_POINT_X2 KEYCONFIG_POS_X-75	//
//KEYCONFIG選択カーソル表示Y座標			//
#define CORSOL_POINT_Y2 KEYCONFIG_POS_Y		//+25文字をしてLOGINの文字の中心へ
//EXIT選択カーソル表示X座標					//
#define CORSOL_POINT_X3 EXIT_POS_X-75		//Y座標を少しずらす
//EXIT選択カーソル表示Y座標					//
#define CORSOL_POINT_Y3 EXIT_POS_Y			//


////ゲームメイン画面
//メニューUI始まりのX座標
#define B_MENU_AXIS_X 1300
//メニューUI始まりのY座標
#define B_MENU_AXIS_Y 1050



//-----------------------------------------------------------------------------------------------ゲームメイン ----------------------------------------------------------------------------------------------

#define KEY_REFRESH_COUNT 2//移動キー長押しした場合の動くスピード


//-----------------------------------------------------------------------------------------------ステージ関係-------------------------------------------------------------------------------------------------

//オブジェクトの数
#define OBJECT_NUM 20
//ステージで使用するオブジェクトの最大数
#define OBJECT_DRAW_MAX_NUM 256

//シャドウマップ描画位置
#define SHADOWMAP_INIT1 VGet(-1000.0f,-1.0f,-1000.0f)
#define SHADOWMAP_INIT2 VGet(1000.0f,1000.0f,1000.0f)

//ステージ描画位置
#define STAGE_INIT VGet(600.0f,255.0f,-400.0f)

// ディレクショナルライトの方向
#define DIRLIGHT_DIRECTION_X		( 0.57735026f)
#define DIRLIGHT_DIRECTION_Y		(-0.57735026f)
#define DIRLIGHT_DIRECTION_Z		( 0.57735026f)

// 空用３Ｄモデル描画用のカメラの手前クリップ距離
#define SKYMODEL_CAMERA_NEAR_Z		(10.0f)

// 空用３Ｄモデル描画用のカメラの奥クリップ距離
#define SKYMODEL_CAMERA_FAR_Z		(100000.0f)

// ポイントライトの光が届く距離
#define POINTLIGHT_RANGE			(8000.0f)

//---------------------------------------------------------------------------------------------マップチップ関係---------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------その他--------------------------------------------------------------------------------------------------
