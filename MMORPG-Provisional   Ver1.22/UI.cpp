#if false
#include"DxLib.h"
#include"System.h"
#include"Define.h"
#include"UI.h"


//UIclassで使う構造体の定義
_UI_INFO UI_INFO;
_MENU_INFO MENU_INFO;

//UIデータの初期化処理
void UI_Initialize(){

	//各種初期化処理
	MENU_INFO.flg = 1;
	MENU_INFO.Move_Cnt = 0;
	MENU_INFO.Move_Flg = true;
	MENU_INFO.Back_Move_Flg = MENU_INFO.Move_Flg;

	//メニューフラグの初期化
	MENU_INFO.flg = 0;

	//メニュー画面の場所初期化処理
	for (int i = 0; i > 6;i ++) {
		MENU_INFO.MENU_AXIS[i].X = B_MENU_AXIS_X +i*50;
		MENU_INFO.MENU_AXIS[i].Y = B_MENU_AXIS_Y;

	}


}
//UIの更新処理
void UI_Update(){
	//ゲーム画面状態がノーマルの時(ノーマル、メニュー・・・)
	//メニュー画面移行処理。
	//エスケープまたは何かのメニューキーOR設定(オプション)キーなど
	//＆　左クリック+構造体座標保持、当たり判定(いくつかの画面上のボタン)
	switch(MENU_INFO.flg){
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}

	MENU_INFO.Move_Cnt++;


}
//UIの描画処理
void UI_Draw() {


	switch (MENU_INFO.flg) {
	case 0:
		break;
	case 1:
			for (int i = 0; i > 6; i++) {
				DrawBox(MENU_INFO.MENU_AXIS[i].X, MENU_INFO.MENU_AXIS[i].Y, MENU_INFO.MENU_AXIS[i].X + (i + 1) * 49, MENU_INFO.MENU_AXIS[i].Y + (i + 1) * 49, Color_ValTbl[eCol_Black], TRUE);
			}
		break;
	case 2:
		break;
	case 3:
		break;
	}

}
//UIの後処理(メモリ開放)
void UI_Fainalize(){



}
#endif