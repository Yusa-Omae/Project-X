/*
				ファイル名		:ScrollWindow.cpp
				作成者			:Syara
				作成日時		:201/01/28
				ソース説明		:
				
					スクロール機能付きウィンドウ
				
				備考
				
				
				
!*/
#include "DxLib.h"

#include "ScrollWindow.h"


void ScrollWindow_Initialize(SCROLL_WINDOW_DATA_t* window,int drawX,int drawY,int windowWidth,int windowHeigh,int scrollSize) {

	WindowBase_Initialize(&window->window, drawX, drawY, windowWidth, windowHeigh);
	Scrollbar_Initialize(&window->scrollbar, scrollSize, 100);
	window->alpha = 255;
	window->windowColor = GetColor(0, 255, 255);
	window->frameColor = GetColor(0, 255, 0);
}

/*
	スクロール機能付きウィンドウ描画
*/
void ScrollWindow_Draw(SCROLL_WINDOW_DATA_t window,int scrollbarType) {

	WindowBase_Draw(window.window, window.alpha, window.windowColor, window.frameColor);
	
	int scrollbarDrawX = 0;
	int scrollbarDrawY = 0;

	switch (scrollbarType) {
	case eScrollWindow_ScrollbarVertical:
		scrollbarDrawX = window.window.drawPosX + window.window.width - 40;
		scrollbarDrawY = window.window.drawPosY;
		Scrollbar_DrawVertical(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	case eScrollWindow_ScrollbarHorizontal:
		
		scrollbarDrawY = window.window.drawPosX;
		scrollbarDrawY = window.window.drawPosY + window.window.width - 40;
		Scrollbar_DrawHorizontal(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	}	
	
}

/*
	スクロール機能付きウィンドウ描画
*/
void ScrollWindow_DrawGraph(SCROLL_WINDOW_DATA_t window,int graphichandle,int scrollbarType) {

	WindowBase_DrawGraph(window.window, graphichandle, window.alpha);

	int scrollbarDrawX = 0;
	int scrollbarDrawY = 0;

	switch (scrollbarType) {
	case eScrollWindow_ScrollbarVertical:
		scrollbarDrawX = window.window.drawPosX + window.window.width - 40;
		scrollbarDrawY = window.window.drawPosY;
		Scrollbar_DrawVertical(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	case eScrollWindow_ScrollbarHorizontal:

		scrollbarDrawY = window.window.drawPosX;
		scrollbarDrawY = window.window.drawPosY + window.window.width - 40;
		Scrollbar_DrawHorizontal(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	}

}

/*
	ウィンドウの描画座標設定
*/
void ScrollWindow_SetWindowPosition(SCROLL_WINDOW_DATA_t* window,int drawPosX,int drawPosY) {
	window->window.drawPosX = drawPosX;
	window->window.drawPosY = drawPosY;

}

/*
	スクロールバーをスクロールする
	SCROLL_WINDOW_DATA_t* window
	int value						:設定した値だけスクロールする
*/
void ScrollWindow_Scroll(SCROLL_WINDOW_DATA_t* window,int value) {
	Scrollbar_AddValue(&window->scrollbar, value);
}
