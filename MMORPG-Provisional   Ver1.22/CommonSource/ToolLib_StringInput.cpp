#include "ToolLib_StringInput.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include "DxLib.h"
#include <string.h>
#include <stdlib.h>

// ��������͏��̍ő吔
#define TOOL_STRINGINPUT_MAX		(256)

// ��������͗̈�̏c��
#define STRINGINPUT_HEIGHT			(18)

// �����ꕶ���̉��̉���
#define ONE_NUMBER_SIZE				(7)

// ������o�b�t�@�̃T�C�Y
#define STRING_BUFFER_SIZE			(2049)

// ��������͍\����
typedef struct _SToolStringInput
{
	// �֘A�t�����Ă���\����ԏ��n���h��
	int    TVisibleHandle;

	// �֘A�t�����Ă���E�C���h�E���n���h��
	int    TWindowHandle;

	// ��������͂̕\���ʒu
	POINT  Position;

	// ���O�̕����񂪊i�[����Ă��郁�����A�h���X��ۑ����Ă���|�C���^
	char  *Name;

	// ���l�̂ݓ��͉\���ǂ���
	bool   NumberOnly;

	// ���͂ł��鐔�l�̌���( NumberOnly �� true �̏ꍇ�̂ݗL�� )
	int    NumberNum;

	// ��������͗̈�̉���( NumberOnly �� false �̏ꍇ�̂ݗL�� )
	int    StringDrawWidth;

	// ��������͂��������Ă��邩�ǂ���
	bool   EndFlag;

	// �\�����
	bool   Visible;

	// ���͕�����𐮐��l�ɕϊ������l
	int    IntNumber;

	// ���͕�����𕂓������_�l�ɕϊ������l
	float  FloatNumber;

	// ��������̓n���h��
	int    InputHandle;

	// ���͕�������擾����ۂɎg�p�����o�b�t�@
	char   InputString[ STRING_BUFFER_SIZE ];
} SToolStringInput;

static int               g_TStringInputNum;
static SToolStringInput *g_TStringInput[ TOOL_STRINGINPUT_MAX ];

// ��������͏��̏�Ԑ��ڏ������s��
//     �߂�l : ����������I���������ǂ���( true:����I�� false:�G���[���� )
static bool ToolStringInput_Step(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏�����ʂɕ`�悷��
static void ToolStringInput_Draw(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏����쐬����
//     �߂�l : ��������͏��n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolStringInput_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,
	
	// ��������͂̕\���ʒu
	int x,
	int y,

	// ���l�̂ݓ��͉\�ɂ��邩�ǂ���
	bool NumberOnly,

	// ���͂ł��鐔�l�̌���( NumberOnly �� true �̏ꍇ�̂ݗL�� )
	int NumberNum,

	// ��������͗̈�̉���( NumberOnly �� false �̏ꍇ�̂ݗL�� )
	int StringDrawWidth
)
{
	int               NewHandle;
	SToolStringInput *TStringInput;

	// ���Ƀ{�^����񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TStringInputNum == TOOL_STRINGINPUT_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TStringInput[ NewHandle ]; NewHandle++ ){}

	// �V�����{�^�������i�[���邽�߂̃������̊m��
	TStringInput = ( SToolStringInput * )calloc( 1, sizeof( SToolStringInput ) );
	if( TStringInput == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TStringInput[ NewHandle ] = TStringInput;

	// ��������͏��̐��𑝂₷
	g_TStringInputNum++; 

	// ����������
	TStringInput->TVisibleHandle  = -1;
	TStringInput->Visible         = true;
	TStringInput->IntNumber       = 0;
	TStringInput->FloatNumber     = 0;
	TStringInput->EndFlag         = false;
	TStringInput->NumberNum       = NumberNum;
	TStringInput->NumberOnly      = NumberOnly;
	TStringInput->StringDrawWidth = StringDrawWidth;
	TStringInput->InputHandle     = -1;
	TStringInput->Position.x      = x;
	TStringInput->Position.y      = y;
	TStringInput->TWindowHandle   = -1;

	if( Name )
	{
		TStringInput->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TStringInput->Name == NULL )
		{
			return -1;
		}
		strcpy( TStringInput->Name, Name );
	}

	// �n���h����Ԃ�
	return NewHandle;
}

// ��������͏����폜����
void ToolStringInput_Delete(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// ���O�ۑ��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
	if( TStringInput->Name )
	{
		free( TStringInput->Name );
		TStringInput->Name = NULL;
	}

	// ��������̓n���h�����쐬���Ă�����폜����
	if( TStringInput->InputHandle != -1 )
	{
		DeleteKeyInput( TStringInput->InputHandle );
	}

	// �m�ۂ��Ă������������������
	free( TStringInput );
	g_TStringInput[ TStringInputHandle ] = NULL;

	// ��������͏��̐������炷
	g_TStringInputNum--;
}

// �S�Ă̕�������͏����폜����
void ToolStringInput_Delete_All( void )
{
	int i;

	// �L���ȑS�Ă̕�������͂��폜����
	for( i = 0; i < TOOL_STRINGINPUT_MAX; i++ )
	{
		if( g_TStringInput[ i ] != NULL )
		{
			ToolStringInput_Delete( i );
		}
	}
}

// ��������͏��Ɋ֘A�t����\����ԏ����Z�b�g����
void ToolStringInput_SetVisibleHandle(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->TVisibleHandle = TVisibleHandle;
}

// ��������͏��̕\����Ԃ�ύX����
void ToolStringInput_SetVisible(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �V�����\����Ԃ�ۑ�����
	TStringInput->Visible = Visible;
}

// ��������͏��̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolStringInput_GetVisible(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( !TStringInput->Visible )
	{
		return false;
	}

	// �֘A�t�����Ă���E�C���h�E���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TStringInput->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TStringInput->TWindowHandle ) )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TStringInput->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TStringInput->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// ��������͏��Ɗ֘A�t����E�C���h�E����ݒ肷��
void ToolStringInput_SetWindow(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->TWindowHandle = TWindowHandle;
}

// ��������͏��̕������͗̈�̍��W��ύX����
void ToolStringInput_SetPosition(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �ύX��̕������͗̈�̍��W
	int x,
	int y
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->Position.x = x;
	TStringInput->Position.y = y;
}

// ��������͏��̓��͒��̕������ύX����
void ToolStringInput_SetString(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �Z�b�g���镶���񂪊i�[���ꂽ�������A�h���X
	const char *String
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �������ۑ�����
	strcpy( TStringInput->InputString, String );

	// ��������̓n���h�����L���ȏꍇ�͕�������̓n���h���ɔ��f����
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputString( String, TStringInput->InputHandle );
	}
}

// ��������͏��̓��͒��̕�������擾����
void ToolStringInput_GetString(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒��̕�������i�[���郁�����̈�̐擪�A�h���X
	char *String
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// ��������̓n���h�����L���ȏꍇ�͓��͒��̕�������擾����
	if( TStringInput->InputHandle != -1 )
	{
		GetKeyInputString( String, TStringInput->InputHandle );
	}
	else
	{
		// ���͒��ł͂Ȃ��ꍇ�͓��͂��m�肵����������R�s�[����
		strcpy( String, TStringInput->InputString );
	}
}

// ��������͏��̕�������͂𖳏����ŏI������
void ToolStringInput_AlwaysEnd(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// ��������̓n���h�����L���ȏꍇ�͓��͒��̕�������擾����
	if( TStringInput->InputHandle != -1 )
	{
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
}

// ��������͏��̓��͒��̕�����Ƃ��Đ����l�ɕύX����
void ToolStringInput_SetIntNumber(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒�������Ƃ��Đݒ肷�鐮���l
	int Number
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �l��ۑ�
	TStringInput->IntNumber   = Number;
	TStringInput->FloatNumber = ( float )Number;

	// ��������̓n���h�����L���ȏꍇ�͕�������̓n���h���ɃZ�b�g����
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputNumber( Number, TStringInput->InputHandle );
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
	else
	{
		// ��������̓n���h���������ȏꍇ�͒P���ɐ��l�𕶎���ɕϊ�����
		sprintf( TStringInput->InputString, "%d", Number );
	}
}

// ��������͏��̓��͒��̕�����Ƃ��ĕ��������_�l�ɕύX����
void ToolStringInput_SetFloatNumber(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒�������Ƃ��Đݒ肷�镂�������_�l
	float Number
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �l��ۑ�
	TStringInput->FloatNumber = Number;
	TStringInput->IntNumber   = ( int )Number;

	// ��������̓n���h�����L���ȏꍇ�͕�������̓n���h���ɃZ�b�g����
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputNumberToFloat( Number, TStringInput->InputHandle );
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
	else
	{
		// ��������̓n���h���������ȏꍇ�͒P���ɐ��l�𕶎���ɕϊ�����
		sprintf( TStringInput->InputString, "%f", Number );
	}
}

// ��������͏��̓��͒��̕�����𐮐��l�ɕϊ��������̂��擾����
//     �߂�l : ���͒��̕�����𐮐��l�ɕϊ������l
int ToolStringInput_GetIntNumber(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	return TStringInput->IntNumber;
}

// ��������͏��̓��͒��̕�����𕂓������_�l�ɕϊ��������̂��擾����
//     �߂�l : ���͒��̕�����𕂓������_�l�ɕϊ������l
float ToolStringInput_GetFloatNumber(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	return TStringInput->FloatNumber;
}

// ��������͏��̕�����̓��͂��I�����Ă��邩�ǂ������擾����
//     �߂�l : ���͂��I�����Ă��邩�ǂ���( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
bool ToolStringInput_CheckEnd(
	// ��������͏��n���h��
	int TStringInputHandle,

	// ������̓��͂��I�����Ă��邩�ǂ����̃t���O�����Z�b�g���邩�ǂ���
	bool Reset
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	bool              Result;

	// �߂�l��ۑ�
	Result = TStringInput->EndFlag;

	// ���Z�b�g�w�肪����ꍇ�̓t���O��|��
	if( Reset )
	{
		TStringInput->EndFlag = false;
	}

	// �߂�l��Ԃ�
	return Result;
}

// ��������͏��̕�������͂��A�N�e�B�u�ɂȂ��Ă����ꍇ�ɃA�N�e�B�u��Ԃ���������
void ToolStringInput_ResetActive(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// �A�N�e�B�u�ȕ�������̓n���h���ƂȂ��Ă�����
	// �A�N�e�B�u�ȕ�������̓n���h����������Ԃɂ���
	if( GetActiveKeyInput() == TStringInput->InputHandle )
	{
		SetActiveKeyInput( -1 );
	}
}

// ��������͏��̏�Ԑ��ڏ������s��
//     �߂�l : ����������I���������ǂ���( true:����I�� false:�G���[���� )
static bool ToolStringInput_Step(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	bool              In;
	int               SizeX;
	int               SizeY;
	int               SelectStart;
	int               SelectEnd;
	int               AreaX;
	int               AreaY;
	int               NameDrawW;
	int               ButtonFontHandle;
	int               MouseX;
	int               MouseY;
	int               MouseSelectStartX;
	int               MouseSelectEndX;
	int               MouseInput;
	int               MouseEdgeInput;

	// �\����Ԃ� false �̏ꍇ�͕�������̓n���h�����폜���ďI��
	if( !ToolStringInput_GetVisible( TStringInputHandle ) )
	{
		// ��������̓n���h�����L���������ꍇ�͓��͒��̕������ۑ�������ō폜
		if( TStringInput->InputHandle != -1 )
		{
			GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
			DeleteKeyInput( TStringInput->InputHandle );
			TStringInput->InputHandle = -1;
		}
		return true;
	}

	// ���͒�������`��p�̃t�H���g�n���h�����擾
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// �}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// �}�E�X�̓��͏�Ԃ��擾
	MouseInput     = ToolStatic_GetMouseButton();
	MouseEdgeInput = ToolStatic_GetMouseEdgeButton();

	// �}�E�X�̑I��͈͂��擾����
	ToolStatic_GetMouseSelectStartPosition(
		&MouseSelectStartX, NULL,
		&MouseSelectEndX,   NULL
	);

	// ��������̓n���h�����쐬����Ă��Ȃ��ꍇ�͍쐬����
	if( TStringInput->InputHandle == -1 )
	{
		// ���l���͂��ǂ����ō쐬������ύX
		if( TStringInput->NumberOnly )
		{
			// ���l�������͂ł��Ȃ���������̓n���h�����쐬
			TStringInput->InputHandle =
					MakeKeyInput( TStringInput->NumberNum, FALSE, FALSE, TRUE );

			// ���l���Z�b�g
			SetKeyInputNumberToFloat(
						TStringInput->FloatNumber, TStringInput->InputHandle );
		}
		else
		{
			// ���l�ȊO�����͂ł���̕�������̓n���h�����쐬
			TStringInput->InputHandle =
					MakeKeyInput( STRING_BUFFER_SIZE - 1, FALSE, FALSE, FALSE );

			// �m�肵�Ă�����͕�������Z�b�g
			SetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
		}

		// ��������̓n���h���̍쐬�Ɏ��s������G���[
		if( TStringInput->InputHandle < 0 )
		{
			return false;
		}

		// ���݂̕�������̓n���h���Őݒ肳��Ă��镶������擾
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}

	// ��������͗̈�̕��ƍ������Z�b�g
	SizeX = TStringInput->NumberOnly ?
				TStringInput->NumberNum * ONE_NUMBER_SIZE + 4 :
				TStringInput->StringDrawWidth;
	SizeY = STRINGINPUT_HEIGHT;

	// ���O�̕`�敝���擾����
	NameDrawW = 0;
	if( TStringInput->Name != NULL )
	{
		NameDrawW = GetDrawStringWidthToHandle( 
					TStringInput->Name, -1, ToolStatic_GetNormalFontHandle() ) + 2;
	}

	// ��������͗̈�̍��W�ƕ�������͗̈��Ƀ}�E�X�J�[�\�������邩�ǂ������擾����
	if( TStringInput->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TStringInput->TWindowHandle, &AreaX, &AreaY );
		AreaX += TStringInput->Position.x + NameDrawW;
		AreaY += TStringInput->Position.y;
//		In = AreaX <= MouseX && AreaX + SizeX >= MouseX &&
//		     AreaY <= MouseY && AreaY + SizeY >= MouseY &&
		In = ToolStatic_CheckMouseIn( AreaX, AreaY, AreaX + SizeX, AreaY + SizeY ) &&
			 ToolWindow_CheckInfoAreaIn( TStringInput->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		AreaX = TStringInput->Position.x + NameDrawW;
		AreaY = TStringInput->Position.y;
//		In = TStringInput->Position.x <= MouseX && TStringInput->Position.x + SizeX >= MouseX &&
//		     TStringInput->Position.y <= MouseY && TStringInput->Position.y + SizeY >= MouseY;
		In = ToolStatic_CheckMouseIn( AreaX, AreaY, AreaX + SizeX, AreaY + SizeY );
	}

	// ���͒��̕�����̑I��͈͂��擾����
	GetKeyInputSelectArea( &SelectStart, &SelectEnd, TStringInput->InputHandle );

	// �}�E�X�̍��N���b�N�������u�Ԃ̏ꍇ�͏����𕪊�
	if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		// ��������͗̈�̒��ŃN���b�N�������ǂ����ŏ����𕪊�
		if( In )
		{
			// ��������̓n���h�����A�N�e�B�u�ɂ���
			SetActiveKeyInput( TStringInput->InputHandle );

			// �_�u���N���b�N�������ꍇ�͓��͒�������S�̂�I����Ԃɂ���
			if( ToolStatic_GetMouseDoubleClick() )
			{
				SetKeyInputSelectArea( 0, 9999, TStringInput->InputHandle );
			}
			else
			{
				// �V���O���N���b�N�̏ꍇ�͓��͒�������̑I��͈͂���������
				SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );

				// �N���b�N�������W�ɍ��킹�ē��͒�������̃J�[�\���̈ʒu��ύX
				SetKeyInputCursorPosition(
					Tool_GetKeyInputCursorPos(
						AreaX,
						MouseX,
						TStringInput->InputHandle,
						ButtonFontHandle
					),
					TStringInput->InputHandle
				);
			}
		}
		else
		{
			// ��������͗̈�̊O�ŃN���b�N�����ꍇ�͕�����̑I��͈͂���������
			SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );
		}
	}
	else
	// �}�E�X�̍��{�^����������Ă��Ċ���������͗̈�Ƀ}�E�X�J�[�\��������ꍇ��
	// ���͒�������̑I��͈͂�ύX����
	if( ( MouseInput & MOUSE_INPUT_LEFT ) != 0 && In )
	{
		int Start;
		int End;

		Start = Tool_GetKeyInputCursorPos(
					AreaX,
					MouseSelectStartX,
					TStringInput->InputHandle,
					ButtonFontHandle
				);

		End   = Tool_GetKeyInputCursorPos(
					AreaX,
					MouseSelectEndX,
					TStringInput->InputHandle,
					ButtonFontHandle
				);

		if( Start != End )
		{
			SetKeyInputSelectArea( Start, End, TStringInput->InputHandle );
		}

		// ���͒�������̃J�[�\���ʒu���}�E�X�J�[�\���̈ʒu�ɂ��킹�ĕύX
		SetKeyInputCursorPosition(
			Tool_GetKeyInputCursorPos(
				AreaX,
				MouseX,
				TStringInput->InputHandle,
				ButtonFontHandle
			),
			TStringInput->InputHandle
		);
	}

	// ��������͗̈���ŉE�N���b�N���ꂽ�ꍇ
	if( ( MouseEdgeInput & MOUSE_INPUT_RIGHT ) != 0 && In )
	{
		if( SelectStart != -1 )
		{
			int  StringLength;
			int  smin;
			int  smax;
			char TempStr[ STRING_BUFFER_SIZE ];

			// ���͒�������̑I��͈͂�����ꍇ�̓N���b�v�{�[�h�ɕ������o�^����

			GetKeyInputString( TempStr, TStringInput->InputHandle );

			if( SelectStart < SelectEnd )
			{
				smin = SelectStart;
				smax = SelectEnd;
			}
			else
			{
				smin = SelectEnd;
				smax = SelectStart;
			}
			StringLength = smax - smin;
			memmove( TempStr, TempStr + smin, StringLength );
			TempStr[ StringLength ] = 0;
			SetClipboardText( TempStr );
			SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );
		}
		else
		{
			char *String;
			char *p;
			int  Size;

			// ���͒�������̑I��͈͂������ꍇ�̓N���b�v�{�[�h�̕��������͕�����ɒǉ�

			// �N���b�v�{�[�h��̕�����̃T�C�Y���擾����
			Size = GetClipboardText( NULL );

			// ��������i�[���邽�߂̃������̊m��
			String = ( char * )malloc( Size );
			if( String != NULL )
			{
				// �������̊m�ۂɐ���������N���b�v�{�[�h��̕�������擾
				GetClipboardText( String );

				// �擾������������͕�����ɒǉ�
				for( p = String; *p; p ++ )
				{
					StockInputChar( *p );
				}

				// �m�ۂ����������̉��
				free( String );
			}
		}
	}

	// ��������͂��������Ă����犮���������s��
	if( CheckKeyInput( TStringInput->InputHandle ) )
	{
		// ��������͂��������Ă��邩�ǂ����̃t���O�𗧂Ă�
		TStringInput->EndFlag = true;

		// �������̕�������擾����
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );

		// �������̕�����𐔒l�Ƃ����ꍇ�̒l���擾����
		TStringInput->IntNumber   = GetKeyInputNumber( TStringInput->InputHandle );
		TStringInput->FloatNumber = GetKeyInputNumberToFloat( TStringInput->InputHandle);

		// ��������̓n���h���͍ēx���͏�Ԃɂ���
		ReStartKeyInput( TStringInput->InputHandle );
	}

	// ����I��
	return true;
}

// ��������͏�����ʂɕ`�悷��
static void ToolStringInput_Draw(
	// ��������͏��n���h��
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	int               SizeX;
	int               SizeY;
	char              String[ STRING_BUFFER_SIZE ];
	int               AreaX;
	int               AreaY;
	RECT              DrawRect;
	int               NameDrawW;
	int               NormalFontHandle;
	int               ButtonFontHandle;

	// ��\����Ԃ̏ꍇ�͉��������I��
	if( !ToolStringInput_GetVisible( TStringInputHandle ) )
	{
		return;
	}

	// ���O�`��p�̃t�H���g�n���h�����擾
	NormalFontHandle = ToolStatic_GetNormalFontHandle();

	// ���͒�������`��p�̃t�H���g�n���h�����擾
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// ��������͗̈�̍��W���擾����
	if( TStringInput->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TStringInput->TWindowHandle, &AreaX, &AreaY );
		AreaX += TStringInput->Position.x;
		AreaY += TStringInput->Position.y;

		// �E�C���h�E���n���h�����L���ȏꍇ�͕`��̈搧���ݒ���s��
		ToolWindow_SetupDrawArea( TStringInput->TWindowHandle );
	}
	else
	{
		AreaX = TStringInput->Position.x;
		AreaY = TStringInput->Position.y;
	}

	// ���O������ꍇ�͖��O��`�悷��
	if( TStringInput->Name )
	{
		// ���O�̕`�敝���擾����
		NameDrawW =
			GetDrawStringWidthToHandle( TStringInput->Name, -1, NormalFontHandle ) + 2;

		// ���O��`�悷��
		DrawStringToHandle(
			AreaX,
			AreaY,
			TStringInput->Name,
			GetColor( 255,255,255 ),
			NormalFontHandle
		);

		// ��������͗̈�𖼑O�̕`�敝�������E�ɂ��炷
		AreaX += NameDrawW;
	}

	// ��������͗̈�̕��ƍ������Z�b�g
	SizeX = TStringInput->NumberOnly ?
				TStringInput->NumberNum * ONE_NUMBER_SIZE + 4 :
				TStringInput->StringDrawWidth;
	SizeY = STRINGINPUT_HEIGHT;
	
	// ��������͗̈�̘g��`��
	Tool_DrawButton(
		AreaX - 3,
		AreaY - 3,
		SizeX + 4,
		18,
		GetColor(   0,  0,  0 ),
		GetColor(  64, 64, 64 ),
		GetColor( 255,255,255 ),
		GetColor( 191,191,191 ),
		GetColor( 220,220,220 )
	);

	// �֘A�t�����Ă���E�C���h�E��񂪂���ꍇ��
	// �E�C���h�E�����l�������`��̈搧���ݒ���s��
	if( TStringInput->TWindowHandle >= 0 )
	{
		DrawRect.left   = AreaX;
		DrawRect.top    = AreaY;
		DrawRect.right  = AreaX + SizeX;
		DrawRect.bottom = AreaY + 17;
		RectClipping( &DrawRect, &ToolWindow_GetInfoArea( TStringInput->TWindowHandle ));
		SetDrawArea( DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom );
	}
	else
	{
		// �E�C���h�E��񂪊֘A�t�����Ă��Ȃ��ꍇ��
		// ��������͗̈�����̂܂ܕ`�搧���̈�Ƃ��Đݒ肷��
		SetDrawArea( AreaX, AreaY, AreaX + SizeX, AreaY + 17 );
	}

	// ���͒�������̕�����`��Ɏg�p����t�H���g��ݒ肷��
	SetKeyInputStringFont( ButtonFontHandle );

	// ���݂̓��͒��̕�������擾����
	GetKeyInputString( String, TStringInput->InputHandle );

	// ���͒������񂪊m�肵��������ƈقȂ�ꍇ�͕`��F��ύX����
	if( strcmp( String, TStringInput->InputString ) != 0 )
	{
		SetKeyInputStringColor( GetColor( 0,0,255 ), -1, -1, -1, -1, -1, -1 );
	}
	else
	{
		SetKeyInputStringColor( GetColor( 0,0,  0 ), -1, -1, -1, -1, -1, -1 );
	}

	// ���͒�������̕`��
	DrawKeyInputString( AreaX, AreaY, TStringInput->InputHandle );

	// �ݒ�����ɖ߂�
	SetKeyInputStringFont( -1 );
	SetKeyInputStringColor( -1, -1, -1, -1, -1, -1, -1 );
	SetDrawArea( 0, 0, 999999, 999999 );
}

// �S�Ă̕�������͏��̏�Ԑ��ڏ������s��
//    �߂�l : ����������ɏI���������ǂ���( true:����I��  false:�G���[���� )
bool ToolStringInput_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TStringInputNum; i++ )
	{
		if( g_TStringInput[ i ] == NULL )
		{
			continue;
		}

		if( !ToolStringInput_Step( i ) )
		{
			return false;
		};
		j++;
	}

	return true;
}

// �S�Ă̕�������͏��̕`��ڏ������s��
void ToolStringInput_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TStringInputNum; i++ )
	{
		if( g_TStringInput[ i ] == NULL )
		{
			continue;
		}

		ToolStringInput_Draw( i );
		j++;
	}
}



