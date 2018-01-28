#include "ToolLib_Visible.h"
#include <string.h>
#include <malloc.h>

// �쐬�ł���\����ԏ��̍ő吔
#define TOOL_VISIBLE_MAX			(256)

// �\����ԏ��\����
typedef struct _SToolVisible
{
	// �e�̕\����ԏ��̃n���h��( �e�����Ȃ��ꍇ�� -1 )
	int  ParentTVisibleHandle;

	// �\�����
	bool Visible;
} SToolVisible;

static int           g_TVisibleNum;
static SToolVisible *g_TVisible[ TOOL_VISIBLE_MAX ];

// �\����ԏ����쐬����
//     �߂�l : �\����ԏ��n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolVisible_Create( void )
{
	int           NewHandle;
	SToolVisible *TVisible;

	// ���ɕ\����ԏ�񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TVisibleNum == TOOL_VISIBLE_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TVisible[ NewHandle ]; NewHandle++ ){}

	// �V�����\����ԏ����i�[���邽�߂̃������̊m��
	TVisible = ( SToolVisible * )calloc( 1, sizeof( SToolVisible ) );
	if( TVisible == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TVisible[ NewHandle ] = TVisible;

	// �\����ԏ��̐��𑝂₷
	g_TVisibleNum++; 

	// ����������
	TVisible->ParentTVisibleHandle = -1;
	TVisible->Visible              = false;

	// �n���h����Ԃ�
	return NewHandle;
}

// �\����ԏ����폜����
void ToolVisible_Delete(
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// �m�ۂ��Ă������������������
	free( TVisible );
	g_TVisible[ TVisibleHandle ] = NULL;

	// �\����ԏ��̐������炷
	g_TVisibleNum--;
}

// �S�Ă̕\����ԏ����폜����
void ToolVisible_Delete_All( void )
{
	int i;

	// �L���ȑS�Ă̕\����Ԃ��폜����
	for( i = 0; i < TOOL_VISIBLE_MAX; i++ )
	{
		if( g_TVisible[ i ] != NULL )
		{
			ToolVisible_Delete( i );
		}
	}
}

// �\����ԏ��̕\����Ԃ�ύX����
void ToolVisible_SetVisible(
	// �\����ԏ��n���h��
	int TVisibleHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// �V�����\����Ԃ�ۑ�
	TVisible->Visible = Visible;
}

// �\����ԏ��ɐe�̕\����ԏ���ݒ肷��
void ToolVisible_SetParent(
	// �\����ԏ��n���h��
	int TVisibleHandle,
	
	// �e�ƂȂ�\����ԏ��n���h��
	int ParentTVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// �e�ƂȂ�\����ԏ��̃n���h����ۑ�
	TVisible->ParentTVisibleHandle = ParentTVisibleHandle;
}

// �\����ԏ��̕\����Ԃ��擾����( �e�̕\����Ԃ��l������ )
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolVisible_GetVisible(
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];
	SToolVisible *ParentTVisible;

	// �\���t���O���|��Ă����� false ��Ԃ�
	if( !TVisible->Visible )
	{
		return false;
	}

	// �e�̕\����ԏ�񂪗L���ȏꍇ�͐e�̕\����Ԃ��l������
	if( TVisible->ParentTVisibleHandle >= 0 )
	{
		ParentTVisible = g_TVisible[ TVisible->ParentTVisibleHandle ];

		// �e�̕\���t���O���|��Ă����� false ��Ԃ�
		if( !ParentTVisible->Visible )
		{
			return false;
		}

		// �e�ɂ��e��������A�e�����Ȃ��e�̕\����Ԃ܂ōl������
		while( ParentTVisible->ParentTVisibleHandle >= 0 )
		{
			ParentTVisible = g_TVisible[ ParentTVisible->ParentTVisibleHandle ];

			// �e�̕\���t���O���|��Ă����� false ��Ԃ�
			if( !ParentTVisible->Visible )
			{
				return false;
			}
		}
	}

	// �����ɂ������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}


