#include "BinaryFile.h"
#include <stdarg.h>

// �������ݗp�Ƀo�C�i���t�@�C�����J��
//     �߂�l : �t�@�C�����J���Ƃɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool WriteBinFile_Open(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
)
{
	va_list VaList;
	char    FilePathS[ MAX_PATH ];

	// �ό�������t�@�C���p�X�𐶐�����
	va_start( VaList, FilePath );
	vsprintf( FilePathS, FilePath, VaList );
	va_end( VaList );

	// �o�C�i���^�C�v�̏������ݎw��Ńt�@�C�����J��
	BinFileData->FilePointer = fopen( FilePathS, "wb" );

	// �t�@�C�����J�����ƂɎ��s�����玸�s�I��
	if( BinFileData->FilePointer == NULL )
	{
		return false;
	}

	// �ꎞ�o�b�t�@���̎��ɏ������ނׂ��A�h���X��������
	BinFileData->Address = 0;

	// �����I��
	return true;
}

// �������ݗp�ɊJ�����o�C�i���t�@�C�������
void WriteBinFile_Close(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData
)
{
	// �ꎞ�o�b�t�@�Ƀf�[�^������ꍇ�̓t�@�C���ɏ����o��
	if( BinFileData->Address > 0 )
	{
		fwrite( BinFileData->Buffer, 1, BinFileData->Address, BinFileData->FilePointer );
	}

	// �t�@�C������
	fclose( BinFileData->FilePointer );

	// �t�@�C���|�C���^�ƃA�h���X��������
	BinFileData->FilePointer = NULL;
	BinFileData->Address     = 0;
}

// �������ݗp�ɊJ�����o�C�i���t�@�C���Ɏw�胁�����A�h���X�ɂ���f�[�^����������
void WriteBinFile_Data(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �������ރf�[�^���i�[����Ă��郁�����A�h���X
	const void *  ParamData,

	// �t�@�C���ɏ����o���f�[�^�̃T�C�Y
	int DataSize
)
{
	int                  i;
	const unsigned char *WriteP;

	// �������ރf�[�^�̃T�C�Y�������J��Ԃ�
	WriteP = ( const unsigned char * )ParamData;
	for( i = 0; i < DataSize; i++ )
	{
		// �������ރf�[�^���ꎞ�I�ɕێ�����o�b�t�@����t�ɂȂ�����t�@�C���ɏ����o��
		if( BinFileData->Address == BINARYFILE_BUFFER_SIZE )
		{
			fwrite( BinFileData->Buffer, 1, BinFileData->Address, BinFileData->FilePointer );
			BinFileData->Address = 0;
		}

		// �������ރf�[�^���ꎞ�I�ɕێ�����o�b�t�@�ɂP�o�C�g��������
		BinFileData->Buffer[ BinFileData->Address ] = *WriteP;

		// �A�h���X���P�o�C�g�i�߂�
		WriteP++;
		BinFileData->Address++;
	}
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����int�^�̒l����������
void WriteBinFile_Int(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������int�^�̒l
	int ParamInt
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamInt, sizeof( int ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����bool�^�̒l����������
void WriteBinFile_Bool(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������bool�^�̒l
	bool ParamBool
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamBool, sizeof( bool ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����float�^�̒l����������
void WriteBinFile_Float(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData, 
	
	// ��������float�^�̒l
	float ParamFloat
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamFloat, sizeof( float ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����VECTOR�^�̒l����������
void WriteBinFile_Vector(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������VECTOR�^�̒l
	VECTOR ParamVector
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamVector, sizeof( VECTOR ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C���ɕ����������������
void WriteBinFile_String(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �������ޕ�����̃A�h���X
	const char *ParamString
)
{
	size_t Length;

	// ������̒������ŏ��ɏ�������
	Length = strlen( ParamString );
	WriteBinFile_Data( BinFileData, &Length, sizeof( size_t ) );

	// ���̌㕶���񎩑̂���������
	WriteBinFile_Data( BinFileData, ParamString, Length + 1 );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����unsigned char�^�̒l����������
void WriteBinFile_UChar(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������unsigned char�^�̒l
	unsigned char ParamUChar
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamUChar, sizeof( unsigned char ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����unsigned int�^�̒l����������
void WriteBinFile_UInt(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������unsigned int�^�̒l
	unsigned int ParamUInt
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamUInt, sizeof( unsigned int ) );
}

// �������ݗp�ɊJ�����o�C�i���t�@�C����COLOR_U8�^�̒l����������
void WriteBinFile_ColorU8(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������COLOR_U8�^�̒l
	COLOR_U8 ParamColorU8
)
{
	// WriteBinFile_Data ���g�p���ăt�@�C���ɏ�������
	WriteBinFile_Data( BinFileData, &ParamColorU8, sizeof( COLOR_U8 ) );
}


// �ǂݍ��ݗp�Ƀo�C�i���t�@�C�����J��
//     �߂�l : �t�@�C�����J���Ƃɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Open(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
)
{
	va_list VaList;
	char    FilePathS[ MAX_PATH ];

	// �ό�������t�@�C���p�X�𐶐�����
	va_start( VaList, FilePath );
	vsprintf( FilePathS, FilePath, VaList );
	va_end( VaList );

	// �o�C�i���^�C�v�̓ǂݍ��ݎw��Ńt�@�C�����J��
	BinFileData->FilePointer = fopen( FilePathS, "rb" );

	// �t�@�C�����J�����ƂɎ��s�����玸�s�I��
	if( BinFileData->FilePointer == NULL )
	{
		return false;
	}

	// �ꎞ�o�b�t�@���̗L���ȃf�[�^�T�C�Y������������
	BinFileData->ValidSize = 0;

	// �ꎞ�o�b�t�@���̎��ɃA�N�Z�X���ׂ��A�h���X������������
	BinFileData->Address = 0;

	// �����I��
	return true;
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C�������
void ReadBinFile_Close(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData
)
{
	// �t�@�C�������
	fclose( BinFileData->FilePointer );

	// �t�@�C���|�C���^�Ȃǂ�������
	BinFileData->FilePointer = NULL;
	BinFileData->ValidSize   = 0;
	BinFileData->Address     = 0;
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������w��̃������A�h���X�Ƀf�[�^��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Data(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ��񂾃f�[�^���������ރ������̃A�h���X
	void *ParamData,

	// �ǂݍ��ރf�[�^�̃T�C�Y
	int DataSize
)
{
	int            i;
	unsigned char *ReadP;

	// �ǂݍ��ރf�[�^�̃T�C�Y�������J��Ԃ�
	ReadP = ( unsigned char * )ParamData;
	for( i = 0; i < DataSize; i++ )
	{
		// �ꎞ�o�b�t�@�ɓǂݍ��񂾃f�[�^��S�Ďg���Ă��܂����ꍇ�͐V���Ƀf�[�^��ǂݍ���
		if( BinFileData->ValidSize == BinFileData->Address )
		{
			BinFileData->ValidSize = fread( BinFileData->Buffer, 1, BINARYFILE_BUFFER_SIZE, BinFileData->FilePointer );

			// �t�@�C���̏I�[�ɗ��Ă��܂����玸�s
			if( BinFileData->ValidSize <= 0 )
			{
				return false;
			}
			BinFileData->Address = 0;
		}

		// �ꎞ�o�b�t�@����P�o�C�g�ǂݎ��
		*ReadP = BinFileData->Buffer[ BinFileData->Address ];

		// �A�h���X���P�o�C�g�i�߂�
		ReadP++;
		BinFileData->Address++;
	}

	// �����I��
	return true;
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������int�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Int(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ���int�^�̒l���������ރ������̃A�h���X
	int *ParamInt
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamInt, sizeof( int ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������bool�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Bool(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ���bool�^�̒l���������ރ������̃A�h���X
	bool *ParamBool
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamBool, sizeof( bool ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������float�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Float(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���float�^�̒l���������ރ������̃A�h���X
	float *ParamFloat
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamFloat, sizeof( float ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������VECTOR�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_Vector(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���VECTOR�^�̒l���������ރ������̃A�h���X
	VECTOR *ParamVector
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamVector, sizeof( VECTOR ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C�����當�����ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_String(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ��񂾕�������������ރ������̃A�h���X
	char *ParamString
)
{
	size_t Length;

	// �ŏ��ɕ�����̒�����ǂݍ���
	if( !ReadBinFile_Data( BinFileData, &Length, sizeof( size_t ) ) )
	{
		return false;
	}

	// ���̌㕶���񎩑̂�ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamString, Length + 1 );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������unsigned char�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_UChar(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���unsigned char�^�̒l���������ރ������̃A�h���X
	unsigned char *ParamUChar
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamUChar, sizeof( unsigned char ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������unsigned int�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_UInt(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���unsigned int�^�̒l���������ރ������̃A�h���X
	unsigned int *ParamUInt
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamUInt, sizeof( unsigned int ) );
}

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������COLOR_U8�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
bool ReadBinFile_ColorU8(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���COLOR_U8�^�̒l���������ރ������̃A�h���X
	COLOR_U8 *ParamColorU8
)
{
	// ReadBinFile_Data ���g�p���ăt�@�C������ǂݍ���
	return ReadBinFile_Data( BinFileData, ParamColorU8, sizeof( COLOR_U8 ) );
}

