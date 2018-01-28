#include "ToolLib_ParamBar.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "ToolLib_StringInput.h"
#include "DxLib.h"
#include <malloc.h>
#include <string.h>

// �p�����[�^�o�[���̍ő吔
#define TOOL_PARAMBAR_MAX			(256)

// �o�[�̂܂݂̍���
#define BAR_CURSOR_HEIGHT			(12)

// �o�[�̂܂݂̕��̔���
#define BAR_CURSOR_HALF_WIDTH		(3)

// �o�[��Y���W
#define BAR_Y						(5)

// �p�����[�^�o�[�\����
typedef struct _SToolParamBar
{
	// �֘A�t�����Ă���\����ԏ��n���h��
	int    TVisibleHandle;

	// �֘A�t�����Ă���E�C���h�E���n���h��
	int    TWindowHandle;

	// ���l���ڕҏW�p�̕�������͏��n���h��
	int    NumberTStringInputHandle;

	// ���O�̕����񂪊i�[����Ă��郁�����A�h���X��ۑ����Ă���|�C���^
	char  *Name;

	// �p�����[�^�o�[�̕\���ʒu
	int    PositionX;
	int    PositionY;

	// �o�[��\�����鑊��X���W
	int    BarPositionX;

	// �o�[�̉���
	int    BarWidth;

	// �o�[�ő���ł���͈͂̒l�̍ŏ��l�ƍő�l
	float  ParamMin;
	float  ParamMax;

	// ���݂̐��l
	float  Param;

	// ���l�ɕω������������ǂ���
	bool   Change;

	// �o�[�̂܂݂�͂�ł��邩�ǂ���
	bool   Grips;

	// �o�[�̂܂݂�͂񂾂Ƃ��̃o�[���[����݂�X���W
	int    GripsX;
} SToolParamBar;

static int            g_TParamBarNum;
static SToolParamBar *g_TParamBar[ TOOL_PARAMBAR_MAX ];

// �p�����[�^�o�[���̏�Ԑ��ڏ������s��
static void ToolParamBar_Step(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �p�����[�^�o�[������ʂɕ`�悷��
static void ToolParamBar_Draw(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �p�����[�^�o�[�����쐬����
//     �߂�l : �p�����[�^�o�[���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolParamBar_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,

	// �\�����W
	int PositionX,
	int PositionY,

	// �o�[��\�����鑊��X���W
	int BarPositionX,

	// �o�[�̉���
	int BarWidth,

	// �\�����鐔�l�̌���
	int NumberLength,
	
	// �o�[�ő���ł���͈͂̒l�̍ŏ��l�ƍő�l
	float ParamMin,
	float ParamMax,

	// �����l
	float InitParam
)
{
	int            NewHandle;
	SToolParamBar *TParamBar;

	// ���Ƀp�����[�^�o�[��񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TParamBarNum == TOOL_PARAMBAR_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TParamBar[ NewHandle ]; NewHandle++ ){}

	// �V�����{�^�������i�[���邽�߂̃������̊m��
	TParamBar = ( SToolParamBar * )calloc( 1, sizeof( SToolParamBar ) );
	if( TParamBar == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TParamBar[ NewHandle ] = TParamBar;

	// �p�����[�^�o�[���̐��𑝂₷
	g_TParamBarNum++; 

	// ����������
	TParamBar->TVisibleHandle = -1;
	TParamBar->TWindowHandle  = -1;
	TParamBar->Param          = InitParam;
	TParamBar->PositionX      = PositionX;
	TParamBar->PositionY      = PositionY;
	TParamBar->BarWidth       = BarWidth;
	TParamBar->ParamMin       = ParamMin;
	TParamBar->ParamMax       = ParamMax;
	TParamBar->Change         = false;
	TParamBar->Grips          = false;
	TParamBar->BarPositionX   = BarPositionX;
	if( Name )
	{
		TParamBar->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TParamBar->Name == NULL )
		{
			return -1;
		}
		strcpy( TParamBar->Name, Name );
	}

	// ���l�ҏW�p�̕�������͏����쐬����
	TParamBar->NumberTStringInputHandle = ToolStringInput_Create(
		NULL,
		PositionX + BarPositionX + BarWidth + 8,
		PositionY,
		true,
		NumberLength,
		0
	);
	if( TParamBar->NumberTStringInputHandle == -1 )
	{
		return -1;
	}

	// �����l��ݒ�
	ToolStringInput_SetFloatNumber( TParamBar->NumberTStringInputHandle, InitParam );

	// �n���h����Ԃ�
	return NewHandle;
}

// �p�����[�^�o�[�����폜����
extern void ToolParamBar_Delete(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ���O�ۑ��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
	if( TParamBar->Name )
	{
		free( TParamBar->Name );
		TParamBar->Name = NULL;
	}

	// ���l�ҏW�p�̕�������͏��n���h�����폜����
	ToolStringInput_Delete( TParamBar->NumberTStringInputHandle );

	// �m�ۂ��Ă������������������
	free( TParamBar );
	g_TParamBar[ TParamBarHandle ] = NULL;

	// �p�����[�^�o�[���̐������炷
	g_TParamBarNum--;
}

// �S�Ẵp�����[�^�o�[�����폜����
void ToolParamBar_Delete_All( void )
{
	int i;

	// �L���ȑS�Ẵp�����[�^�o�[���폜����
	for( i = 0; i < TOOL_PARAMBAR_MAX; i++ )
	{
		if( g_TParamBar[ i ] != NULL )
		{
			ToolParamBar_Delete( i );
		}
	}
}

// �p�����[�^�o�[���Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolParamBar_SetVisibleHandle(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// �\����ԏ��n���h����ۑ�
	TParamBar->TVisibleHandle = TVisibleHandle;

	// ���l�ҏW�p�̕�������͏��n���h���ɂ��ݒ肷��
	ToolStringInput_SetVisibleHandle(
			TParamBar->NumberTStringInputHandle, TVisibleHandle );
}

// �p�����[�^�o�[���̕\����Ԃ�ύX����
extern void ToolParamBar_SetVisible(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ���l�ҏW�p�̕�������͏��n���h���ɑ΂��ĕ\��Ԑݒ���s��
	ToolStringInput_SetVisible( TParamBar->NumberTStringInputHandle, Visible );
}

// �p�����[�^�o�[���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolParamBar_GetVisible(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ���l�ҏW�p�̕�������͏��n���h������\����Ԃ������� false ��Ԃ�
	if( !ToolStringInput_GetVisible( TParamBar->NumberTStringInputHandle ) )
	{
		return false;
	}

	// �֘A�t�����Ă���E�C���h�E���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TParamBar->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TParamBar->TWindowHandle ) )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TParamBar->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TParamBar->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// �p�����[�^�o�[���Ɗ֘A�t����E�C���h�E����ݒ肷��
extern void ToolParamBar_SetWindow(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// �E�C���h�E���n���h����ۑ�
	TParamBar->TWindowHandle = TWindowHandle;

	// ���l�ҏW�p�̕�������͏��n���h���ɂ��E�C���h�E���n���h�����Z�b�g
	ToolStringInput_SetWindow(
		TParamBar->NumberTStringInputHandle, TParamBar->TWindowHandle );
}

// �p�����[�^�o�[���̍��W��ύX����
extern void ToolParamBar_SetPosition(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �ύX��̍��W
	int PositionX,
	int PositionY
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ���W��ۑ�
	TParamBar->PositionX = PositionX;
	TParamBar->PositionY = PositionY;

	// ���l�ҏW�p�̕�������͏��n���h���̍��W���ύX
	ToolStringInput_SetPosition(
		TParamBar->NumberTStringInputHandle,
		TParamBar->PositionX + TParamBar->BarPositionX + TParamBar->BarWidth + 8,
		TParamBar->PositionY
	);
}

// �p�����[�^�o�[���̐��l��ύX����
extern void ToolParamBar_SetParam(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �ύX��̐��l
	float Param
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// �p�����[�^��ۑ�
	TParamBar->Param = Param;

	// ���l�ҏW�p�̕�������͏��n���h���ɂ����l���Z�b�g
	ToolStringInput_SetFloatNumber( TParamBar->NumberTStringInputHandle, Param );
}

// �p�����[�^�o�[���̃o�[�ŕύX�ł��鐔�l�̍ő�l�ƍŏ��l��ύX����
extern void ToolParamBar_SetMinMax(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �V�����ŏ��l�ƍő�l
	float ParamMin,
	float ParamMax
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	TParamBar->ParamMin = ParamMin;
	TParamBar->ParamMax = ParamMax;
}

// �p�����[�^�o�[���̐��l�ҏW���s���Ă����ꍇ�ɖ������ŏI��������
extern void ToolParamBar_AlwaysEnd(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	// ���l�ҏW�p�̕�������͏��n���h���ɑ΂��Ė������I���̏������s��
	ToolStringInput_AlwaysEnd( TParamBar->NumberTStringInputHandle );
}

// �p�����[�^�o�[���̐��l�ɕω������������ǂ������擾����
//     �߂�l : ���l�ɕω������������ǂ���( true:�ω����� false:�ω��Ȃ� )
extern bool ToolParamBar_CheckChange(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	bool Result;

	// ���l�ɕω������������ǂ����̃t���O�����[�J���ϐ��ɃR�s�[���Ă���
	Result = TParamBar->Change;

	// ���l�ɕω������������ǂ����̃t���O��|��
	TParamBar->Change = false;

	// �R�s�[���Ă������ϐ��̓��e��Ԃ�
	return Result;
}

// �p�����[�^�o�[���̌��݂̐��l���擾����
//     �߂�l : �p�����[�^�o�[�̒l
extern float ToolParamBar_GetParam(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];

	return TParamBar->Param;
}

// �p�����[�^�o�[���̏�Ԑ��ڏ������s��
static void ToolParamBar_Step(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	int            CursorX;
	int            AreaX;
	int            AreaY;
	int            WinDrawX;
	int            WinDrawY;
	float          TempF;
	bool           In;
	int            MouseX;
	int            MouseY;

	// ��\����Ԃ������ꍇ�͉��������I��
	if( !ToolParamBar_GetVisible( TParamBarHandle ) )
	{
		return;
	}

	// �}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// ���l�ҏW�p��������͏��ɕω����������ꍇ�̓o�[�ɂ����f����
	if( ToolStringInput_CheckEnd( TParamBar->NumberTStringInputHandle, true ) )
	{
		// ���l�ɕω������������ǂ����̃t���O�𗧂Ă�
		TParamBar->Change = true;

		// ���l�ҏW�p��������͏��̌��݂̐��l���擾����
		TempF = ToolStringInput_GetFloatNumber( TParamBar->NumberTStringInputHandle );

		// ���l���ŏ��l��ő�l�͈̔͂���͂ݏo�Ă�����␳����
		if( TempF < TParamBar->ParamMin )
		{
			TempF = TParamBar->ParamMin;
		}
		if( TempF > TParamBar->ParamMax )
		{
			TempF = TParamBar->ParamMax;
		}

		// ���l��ݒ肷��֐��Œl�𔽉f
		ToolParamBar_SetParam( TParamBarHandle, TempF );
	}

	// �}�E�X�̍��{�^����������Ă��Ȃ�������
	// �o�[�̂܂݂�͂�ł��邩�ǂ����̃t���O��|���ďI��
	if( ( ToolStatic_GetMouseButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		TParamBar->Grips = false;
		return;
	}

	// �o�[�̂܂݂�͂�ł��Ȃ��ꍇ��
	// �}�E�X�̍��{�^�����������u�Ԃł͂Ȃ��ꍇ�͂����ŏI��
	if( !TParamBar->Grips &&
		( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		return;
	}

	// �E�C���h�E���Ɋ֘A�t�����Ă���ꍇ�̓E�C���h�E���̍��W��
	// �E�C���h�E���Ƀ}�E�X�J�[�\�������邩�ǂ����Ȃǂ��擾����
	if( TParamBar->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TParamBar->TWindowHandle, &WinDrawX, &WinDrawY );
		In = ToolWindow_CheckInfoAreaIn( TParamBar->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
		In       = true;
	}

	// �o�[�\���̈�̍�����W���Z�o
	AreaX = TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX;
	AreaY = TParamBar->PositionY + WinDrawY;

	// �܂݂̈ʒu���Z�o
	CursorX = ( int )( ( TParamBar->Param - TParamBar->ParamMin ) * TParamBar->BarWidth /
										( TParamBar->ParamMax - TParamBar->ParamMin ) );

	// �܂݂�͂�ł��邩�ǂ����ŏ����𕪊�
	if( !TParamBar->Grips )
	{
		// �܂݂�͂�ł��Ȃ��ꍇ�̓o�[�̕\���̈�ɃJ�[�\���������ꍇ�͏I��
		if( !In )
		{
			return;
		}

		// �o�[�̕\���̈悩�炸��Ă�����I��
		if( MouseX < AreaX - BAR_CURSOR_HALF_WIDTH ||
			MouseX > AreaX + BAR_CURSOR_HALF_WIDTH + TParamBar->BarWidth ||
		    MouseY < AreaY ||
			MouseY > AreaY + BAR_CURSOR_HEIGHT )
		{
			return;
		}

		// �܂݂̊O�����N���b�N�����ꍇ�͂܂݂̈ʒu���N���b�N�����ʒu�Ɉړ�����
		if( MouseX <  CursorX + AreaX - BAR_CURSOR_HALF_WIDTH ||
			MouseX >= CursorX + AreaX + BAR_CURSOR_HALF_WIDTH )
		{
			CursorX           = MouseX - AreaX;
			TParamBar->GripsX = 0;
		}
		else
		{
			TParamBar->GripsX = MouseX - ( AreaX + CursorX );
		}

		// �܂݂�͂�ł��邩�ǂ����̃t���O�𗧂Ă�
		TParamBar->Grips = true;
	}
	else
	{
		// �܂݂�͂�ł���ꍇ�͒͂ݎn�߂��}�E�X�J�[�\���ʒu��
		// ���݂̃}�E�X�J�[�\���ʒu���猻�݂̂܂݂̈ʒu�����肷��
		if( MouseX - TParamBar->GripsX < AreaX - BAR_CURSOR_HALF_WIDTH )
		{
			CursorX = 0;
		}
		else
		if( MouseX - TParamBar->GripsX >
					AreaX + TParamBar->BarWidth + BAR_CURSOR_HALF_WIDTH )
		{
			CursorX = TParamBar->BarWidth;
		}
		else
		{
			CursorX = MouseX - TParamBar->GripsX - AreaX;

			if( CursorX < 0 )
			{
				CursorX = 0;
			}

			if( CursorX > TParamBar->BarWidth ) 
			{
				CursorX = TParamBar->BarWidth;
			}
		}
	}

	// �܂݂̈ʒu�ɉ����ăp�����[�^���Z�o����
	TempF = CursorX * ( TParamBar->ParamMax - TParamBar->ParamMin ) /
											TParamBar->BarWidth + TParamBar->ParamMin;

	// �l���ω����Ă�����l���ω��������ǂ����̃t���O�𗧂āA
	// ���l�ҏW�p�̕�������͏��ɂ����f����
	if( TempF != TParamBar->Param )
	{
		TParamBar->Change = true;
		TParamBar->Param  = TempF;
		ToolStringInput_SetFloatNumber(
					TParamBar->NumberTStringInputHandle, TParamBar->Param );
	}
}

// �p�����[�^�o�[������ʂɕ`�悷��
static void ToolParamBar_Draw(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
)
{
	SToolParamBar *TParamBar = g_TParamBar[ TParamBarHandle ];
	int            CursorX;
	int            WinDrawX;
	int            WinDrawY;

	// ��\���ݒ�ɂȂ��Ă����牽�������ɏI��
	if( !ToolParamBar_GetVisible( TParamBarHandle ) )
	{
		return;
	}

	// �E�C���h�E���Ɋ֘A�t�����Ă���ꍇ�̓E�C���h�E���̍��W���擾����
	if( TParamBar->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TParamBar->TWindowHandle, &WinDrawX, &WinDrawY );

		// �E�C���h�E������ɂ����`��͈͐����ݒ���s��
		ToolWindow_SetupDrawArea( TParamBar->TWindowHandle );
	}
	else
	{
		WinDrawX = 0;
		WinDrawY = 0;
	}

	// ���O������ꍇ�͖��O��`�悷��
	if( TParamBar->Name != NULL )
	{
		DrawStringToHandle(
			TParamBar->PositionX + WinDrawX,
			TParamBar->PositionY + WinDrawY,
			TParamBar->Name,
			GetColor( 255,255,255 ),
			ToolStatic_GetNormalFontHandle()
		);
	}

	// �o�[�̂܂݂̈ʒu���Z�o
	CursorX = ( int )( ( TParamBar->Param - TParamBar->ParamMin ) * TParamBar->BarWidth /
										( TParamBar->ParamMax - TParamBar->ParamMin ) );

	// �o�[��`��
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
	DrawBox(
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX,
		TParamBar->PositionY + WinDrawY + BAR_Y,
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + TParamBar->BarWidth,
		TParamBar->PositionY + WinDrawY + BAR_Y + 1,
		GetColor( 255,255,255 ),
		TRUE
	);

	// �o�[�̂܂݂�`��
	DrawBox(
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + CursorX - BAR_CURSOR_HALF_WIDTH,
		TParamBar->PositionY + WinDrawY,
		TParamBar->PositionX + WinDrawX + TParamBar->BarPositionX + CursorX + BAR_CURSOR_HALF_WIDTH,
		TParamBar->PositionY + WinDrawY + BAR_CURSOR_HEIGHT,
		GetColor( 255,255,255 ),
		TRUE
	);

	// �`��ݒ�����ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	SetDrawArea( 0, 0, 999999, 999999 );
}

// �S�Ẵp�����[�^�o�[���̏�Ԑ��ڏ������s��
void ToolParamBar_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TParamBarNum; i++ )
	{
		if( g_TParamBar[ i ] == NULL )
		{
			continue;
		}

		ToolParamBar_Step( i );
		j++;
	}
}

// �S�Ẵp�����[�^�o�[���̕`��ڏ������s��
void ToolParamBar_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TParamBarNum; i++ )
	{
		if( g_TParamBar[ i ] == NULL )
		{
			continue;
		}

		ToolParamBar_Draw( i );
		j++;
	}
}

