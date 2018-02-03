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

#define ITEM_WINDOW_NUM (10)					//画面上に表示させるアイテムウィンドウの数

enum eScrollWindow {
	eScrollWindow_ScrollbarVertical,		//縦向きスクロールバー
	eScrollWindow_ScrollbarHorizontal,		//横向きスクロールバー
};

typedef struct {
	SCROLLBAR_DATA_t scrollbar;
	WINDOW_BASE_t window;
	WINDOW_BASE_t itemWindow[ITEM_WINDOW_NUM];
	int alpha;					//アルファ値
	unsigned int windowColor;	//ウィンドウの色
	unsigned int frameColor;	//枠の色
	void* imageHandles;			//イメージハンドル
	int imageHandleNum;			//イメージハンドルの数
}SCROLL_WINDOW_DATA_t;


void ScrollWindow_Initialize(SCROLL_WINDOW_DATA_t* window, int drawX, int drawY, int windowWidth, int windowHeigh, int scrollSize);

/*
スクロール機能付きウィンドウ描画
*/
void ScrollWindow_Draw(SCROLL_WINDOW_DATA_t window, int scrollbarType);
/*
スクロール機能付きウィンドウ描画
*/
void ScrollWindow_DrawGraph(SCROLL_WINDOW_DATA_t* window, int graphichandle, int scrollbarType, float time = -1.0f);
/*
ウィンドウの描画座標設定
*/
void ScrollWindow_SetWindowPosition(SCROLL_WINDOW_DATA_t* window, int drawPosX, int drawPosY);

/*
ウィンドウの描画座標取得
*/
void ScrollWindow_GetWindowPosition(SCROLL_WINDOW_DATA_t* window, int* drawPosX, int* drawPosY);

/*
スクロールバーをスクロールする
SCROLL_WINDOW_DATA_t* window
int value						:設定した値だけスクロールする
*/
void ScrollWindow_Scroll(SCROLL_WINDOW_DATA_t* window, int value);

/*
値を返却する
*/
int ScrollWindow_GetValue(SCROLL_WINDOW_DATA_t window, int posX, int posY);
int ScrollWindow_GetValue(SCROLL_WINDOW_DATA_t window);

void ScrolWindow_SetValue(SCROLL_WINDOW_DATA_t* window, int value);

/*
	画像を設定する
*/
void ScrollWindow_SetImageHndles(SCROLL_WINDOW_DATA_t* window,void* imageHandles, int imageHandleNum);


#endif // __SCROLL_WINDOW_H__