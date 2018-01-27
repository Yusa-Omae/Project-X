/*
				ファイル名		:WindowBase.cpp
				作成者			:Syara
				作成日時		:201/01/27
				ソース説明		:
				
					ウィンドウ基底
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "WindowBase.h"


/*
	ウィンドウ初期化
*/
void WindowBase_Initialize(WINDOW_BASE_t* win,int posX, int posY, int width, int height) {
	win->drawPosX = posX;
	win->drawPosY = posY;
	win->width = width;
	win->height = height;
}

/*
	ウィンドウ描画
	WINDOW_BASE_t win		:ウィンドウサイズ
	int apha				:アルファ値
	unsigned int color1		:ウィンドウカラー
	unsigned int color2		:ウィンドウの枠の色
*/
void WindowBase_Draw(WINDOW_BASE_t win,int alpha ,unsigned int color1,unsigned int color2){

	int drawX = win.drawPosX;
	int drawY = win.drawPosY;
	int width = win.width;
	int height= win.height;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(drawX, drawY, drawX + width, drawY + height, color1, TRUE);
	DrawBox(drawX, drawY, drawX + width, drawY + height, color2, FALSE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

/*
	ウィンドウ描画
	WINDOW_BASE_t win	:ウィンドウサイズ
	int graphicHandle	:画像ハンドル
	int alpha = 255		:アルファ値(デフォルトは255)
*/
void WindowBase_DrawGraph(WINDOW_BASE_t win,int graphicHandle,int alpha/* = 255*/) {
	int drawX = win.drawPosX;
	int drawY = win.drawPosY;
	int width = win.width;
	int height = win.height;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawExtendGraph(drawX, drawY, width, height, graphicHandle,TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}