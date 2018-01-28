/*
				ファイル名		:WindowBase.h
				作成者			:Syara
				作成日時		:2018/01/27
				ソース説明		:
				
					ウィンドウ基底
				
				備考
				
				
				
!*/

#ifndef __WINDOW_BASE_H__
#define __WINDOW_BASE_H__


typedef struct {

	int drawPosX;	//秒が座標
	int drawPosY;
	int width;		//ウィンドウサイズ
	int height;

}WINDOW_BASE_t;


/*
ウィンドウ初期化
*/
void WindowBase_Initialize(WINDOW_BASE_t* win, int posX, int posY, int width, int height);

/*
ウィンドウ描画
WINDOW_BASE_t win		:ウィンドウサイズ
int apha				:アルファ値
unsigned int color1		:ウィンドウカラー
unsigned int color2		:ウィンドウの枠の色
*/
void WindowBase_Draw(WINDOW_BASE_t win, int alpha, unsigned int color1, unsigned int color2);
/*
ウィンドウ描画
WINDOW_BASE_t win	:ウィンドウサイズ
int graphicHandle	:画像ハンドル
int alpha = 255		:アルファ値(デフォルトは255)
*/
void WindowBase_DrawGraph(WINDOW_BASE_t win, int graphicHandle, int alpha = 255);


/*
ウィンドウの中にあるか判断する
WINDOW_BASE_t win
int posX			:座標
int posY			:
return	true	:ウィンドウの中にある
false	:ウィンドウの中にない
*/
bool WindowBase_IsInSide(WINDOW_BASE_t win, int posX, int posY);

#endif // __WINDOW_BASE_H__