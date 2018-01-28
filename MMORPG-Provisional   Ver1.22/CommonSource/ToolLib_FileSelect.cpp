#include "ToolLib_FileSelect.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Window.h"
#include "ToolLib_Button.h"
#include "ToolLib_StringInput.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include "DxLib.h"

// �t�@�C���I���������̍ő吔
#define TOOL_FILESELECT_MAX		(256)

// ���O��`�悷����W
#define NAME_X					(4)
#define NAME_Y					(0)

// �p�X�\���̈�̍�����W
#define PATH_X					(4)
#define PATH_Y					(18)

// �p�X�\���̈�̃T�C�Y
#define PATH_W					(242)
#define PATH_H					(17)

// �{�^���\���̈��Y���W
#define BUTTON_Y				(PATH_Y + PATH_H + 4)

// �{�^���\���̈�̃T�C�Y
#define BUTTON_W				(42)
#define BUTTON_H				(20)

// �u�Q�Ɓv�{�^����X���W
#define OPEN_X					(4)

// �u�K�p�v�{�^����X���W
#define APPLY_X					(OPEN_X + BUTTON_W + 4)

// �t�@�C���I���\����
typedef struct _SToolFileSelect
{
	// �֘A�t�����Ă���\����ԏ��n���h��
	int    TVisibleHandle;

	// �֘A�t�����Ă���E�C���h�E���n���h��
	int    TWindowHandle;

	// �u�Q�Ɓv�{�^�����n���h��
	int    OpenTButtonHandle;

	// �u�K�p�v�{�^�����n���h��
	int    ApplyTButtonHandle;

	// �t�@�C���p�X�ҏW�p��������͏��n���h��
 	int    TStringInputHandle;

	// �\�����W
	POINT  Position;

	// ���O�̕����񂪊i�[����Ă��郁�����A�h���X��ۑ����Ă���|�C���^
	char  *Name;

	// �\�����
	bool   Visible;

	// �t�@�C���I���������I�����Ă��邩�ǂ���
	bool   EndFlag;
} SToolFileSelect;

int              g_TFileSelectNum;
SToolFileSelect *g_TFileSelect[ TOOL_FILESELECT_MAX ];

// �t�@�C���I���������̏�Ԑ��ڏ������s��
static void ToolFileSelect_Step(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
);

// �t�@�C���I������������ʂɕ`�悷��
static void ToolFileSelect_Draw(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
);

// �t�@�C���I�����������쐬����
//     �߂�l : �t�@�C���I���������n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolFileSelect_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,
	
	// �t�@�C���I�������̕\����������W
	int x,
	int y
)
{
	int              NewHandle;
	SToolFileSelect *TFileSelect;

	// ���Ƀ{�^����񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TFileSelectNum == TOOL_FILESELECT_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TFileSelect[ NewHandle ]; NewHandle++ ){}

	// �V�����{�^�������i�[���邽�߂̃������̊m��
	TFileSelect = ( SToolFileSelect * )calloc( 1, sizeof( SToolFileSelect ) );
	if( TFileSelect == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TFileSelect[ NewHandle ] = TFileSelect;

	// ��������͏��̐��𑝂₷
	g_TFileSelectNum++; 

	// �u�Q�Ɓv�{�^�����쐬
	TFileSelect->OpenTButtonHandle  =
					ToolButton_Create( false, "�Q��", 0, 0, BUTTON_W, BUTTON_H );
	if( TFileSelect->OpenTButtonHandle == -1 )
	{
		return -1;
	}

	// �u�K�p�v�{�^�����쐬
	TFileSelect->ApplyTButtonHandle =
					ToolButton_Create( false, "�K�p", 0, 0, BUTTON_W, BUTTON_H ); 
	if( TFileSelect->ApplyTButtonHandle == -1 )
	{
		return -1;
	}

	// �t�@�C���p�X�ҏW�p��������͏����쐬
	TFileSelect->TStringInputHandle =
			ToolStringInput_Create( NULL, x + PATH_X, y + PATH_Y, false, 100, PATH_W );
	if( TFileSelect->TStringInputHandle == -1 )
	{
		return -1;
	}

	// ����������
	TFileSelect->TVisibleHandle = -1;
	TFileSelect->TWindowHandle  = -1;
	TFileSelect->Visible        = true;
	TFileSelect->EndFlag        = false;

	if( Name )
	{
		TFileSelect->Name = ( char * )malloc( sizeof( char ) * strlen( Name ) + 1 );
		if( TFileSelect->Name == NULL )
		{
			return -1;
		}
		strcpy( TFileSelect->Name, Name );
	}

	ToolFileSelect_SetPosition( NewHandle, x, y );

	// �n���h����Ԃ�
	return NewHandle;
}

// �t�@�C���I�����������폜����
void ToolFileSelect_Delete(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �{�^�����╶������͏��n���h���̍폜
	ToolButton_Delete( TFileSelect->OpenTButtonHandle );
	ToolButton_Delete( TFileSelect->ApplyTButtonHandle );
	ToolStringInput_Delete( TFileSelect->TStringInputHandle );

	// ���O�ۑ��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
	if( TFileSelect->Name )
	{
		free( TFileSelect->Name );
		TFileSelect->Name = NULL;
	}

	// �m�ۂ��Ă������������������
	free( TFileSelect );
	g_TFileSelect[ TFileSelectHandle ] = NULL;

	// ��������͏��̐������炷
	g_TFileSelectNum--;
}

// �S�Ẵt�@�C���I�����������폜����
void ToolFileSelect_Delete_All( void )
{
	int i;

	// �L���ȑS�Ẵt�@�C���I���������폜����
	for( i = 0; i < TOOL_FILESELECT_MAX; i++ )
	{
		if( g_TFileSelect[ i ] != NULL )
		{
			ToolFileSelect_Delete( i );
		}
	}
}

// �t�@�C���I���������Ɋ֘A�t����\����ԏ����Z�b�g����
void ToolFileSelect_SetVisibleHandle(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �\����ԏ��n���h���̕ۑ�
	TFileSelect->TVisibleHandle = TVisibleHandle;

	// �{�^�����╶������͏��ɂ��\����ԏ��n���h����o�^
	ToolButton_SetVisibleHandle( TFileSelect->OpenTButtonHandle, TVisibleHandle );
	ToolButton_SetVisibleHandle( TFileSelect->ApplyTButtonHandle, TVisibleHandle );
	ToolStringInput_SetVisibleHandle( TFileSelect->TStringInputHandle, TVisibleHandle );
}

// �t�@�C���I���������Ɗ֘A�t����E�C���h�E����ݒ肷��
void ToolFileSelect_SetWindow(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �E�C���h�E���n���h���̕ۑ�
	TFileSelect->TWindowHandle = TWindowHandle;

	// �{�^�����╶������͏��ɂ��E�C���h�E���n���h����o�^
	ToolButton_SetWindow( TFileSelect->OpenTButtonHandle, TWindowHandle );
	ToolButton_SetWindow( TFileSelect->ApplyTButtonHandle, TWindowHandle );
	ToolStringInput_SetWindow( TFileSelect->TStringInputHandle, TWindowHandle );
}

// �t�@�C���I���������̕\���ʒu��ύX����
void ToolFileSelect_SetPosition(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �ύX��̍��W
	int x,
	int y
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ���W��ۑ�
	TFileSelect->Position.x = x;
	TFileSelect->Position.y = y;

	// �{�^�����╶������͏��̍��W���ύX
	ToolButton_SetPosition(
		TFileSelect->OpenTButtonHandle,  x + OPEN_X,  y + BUTTON_Y );

	ToolButton_SetPosition(
		TFileSelect->ApplyTButtonHandle, x + APPLY_X, y + BUTTON_Y );

	ToolStringInput_SetPosition(
		TFileSelect->TStringInputHandle, x + PATH_X,  y + PATH_Y );
}

// �t�@�C���I���������̕\����Ԃ�ύX����
void ToolFileSelect_SetVisible(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ��\���̏ꍇ�͕�������̓n���h���̃A�N�e�B�u��Ԃ���������
	if( !Visible )
	{
		ToolStringInput_ResetActive( TFileSelect->TStringInputHandle );
	}

	// �V�����\����Ԃ�ۑ�����
	TFileSelect->Visible = Visible;

	// �{�^�����╶������͏��̕\����Ԃ��ύX
	ToolButton_SetVisible( TFileSelect->OpenTButtonHandle, Visible );
	ToolButton_SetVisible( TFileSelect->ApplyTButtonHandle, Visible );
	ToolStringInput_SetVisible( TFileSelect->TStringInputHandle, Visible );
}

// �t�@�C���I���������̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolFileSelect_GetVisible(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( !TFileSelect->Visible )
	{
		return false;
	}

	// �֘A�t�����Ă���E�C���h�E���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TFileSelect->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TFileSelect->TWindowHandle ) )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TFileSelect->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TFileSelect->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// �t�@�C���I���������̑I���������������Ă��邩�ǂ������擾����
//     �߂�l : �������������Ă��邩�ǂ���( true:�������Ă���  false:�܂��I����Ă��Ȃ� )
bool ToolFileSelect_CheckEnd(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,

	// �������������Ă��邩�ǂ����̃t���O��|�����ǂ���
	bool Reset
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];
	bool             Result;

	// �������������Ă��邩�ǂ����̃t���O�����[�J���ϐ��ɃR�s�[���Ă���
	Result = TFileSelect->EndFlag;

	// �t���O�����Z�b�g����w�肾�����ꍇ�͊����t���O��|��
	if( Reset )
	{
		TFileSelect->EndFlag = false;
	}

	// �������������Ă��邩�ǂ����̃t���O���R�s�[���Ă��������[�J���ϐ��̓��e��Ԃ�
	return Result;
}

// �t�@�C���I���������̃t�@�C���p�X��ݒ肷��
void ToolFileSelect_SetPath(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �t�@�C���p�X�����񂪊i�[���ꂽ�������̈�̐擪�A�h���X
	const char *FilePath
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �t�@�C���p�X�ҏW�p�̕�������͏��Ƀp�X���Z�b�g����
	ToolStringInput_SetString(
		TFileSelect->TStringInputHandle, FilePath == NULL ? "" : FilePath );
}

// �t�@�C���I���������̃t�@�C���p�X���擾����
void ToolFileSelect_GetPath(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �t�@�C���p�X���i�[����o�b�t�@�̐擪�A�h���X
	char *DestBuffer
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// �t�@�C���p�X�ҏW�p�̕�������͏��̕�������擾����
	ToolStringInput_GetString(
		TFileSelect->TStringInputHandle, DestBuffer );
}

// �t�@�C���I���������̏�Ԑ��ڏ������s��
static void ToolFileSelect_Step(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];

	// ��\����Ԃ̏ꍇ�͉��������I��
	if( !ToolFileSelect_GetVisible( TFileSelectHandle ) )
	{
		return;
	}

	// �t�@�C���p�X�ҏW�p�̕�������͏�񏈗����������Ă�����
	// �t�@�C���I��������������Ԃɂ���
	if( ToolStringInput_CheckEnd( TFileSelect->TStringInputHandle, true ) )
	{
		TFileSelect->EndFlag = true;
	}

	// �u�K�p�v�{�^���������ꂽ��t�@�C���I��������������Ԃɂ���
	if( ToolButton_GetClick( TFileSelect->ApplyTButtonHandle, true ) )
	{
		TFileSelect->EndFlag = true;
	}

	// �u�Q�Ɓv�{�^���������ꂽ��t�@�C���I���_�C�A���O��\������
	if( ToolButton_GetClick( TFileSelect->OpenTButtonHandle, true ) )
	{
		OPENFILENAME ofn;
		char         FullPath[ 1024 ];
		char         FileName[ 1024 ];

		memset( &ofn,     0, sizeof( OPENFILENAME ) );
		memset( FullPath, 0, sizeof( FullPath     ) );
		memset( FileName, 0, sizeof( FileName     ) );

		ofn.lStructSize    = sizeof( OPENFILENAME );
		ofn.hwndOwner      = GetMainWindowHandle();
		ofn.lpstrFilter    =
			"Sound File(*.wav *.ogg *.mp3)\0*.wav;*.ogg;*.mp3\0All File\0*.*\0\0";
		ofn.lpstrFile      = FullPath;
		ofn.lpstrFileTitle = FileName;
		ofn.nMaxFile       = sizeof( FullPath );
		ofn.nMaxFileTitle  = sizeof( FileName );
		ofn.Flags          = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt    = "";
		ofn.lpstrTitle     = "�J��";

		if( GetOpenFileName( &ofn ) != 0 )
		{
			// �t�@�C���I���_�C�A���O�őI�����ꂽ�t�@�C���̃p�X��
			// �t�@�C���p�X�ҏW�p��������͏��ɔ��f����
			ToolStringInput_SetString( TFileSelect->TStringInputHandle, FullPath );
		}
	}
}

// �t�@�C���I������������ʂɕ`�悷��
static void ToolFileSelect_Draw(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
)
{
	SToolFileSelect *TFileSelect = g_TFileSelect[ TFileSelectHandle ];
	int              WinDrawX;
	int              WinDrawY;

	// ��\����Ԃ̏ꍇ�͉��������I��
	if( !ToolFileSelect_GetVisible( TFileSelectHandle ) )
	{
		return;
	}

	// �E�C���h�E���Ɋ֘A�t�����Ă���ꍇ�̓E�C���h�E���̍��W���擾����
	if( TFileSelect->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TFileSelect->TWindowHandle, &WinDrawX, &WinDrawY );

		// �E�C���h�E������ɂ����`��͈͐����ݒ���s��
		ToolWindow_SetupDrawArea( TFileSelect->TWindowHandle );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
	}

	// ���O�̕`��
	DrawStringToHandle(
		TFileSelect->Position.x + NAME_X + WinDrawX,
		TFileSelect->Position.y + NAME_Y + WinDrawY,
		TFileSelect->Name,
		GetColor( 255,255,255 ),
		ToolStatic_GetNormalFontHandle()
	);

	// �`��͈͐��������ɖ߂�
	SetDrawArea( 0, 0, 999999, 999999 );
}

// �S�Ẵt�@�C���I���������̏�Ԑ��ڏ������s��
void ToolFileSelect_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TFileSelectNum; i++ )
	{
		if( g_TFileSelect[ i ] == NULL )
		{
			continue;
		}

		ToolFileSelect_Step( i );
		j++;
	}
}

// �S�Ẵt�@�C���I���������̕`��ڏ������s��
void ToolFileSelect_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TFileSelectNum; i++ )
	{
		if( g_TFileSelect[ i ] == NULL )
		{
			continue;
		}

		ToolFileSelect_Draw( i );
		j++;
	}
}


