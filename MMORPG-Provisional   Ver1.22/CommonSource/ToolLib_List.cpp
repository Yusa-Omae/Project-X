#include "ToolLib_List.h"
#include "ToolLib_Window.h"
#include "ToolLib_Static.h"
#include "ToolLib_Visible.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>

// �쐬�ł��郊�X�g���̍ő吔
#define TOOL_LIST_MAX				(256)

// ���X�g�P�s�ӂ�̏c��
#define ONE_LINE_HEIGHT				(16)

// �s�̃|�C���^�o�b�t�@���g������ۂɈ�x�ɒǉ�����o�b�t�@�T�C�Y
#define ADD_OBJPOINTER_NUM			(100)

// ���X�g�\���p�̕�����o�b�t�@���g������ۂɈ�x�ɒǉ�����o�b�t�@�T�C�Y
#define ADD_STRINGBUFFER_SIZE			(16 * 1024)

// ���X�g�\����
typedef struct _SToolList
{
	// �֘A�t�����Ă���\����ԏ��n���h��
	int    TVisibleHandle;

	// �֘A�t�����Ă���E�C���h�E���n���h��
	int    TWindowHandle;

	// �ǉ�����Ă���s�̕�����̃A�h���X���i�[����Ă���|�C���^�z��
	char **ObjList;

	// �ǉ��\�ȍő�s��
	int    MaxObjNum;

	// �ǉ�����Ă���s�̐�
	int    ObjNum;

	// �ǉ�����Ă��镶����̒��ōő�̉���
	int    Width;

	// �ǉ�����Ă���s�̕����񂪊i�[����Ă���o�b�t�@
	char  *StringBuffer;

	// StringBuffer �Ŋm�ۂ��Ă��郁�����T�C�Y
	int    StringBufferSize;

	// StringBuffer ���Ŋ��Ɏg�p���Ă��郁�����T�C�Y
	int    StringBufferUseSize;
	
	// �I������Ă���s
	int    SelectIndex;

	// ���X�g�̏�Ԃɕω������������ǂ���
	bool   Change;

	// �\�����
	bool   Visible;
} SToolList;

static int        g_TListNum;
static SToolList *g_TList[ TOOL_LIST_MAX ];

// ���X�g���̏�Ԑ��ڏ������s��
static void ToolList_Step(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g������ʂɕ`�悷��
static void ToolList_Draw(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g�����쐬����
//     �߂�l : ���X�g���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolList_Create( void )
{
	int        NewHandle;
	SToolList *TList;

	// ���Ƀ��X�g��񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TListNum == TOOL_LIST_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TList[ NewHandle ]; NewHandle++ ){}

	// �V�������X�g�����i�[���邽�߂̃������̊m��
	TList = ( SToolList * )calloc( 1, sizeof( SToolList ) );
	if( TList == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TList[ NewHandle ] = TList;

	// ���X�g���̐��𑝂₷
	g_TListNum++; 

	// ����������
	TList->TVisibleHandle = -1;
	TList->TWindowHandle  = -1;
	TList->Visible        = true;

	// �n���h����Ԃ�
	return NewHandle;
}

// ���X�g�����폜����
void ToolList_Delete(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// �s���p�Ɋm�ۂ��Ă��������������
	if( TList->ObjList )
	{
		free( TList->ObjList );
		TList->ObjList = NULL;
	}

	// �s�ɕ\�����镶������i�[���Ă��������������
	if( TList->StringBuffer )
	{
		free( TList->StringBuffer );
		TList->StringBuffer = NULL;
	}

	// �m�ۂ��Ă������������������
	free( TList );
	g_TList[ TListHandle ] = NULL;

	// ���X�g���̐������炷
	g_TListNum--;
}

// �S�Ẵ��X�g�����폜����
void ToolList_Delete_All( void )
{
	int i;

	// �L���ȑS�Ẵ��X�g���폜����
	for( i = 0; i < TOOL_LIST_MAX; i++ )
	{
		if( g_TList[ i ] != NULL )
		{
			ToolList_Delete( i );
		}
	}
}

// ���X�g���Ɋ֘A�t����\����ԏ����Z�b�g����
void ToolList_SetVisibleHandle(
	// ���X�g���n���h��
	int TListHandle,

	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	TList->TVisibleHandle = TVisibleHandle;
}

// ���X�g��������������
void ToolList_Initialize(
	// ���X�g���n���h��
	int TListHandle,
	
	// �֘A�t����E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// �s���p�Ɋm�ۂ��Ă��������������
	if( TList->ObjList )
	{
		free( TList->ObjList );
		TList->ObjList = NULL;
	}

	// �s�ɕ\�����镶������i�[���Ă��������������
	if( TList->StringBuffer )
	{
		free( TList->StringBuffer );
		TList->StringBuffer = NULL;
	}

	// ��������������
	TList->TWindowHandle       = TWindowHandle;
	TList->SelectIndex         = -1;
	TList->ObjNum              = 0;
	TList->StringBufferUseSize = 0;
	TList->MaxObjNum           = 0;
	TList->StringBufferSize    = 0;
	TList->Change              = false;
	TList->Width               = 0;

	// �E�C���h�E�̃N���C�A���g�̈��������
	ToolWindow_SetClientSize( TList->TWindowHandle, 16, 16 );
}

// ���X�g���̑I������Ă���s�̕ύX�Ȃǂ������������ǂ������擾����
//     �߂�l : �ύX�������������ǂ���( true:�ύX����������  false:�ύX�͔������Ă��Ȃ� )
bool ToolList_GetChange(
	// ���X�g���n���h��
	int TListHandle,

	// �ύX�������������ǂ����̏������Z�b�g���邩�ǂ���
	bool Reset
)
{
	SToolList *TList = g_TList[ TListHandle ];
	bool       Result;

	Result = TList->Change;
	if( Reset )
	{
		TList->Change = false;
	}

	return Result;
}

// ���X�g���ɍs����ǉ�����
//     �߂�l : ����ɏ������I���������ǂ���( true:����I��  false:�G���[���� )
bool ToolList_AddObj(
	// ���X�g���n���h��
	int TListHandle,

	// �ǉ�����s�ɕ\�����镶����
	const char *String,
	
	// �ό�����
	...
)
{
	SToolList *TList = g_TList[ TListHandle ];
	va_list    VaList;
	char       FixString[ 1024 ];
	int        StrLength;
	int        DrawWidth;
	int        i;
	void      *NewBuf;
	
	// �ό�������ǉ����镶������\�z����
	va_start( VaList , String );
	vsprintf( FixString , String , VaList );
	va_end( VaList );

	// ���ɒǉ��ł���ő�s���ɒB���Ă�����ő�s���𑝂₷
	if( TList->ObjNum == TList->MaxObjNum )
	{
		// �s�̏����i�[���郁�����̈��V���Ɋm��
		NewBuf = malloc( sizeof( char * ) * ( TList->MaxObjNum + ADD_OBJPOINTER_NUM ) );
		if( NewBuf == NULL )
		{
			return false;
		}

		// ���܂ł̍s�̏�񂪂���ꍇ�̓R�s�[
		if( TList->ObjList != NULL )
		{
			if( TList->ObjNum != 0 )
			{
				memcpy( NewBuf, TList->ObjList, sizeof( char * ) * TList->ObjNum );
			}

			// ���܂ōs�̏����i�[���Ă����������̈�����
			free( TList->ObjList );
		}

		// �V�����s�̏����i�[���邽�߂̃������̈��ۑ�
		TList->ObjList = ( char ** )NewBuf;

		// �ǉ��ł���s�̍ő吔�𑝂₷
		TList->MaxObjNum += ADD_OBJPOINTER_NUM;
	}

	// �ǉ����镶����̒������擾
	StrLength = strlen( FixString );

	// �ǉ����镶���񂪕������ۑ����Ă����o�b�t�@�̃T�C�Y�𒴂��Ă��܂��ꍇ��
	// �������ۑ����Ă����o�b�t�@�̃T�C�Y��傫������
	if( TList->StringBufferUseSize + StrLength + 1 >= TList->StringBufferSize )
	{
		// �������ۑ����邽�߂̃o�b�t�@��V���Ɋm�ۂ���
		NewBuf = ( char * )malloc( sizeof( char ) *
			( TList->StringBufferSize + StrLength + 1 + ADD_STRINGBUFFER_SIZE ) );
		if( NewBuf == NULL )
		{
			return false;
		}

		// ���܂ł̕����񂪂���ꍇ�͓��e���R�s�[
		if( TList->StringBuffer != NULL )
		{
			if( TList->StringBufferUseSize != 0 )
			{
				memcpy( NewBuf, TList->StringBuffer, TList->StringBufferUseSize );
			}

			// ���܂ł̃o�b�t�@���w���Ă����s�̃A�h���X����
			// �V�����o�b�t�@�p�ɕύX����
			for( i = 0; i < TList->ObjNum; i ++ )
			{
				TList->ObjList[ i ] =
					( char * )NewBuf + ( TList->ObjList[ i ] - TList->StringBuffer );
			}

			// ���܂ŕ������ۑ����Ă����o�b�t�@�����
			free( TList->StringBuffer );
		}
		
		// �V����������o�b�t�@�̃A�h���X��ۑ�
		TList->StringBuffer = ( char * )NewBuf;

		// ������o�b�t�@�̃T�C�Y�𑝂₷
		TList->StringBufferSize += StrLength + 1 + ADD_STRINGBUFFER_SIZE;
	}

	// �ǉ�����s�̕�����̉������擾����
	DrawWidth = GetDrawStringWidthToHandle(
					FixString, StrLength, ToolStatic_GetNormalFontHandle() );

	// ���܂ł̍ő剡�����傫���ꍇ�͍ő剡�����X�V
	if( DrawWidth > TList->Width )
	{
		TList->Width = DrawWidth;
	}

	// �V�����s�̏���ۑ�
	TList->ObjList[ TList->ObjNum ] = TList->StringBuffer + TList->StringBufferUseSize;
	memcpy( TList->ObjList[ TList->ObjNum ], FixString, StrLength + 1 );

	// �g�p���Ă��镶����o�b�t�@�̃T�C�Y�𑝂₷
	TList->StringBufferUseSize += StrLength + 1;

	// �s�̐��𑝂₷
	TList->ObjNum ++;

	// �E�C���h�E�̃N���C�A���g�̈���X�V
	ToolWindow_SetClientSize(
		TList->TWindowHandle, TList->Width, ToolList_GetHeight( TListHandle ) );

	// ��ڂ̍s�������ꍇ�́A�ǉ������s��I����Ԃɂ���
	if( TList->ObjNum == 1 )
	{
		ToolList_SetSelectIndex( TListHandle, 0 );
	}

	// ����I��
	return true;
}

// ���X�g���̕\����Ԃ�ύX����
void ToolList_SetVisible(
	// ���X�g���n���h��
	int TListHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// �V�����\����Ԃ�ۑ�����
	TList->Visible = Visible;
}

// ���X�g���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolList_GetVisible(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// �\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( !TList->Visible )
	{
		return false;
	}

	// �֘A�t�����Ă���E�C���h�E���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TList->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TList->TWindowHandle ) )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TList->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TList->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// ���X�g���̑I���s��ύX����
void ToolList_SetSelectIndex(
	// ���X�g���n���h��
	int TListHandle,
	
	// �s�ԍ�
	int Index
)
{
	SToolList *TList = g_TList[ TListHandle ];

	TList->SelectIndex = Index;
}

// ���X�g���̑I�����Ă���s���擾����
//     �߂�l : �I�����Ă���s�̔ԍ�( �ǂ̍s���I������Ă��Ȃ��ꍇ�� -1 )
int ToolList_GetSelectIndex(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->SelectIndex;
}

// ���X�g���ɒǉ������s�̐����擾����
int ToolList_GetObjNum(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->ObjNum;
}

// ���X�g���ɒǉ������S�Ă̍s�����킹���c�����擾����
//     �߂�l : ���X�g�̑S�s�̏c��( �s�N�Z���P�� )
int ToolList_GetHeight(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return ONE_LINE_HEIGHT * TList->ObjNum;
}

// ���X�g���ɒǉ������S�Ă̍s�����킹���������擾����
//     �߂�l : ���X�g�̑S�s�̉���( �s�N�Z���P�� )
int ToolList_GetWidth(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->Width;
}

// ���X�g���̏�Ԑ��ڏ������s��
static void ToolList_Step(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];
	int        MouseOffsetX;
	int        MouseOffsetY;
	int        Index;
	int        DrawX;
	int        DrawY;
	RECT       InfoArea;
	int        MouseX;
	int        MouseY;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolList_GetVisible( TListHandle ) )
	{
		return;
	}

	// �}�E�X�̍��{�^���������ꂽ�u�Ԃł͂Ȃ��ꍇ�͓��ɉ������Ȃ�
	if( ( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		return;
	}

	// �{�^���������ꂽ�u�Ԃ̃}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMouseEdgePosition( &MouseX, &MouseY );

	// �֘A�t������Ă���E�C���h�E���̏��\���̈���擾
	InfoArea = ToolWindow_GetInfoArea( TList->TWindowHandle );

	// �֘A�t������Ă���E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
	// �N���C�A���g�̈�̕`�挴�_�ƂȂ�X�N���[�����W���擾����
	ToolWindow_GetDrawLeftUpPos( TList->TWindowHandle, &DrawX, &DrawY );

	// ���X�g����ł̃}�E�X�̈ʒu���Z�o
	MouseOffsetX = MouseX - DrawX;
	MouseOffsetY = MouseY - DrawY;

	// �}�E�X�̃{�^�����������u�Ԃ����X�g�̗̈悩��O��Ă����牽�������I��
	if( MouseX < InfoArea.left || MouseX >= InfoArea.right  ||
		MouseY < InfoArea.top  || MouseY >= InfoArea.bottom ||
		MouseOffsetY                   <  0 ||
		MouseOffsetY / ONE_LINE_HEIGHT >= TList->ObjNum )
	{
		return;
	}

	// �}�E�X�J�[�\���̈ʒu�ɊY������s���Z�o
	Index = MouseOffsetY / ONE_LINE_HEIGHT;

	// �}�E�X�J�[�\���̈ʒu�ɂ���s�̕����񕔕��ɃJ�[�\�����Ȃ������牽�������ɏI��
	if( MouseOffsetX < 0 ||
		GetDrawStringWidthToHandle( TList->ObjList[ Index ], -1,
								ToolStatic_GetNormalFontHandle() ) <= MouseOffsetX )
	{
		return;
	}

	// �I�����Ă���s�̕ύX
	ToolList_SetSelectIndex( TListHandle, Index );

	// ��Ԃɕω������������ǂ����̃t���O�𗧂Ă�
	TList->Change = true;
}

// ���X�g������ʂɕ`�悷��
static void ToolList_Draw(
	// ���X�g���n���h��
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];
	int        WinDrawX;
	int        WinDrawY;
	int        i;
	int        ClientY;
	RECT       InfoArea;
	int        NormalColor;
	int        SelectColor;
	int        NormalFontHandle;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolList_GetVisible( TListHandle ) )
	{
		return;
	}

	// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
	// �N���C�A���g�̈�̕`�挴�_�ƂȂ�X�N���[�����W���擾����
	ToolWindow_GetDrawLeftUpPos( TList->TWindowHandle, &WinDrawX, &WinDrawY );

	// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
	// �N���C�A���g�̈�̕`�挴�_�ƂȂ���W���擾����
	ToolWindow_GetClientArea( TList->TWindowHandle, NULL, &ClientY );

	// �E�C���h�E�������� SetDrawArea �ŕ`��̈�𐧌�����ݒ���s��
	ToolWindow_SetupDrawArea( TList->TWindowHandle );

	// �E�C���h�E���̏��\���̈���擾����
	InfoArea = ToolWindow_GetInfoArea( TList->TWindowHandle );

	// �ʏ�̕�����̕`��F�ƑI������Ă��镶����̕`��F���擾����
	NormalColor = GetColor( 255,255,255 );
	SelectColor = GetColor( 255,  0,  0 );

	// ������`��p�̃t�H���g�n���h�����擾
	NormalFontHandle = ToolStatic_GetNormalFontHandle();

	// �s�̕`�惋�[�v
	for( i = ClientY / ONE_LINE_HEIGHT; i < TList->ObjNum; i ++ )
	{
		// �`��ʒu�����\���̈悩��O��Ă����烋�[�v�𔲂���
		if( i * ONE_LINE_HEIGHT + WinDrawY > InfoArea.bottom )
		{
			break;
		}

		// �s�̕������`��
		DrawStringToHandle(
			WinDrawX,
			WinDrawY + i * ONE_LINE_HEIGHT,
			TList->ObjList[ i ],
			TList->SelectIndex == i ? SelectColor : NormalColor,
			NormalFontHandle
		);
	}

	// �`��\�͈͂����ɖ߂�
	SetDrawArea( 0, 0, 999999, 999999 );
}

// �S�Ẵ��X�g���̏�Ԑ��ڏ������s��
void ToolList_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TListNum; i++ )
	{
		if( g_TList[ i ] == NULL )
		{
			continue;
		}

		ToolList_Step( i );
		j++;
	}
}

// �S�Ẵ��X�g���̕`��ڏ������s��
void ToolList_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TListNum; i++ )
	{
		if( g_TList[ i ] == NULL )
		{
			continue;
		}

		ToolList_Draw( i );
		j++;
	}
}

