/*
				ファイル名		:SaveData.cpp
				作成者			:Syara
				作成日時		:2018/01/23
				ソース説明		:
					
					セーブデータ
				
				備考
				
				
				
!*/
#include "DxLib.h"
#include "SaveData.h"
#include "../Code/Common/FileIO/WriteBynary.h"
#include "../Code/Common/FileIO/ReadBynary.h"

/*
	セーブデータが存在しているか判断する
	return	true	:存在している
			false	:存在していない
*/
bool SaveData_Exits(const char* fileName, ...) {

	char path[256];

	va_list vaList;

	va_start(vaList, fileName);
	vsprintfDx(path, fileName, vaList);
	va_end(vaList);
	
	ReadBynary file;
	return file.Exist(path);
}

/*
	ファイルの削除を行う
	return	true	:削除成功
			false	:削除失敗
*/
bool SaveData_Remove(const char* fileName, ...) {

	char path[256];

	va_list vaList;

	va_start(vaList, fileName);
	vsprintfDx(path, fileName, vaList);
	va_end(vaList);

	ReadBynary file;
	return file.Remove(path);
}