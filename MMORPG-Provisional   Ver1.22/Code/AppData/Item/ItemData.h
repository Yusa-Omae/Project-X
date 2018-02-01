/*
				ファイル名		:ItemData.cpp
				作成者			:Syara
				作成日時		:2018/01/24
				ソース説明		:
					
					アイテムデータ
				
				備考
				
				
				
!*/

#ifndef __ITEM_DATA_H__
#define __ITEM_DATA_H__

typedef struct _ITEM_PARAM {

	int		Level;
	int		Price;
	int		Durable;
	int		Hp ;
	int		Atk;
	int		Def;
	float	Critical;
	float	Spd;
	float	Absorption;
	int		AutoHeal;
	float	GoldPrice;
	int		Evol[5];

}ITEM_PARAM_DATA_t;

/*
データファイルを読み込む
return	true	:読み込み成功
false	:読み込み失敗
*/
bool ItemData_ReadData();

/*
アイテムデータを返却
*/
void ItemData_GetItemData(int idx, ITEM_PARAM_DATA_t* item);

/*
アイテムデータの数を返却する
return	アイテムデータ数
*/
int ItemData_GetItemDataNum();

#endif // __ITEM_DATA_H__