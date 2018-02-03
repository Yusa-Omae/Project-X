/*
				ファイル名		:ScrollWindow.cpp
				作成者			:Syara
				作成日時		:201/01/28
				ソース説明		:
				
					スクロール機能付きウィンドウ
				
				備考
				
				
				
!*/
#include "DxLib.h"

#include "../Code/AppData/Item/ItemData.h"
#include "ScrollWindow.h"
#include "../System.h"

#include <math.h>

void ScrollWindow_Initialize(SCROLL_WINDOW_DATA_t* window,int drawX,int drawY,int windowWidth,int windowHeigh,int scrollSize) {

	WindowBase_Initialize(&window->window, drawX, drawY, windowWidth, windowHeigh);
	Scrollbar_Initialize(&window->scrollbar, scrollSize, ItemData_GetItemDataNum());
	window->alpha = 255;
	window->windowColor = GetColor(0, 255, 255);
	window->frameColor = GetColor(0, 255, 0);



	int winHeigh = window->scrollbar.width / ITEM_WINDOW_NUM-5;
	int posY = drawY + (GAME_SCREEN_HEIGHT - window->scrollbar.width);
	for (int i = 0; i < ITEM_WINDOW_NUM; i++) {
		int x = drawX + 5;
		int y = (i * winHeigh) + posY + 5;
		int width = x + windowWidth - 75;
		int height = winHeigh - 5;
		WindowBase_Initialize(&window->itemWindow[i], x, y, width, height);
	}

	/*
	アイテムデータ一覧
	*/
#if false
	int itemNum = ItemData_GetItemDataNum();
	ITEM_PARAM_DATA_t itemData;
	for (int i = 0; i < itemNum; i++) {
		ItemData_GetItemData(i, &itemData);
		DrawString(20, 60 + i * 20, itemData.name, GetColor(255, 255, 255));
	}
#endif

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
		scrollbarDrawY = window.window.drawPosY + (GAME_SCREEN_HEIGHT - window.window.height);
		Scrollbar_DrawVertical(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	case eScrollWindow_ScrollbarHorizontal:
		
		scrollbarDrawY = window.window.drawPosX;
		scrollbarDrawY = window.window.drawPosY + window.window.width - 40;
		Scrollbar_DrawHorizontal(window.scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	}	
#if false
	ITEM_PARAM_DATA_t itemData;
	for (int i = 0; i < window.scrollbar.valueMax; i++) {
		
		
		int drawItem = i + (int)window.scrollbar.nowValue;
		if (drawItem >= 0 && drawItem < window.scrollbar.valueMax) {
			
			int alpha = 100;
			if (drawItem == ((int)window.scrollbar.nowValue)) {
				alpha = 255;
			}


			WindowBase_Draw(window.itemWindow[i], alpha, GetColor(255, 255, 0), GetColor(255, 255, 255));

			if (drawItem < ItemData_GetItemDataNum()) {
				ItemData_GetItemData(drawItem, &itemData);
				DrawString(20, 60 + i * 181, itemData.name, GetColor(0, 0, 0));
			}
		}
		
	}
#endif
}

/*
	スクロール機能付きウィンドウ描画
*/
void ScrollWindow_DrawGraph(SCROLL_WINDOW_DATA_t* window,int graphichandle,int scrollbarType,float time/* = -1.0f*/) {

	WindowBase_DrawGraph(window->window, graphichandle, window->alpha);

	int scrollbarDrawX = 0;
	int scrollbarDrawY = 0;
	

	switch (scrollbarType) {
	case eScrollWindow_ScrollbarVertical:
		scrollbarDrawX = window->window.drawPosX + window->window.width - 40;
		scrollbarDrawY = window->window.drawPosY + (GAME_SCREEN_HEIGHT - window->scrollbar.width);
		Scrollbar_DrawVertical(window->scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	case eScrollWindow_ScrollbarHorizontal:

		scrollbarDrawY = window->window.drawPosX;
		scrollbarDrawY = window->window.drawPosY + window->window.width - 40;
		Scrollbar_DrawHorizontal(window->scrollbar, scrollbarDrawX, scrollbarDrawY);
		break;
	}

	for (int i = 0; i < ITEM_WINDOW_NUM; i++) {
		WindowBase_SetPosition(&window->itemWindow[i], window->window.drawPosX, window->itemWindow[i].drawPosY);
	}

	ITEM_PARAM_DATA_t itemData;
	for (int i = 0; i < ITEM_WINDOW_NUM; i++) {


		int drawSelectItem = (int)window->scrollbar.nowValue;
		int drawItem = i;


		if ((int)window->scrollbar.nowValue > window->scrollbar.valueMax - (ITEM_WINDOW_NUM / 2)) {
			drawSelectItem = ITEM_WINDOW_NUM - (int)(window->scrollbar.valueMax - window->scrollbar.nowValue);
			drawItem = (i + window->scrollbar.valueMax) - (ITEM_WINDOW_NUM);
		}
		else if ((int)window->scrollbar.nowValue < ITEM_WINDOW_NUM / 2) {
			drawSelectItem = (int)window->scrollbar.nowValue;
			drawItem = i;
		}
		else {
			drawSelectItem = ITEM_WINDOW_NUM / 2;
			drawItem = (i + (int)window->scrollbar.nowValue) - (ITEM_WINDOW_NUM / 2);
		}
		

		char str[256];
		sprintf_s(str, "テスト%d", drawItem);
		if (drawSelectItem >= 0 && drawSelectItem < window->scrollbar.valueMax) {
			const int* imaage = (const int*)(window->imageHandles);
			int alpha = 100;
			
			if (drawSelectItem == i) {
				if (time <= -1) {
					alpha = 255;
				}
				else {
					alpha = (1.0f + sin(DX_PI_F * 2.0f/60 * time) / 2.0f)  *256;
				}
			}

			

			WindowBase_Draw(window->itemWindow[i], alpha, GetColor(255, 255, 0), GetColor(255, 255, 255));

			if (drawSelectItem < ItemData_GetItemDataNum()) {
				//ItemData_GetItemData(drawItem, &itemData);
				int DrawPosX = window->itemWindow[i].drawPosX + 5;
				int DrawPosY = window->itemWindow[i].drawPosY + 2;
				//DrawString(20, 60 + i * 181, itemData.name, GetColor(0, 0, 0));
				DrawGraph(DrawPosX, DrawPosY, imaage[drawItem], TRUE);
				DrawPosY = window->itemWindow[i].drawPosY + window->itemWindow[i].height / 2;
				DrawString(DrawPosX + 53, DrawPosY, str, GetColor(0, 0, 0));
			}
		}

	}
}

/*
	ウィンドウの描画座標設定
*/
void ScrollWindow_SetWindowPosition(SCROLL_WINDOW_DATA_t* window,int drawPosX,int drawPosY) {
	
	WindowBase_SetPosition(&window->window, drawPosX, drawPosY);

}

/*
	ウィンドウの描画座標取得
*/
void ScrollWindow_GetWindowPosition(SCROLL_WINDOW_DATA_t* window, int* drawPosX, int* drawPosY) {
	 *drawPosX = window->window.drawPosX;
	 *drawPosY = window->window.drawPosY;

}

/*
	スクロールバーをスクロールする
	SCROLL_WINDOW_DATA_t* window
	int value						:設定した値だけスクロールする
*/
void ScrollWindow_Scroll(SCROLL_WINDOW_DATA_t* window,int value) {
	Scrollbar_AddValue(&window->scrollbar, value);
}

void ScrolWindow_SetValue(SCROLL_WINDOW_DATA_t* window, int value) {
	Scrollbar_SetValue(&window->scrollbar, value);
}

int ScrollWindow_GetValue(SCROLL_WINDOW_DATA_t window) {
	return window.scrollbar.nowValue;
}

/*
	値を返却する
*/
int ScrollWindow_GetValue(SCROLL_WINDOW_DATA_t window,int posX,int posY) {
	
	int result = -1;

	for (int i = 0; i < ITEM_WINDOW_NUM; i++) {
	
		if (WindowBase_IsInSide(window.itemWindow[i], posX, posY) == true) {

			int drawItem = i + (int)window.scrollbar.nowValue;
			if (drawItem >= 0 && drawItem < window.scrollbar.valueMax) {
				result = drawItem;
				break;
			}
		}
	}
	return result;
}


/*
画像を設定する
*/
void ScrollWindow_SetImageHndles(SCROLL_WINDOW_DATA_t* window,void* imageHandles, int imageHandleNum) {
	window->imageHandles = imageHandles;
	window->imageHandleNum = imageHandleNum;
}