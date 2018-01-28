/*
				ファイル名		:WriteBynary.h
				作成者			:
				作成日時		:2018/01/18
				ソース説明		:
					バイナリでファイル書き込みを行う
				
				備考
				
				
				
!*/

#ifndef __WRITE_BYNARY_H__
#define __WRITE_BYNARY_H__

#include "FileIO.h"

class WriteBynary : public FileIO {

private:

	/*
		データの書き込みを行う
		const void* data	:書き込むデータが格納されているメモリアドレス
		int			size	:ファイルに書き出すデータのサイズ
	*/
	void WriteData(const void* data, int size);

public :

	WriteBynary();

	/*
		ファイルを開く
		return	true	:ファイルのオープンに成功
				false	:ファイルのオープンに失敗
	*/
	bool Open(const char* fileName, ...) override;
	bool Close() override;

	void WriteInt(int paramInt);
	void WriteFloat(float paramFloat);
	void WriteDouble(double paramDouble);
	void WriteBool(bool paramBool);
	void WriteChar(char paramChr);
	void WriteString(const char* paramString);
	void WriteUChar(unsigned char paramUChar);
	void WriteUInt(unsigned int paramUInt);
	void WriteVector(VECTOR paramVector);
};

#endif // __WRITE_BYNARY_H__