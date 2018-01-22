/*
				ファイル名		:SaveData.h
				作成者			:
				作成日時		:2018/01/23
				ソース説明		:
					
					セーブデータ
				
				備考
				
				
				
!*/

#ifndef __SAVE_DATA_H__
#define __SAVE_DATA_H__

/*
	セーブデータが存在しているか判断する
	return	true	:存在している
			false	:存在していない
*/
bool SaveData_Exits(const char* fileName, ...);

/*
	ファイルの削除を行う
	return	true	:削除成功
			false	:削除失敗
*/
bool SaveData_Remove(const char* fileName, ...);

#endif // __SAVE_DATA_H__