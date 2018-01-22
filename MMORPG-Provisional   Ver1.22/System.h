#pragma once

void Set_Exit_Game();
bool Get_Exit_Game();
void SystemInitialize();
void SystemMain();
//void Fade_io(int Color,int Fade_Flg);


enum eCol{
	eCol_White,
	eCol_EmeraldGreen,
	eCol_Black,
	eCol_Max,
};

//カラーテーブル0:白1:エメラルドグリーン2:黒
const int Color_ValTbl[eCol_Max] = {
	GetColor(255,255,255),
	GetColor(0,255,127),
	GetColor(0,0,0)
};

//システム系統の構造体
struct _SYSTEM_INFO{

	//ゲームを終了するかどうか
	bool Exit_Game;
	//全体のフェードアウトで使用するカウント変数
	int Fade_Out_Count;
	//Windowのハンドル
	HWND WinHndl;
	//Windowの座標値
	LPRECT WinAxis;
	//Windowのタイトル
	char WinTitle;

};