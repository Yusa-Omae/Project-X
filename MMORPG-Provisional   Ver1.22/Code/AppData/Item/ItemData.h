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

#define ITEM_PARAM_DATA_NUM (69)


typedef struct {

	int		id;					//管理番号
	int		Level;				//レベル
	int		Price;				//値段
	int		Durable;			//耐久値
	int		Health ;				//体力
	int		Attack;				//攻撃力
	int		Def;				//防御力
	float	Critical;			//クリティカル率
	float	Spd;				//速度
	float	Absorption;			//吸収
	int		AutoHeal;			//自動回復
	float	GoldPrice;			//取得金額の倍率
	int		Evol[4];			//進化先
	char	name[256];			//アイテム名
	char	Description[1024];	//説明

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