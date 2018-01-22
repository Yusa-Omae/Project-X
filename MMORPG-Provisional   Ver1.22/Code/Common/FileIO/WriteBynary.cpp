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
#include "WriteBynary.h"



WriteBynary::WriteBynary() : FileIO() {

}

bool WriteBynary::Open(const char* fileName, ...) {

	va_list vlist;
	char path[256];

	va_start(vlist, fileName);
	vsprintfDx(path,fileName,vlist);
	va_end(vlist);

	mFilePointer = fopen(path,"wb");

	if (mFilePointer == NULL) {
		return false;
	}

	//一時バイナリバッファ中の次に読み込むべきアドレスを初期化
	mAddress = 0;

	return true;

}

bool WriteBynary::Close() {

	//アドレスが0でなければファイルに書き込む
	if (mAddress > 0) {
		fwrite(mBuffer, 1, mAddress, mFilePointer);
	}

	fclose(mFilePointer);

	mFilePointer = NULL;
	mAddress = 0;
	return true;
}


/*
データの書き込みを行う
const void* data	:書き込むデータが格納されているメモリアドレス
int			size	:ファイルに書き出すデータのサイズ
*/
void WriteBynary::WriteData(const void* data, int size) {

	const unsigned char* writePointer = (const unsigned char*)data;

	for (int i = 0; i < size; i++) {
		
		//書き込むデータを一時保存するバッファがいっぱいになったらファイルに書き出す
		if (mAddress == FILE_DATA_SIZE) {
			fwrite(mBuffer, 1, mAddress, mFilePointer);
			mAddress = 0;
		}

		mBuffer[mAddress] = *writePointer;
		
		//アドレスを1倍と進める
		writePointer++;
		mAddress++;
	}


}

void WriteBynary::WriteInt(int paramInt) {
	WriteData(&paramInt, sizeof(int));
}

void WriteBynary::WriteFloat(float paramFloat) {
	WriteData(&paramFloat, sizeof(float));
}

void WriteBynary::WriteDouble(double paramDouble) {
	WriteData(&paramDouble, sizeof(double));
}

void WriteBynary::WriteBool(bool paramBool) {
	WriteData(&paramBool, sizeof(bool));
}

void WriteBynary::WriteChar(char paramChr) {
	WriteData(&paramChr, sizeof(char));
}

void WriteBynary::WriteString(const char* paramString) {

	size_t length = 0;

	length = strlenDx(paramString);
	//文字列の長さを書き込む
	WriteData(&length, sizeof(size_t));

	//文字列の書き込み
	WriteData(paramString, length + 1);

}

void WriteBynary::WriteUChar(unsigned char paramUChar) {
	WriteData(&paramUChar, sizeof(unsigned char));
}

void WriteBynary::WriteUInt(unsigned int paramUInt) {
	WriteData(&paramUInt, sizeof(unsigned int));
}