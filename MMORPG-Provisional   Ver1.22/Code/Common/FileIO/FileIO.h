/*
				ファイル名		:FileIO.h
				作成者			:
				作成日時		:2018/01/17
				ソース説明		:
				
				
				備考
				
				
				
!*/

#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#define FILE_DATA_SIZE (1024 * 64)

class FileIO {

protected:

	FILE * mFilePointer;
	unsigned char mBuffer[FILE_DATA_SIZE];
	unsigned int mAddress;

public :

	FileIO();
	virtual ~FileIO();

	virtual bool Open(const char* fileName,...) = 0;
	virtual bool Close() = 0;

	/*
		ファイルの有無を調べる
		return	true	:存在している
				false	:存在していない
	*/
	bool Exist(const char* fileName,...);

	/*
		ファイルの削除を行う
		return	true	:成功
				false	:失敗
	*/
	bool Remove(const char* fileName, ...);

};

#endif // __FILE_IO_H__