#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <stdio.h>
#include "DxLib.h"

// �t�@�C���̃f�[�^���ꎞ�I�ɕێ����Ă����o�b�t�@�̃T�C�Y
#define BINARYFILE_BUFFER_SIZE		(64 * 1024)

// �o�C�i���t�@�C���A�N�Z�X�p���
typedef struct _SBinaryFileData
{
	// �t�@�C���|�C���^
	FILE *        FilePointer;

	// �t�@�C���̃f�[�^���ꎞ�I�ɕێ����Ă������߂̃o�b�t�@
	unsigned char Buffer[ BINARYFILE_BUFFER_SIZE ];

	// Buffer���̗L���ȃf�[�^�T�C�Y( �ǂݍ��ݎ��̂ݎg�p )
	size_t        ValidSize;

	// ���ɃA�N�Z�X���ׂ�Buffer���̃A�h���X
	int           Address;
} SBinaryFileData;

// �������ݗp�Ƀo�C�i���t�@�C�����J��
//     �߂�l : �t�@�C�����J���Ƃɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool WriteBinFile_Open(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
);

// �������ݗp�ɊJ�����o�C�i���t�@�C�������
extern void WriteBinFile_Close(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData
);

// �������ݗp�ɊJ�����o�C�i���t�@�C���Ɏw�胁�����A�h���X�ɂ���f�[�^����������
extern void WriteBinFile_Data(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �������ރf�[�^���i�[����Ă��郁�����A�h���X
	const void *  ParamData,

	// �t�@�C���ɏ����o���f�[�^�̃T�C�Y
	int DataSize
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����int�^�̒l����������
extern void WriteBinFile_Int(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������int�^�̒l
	int ParamInt
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����bool�^�̒l����������
extern void WriteBinFile_Bool(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������bool�^�̒l
	bool ParamBool
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����float�^�̒l����������
extern void WriteBinFile_Float(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData, 
	
	// ��������float�^�̒l
	float ParamFloat
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����VECTOR�^�̒l����������
extern void WriteBinFile_Vector(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������VECTOR�^�̒l
	VECTOR ParamVector
);

// �������ݗp�ɊJ�����o�C�i���t�@�C���ɕ����������������
extern void WriteBinFile_String(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �������ޕ�����̃A�h���X
	const char *ParamString
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����unsigned char�^�̒l����������
extern void WriteBinFile_UChar(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������unsigned char�^�̒l
	unsigned char ParamUChar
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����unsigned int�^�̒l����������
extern void WriteBinFile_UInt(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// ��������unsigned int�^�̒l
	unsigned int ParamUInt
);

// �������ݗp�ɊJ�����o�C�i���t�@�C����COLOR_U8�^�̒l����������
extern void WriteBinFile_ColorU8(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// ��������COLOR_U8�^�̒l
	COLOR_U8 ParamColorU8
);


// �ǂݍ��ݗp�Ƀo�C�i���t�@�C�����J��
//     �߂�l : �t�@�C�����J���Ƃɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Open(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C�������
extern void ReadBinFile_Close(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������w��̃������A�h���X�Ƀf�[�^��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Data(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ��񂾃f�[�^���������ރ������̃A�h���X
	void *ParamData,

	// �ǂݍ��ރf�[�^�̃T�C�Y
	int DataSize
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������int�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Int(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ���int�^�̒l���������ރ������̃A�h���X
	int *ParamInt
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������bool�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Bool(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,

	// �ǂݍ���bool�^�̒l���������ރ������̃A�h���X
	bool *ParamBool
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������float�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Float(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���float�^�̒l���������ރ������̃A�h���X
	float *ParamFloat
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������VECTOR�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_Vector(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���VECTOR�^�̒l���������ރ������̃A�h���X
	VECTOR *ParamVector
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C�����當�����ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_String(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ��񂾕�������������ރ������̃A�h���X
	char *ParamString
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������unsigned char�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_UChar(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���unsigned char�^�̒l���������ރ������̃A�h���X
	unsigned char *ParamUChar
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������unsigned int�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_UInt(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���unsigned int�^�̒l���������ރ������̃A�h���X
	unsigned int *ParamUInt
);

// �ǂݍ��ݗp�ɊJ�����o�C�i���t�@�C������COLOR_U8�^�̒l��ǂݍ���
//     �߂�l : �f�[�^�̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� ) 
extern bool ReadBinFile_ColorU8(
	// �o�C�i���t�@�C���A�N�Z�X�p���\���̂̃A�h���X
	SBinaryFileData *BinFileData,
	
	// �ǂݍ���COLOR_U8�^�̒l���������ރ������̃A�h���X
	COLOR_U8 *ParamColorU8
);

#endif


