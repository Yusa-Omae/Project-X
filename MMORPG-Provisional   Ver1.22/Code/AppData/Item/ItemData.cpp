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

#define FILE_NAME ("Data/ItemData/ItemData.csv")
#define ITEM_PARAM_DATA_NUM (38)

enum eData {
	
	eData_Level,
	eData_Price,
	eData_Durable,
	eData_Hp,
	eData_Atk,
	eData_Def,
	eData_Critical,
	eData_Spd,
	eData_Absorption,
	eData_AutoHeal,
	eData_GoldPrice,
	eData_Evol1,
	eData_Evol2,
	eData_Evol3,
	eData_Evol4,
	eData_Num,
};

static const ITEM_DATA_t s_cItemTbl[] = {
	//ID　アイテム名				説明													//アイテム効果				効果量	持てる数
	{ 0,"移動速度アップ",			"移動速度を少し速くします.",							eItemType_MoveSpeed_Up,		1.25f,	20},
	{ 1,"防御力アップ",				"防御力を気持ち上昇させます.",							eItemType_Deffence_Up ,		1.115f,	99},
	{ 1,"クリティカルヒット率アップ","クリティカルヒットが起きる確率を0.15%増加します.",	eItemType_CriticalHit_Up,	1.15f,	30},
	{ 1,"No Empty1","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
	{ 1,"No Empty2","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
	{ 1,"No Empty3","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
	{ 1,"No Empty4","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
	{ 1,"No Empty5","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
	{ 1,"No Empty6","No Empty.",	eItemType_CriticalHit_Up,	1.15f,	30 },
};

static ITEM_PARAM_DATA_t s_ItemParam[ITEM_PARAM_DATA_NUM];

/*
	データファイルを読み込む
	return	true	:読み込み成功
			false	:読み込み失敗
*/
bool ItemData_ReadData() {

	FILE *fp;

	fopen_s(&fp, FILE_NAME, "r");

	if (fp == NULL) {
		printfDx("ファイルの読み込みに失敗しました.(%s)\n",FILE_NAME);
		return false;
	}

	int num = 0;
	while (1) {

		if (num >= ITEM_PARAM_DATA_NUM) break;

		//ファイル読み込み処理
		char buffer[256]="";
		fgets(buffer, 256, fp);

		//データがなければ終了する
		if (buffer[0] == EOF) break;

		//一文字目がスラッシュまたは改行なら読み飛ばす
		if (buffer[0] == '/' || buffer[0] == '\n' || 
			buffer[0] == ' ') continue;

		ITEM_PARAM_DATA_t* item = &s_ItemParam[num];

		int pos = 0;
		int continueNum = 0;
		for (int i = 0; i < 256; i++) {

			char ch = buffer[i];

			if (ch == '\0') break;
			if (ch == ',') {
				continueNum++;
				if (continueNum >= 2) {
					printfDx("ファイルのデータが不正です.\n");
					fclose(fp);
					return false;
				}
				continue;
			}

			switch (pos) {
			case eData_Level:
				item->Level = atoi(&ch);
				break;
			case eData_Price:
				item->Price = atoi(&ch);
				break;
			case eData_Durable:
				item->Durable = atoi(&ch);
				break;
			case eData_Hp:
				item->Hp = atoi(&ch);
				break;
			case eData_Atk:
				item->Atk = atoi(&ch);
				break;
			case eData_Def:
				item->Def = atoi(&ch);
				break;
			case eData_Critical:
				item->Critical = atof(&ch);
				break;
			case eData_Spd:
				item->Spd = atof(&ch);
				break;
			case eData_Absorption:
				item->Absorption = atof(&ch);
				break;
			case eData_AutoHeal:
				item->AutoHeal = atoi(&ch);
				break;
			case eData_GoldPrice:
				item->GoldPrice = atof(&ch);
				break;
			case eData_Evol1:
				item->Evol[0] = atoi(&ch);
				break;
			case eData_Evol2:
				item->Evol[1] = atoi(&ch);
				break;
			case eData_Evol3:
				item->Evol[2] = atoi(&ch);
				break;
			case eData_Evol4:
				item->Evol[3] = atoi(&ch);
				break;
			}

			pos++;
			continueNum = 0;

			if (pos == eData_Num) {
				break;
			}

		}


		num++;

	}



	fclose(fp);

	return true;
}

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