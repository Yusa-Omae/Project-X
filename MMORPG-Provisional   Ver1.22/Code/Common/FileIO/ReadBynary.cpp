/*
				ファイル名		:WriteBynary.cpp
				作成者			:
				作成日時		:2018/01/18
				ソース説明		:
				
				
				備考
				
				
				
!*/

//fopneエラー回避
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "DxLib.h"
#include "ReadBynary.h"



ReadBynary::ReadBynary() : FileIO() {
	mValidSize = 0;
}

bool ReadBynary::Open(const char* fileName, ...) {

	va_list vlist;
	char path[256];

	va_start(vlist, fileName);
	vsprintfDx(path,fileName,vlist);
	va_end(vlist);

	mFilePointer = fopen(path,"rb");

	if (mFilePointer == NULL) {
		return false;
	}

	//一時バイナリバッファ中の次に読み込むべきアドレスを初期化
	mAddress = 0;

	// 一時バッファ中の有効なデータサイズを初期化する
	mValidSize = 0;

	return true;

}

bool ReadBynary::Close() {

	fclose(mFilePointer);

	mFilePointer = NULL;
	mAddress = 0;
	mValidSize = 0;

	return true;
}


/*
データの書き込みを行う
const void* data	:書き込むデータが格納されているメモリアドレス
int			size	:ファイルに書き出すデータのサイズ
*/
bool ReadBynary::ReadData(const void* data, int size) {

	unsigned char* readPointer = (unsigned char*)data;

	for (int i = 0; i < size; i++) {
		
		//書き込むデータを一時保存するバッファがいっぱいになったらファイルに書き出す
		if (mValidSize == mAddress) {
			mValidSize = fread(mBuffer, 1, FILE_DATA_SIZE, mFilePointer);
			
			//ファイルの終端に来てしまったら失敗
			if (mValidSize <= 0) {
				return false;
			}

			mAddress = 0;
		}

		//一時バッファから1バイト読み取る
		*readPointer = mBuffer[mAddress];
		
		//アドレスを1バイト進める
		readPointer++;
		mAddress++;
	}
	return true;
}

bool ReadBynary::ReadInt(int* paramInt) {
	return ReadData(paramInt, sizeof(int));
}

bool ReadBynary::ReadFloat(float* paramFloat) {
	return ReadData(paramFloat, sizeof(float));
}

bool ReadBynary::ReadDouble(double* paramDouble) {
	return ReadData(paramDouble, sizeof(double));
}

bool ReadBynary::ReadBool(bool* paramBool) {
	return ReadData(paramBool, sizeof(bool));
}

bool ReadBynary::ReadChar(char* paramChr) {
	return ReadData(paramChr, sizeof(char));
}

bool ReadBynary::ReadString(char* paramString) {

	size_t length = 0;

	//文字列の長さを読み込む
	if (ReadData(&length, sizeof(size_t)) == false) {
		return false;
	}

	//文字列の読み込み
	return ReadData(paramString, length + 1);
}

bool ReadBynary::ReadUChar(unsigned char* paramUChar) {
	return ReadData(paramUChar, sizeof(unsigned char));
}

bool ReadBynary::ReadUInt(unsigned int* paramUInt) {
	return ReadData(paramUInt, sizeof(unsigned int));
}

bool ReadBynary::ReadVector(VECTOR *paramVector) {
	return ReadData(paramVector, sizeof(VECTOR));
}