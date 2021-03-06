﻿/*
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
#define ITEM_NAME_FILE_NAME ("Data/ItemData/Item_Char.csv")


enum eData {

	eData_Id,
	eData_Level,
	eData_Price,
	eData_Durable,
	eData_Health,
	eData_Attack,
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

enum eNameData {
	eNameData_Id,
	eNameData_Name,
	eNameData_Description,
	eNameData_Num,
};

static ITEM_PARAM_DATA_t s_ItemParam[ITEM_PARAM_DATA_NUM];

static bool Item_ReadImteName() {

	FILE *fp;

	fopen_s(&fp, ITEM_NAME_FILE_NAME, "r");

	if (fp == NULL) {
		printfDx("ファイルの読み込みに失敗しました.(%s)\n", FILE_NAME);
		return false;
	}

	int num = 0;
	while (1) {

		if (num >= ITEM_PARAM_DATA_NUM) break;

		//ファイル読み込み処理
		char buffer[256] = "";
		fgets(buffer, 256, fp);

		//データがなければ終了する
		if (buffer[0] == EOF) break;

		//一文字目がスラッシュまたは改行なら読み飛ばす
		if (buffer[0] == '/' || buffer[0] == '\n' ||
			buffer[0] == ' ') continue;

		ITEM_PARAM_DATA_t* item = &s_ItemParam[num];

		int pos = 0;
		int continueNum = 0;
		int input = 0;
		char str[256] = "";
		for (int i = 0; i < 256; i++) {

			char ch = buffer[i];

			if (ch == '\0') break;
			if (ch != ',' && ch != '\n') {
				str[input] = ch;
				input++;
				continue;
			}
			else if (ch == ',' || ch == '\n') {
				str[input] = '\0';
				input = 0;
			}

			switch (pos) {
			case eNameData_Id:
				//item->Level = atoi(&ch);
				break;
			case eNameData_Name:
				strcpy(item->name, str);
				break;
			case eNameData_Description:
				strcpy(item->Description, str);
				break;
			}

			pos++;
			continueNum = 0;

			if (pos == eNameData_Num) {
				break;
			}

		}

		num++;
	}

	fclose(fp);
	return true;
}

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
		int input = 0;
		char str[256] = "";
		for (int i = 0; i < 256; i++) {

			char ch = buffer[i];

			if (ch == '\0') break;
			if (ch != ',' && ch != '\n') {
				str[input] = ch;
				input++;
				continue;
			}
			else if (ch == ',' || ch == '\n') {
				str[input] = '\0';
				input = 0;
			}

			switch (pos) {
			case eData_Id:
				item->id = atoi(str);
				break;
			case eData_Level:
				item->Level = atoi(str);
				break;
			case eData_Price:
				item->Price = atoi(str);
				break;
			case eData_Durable:
				item->Durable = atoi(str);
				break;
			case eData_Health:
				item->Health = atoi(str);
				break;
			case eData_Attack:
				item->Attack = atoi(str);
				break;
			case eData_Def:
				item->Def = atoi(str);
				break;
			case eData_Critical:
				item->Critical = atof(str);
				break;
			case eData_Spd:
				item->Spd = atof(str);
				break;
			case eData_Absorption:
				item->Absorption = atof(str);
				break;
			case eData_AutoHeal:
				item->AutoHeal = atoi(str);
				break;
			case eData_GoldPrice:
				item->GoldPrice = atof(str);
				break;
			case eData_Evol1:
				item->Evol[0] = atoi(str);
				break;
			case eData_Evol2:
				item->Evol[1] = atoi(str);
				break;
			case eData_Evol3:
				item->Evol[2] = atoi(str);
				break;
			case eData_Evol4:
				item->Evol[3] = atoi(str);
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

	return Item_ReadImteName();
}

/*
	アイテムデータを返却
*/
void ItemData_GetItemData(int idx, ITEM_PARAM_DATA_t* item) {
	
	if (idx < 0 || idx >= ItemData_GetItemDataNum()) {
		
#ifdef __MY_DEBUG__
		printfDx("アイテムデータの範囲外にアクセスしようとしました.\n");
#endif

		return;
	}
	
	*item = s_ItemParam[idx];
}

/*
	アイテムデータの数を返却する
	return	アイテムデータ数
*/
int ItemData_GetItemDataNum() {
	return sizeof(s_ItemParam) / sizeof(s_ItemParam[0]);
}