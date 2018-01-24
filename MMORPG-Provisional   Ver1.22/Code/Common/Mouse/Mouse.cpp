/*
				ファイル名		:Mouse.cpp
				作成者			:Syara
				作成日時		:2018/01/24
				ソース説明		:
				
					マウス入力
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "Mouse.h"

typedef struct {
	eMouseInputBotton button;
	int input;
}MOUSE_BOTTON_DATA_t;

typedef struct {
	int InputBotton[eMouseInputBotton_Num];
	float wheelValue;
	int posX;
	int posY;
}MOUSE_INPUT_DATA_t;

static const MOUSE_BOTTON_DATA_t MOUSE_BUTTON_TBL[eMouseInputBotton_Num] = {
	{ eMouseInputBotton_Left,MOUSE_INPUT_LEFT},
	{ eMouseInputBotton_Rigth,MOUSE_INPUT_RIGHT },
	{ eMouseInputBotton_Wheel,MOUSE_INPUT_MIDDLE },
};

static MOUSE_INPUT_DATA_t s_Mouse;

/*
マウス更新処理
*/
void Mouse_Update() {

	//座標取得
	GetMousePoint(&s_Mouse.posX, &s_Mouse.posY);

	eMouseInputBotton num = eMouseInputBotton_None;
	for (int i = 0; i < eMouseInputBotton_Num; i++) {
		for (int j = 0; j < eMouseInputBotton_Num; j++) {
			if (MOUSE_BUTTON_TBL[j].button == (eMouseInputBotton)i) {
				num = MOUSE_BUTTON_TBL[j].button;
				break;
			}
		}

		if (num != eMouseInputBotton_None) {
			if (GetMouseInput() & MOUSE_BUTTON_TBL[num].input) {
				s_Mouse.InputBotton[num]++;
			}
			else {
				s_Mouse.InputBotton[num] = 0;
			}
		}
	}

	s_Mouse.wheelValue = GetMouseWheelRotVolF();

}

//キー入力されている
bool Mouse_On(int keyCode) {
	return (s_Mouse.InputBotton[keyCode] >= 1);
}


//キー入力されていない
bool Mouse_Off(int keyCode) {
	return (s_Mouse.InputBotton[keyCode] == 0);
}

//キーが押された状態から解放された
bool Mouse_Relese(int keyCode) {
	return false;
}


//キーが押され続けている
bool Mouse_Repeat(int keyCode) {
	return false;
}

//キーが押された
bool Mouse_Press(int keyCode) {
	return (s_Mouse.InputBotton[keyCode] == 1);
}

/*
	ホイールの値を返却する
*/
int Mouse_WheelValue() {
	return (int)(s_Mouse.wheelValue);
}

float Mouse_WheelValueF() {
	return (s_Mouse.wheelValue);
}

/*
	マウスの座標を取得する
*/
void Mouse_GetPositioin(int* posX,int* posY) {
	*posX = s_Mouse.posX;
	*posY = s_Mouse.posY;
}