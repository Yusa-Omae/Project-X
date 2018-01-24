/*
				ファイル名		:Mouse.h
				作成者			:Syara
				作成日時		:018/01/24
				ソース説明		:
				
					マウス入力
				
				備考
				
				
				
!*/

#ifndef __MOUSE_H__
#define __MOUSE_H__

enum eMouseInputBotton {
	eMouseInputBotton_None = -1,
	eMouseInputBotton_Left,
	eMouseInputBotton_Rigth,
	eMouseInputBotton_Wheel,
	eMouseInputBotton_Num,
};

/*
	マウス更新処理
*/
void Mouse_Update();

//マウスボタンが入力されている
bool Mouse_On(int keyCode);

//マウスボタンが入力されていない
bool Mouse_Off(int keyCode);

//マウスボタンが押された状態から解放された
bool Mouse_Relese(int keyCode);

//マウスボタンが押され続けている
bool Mouse_Repeat(int keyCode);

//マウスボタンが押された
bool Mouse_Press(int keyCode);

/*
ホイールの値を返却する
*/
int Mouse_WheelValue();
float Mouse_WheelValueF();

/*
マウスの座標を取得する
*/
void Mouse_GetPositioin(int* posX, int* posY);

#endif // __MOUSEE_H__