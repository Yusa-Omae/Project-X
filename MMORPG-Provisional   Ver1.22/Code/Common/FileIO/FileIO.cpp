/*
				ファイル名		:FileIO.cpp
				作成者			:
				作成日時		:2018/01/17
				ソース説明		:
				
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "FileIO.h"
#include <sys/stat.h>

FileIO::FileIO() {
	mFilePointer = NULL;
	mAddress = 0;
	memset(mBuffer, 0, sizeof(unsigned char) * FILE_DATA_SIZE);
}


FileIO::~FileIO() {
	mFilePointer = NULL;
	mAddress = 0;
	memset(mBuffer, 0, sizeof(unsigned char) * FILE_DATA_SIZE);
}


/*
	ファイルの有無を調べる
	return	true	:存在している
			false	:存在していない
*/
bool FileIO::Exist(const char* fileName, ...) {

	va_list vlist;
	char path[256];

	bool result = false;

	va_start(vlist, fileName);
	vsprintfDx(path, fileName, vlist);
	va_end(vlist);

	struct stat statBuf;

	if (stat(path, &statBuf) == 0) {
		result = true;
	}

	return result;
}

/*
	ファイルの削除を行う
	return	true	:成功
			false	:失敗
*/
bool FileIO::Remove(const char* fileName, ...) {

	va_list vlist;
	char path[256];

	bool result = false;

	va_start(vlist, fileName);
	vsprintfDx(path, fileName, vlist);
	va_end(vlist);

	if (this->Exist(path) == true) {
		if (remove(path) == 0) {
			result = true;
		}
	}

	return result;
}