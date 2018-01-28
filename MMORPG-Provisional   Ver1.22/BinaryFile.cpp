#include "BinaryFile.h"
#include <stdarg.h>

// 書き込み用にバイナリファイルを開く
//     戻り値 : ファイルを開くとに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool WriteBinFile_Open(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// ファイルパス
	const char *FilePath,

	// ファイルパス生成用可変個引数
	...
)
{
	va_list VaList;
	char    FilePathS[ MAX_PATH ];

	// 可変個引数からファイルパスを生成する
	va_start( VaList, FilePath );
	vsprintf( FilePathS, FilePath, VaList );
	va_end( VaList );

	// バイナリタイプの書き込み指定でファイルを開く
	BinFileData->FilePointer = fopen( FilePathS, "wb" );

	// ファイルを開くことに失敗したら失敗終了
	if( BinFileData->FilePointer == NULL )
	{
		return false;
	}

	// 一時バッファ中の次に書き込むべきアドレスを初期化
	BinFileData->Address = 0;

	// 成功終了
	return true;
}

// 書き込み用に開いたバイナリファイルを閉じる
void WriteBinFile_Close(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData
)
{
	// 一時バッファにデータがある場合はファイルに書き出す
	if( BinFileData->Address > 0 )
	{
		fwrite( BinFileData->Buffer, 1, BinFileData->Address, BinFileData->FilePointer );
	}

	// ファイル閉じる
	fclose( BinFileData->FilePointer );

	// ファイルポインタとアドレスを初期化
	BinFileData->FilePointer = NULL;
	BinFileData->Address     = 0;
}

// 書き込み用に開いたバイナリファイルに指定メモリアドレスにあるデータを書き込む
void WriteBinFile_Data(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むデータが格納されているメモリアドレス
	const void *  ParamData,

	// ファイルに書き出すデータのサイズ
	int DataSize
)
{
	int                  i;
	const unsigned char *WriteP;

	// 書き込むデータのサイズ分だけ繰り返し
	WriteP = ( const unsigned char * )ParamData;
	for( i = 0; i < DataSize; i++ )
	{
		// 書き込むデータを一時的に保持するバッファが一杯になったらファイルに書き出す
		if( BinFileData->Address == BINARYFILE_BUFFER_SIZE )
		{
			fwrite( BinFileData->Buffer, 1, BinFileData->Address, BinFileData->FilePointer );
			BinFileData->Address = 0;
		}

		// 書き込むデータを一時的に保持するバッファに１バイト書き込む
		BinFileData->Buffer[ BinFileData->Address ] = *WriteP;

		// アドレスを１バイト進める
		WriteP++;
		BinFileData->Address++;
	}
}

// 書き込み用に開いたバイナリファイルにint型の値を書き込む
void WriteBinFile_Int(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むint型の値
	int ParamInt
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamInt, sizeof( int ) );
}

// 書き込み用に開いたバイナリファイルにbool型の値を書き込む
void WriteBinFile_Bool(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むbool型の値
	bool ParamBool
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamBool, sizeof( bool ) );
}

// 書き込み用に開いたバイナリファイルにfloat型の値を書き込む
void WriteBinFile_Float(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData, 
	
	// 書き込むfloat型の値
	float ParamFloat
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamFloat, sizeof( float ) );
}

// 書き込み用に開いたバイナリファイルにVECTOR型の値を書き込む
void WriteBinFile_Vector(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むVECTOR型の値
	VECTOR ParamVector
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamVector, sizeof( VECTOR ) );
}

// 書き込み用に開いたバイナリファイルに文字列をを書き込む
void WriteBinFile_String(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込む文字列のアドレス
	const char *ParamString
)
{
	size_t Length;

	// 文字列の長さを最初に書き込む
	Length = strlen( ParamString );
	WriteBinFile_Data( BinFileData, &Length, sizeof( size_t ) );

	// その後文字列自体を書き込む
	WriteBinFile_Data( BinFileData, ParamString, Length + 1 );
}

// 書き込み用に開いたバイナリファイルにunsigned char型の値を書き込む
void WriteBinFile_UChar(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むunsigned char型の値
	unsigned char ParamUChar
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamUChar, sizeof( unsigned char ) );
}

// 書き込み用に開いたバイナリファイルにunsigned int型の値を書き込む
void WriteBinFile_UInt(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 書き込むunsigned int型の値
	unsigned int ParamUInt
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamUInt, sizeof( unsigned int ) );
}

// 書き込み用に開いたバイナリファイルにCOLOR_U8型の値を書き込む
void WriteBinFile_ColorU8(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 書き込むCOLOR_U8型の値
	COLOR_U8 ParamColorU8
)
{
	// WriteBinFile_Data を使用してファイルに書き込む
	WriteBinFile_Data( BinFileData, &ParamColorU8, sizeof( COLOR_U8 ) );
}


// 読み込み用にバイナリファイルを開く
//     戻り値 : ファイルを開くとに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Open(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// ファイルパス
	const char *FilePath,

	// ファイルパス生成用可変個引数
	...
)
{
	va_list VaList;
	char    FilePathS[ MAX_PATH ];

	// 可変個引数からファイルパスを生成する
	va_start( VaList, FilePath );
	vsprintf( FilePathS, FilePath, VaList );
	va_end( VaList );

	// バイナリタイプの読み込み指定でファイルを開く
	BinFileData->FilePointer = fopen( FilePathS, "rb" );

	// ファイルを開くことに失敗したら失敗終了
	if( BinFileData->FilePointer == NULL )
	{
		return false;
	}

	// 一時バッファ中の有効なデータサイズを初期化する
	BinFileData->ValidSize = 0;

	// 一時バッファ中の次にアクセスすべきアドレスを初期化する
	BinFileData->Address = 0;

	// 成功終了
	return true;
}

// 読み込み用に開いたバイナリファイルを閉じる
void ReadBinFile_Close(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData
)
{
	// ファイルを閉じる
	fclose( BinFileData->FilePointer );

	// ファイルポインタなどを初期化
	BinFileData->FilePointer = NULL;
	BinFileData->ValidSize   = 0;
	BinFileData->Address     = 0;
}

// 読み込み用に開いたバイナリファイルから指定のメモリアドレスにデータを読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Data(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだデータを書き込むメモリのアドレス
	void *ParamData,

	// 読み込むデータのサイズ
	int DataSize
)
{
	int            i;
	unsigned char *ReadP;

	// 読み込むデータのサイズ分だけ繰り返し
	ReadP = ( unsigned char * )ParamData;
	for( i = 0; i < DataSize; i++ )
	{
		// 一時バッファに読み込んだデータを全て使ってしまった場合は新たにデータを読み込む
		if( BinFileData->ValidSize == BinFileData->Address )
		{
			BinFileData->ValidSize = fread( BinFileData->Buffer, 1, BINARYFILE_BUFFER_SIZE, BinFileData->FilePointer );

			// ファイルの終端に来てしまったら失敗
			if( BinFileData->ValidSize <= 0 )
			{
				return false;
			}
			BinFileData->Address = 0;
		}

		// 一時バッファから１バイト読み取る
		*ReadP = BinFileData->Buffer[ BinFileData->Address ];

		// アドレスを１バイト進める
		ReadP++;
		BinFileData->Address++;
	}

	// 成功終了
	return true;
}

// 読み込み用に開いたバイナリファイルからint型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Int(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだint型の値を書き込むメモリのアドレス
	int *ParamInt
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamInt, sizeof( int ) );
}

// 読み込み用に開いたバイナリファイルからbool型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Bool(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,

	// 読み込んだbool型の値を書き込むメモリのアドレス
	bool *ParamBool
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamBool, sizeof( bool ) );
}

// 読み込み用に開いたバイナリファイルからfloat型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Float(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだfloat型の値を書き込むメモリのアドレス
	float *ParamFloat
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamFloat, sizeof( float ) );
}

// 読み込み用に開いたバイナリファイルからVECTOR型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_Vector(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだVECTOR型の値を書き込むメモリのアドレス
	VECTOR *ParamVector
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamVector, sizeof( VECTOR ) );
}

// 読み込み用に開いたバイナリファイルから文字列を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_String(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだ文字列を書き込むメモリのアドレス
	char *ParamString
)
{
	size_t Length;

	// 最初に文字列の長さを読み込む
	if( !ReadBinFile_Data( BinFileData, &Length, sizeof( size_t ) ) )
	{
		return false;
	}

	// その後文字列自体を読み込む
	return ReadBinFile_Data( BinFileData, ParamString, Length + 1 );
}

// 読み込み用に開いたバイナリファイルからunsigned char型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_UChar(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだunsigned char型の値を書き込むメモリのアドレス
	unsigned char *ParamUChar
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamUChar, sizeof( unsigned char ) );
}

// 読み込み用に開いたバイナリファイルからunsigned int型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_UInt(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだunsigned int型の値を書き込むメモリのアドレス
	unsigned int *ParamUInt
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamUInt, sizeof( unsigned int ) );
}

// 読み込み用に開いたバイナリファイルからCOLOR_U8型の値を読み込む
//     戻り値 : データの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した ) 
bool ReadBinFile_ColorU8(
	// バイナリファイルアクセス用情報構造体のアドレス
	SBinaryFileData *BinFileData,
	
	// 読み込んだCOLOR_U8型の値を書き込むメモリのアドレス
	COLOR_U8 *ParamColorU8
)
{
	// ReadBinFile_Data を使用してファイルから読み込む
	return ReadBinFile_Data( BinFileData, ParamColorU8, sizeof( COLOR_U8 ) );
}

