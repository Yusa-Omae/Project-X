#pragma once

void TitleInitialize();
void Titleupdate();
void TitleDraw();
void TitleFainalize();


enum eImg{
	eImg_Back,
	eImg_Back2,
	eImg_BackScroll,
	eImg_Exit,
	eImg_Login,
	eImg_KeyConfig,
	eImg_TitleLogo,
	eImg_Corsol,
	eImg_Mark,
	eImg_Max,
};

struct _CORSOL{
	int PosX;
	int PosY;
	int Cnt;
	int AlphaCnt;
	int Alpha;
	bool Alpha_flg;
	int flg;
};

struct _TITLE_DATA{
	_CORSOL CORSOL[3];
	int img[eImg_Max];
	int Blend_Cnt;
	bool Count_Switch_flg;
	int Mark_Count;
	int choice_flg;
	int Fadeout_Cnt;
};
