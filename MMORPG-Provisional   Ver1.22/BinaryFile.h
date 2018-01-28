#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <stdio.h>
#include "DxLib.h"

// ファイルのデータを一時的に保持しておくバッファのサイズ
#define BINARYFILE_BUFFER_SIZE		(64 * 1024)

// バイナリファイルアクセス用情報
typedef struct _SBinaryFileData
{
	// ファイルポインタ
	FILE *        FilePointer;

	// ファイルのデータを一時的に保持しておくためのバッファ
	unsigned char Buffer[ BINARYFILE_BUFFER_SIZE ];

	// Buffer中の有効なデータサイズ( 読み込み時のみ使用 )
	size_t        ValidSize;

	// 次にアクセスすべきBuffer中のアドレス
	int           Address;
} SBinaryFileData;

// 書き込み用にバイナリファイルを開く
//     戻り値 : ファイルを開くとに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool WriteBinFile_Open(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// ファイルパス
	const char *FilePath,

	// ファイルパス生成用可変個引数
	...
);

// 書き込み用に開いたバイナリファイルを閉じる
extern void WriteBinFile_Close(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData
);

// 書き込み用に開いたバイナリファイルに指定メモリアドレスにあるデータを書き込む
extern void WriteBinFile_Data(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むデータが格納されているメモリアドレス
	const void *  ParamData,

	// ファイルに書き出すデータのサイズ
	int DataSize
);

// 書き込み用に開いたバイナリファイルにint型の値を書き込む
extern void WriteBinFile_Int(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むint型の値
	int ParamInt
);

// 書き込み用に開いたバイナリファイルにbool型の値を書き込む
extern void WriteBinFile_Bool(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むbool型の値
	bool ParamBool
);

// 書き込み用に開いたバイナリファイルにfloat型の値を書き込む
extern void WriteBinFile_Float(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData, 
	
	// 書き込むfloat型の値
	float ParamFloat
);

// 書き込み用に開いたバイナリファイルにVECTOR型の値を書き込む
extern void WriteBinFile_Vector(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むVECTOR型の値
	VECTOR ParamVector
);

// 書き込み用に開いたバイナリファイルに文字列をを書き込む
extern void WriteBinFile_String(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込む文字列のアドレス
	const char *ParamString
);

// 書き込み用に開いたバイナリファイルにunsigned char型の値を書き込む
extern void WriteBinFile_UChar(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むunsigned char型の値
	unsigned char ParamUChar
);

// 書き込み用に開いたバイナリファイルにunsigned int型の値を書き込む
extern void WriteBinFile_UInt(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むunsigned int型の値
	unsigned int ParamUInt
);

// 書き込み用に開いたバイナリファイルにCOLOR_U8型の値を書き込む
extern void WriteBinFile_ColorU8(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むCOLOR_U8型の値
	COLOR_U8 ParamColorU8
);


// 読み込み用にバイナリファイルを開く
//     戻り値 : ファイルを開くとに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Open(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// ファイルパス
	const char *FilePath,

	// ファイルパス生成用可変個引数
	...
);

// 読み込み用に開いたバイナリファイルを閉じる
extern void ReadBinFile_Close(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData
);

// 読み込み用に開いたバイナリファイルから指定のメモリアドレスにデータを読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Data(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだデータを書き込むメモリのアドレス
	void *ParamData,

	// 読み込むデータのサイズ
	int DataSize
);

// 読み込み用に開いたバイナリファイルからint型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Int(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだint型の値を書き込むメモリのアドレス
	int *ParamInt
);

// 読み込み用に開いたバイナリファイルからbool型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Bool(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだbool型の値を書き込むメモリのアドレス
	bool *ParamBool
);

// 読み込み用に開いたバイナリファイルからfloat型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Float(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだfloat型の値を書き込むメモリのアドレス
	float *ParamFloat
);

// 読み込み用に開いたバイナリファイルからVECTOR型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_Vector(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだVECTOR型の値を書き込むメモリのアドレス
	VECTOR *ParamVector
);

// 読み込み用に開いたバイナリファイルから文字列を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_String(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだ文字列を書き込むメモリのアドレス
	char *ParamString
);

// 読み込み用に開いたバイナリファイルからunsigned char型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_UChar(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだunsigned char型の値を書き込むメモリのアドレス
	unsigned char *ParamUChar
);

// 読み込み用に開いたバイナリファイルからunsigned int型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_UInt(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだunsigned int型の値を書き込むメモリのアドレス
	unsigned int *ParamUInt
);

// 読み込み用に開いたバイナリファイルからCOLOR_U8型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
extern bool ReadBinFile_ColorU8(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだCOLOR_U8型の値を書き込むメモリのアドレス
	COLOR_U8 *ParamColorU8
);

#endif


