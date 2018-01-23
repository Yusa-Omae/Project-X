/*
				ファイル名		:ItemData.cpp
				作成者			:Syara
				作成日時		:2018/01/24
				ソース説明		:
				
					アイテムデータ
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "ItemData.h"

#define ITEM_DATA_MAX (10)



typedef struct {
	int select;
}WORK_OBJ_t;


static const ITEM_DATA_t s_cItemTbl[] = {
	//ID　アイテム名				説明													//アイテム効果				効果量	持てる数
	{ 0,"移動速度アップ",			"移動速度を少し速くします.",							eItemType_MoveSpeed_Up,		1.25f,	20},
	{ 1,"防御力アップ",				"防御力を気持ち上昇させます.",							eItemType_Deffence_Up ,		1.115f,	99},
	{ 1,"クリティカルヒット率アップ","クリティカルヒットが起きる確率を0.15%増加します.",	eItemType_CriticalHit_Up,	1.15f,	30},
};


static WORK_OBJ_t s_Work = { 0 };

/*
	アイテムデータを返却
*/
void ItemData_GetItemData(int idx,ITEM_DATA_t* item) {
	
	if (idx < 0 || idx >= ItemData_GetItemDataNum()) {
		
#ifdef __MY_DEBUG__
		printfDx("アイテムデータの範囲外にアクセスしようとしました.\n");
#endif

		return;
	}
	
	*item = s_cItemTbl[idx];
}

/*
	アイテムデータの数を返却する
	return	アイテムデータ数
*/
int ItemData_GetItemDataNum() {
	return sizeof(s_cItemTbl) / sizeof(s_cItemTbl[0]);
}