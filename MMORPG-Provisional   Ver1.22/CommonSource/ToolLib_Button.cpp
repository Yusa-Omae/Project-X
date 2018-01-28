#include "ToolLib_Button.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "DxLib.h"
#include <string.h>

// �쐬�ł���{�^�����̍ő吔
#define TOOL_BUTTON_MAX			(256)

// �{�^�����s�[�g�̍ŏ��̑҂�����
#define FIRST_REPEAT_WAIT		(0.2f)

// �Q��ڈȍ~�̃{�^�����s�[�g�̊Ԋu
#define REPEAT_WAIT				(0.017f)

// �{�^���\����
typedef struct _SToolButton
{
	// �֘A�t�����Ă���\����ԏ��n���h��
	int    TVisibleHandle;

	// �֘A�t�����Ă���E�C���h�E���n���h��
	int    TWindowHandle;

	// �n�m��ԂƂn�e�e��Ԃ��������тɐ؂�ւ��^�C�v�̃{�^�����ǂ���
	bool   On_Off_SwitchType;

	// �{�^���̈ʒu
	POINT  Position;

	// �{�^���̃T�C�Y
	SIZE   Size;

	// �{�^���̖��O�̕����񂪊i�[����Ă��郁�����A�h���X��ۑ����Ă���|�C���^
	char  *Name;

	// �\�����
	bool   Visible;

	// �{�^����������Ԃ��ǂ���
	bool   Bottom;

	// �{�^������������Ă��鎞��
	float  BottomTime;

	// �ŏ��̃��s�[�g�������s��ꂽ���ǂ���
	bool   FirstRepeat;

	// ���s�[�g���͂��l�������{�^���������
	bool   RepeatBottom;

	// �{�^�����N���b�N���ꂽ���ǂ���
	bool   Click;

	// �{�^���̂n�m�^�n�e�e���
	bool   OnOffFlag;
} SToolButton;

static int          g_TButtonNum;
static SToolButton *g_TButton[ TOOL_BUTTON_MAX ];

// �{�^�����̏�Ԑ��ڏ������s��
static void ToolButton_Step(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �i�߂鎞��( �P�� : �b )
	float StepTime
);

// �{�^��������ʂɕ`�悷��
static void ToolButton_Draw(
	// �{�^�����n���h��
	int TButtonHandle
);


// �{�^�������쐬����
//     �߂�l : �{�^�����n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolButton_Create(
	// �n�m��ԂƂn�e�e��Ԃ��������тɐ؂�ւ��^�C�v�̃{�^�����ǂ���
	bool On_Off_SwitchType,

	// ���O�ƂȂ镶����̃A�h���X
	const char *Name,
	
	// �{�^����\�����鍶����W
	int x,
	int y,
	
	// �{�^���̕��ƍ���
	int w,
	int h
)
{
	int          NewHandle;
	SToolButton *TButton;

	// ���Ƀ{�^����񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TButtonNum == TOOL_BUTTON_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TButton[ NewHandle ]; NewHandle++ ){}

	// �V�����{�^�������i�[���邽�߂̃������̊m��
	TButton = ( SToolButton * )calloc( 1, sizeof( SToolButton ) );
	if( TButton == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TButton[ NewHandle ] = TButton;

	// �{�^�����̐��𑝂₷
	g_TButtonNum++; 

	// ����������
	TButton->TVisibleHandle    = -1;
	TButton->On_Off_SwitchType = On_Off_SwitchType;
	TButton->TWindowHandle     = -1;
	TButton->Position.x        = x;
	TButton->Position.y        = y;
	TButton->Size.cx           = w;
	TButton->Size.cy           = h;
	TButton->Bottom            = false;
	TButton->BottomTime        = 0.0f;
	TButton->FirstRepeat       = false;
	TButton->Click             = false;
	TButton->OnOffFlag         = false;
	TButton->Visible           = true;
	if( !ToolButton_SetName( NewHandle, Name ) )
	{
		return -1;
	}

	// �n���h����Ԃ�
	return NewHandle;
}

// �{�^�������폜����
void ToolButton_Delete(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// ���O�ۑ��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
	if( TButton->Name )
	{
		free( TButton->Name );
		TButton->Name = NULL;
	}

	// �m�ۂ��Ă������������������
	free( TButton );
	g_TButton[ TButtonHandle ] = NULL;

	// �{�^�����̐������炷
	g_TButtonNum--;
}

// �S�Ẵ{�^�������폜����
void ToolButton_Delete_All( void )
{
	int i;

	// �L���ȑS�Ẵ{�^�����폜����
	for( i = 0; i < TOOL_BUTTON_MAX; i++ )
	{
		if( g_TButton[ i ] != NULL )
		{
			ToolButton_Delete( i );
		}
	}
}

// �{�^�����Ɋ֘A�t����\����ԏ����Z�b�g����
void ToolButton_SetVisibleHandle(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->TVisibleHandle = TVisibleHandle;
}

// �{�^�����̖��O��ݒ肷��
//     �߂�l : ����������ɏI���������ǂ���( true:����ɏI������  false:�G���[���� )
bool ToolButton_SetName(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// ���O�ƂȂ镶����̃A�h���X
	const char *Name
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// ���ɖ��O�p�Ƀ��������m�ۂ��Ă����ꍇ�͉������
	if( TButton->Name )
	{
		free( TButton->Name );
		TButton->Name = NULL;
	}

	// ���O�̃A�h���X���L���ȏꍇ�̓��������m�ۂ��ĕۑ�����
	if( Name != NULL )
	{
		TButton->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TButton->Name == NULL )
		{
			return false;
		}
		strcpy( TButton->Name, Name );
	}

	// ����I��
	return true;
}

// �{�^�����Ɗ֘A�t����E�C���h�E����ݒ肷��
void ToolButton_SetWindow(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->TWindowHandle = TWindowHandle;
}

// �{�^�����̈ʒu��ύX����
void ToolButton_SetPosition(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �ύX��̃{�^���̍��W
	int x,
	int y
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->Position.x = x;
	TButton->Position.y = y;
}

// �{�^�����̂n�m�^�n�e�e��Ԃ�ύX����
void ToolButton_SetOnOffFlag(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �ύX��̂n�m�^�n�e�e���( true:ON  false:OFF )
	bool Flag
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	TButton->OnOffFlag = Flag;
}

// �{�^�����̂n�m�^�n�e�e��Ԃ��擾����
//     �߂�l : �n�m�^�n�e�e���( ture:ON  false:OFF )
bool ToolButton_GetOnOffFlag(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->OnOffFlag;
}

// �{�^�����̃{�^�����N���b�N���ꂽ���ǂ������擾����
//     �߂�l : �{�^�����N���b�N���ꂽ���ǂ���
//              ( true:�N���b�N���ꂽ  false:�N���b�N����Ă��Ȃ� )
bool ToolButton_GetClick(
	// �{�^�����n���h��
	int TButtonHandle,

	// �N���b�N���ꂽ���ǂ����̏�Ԃ����Z�b�g���邩�ǂ���
	// ( true:���Z�b�g����  false:���Z�b�g���Ȃ� )
	bool Reset
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	bool         Ret;

	Ret = TButton->Click;
	if( Reset )
	{
		TButton->Click = false;
	}

	return Ret;
}

//     �߂�l : �{�^������������Ă��邩�ǂ���
//              ( true:��������Ă���  false:��������Ă��Ȃ� )
bool ToolButton_GetBottom(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->Bottom;
}

// �{�^�����̃{�^������������Ă��邩�ǂ������擾����( ���s�[�g�@�\�t���� )
//     �߂�l : �{�^������������Ă��邩�ǂ���
//              ( true:��������Ă���  false:��������Ă��Ȃ� )
bool ToolButton_GetRepeatBottom(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	return TButton->RepeatBottom;
}

// �{�^�����̕\����Ԃ�ύX����
void ToolButton_SetVisible(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// �V�����\����Ԃ�ۑ�����
	TButton->Visible = Visible;
}

// �{�^�����̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolButton_GetVisible(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];

	// �\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( !TButton->Visible )
	{
		return false;
	}

	// �֘A�t�����Ă���E�C���h�E���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TButton->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TButton->TWindowHandle ) )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TButton->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TButton->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// �{�^�����̏�Ԑ��ڏ������s��
static void ToolButton_Step(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �i�߂鎞��( �P�� : �b )
	float StepTime
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	bool         In;
	bool         MouseEdgeInput;
	bool         MouseInput;
	bool         MouseRepeatInput;
	int          MouseX;
	int          MouseY;
	int          TempX;
	int          TempY;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolButton_GetVisible( TButtonHandle ) )
	{
		return;
	}

	// �}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// �}�E�X�̓��͏�Ԃ��擾
	MouseRepeatInput = false;
	MouseInput       = ( ToolStatic_GetMouseButton()     & MOUSE_INPUT_LEFT ) != 0;
	MouseEdgeInput   = ( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) != 0;

	// �{�^���̕\���̈�Ƀ}�E�X�J�[�\�������邩�ǂ����𒲂ׂ�
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TButton->TWindowHandle, &TempX, &TempY );
		TempX += TButton->Position.x;
		TempY += TButton->Position.y;

		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy ) &&
				ToolWindow_CheckInfoAreaIn(
				TButton->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		In = ToolStatic_CheckMouseIn(
				TButton->Position.x,
				TButton->Position.y,
				TButton->Position.x + TButton->Size.cx,
				TButton->Position.y + TButton->Size.cy );
	}

	// ���{�^����������Ă��邩�ǂ����ŏ����𕪊�
	if( MouseInput )
	{
		// ���{�^����������Ă���ꍇ�̓��s�[�g�������s��
		TButton->BottomTime += StepTime;
		if( TButton->FirstRepeat )
		{
			if( TButton->BottomTime >= REPEAT_WAIT )
			{
				MouseRepeatInput      = true;
				TButton->BottomTime  -= REPEAT_WAIT;
			}
		}
		else
		{
			if( TButton->BottomTime >= FIRST_REPEAT_WAIT )
			{
				MouseRepeatInput      = true;
				TButton->FirstRepeat  = true;
				TButton->BottomTime  -= FIRST_REPEAT_WAIT;
			}
		}

		// ���{�^���������ꂽ�u�Ԃ͕K�����s�[�g���͍l���̃{�^��������Ԃ� true �ɂ���
		if( MouseEdgeInput )
		{
			MouseRepeatInput = true;
		}

		// �{�^���\���̈�ɃJ�[�\��������A���{�^���������ꂽ�u�Ԃ̏ꍇ��
		// �{�^��������Ԃɂ���
		if( MouseEdgeInput && In )
		{
			TButton->Bottom = true;
		}
	}
	else
	{
		// ���{�^����������Ă��Ȃ��ꍇ�̓��s�[�g���������Z�b�g����
		TButton->FirstRepeat = false;
		TButton->BottomTime  = 0.0f;

		// �}�E�X�J�[�\�����{�^���̕\���̈�ɂ���A�����܂Ń{�^�����������
		// �������ꍇ�̓N���b�N�t���O�𗧂āA�n�m�^�n�e�e��Ԃ��n�m��Ԃɂ���
		if( In && TButton->Bottom )
		{
			TButton->Click     = true;
			TButton->OnOffFlag = true;
		}

		// �{�^��������Ԃ����Z�b�g����
		TButton->Bottom      = false;
	}

	// ���s�[�g���͂�����Ă��āA���}�E�X�J�[�\�����{�^���̕\���̈��
	// �������烊�s�[�g���͍l���̃{�^��������Ԃ� true �ɂ���
	if( MouseRepeatInput && In )
	{
		TButton->RepeatBottom = true;
	}
	else
	{
		TButton->RepeatBottom = false;
	}
}

// �{�^��������ʂɕ`�悷��
static void ToolButton_Draw(
	// �{�^�����n���h��
	int TButtonHandle
)
{
	SToolButton *TButton = g_TButton[ TButtonHandle ];
	int          MouseX;
	int          MouseY;
	bool         In;
	int          TempX;
	int          TempY;
	int          TopLeftC1;
	int          TopLeftC2;
	int          RightBottomC1;
	int          RightBottomC2;
	int          CenterColor;
	int          DrawWidth;
	int          addx;
	int          addy;
	int          ButtonFontHandle;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolButton_GetVisible( TButtonHandle ) )
	{
		return;
	}

	// �}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// �{�^�����`��p�̃t�H���g�n���h�����擾
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// �{�^�����̕`�敝���擾
	DrawWidth = GetDrawStringWidthToHandle( TButton->Name, -1, ButtonFontHandle );

	// �{�^���̕\���ʒu�ƃ{�^���̕\���̈�Ƀ}�E�X�J�[�\�������邩�ǂ������擾
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TButton->TWindowHandle, &TempX, &TempY );
		TempX += TButton->Position.x;
		TempY += TButton->Position.y;
		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy ) &&
			 ToolWindow_CheckInfoAreaIn(
				TButton->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		TempX = TButton->Position.x;
		TempY = TButton->Position.y;
		In = ToolStatic_CheckMouseIn(
				TempX,
				TempY,
				TempX + TButton->Size.cx,
				TempY + TButton->Size.cy );
	}

	// �{�^����������Ă��邩�A�Ⴕ���͂n�m�^�n�e�e�؂�ւ����[�h�̂n�m��Ԃ̏ꍇ��
	// ������Ă���{�^���̔z�F�ɂ���
	if( ( In && TButton->Bottom ) ||
		( TButton->On_Off_SwitchType && TButton->OnOffFlag ) )
	{
		addx          = 1;
		addy          = 1;
		CenterColor   = GetColor( 192,192,192 );
		TopLeftC1     = GetColor(   0,  0,  0 );
		TopLeftC2     = GetColor(  64, 64, 64 );
		RightBottomC1 = GetColor( 255,255,255 );
		RightBottomC2 = GetColor( 191,191,191 );
	}
	else
	{
		addx          = 0;
		addy          = 0;
		CenterColor   = GetColor( 255,255,255 );
		RightBottomC1 = GetColor(   0,  0,  0 );
		RightBottomC2 = GetColor(  64, 64, 64 );
		TopLeftC1     = GetColor( 255,255,255 );
		TopLeftC2     = GetColor( 191,191,191 );
	}

	// �{�^�����ɃE�C���h�E��񂪊֘A�t�����Ă���ꍇ�͕`��̈�̐����ݒ���s��
	if( TButton->TWindowHandle >= 0 )
	{
		ToolWindow_SetupDrawArea( TButton->TWindowHandle );
	}

	// �{�^���̕`��
	Tool_DrawButton(
		TempX,
		TempY,
		TButton->Size.cx,
		TButton->Size.cy,
		TopLeftC1,
		TopLeftC2,
		RightBottomC1,
		RightBottomC2,
		CenterColor
	);

	// �{�^�����̕`��
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 180 );
	DrawStringToHandle(
		TempX + ( TButton->Size.cx - DrawWidth               ) / 2 + addx,
		TempY + ( TButton->Size.cy - TOOL_BUTTON_FONT_HEIGHT ) / 2 + addy,
		TButton->Name,
		GetColor( 0,0,0 ),
		ButtonFontHandle
	);

	// �u�����h���[�h�ƕ`��\�̈�����ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	SetDrawArea( 0, 0, 999999, 999999 );
}


// �S�Ẵ{�^�����̏�Ԑ��ڏ������s��
void ToolButton_Step_All(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
)
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TButtonNum; i++ )
	{
		if( g_TButton[ i ] == NULL )
		{
			continue;
		}

		ToolButton_Step( i, StepTime );
		j++;
	}
}

// �S�Ẵ{�^�����̕`��ڏ������s��
void ToolButton_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TButtonNum; i++ )
	{
		if( g_TButton[ i ] == NULL )
		{
			continue;
		}

		ToolButton_Draw( i );
		j++;
	}
}

