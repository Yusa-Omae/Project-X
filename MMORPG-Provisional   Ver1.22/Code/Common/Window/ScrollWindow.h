/*
				ファイル名		:ScrollWindow.h
				作成者			:Syara
				作成日時		:2018/01828
				ソース説明		:
				
					スクロール機能付きウィンドウ
				
				備考
				
				
				
!*/

#ifndef __SCROLL_WINDOW_H__
#define __SCROLL_WINDOW_H__

#include "Scrollbar.h"
#include "WindowBase.h"

enum eScrollWindow {
	eScrollWindow_ScrollbarVertical,		//縦向きスクロールバー
	eScrollWindow_ScrollbarHorizontal,		//横向きスクロールバー
};

typedef struct {
	SCROLLBAR_DATA_t scrollbar;
	WINDOW_BASE_t window;
	int alpha;					//アルファ値
	unsigned int windowColor;	//ウィンドウの色
	unsigned int frameColor;	//枠の色	
}SCROLL_WINDOW_DATA_t;


void ScrollWindow_Initialize(SCROLL_WINDOW_DATA_t* window, int drawX, int drawY, int windowWidth, int windowHeigh, int scrollSize);

/*
スクロール機能付きウィンドウ描画
*/
void ScrollWindow_Draw(SCROLL_WINDOW_DATA_t window, int scrollbarType);
/*
スクロール機能付きウィンドウ描画
*/
void ScrollWindow_DrawGraph(SCROLL_WINDOW_DATA_t window, int graphichandle, int scrollbarType);
/*
ウィンドウの描画座標設定
*/
void ScrollWindow_SetWindowPosition(SCROLL_WINDOW_DATA_t* window, int drawPosX, int drawPosY);

/*
スクロールバーをスクロールする
SCROLL_WINDOW_DATA_t* window
int value						:設定した値だけスクロールする
*/
void ScrollWindow_Scroll(SCROLL_WINDOW_DATA_t* window, int value);

#endif // __SCROLL_WINDOW_H__