#include "TextParam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// �p�����[�^���擾����֐��̐擪�ŕK���s������
// �ό�������p�����[�^���𐶐����āA�z�񒆂̃p�����[�^�̃C���f�b�N�X���擾����
#define GETTEXTPARAM_COMMON		\
	int Index;\
	va_list VaList;\
	char ParamNameS[ MAX_PATH ];\
	\
	va_start( VaList, ParamName );\
	vsprintf( ParamNameS, ParamName, VaList );\
	va_end( VaList );\
	\
	Index = SearchTextParam( TxtParam, ParamNameS );

// �p�����[�^�^�C�v��
static const char *g_TextParamTypeName[ ETextParamType_Num ] =
{
	"int",
	"bool",
	"float",
	"vector",
	"string",
	"color"
};

// �p�����[�^�t�@�C����ǂݍ���
//		true:���� false:���s
bool LoadTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �t�@�C���p�X
	const char *FilePath,

	// �t�@�C���p�X�����p�ό���
	...
)
{
	FILE   *fp;
	char    Type[ 64 ];
	char    Name[ TEXTPARAM_NAME_MAXLENGTH ];
	char    Param1[ 64 ];
	char    Param2[ 64 ];
	char    Param3[ 64 ];
	char    Param4[ 64 ];
	va_list VaList;
	char    FilePathS[ MAX_PATH ];
	int     i;
	int     Num;

	// �ό�������t�@�C���p�X�𐶐�����
	va_start( VaList, FilePath );
	vsprintf( FilePathS, FilePath, VaList );
	va_end( VaList );

	// �e�L�X�g�^�C�v�̓ǂݍ��ݎw��Ńt�@�C�����J��
	fp = fopen( FilePathS, "rt" );

	if( fp == NULL )
	{
		return false;
	}

	// �e�L�X�g�p�����[�^�\���̂��O�ŏ���������
	memset( TxtParam, 0, sizeof( STextParamInfo ) );

	// �e�L�X�g�t�@�C���̍Ō�ɓ��B����܂Ń��[�v
	Num = 0;
	while( fscanf( fp, "%s %s = %s", Type, Name, Param1 ) != EOF )
	{
		// �p�����[�^�̖��O��ۑ�
		strcpy( TxtParam->Param[ Num ].Name, Name );

		// �p�����[�^�̃^�C�v�𒲂ׂ�
		for( i = 0; i < ETextParamType_Num; i++ )
		{
			if( strcmp( g_TextParamTypeName[ i ], Type ) == 0 )
			{
				break;
			}
		}

		// �^�C�v�������������ꍇ�̂ݏ���
		if( i == ETextParamType_Num )
		{
			// ���s�̈ʒu�܂œǂݐi�߂�
			fscanf( fp, "\n" );
			continue;
		}

		// �^�C�v��ۑ�
		TxtParam->Param[ Num ].Type = ( ETextParamType )i;

		// �^�C�v�ɂ���ď����𕪊�
		switch( TxtParam->Param[ Num ].Type )
		{
		case ETextParamType_Int:			// �����l
			// �^�C�v����int�������ꍇ�̓p�����[�^������𐮐��l�ɕϊ�����
			TxtParam->Param[ Num ].Param.Int = atoi( Param1 );
			break;

		case ETextParamType_Bool:			// bool�l
			// �^�C�v����bool�������ꍇ�̓p�����[�^�������bool�l�ɕϊ�����
			TxtParam->Param[ Num ].Param.Bool = strcmp( Param1, "true" ) == 0;
			break;

		case ETextParamType_Float:			// ���������_���l
			// �^�C�v����float�������ꍇ�̓p�����[�^������𕂓������_���l�ɕϊ�����
			TxtParam->Param[ Num ].Param.Float = ( float )atof( Param1 );
			break;

		case ETextParamType_Vector:			// VECTOR�l
			// �^�C�v����vector�������ꍇ�͒ǉ��œ�p�����[�^��������擾����
			fscanf( fp, "%s %s", Param2, Param3 );

			// �R�̃p�����[�^������������l�ɕϊ�����
			TxtParam->Param[ Num ].Param.Vector.x = ( float )atof( Param1 );
			TxtParam->Param[ Num ].Param.Vector.y = ( float )atof( Param2 );
			TxtParam->Param[ Num ].Param.Vector.z = ( float )atof( Param3 );
			break;

		case ETextParamType_String:			// ������
			// �^�C�v����string�������ꍇ�̓p�����[�^����������̂܂ܕۑ�����
			strcpy( TxtParam->Param[ Num ].Param.String, Param1 );
			break;

		case ETextParamType_Color:			// �J���[�l
			// �^�C�v����color�������ꍇ�͒ǉ��ŎO�p�����[�^��������擾����
			fscanf( fp, "%s %s %s", Param2, Param3, Param4 );

			// �S�̃p�����[�^������𐮐��l�ɕϊ�����
			TxtParam->Param[ Num ].Param.Color.r = atoi( Param1 );
			TxtParam->Param[ Num ].Param.Color.g = atoi( Param2 );
			TxtParam->Param[ Num ].Param.Color.b = atoi( Param3 );
			TxtParam->Param[ Num ].Param.Color.a = atoi( Param4 );
			break;
		}

		// �p�����[�^�̐��𑝂₷
		Num++;

		// ���s�̈ʒu�܂œǂݐi�߂�
		fscanf( fp, "\n" );
	}

	// �p�����[�^�̐���ۑ�
	TxtParam->ParamNum = Num;

	// �t�@�C�������
	fclose( fp );

	// �����I��
	return true;
}

// �w��̖��O�̃p�����[�^�̔z����擾����
int         SearchTextParam(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,
	// �p�����[�^��
	const char *ParamName,
	// �p�����[�^�������p�ό���
	...
)
{
	int     i;
	va_list VaList;
	char    ParamNameS[ MAX_PATH ];

	// �ό�������p�����[�^���𐶐�����
	va_start( VaList, ParamName );
	vsprintf( ParamNameS, ParamName, VaList );
	va_end( VaList );

	// �p�����[�^�̐������J��Ԃ�
	for( i = 0; i < TxtParam->ParamNum; i++ )
	{
		// �w��̃p�����[�^���ƈ�v����p�����[�^���������ꍇ�͔z��̃C���f�b�N�X��Ԃ�
		if( strcmp( TxtParam->Param[ i ].Name, ParamNameS ) == 0 )
		{
			return i;
		}
	}

	// ������Ȃ������ꍇ�� -1 ��Ԃ�
	return -1;
}

// �w��̖��O��int�^�C�v�̃p�����[�^���擾����
int         GetTextParamInt(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� 0 ��Ԃ�
	if( Index < 0 )
	{
		return 0;
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^�̐����l��Ԃ�
	return TxtParam->Param[ Index ].Param.Int;
}

// �w��̖��O��bool�^�C�v�̃p�����[�^���擾����

bool        GetTextParamBool(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� false ��Ԃ�
	if( Index < 0 )
	{
		return false;
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^��bool�l��Ԃ�
	return TxtParam->Param[ Index ].Param.Bool;
}

// �w��̖��O��float�^�C�v�̃p�����[�^���擾����
float       GetTextParamFloat(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� 0.0f ��Ԃ�
	if( Index < 0 )
	{
		return 0.0f;
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^�̕��������_���l��Ԃ�
	return TxtParam->Param[ Index ].Param.Float;
}

// �w��̖��O��vector�^�C�v�̃p�����[�^���擾����
VECTOR      GetTextParamVector(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� x=0.0f, y=0.0f, z=0.0f ��Ԃ�
	if( Index < 0 )
	{
		return VGet( 0.0f, 0.0f, 0.0f );
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^��VECTOR�l��Ԃ�
	return TxtParam->Param[ Index ].Param.Vector;
}

// �w��̖��O��string�^�C�v�̃p�����[�^���擾����
const char *GetTextParamString(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� NULL ��Ԃ�
	if( Index < 0 )
	{
		return NULL;
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^�̕�����̃A�h���X��Ԃ�
	return TxtParam->Param[ Index ].Param.String;
}

// �w��̖��O��color�^�C�v�̃p�����[�^���擾����
COLOR_U8    GetTextParamColor(
	// �e�L�X�g�p�����[�^���\���̂̃A�h���X
	STextParamInfo *TxtParam,

	// �p�����[�^��
	const char *ParamName,

	// �p�����[�^�������p�ό���
	...
)
{
	// �ό�������p�����[�^���𐶐����A�p�����[�^�̔z�񒆂�
	// �C���f�b�N�X���擾�����^���������s����
	GETTEXTPARAM_COMMON

	// �p�����[�^��������Ȃ������ꍇ�� r=0 g=0 b=0 a=0 ��Ԃ�
	if( Index < 0 )
	{
		return GetColorU8( 0,0,0,0 );
	}

	// �p�����[�^�����������ꍇ�́A�p�����[�^��COLOR_U8�̒l��Ԃ�
	return TxtParam->Param[ Index ].Param.Color;
}
