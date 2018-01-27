/*
				ファイル名		:Scrollbar.cpp
				作成者			:Syara
				作成日時		:2018/01/27
				ソース説明		:
					
					スクロールバー
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "Scrollbar.h"

#define SCROLLBAR_WIDTH (40)		//スクロールバーの幅 


/*
スクロールバーの初期化

*/
void Scrollbar_Initialize(SCROLLBAR_DATA_t* bar,int width,int maxNum) {
	bar->width = width;
	bar->nowValue = 0.0f;
	bar->valueMax = maxNum;
}

/*
縦方向のスクロールバーを描画する
int drawPosX
return	現在の位置を返却する
*/
void Scrollbar_DrawVertical(SCROLLBAR_DATA_t bar,int drawPosX,int drawPosY) {

	if (bar.valueMax == 0) return;

	float drawY = drawPosY + (float)bar.width * (float)(bar.nowValue) / (float)bar.valueMax;
	int Height = drawY + bar.width / bar.valueMax;
	
	//スクロールバー本体
	DrawBox(drawPosX, drawPosY, drawPosX + SCROLLBAR_WIDTH, drawPosY + bar.width, GetColor(255, 255, 255), FALSE);

	//バー
	DrawBox(drawPosX, drawY, drawPosX + SCROLLBAR_WIDTH, Height, GetColor(255, 255, 255), TRUE);

}


/*
横方向のスクロールバーを描画する
int DrawPosY
return	現在の位置を返却する
*/
void Scrollbar_DrawHorizontal(SCROLLBAR_DATA_t bar, int drawPosX, int drawPosY) {
	
	if (bar.valueMax == 0) return;

	float drawX = drawPosX + (float)bar.width * (float)(bar.nowValue) / (float)bar.valueMax;
	int Width = drawX + bar.width / bar.valueMax;

	//スクロールバー本体
	DrawBox(drawPosX, drawPosY , drawPosX + bar.width, drawPosY + SCROLLBAR_WIDTH, GetColor(255, 255, 255), FALSE);

	//バー
	DrawBox(drawX, drawPosY, Width,drawPosY + SCROLLBAR_WIDTH, GetColor(255, 255, 255), TRUE);
}

void Scrollbar_AddValue(SCROLLBAR_DATA_t* bar,int value) {
	bar->nowValue += value;
	if (bar->nowValue < 0) {
		bar->nowValue = 0;
	}
	else if (bar->nowValue >= bar->valueMax) {
		bar->nowValue = bar->valueMax - 1;
	}
}

/*
現在指している要素を返却する
*/
int Scrollbar_GetValue(SCROLLBAR_DATA_t bar) {
	return (int)bar.nowValue;
}

void Scrollbar_SetValue(SCROLLBAR_DATA_t* bar,int value) {
	
}