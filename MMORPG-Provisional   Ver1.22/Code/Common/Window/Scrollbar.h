/*
				ファイル名		:Scrollbar.h
				作成者			:Syara
				作成日時		:2018/01/27
				ソース説明		:
					
					スクロールバー
				
				備考
				
				
				
!*/

#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__


typedef struct {

	int width;		//スクロールバーの幅
	float nowValue;	//現在の要素数を取得する
	int valueMax;	//最大要素数

}SCROLLBAR_DATA_t;

/*
スクロールバーの初期化

*/
void Scrollbar_Initialize(SCROLLBAR_DATA_t* bar, int width, int maxNum);

/*
縦方向のスクロールバーを描画する
int drawPosX
return	現在の位置を返却する
*/
void Scrollbar_DrawVertical(SCROLLBAR_DATA_t bar, int drawPosX, int drawPosY);
/*
横方向のスクロールバーを描画する
int DrawPosY
return	現在の位置を返却する
*/
void Scrollbar_DrawHorizontal(SCROLLBAR_DATA_t bar, int drawPosX, int drawPosY);

void Scrollbar_AddValue(SCROLLBAR_DATA_t* bar, int value);

/*
	現在指している要素を返却する
*/
int Scrollbar_GetValue();

/*
値を設定する
*/
void Scrollbar_SetValue(SCROLLBAR_DATA_t* bar, int value);

#endif // __SCROLLBAR_H__