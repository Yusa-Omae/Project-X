/*
				ファイル名		:ReadBynary.h
				作成者			:
				作成日時		:2018/01/18
				ソース説明		:
					バイナリでファイル書き込みを行う
				
				備考
				
				
				
!*/

#ifndef __READ_BYNARY_H__
#define __READ_BYNARY_H__

#include "FileIO.h"

class ReadBynary : public FileIO {

private:

	int mValidSize;	//Buffer中の有効なデータサイズ

private:

	/*
		データの書き込みを行う
		const void* data	:書き込むデータが格納されているメモリアドレス
		int			size	:ファイルに書き出すデータのサイズ
		return	true		:読み込み成功
				false		:読み込み失敗
	*/
	bool ReadData(const void* data, int size);

public :

	ReadBynary();

	/*
		ファイルを開く
		return	true	:ファイルのオープンに成功
				false	:ファイルのオープンに失敗
	*/
	bool Open(const char* fileName, ...) override;
	bool Close() override;

	bool ReadInt(int* paramInt);
	bool ReadFloat(float* paramFloat);
	bool ReadDouble(double* paramDouble);
	bool ReadBool(bool* paramBool);
	bool ReadChar(char* paramChr);
	bool ReadString(char* paramString);
	bool ReadUChar(unsigned char* paramUChar);
	bool ReadUInt(unsigned int* paramUInt);
	bool ReadVector(VECTOR *paramVector);

};

#endif // __READ_BYNARY_H__