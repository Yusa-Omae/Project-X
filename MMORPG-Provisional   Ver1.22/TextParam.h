#ifndef TEXTPARAM_H
#define TEXTPARAM_H

#include "DxLib.h"

// �e�L�X�g�p�����[�^�̖��O�Ɏg����ő啶����
#define TEXTPARAM_NAME_MAXLENGTH		(64)

// �t�@�C����Ɋ܂߂���e�L�X�g�p�����[�^�̍ő吔
#define TEXTPARAM_MAXNUM				(1024)

// ������^�C�v�̃p�����[�^�̍ő啶����
#define TEXTPARAM_STRING_MAXLENGTH		(64)

// �p�����[�^�^�C�v
typedef enum _ETextParamType
{
	ETextParamType_Int,				// �����l
	ETextParamType_Bool,			// bool�l
	ETextParamType_Float,			// ���������_���l
	ETextParamType_Vector,			// VECTOR�l
	ETextParamType_String,			// ������
	ETextParamType_Color,			// �J���[�l

	ETextParamType_Num,				// �p�����[�^�^�C�v�̐�
} ETextParamType;

// �p�����[�^�{�̂̋��p��
typedef union _STextParam
{
	// int�^�C�v�̃p�����[�^�p�ϐ�
	int                Int;

	// bool�^�C�v�̃p�����[�^�p�ϐ�
	bool               Bool;

	// float�^�C�v�̃p�����[�^�p�ϐ�
	float              Float;

	// vector�^�C�v�̃p�����[�^�p�ϐ�
	VECTOR             Vector;

	// string�^�C�v�̃p�����[�^�p�ϐ�
	char               String[TEXTPARAM_STRING_MAXLENGTH];

	// color�^�C�v�̃p�����[�^�p�ϐ�
	COLOR_U8           Color;
} STextParam;

// �p�����[�^��̏��
typedef struct _STextParamInfoUnit
{
	// �p�����[�^�̖��O
	char               Name[TEXTPARAM_NAME_MAXLENGTH];

	// �p�����[�^�^�C�v
	ETextParamType     Type;

	// �p�����[�^�{��
	STextParam         Param;
} STextParamInfoUnit;

// �P�t�@�C���̃e�L�X�g�p�����[�^���
typedef struct _STextParamInfo
{
	// �p�����[�^�̐�
	int                ParamNum;

	// �p�����[�^�z��
	STextParamInfoUnit Param[TEXTPARAM_MAXNUM];
} STextParamInfo;

// �p�����[�^�t�@�C����ǂݍ���
//     �߂�l : �ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool        LoadTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
);

// �w��̖��O�̃p�����[�^�̔z�񒆂̃C���f�b�N�X���擾����
//     �߂�l : �z��C���f�b�N�X
//              ( �p�����[�^��������Ȃ������ꍇ�� -1 ���Ԃ� )
extern int         SearchTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��int�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^�̐����l
extern int         GetTextParamInt(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��bool�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^��bool�l
extern bool        GetTextParamBool(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��float�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^�̕��������_�l
extern float       GetTextParamFloat(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��vector�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^��VECTOR�l
extern VECTOR      GetTextParamVector(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��string�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^�̕�����̃A�h���X
extern const char *GetTextParamString(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��color�^�C�v�̃p�����[�^���擾����
//     �߂�l : �p�����[�^��COLOR_U8�l
extern COLOR_U8    GetTextParamColor(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

#endif

#if false
#include"DxLib.h"
#include"Define.h"

// �p�����[�^�^�C�v
typedef enum _ETextParamType
{
	ETextParamType_Int,				// �����l
	ETextParamType_Bool,			// bool�l
	ETextParamType_Float,			// ���������_���l
	ETextParamType_Vector,			// VECTOR�l
	ETextParamType_String,			// ������
	ETextParamType_Color,			// �J���[�l

	ETextParamType_Num,				// �p�����[�^�^�C�v�̐�
} ETextParamType;

// �p�����[�^�{��
typedef union _STextParam
{
	// int�^�C�v�̃p�����[�^�p�ϐ�
	int                Int;
	// bool�^�C�v�̃p�����[�^�p�ϐ�
	bool               Bool;
	// float�^�C�v�̃p�����[�^�p�ϐ�
	float              Float;
	// vector�^�C�v�̃p�����[�^�p�ϐ�
	VECTOR             Vector;
	// string�^�C�v�̃p�����[�^�p�ϐ�
	char               String[ TEXTPARAM_STRING_MAXLENGTH ];
	// color�^�C�v�̃p�����[�^�p�ϐ�
	COLOR_U8           Color;
} STextParam;


// �p�����[�^��̏��
typedef struct _STextParamInfoUnit
{
	// �p�����[�^�̖��O
	char               Name[ TEXTPARAM_NAME_MAXLENGTH ];
	// �p�����[�^�^�C�v
	ETextParamType     Type;
	// �p�����[�^�{��
	STextParam         Param;
} STextParamInfoUnit;


// �P�t�@�C���̃e�L�X�g�p�����[�^���
typedef struct _STextParamInfo
{
	// �p�����[�^�̐�
	int                ParamNum;
	// �p�����[�^�z��
	STextParamInfoUnit Param[ TEXTPARAM_MAXNUM ];
} STextParamInfo;

// �p�����[�^�t�@�C����ǂݍ���
extern bool        LoadTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �t�@�C���p�X
	const char *FilePath,
	// �t�@�C���p�X�����p�ό���
	...
);

// �w��̖��O�̃p�����[�^�̔z����擾����
extern int         SearchTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��int�̃p�����[�^���擾����
extern int         GetTextParamInt(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��bool�̃p�����[�^���擾����
extern bool        GetTextParamBool(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��float�̃p�����[�^���擾����
extern float       GetTextParamFloat(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��vector�̃p�����[�^���擾����
extern VECTOR      GetTextParamVector(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��string�̃p�����[�^���擾����
extern const char *GetTextParamString(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
);

// �w��̖��O��color�̃p�����[�^���擾����
extern COLOR_U8    GetTextParamColor(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
);
#endif