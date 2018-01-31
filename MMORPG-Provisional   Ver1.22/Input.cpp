#include "Input.h"
#include "Code\Common\Mouse\Mouse.h"
#include "DxLib.h"
#include "BinaryFile.h"
#include "System.h"
#include <string.h>
#include <math.h>

// ���̓R���t�B�O�t�@�C���̃p�X
#define INPUTCONFIG_PATH		"Data\\InputConfig.dat"

// �`�F�b�N����Q�[���p�b�h�̐�
#define MAX_GAMEPAD_NUM				(16)

// �L�[�{�[�h�̓��͏�Ԃ��擾���邽�߂̃o�b�t�@�̃T�C�Y
#define KEY_STATE_BUFFER_SIZE		(256)

// DirectInput ���瓾����l�̍ő�l
#define DIRECTINPUT_MAX_VALUE		(1000)

// DirectInput �̓��͏��^�C�v
typedef enum _EDInputType
{
	EDInputType_X,					// �X�e�B�b�N�̂w��
	EDInputType_Y,					// �X�e�B�b�N�̂x��
	EDInputType_Z,					// �X�e�B�b�N�̂y��
	EDInputType_Rx,					// �X�e�B�b�N�̂w����]
	EDInputType_Ry,					// �X�e�B�b�N�̂x����]
	EDInputType_Rz,					// �X�e�B�b�N�̂y����]
	EDInputType_POV,				// �����R���g���[��
	EDInputType_Button,				// �{�^��
} EDInputType;

// �L�[�{�[�h�̂P�L�[�ӂ�̏��
typedef struct _SKeyInfo
{
	// ���̓R�[�h( KEY_INPUT_LEFT �Ȃ� )
	int              KeyInput;

	// �L�[�̖��O
	const char *     Name;
} SKeyInfo;

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή����
typedef struct _SInputTypeInfo
{
	// �p�b�h�ԍ�
	int              PadNo;

	// DirectInput �̓��͏��^�C�v
	EDInputType      DirectInputType;

	// �T�u���( DirectInputType �ɂ���ĈӖ����ς�� )
	int              SubInfo[ 2 ];

	// �Ή�����L�[�{�[�h�̃L�[( KEY_INPUT_UP �Ȃ� )
	int              KeyInput;

} SInputTypeInfo;

// ���͏����p�̏��
typedef struct _SInputSystemData
{
	// �Q�[�����̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή����
	SInputTypeInfo   InputTypeInfo[ EInputType_Num ];
	
	// DirectInput �̃p�b�h�̓��͏��
	DINPUT_JOYSTATE  DirectInputState[ MAX_GAMEPAD_NUM ];

	// �L�[�̓��͏��
	char             KeyInput[ KEY_STATE_BUFFER_SIZE ];

	// �{�^���̓��͏��
	int              ButtonInput;

	// �{�^���̉����ꂽ�u�Ԃ݂̂̓��͏��
	int              ButtonEdgeInput;

	// �J��������p�̃X�e�B�b�N���͏��
	VECTOR           CameraStickInput;
} SInputSystemData;

// ���͏����p�̏��
static SInputSystemData g_InpSys;

// �Q�[���ł̊e���͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̃f�t�H���g�̑Ή��ݒ�
static SInputTypeInfo g_DefaultInputTypeInfo[ EInputType_Num ] =
{
	0, EDInputType_X,      -1, 0, KEY_INPUT_A,		// EInputType_Left
	0, EDInputType_X,       1, 0, KEY_INPUT_D,		// EInputType_Right
	0, EDInputType_Y,      -1, 0, KEY_INPUT_W,		// EInputType_Up 
	0, EDInputType_Y,       1, 0, KEY_INPUT_S,		// EInputType_Down 
	0, EDInputType_Rx,     -1, 0, KEY_INPUT_V,		// EInputType_Camera_Left
	0, EDInputType_Rx,      1, 0, KEY_INPUT_N,		// EInputType_Camera_Right
	0, EDInputType_Ry,     -1, 0, KEY_INPUT_G,		// EInputType_Camera_Up
	0, EDInputType_Ry,      1, 0, KEY_INPUT_B,		// EInputType_Camera_Down
	0, EDInputType_Button,  0, 0, KEY_INPUT_Z,		// EInputType_Attack
	0, EDInputType_Button,  1, 0, KEY_INPUT_X,		// EInputType_Defence
	0, EDInputType_Button,  2, 0, KEY_INPUT_C,		// EInputType_Jump
	0, EDInputType_Button,  3, 0, KEY_INPUT_SPACE,	// EInputType_Pause
};

// �p�b�h�̎��ʎq�e�[�u��
static int g_PadIDTable[ MAX_GAMEPAD_NUM ] =
{
	DX_INPUT_PAD1,
	DX_INPUT_PAD2,
	DX_INPUT_PAD3,
	DX_INPUT_PAD4,
	DX_INPUT_PAD5,
	DX_INPUT_PAD6,
	DX_INPUT_PAD7,
	DX_INPUT_PAD8,
	DX_INPUT_PAD9,
	DX_INPUT_PAD10,
	DX_INPUT_PAD11,
	DX_INPUT_PAD12,
	DX_INPUT_PAD13,
	DX_INPUT_PAD14,
	DX_INPUT_PAD15,
	DX_INPUT_PAD16,
};

// �L�[�R�[�h�̏��
static SKeyInfo g_KeyInfoTable[] =
{
	KEY_INPUT_BACK,			"BACK",			// �o�b�N�X�y�[�X�L�[
	KEY_INPUT_TAB,			"TAB",			// �^�u�L�[
	KEY_INPUT_RETURN,		"ENTER",		// �G���^�[�L�[

	KEY_INPUT_LSHIFT,		"L SHIFT",		// ���V�t�g�L�[
	KEY_INPUT_RSHIFT,		"R SHIFT",		// �E�V�t�g�L�[
	KEY_INPUT_LCONTROL,		"L CONTROL",	// ���R���g���[���L�[
	KEY_INPUT_RCONTROL,		"R CONTROL",	// �E�R���g���[���L�[
	KEY_INPUT_ESCAPE,		"ESCAPE",		// �G�X�P�[�v�L�[
	KEY_INPUT_SPACE,		"SPACE",		// �X�y�[�X�L�[
	KEY_INPUT_PGUP,			"PAGE UP",		// �o�������t�o�L�[
	KEY_INPUT_PGDN,			"PAGE DOWN",	// �o�������c�������L�[
	KEY_INPUT_END,			"END",			// �G���h�L�[
	KEY_INPUT_HOME,			"HOME",			// �z�[���L�[
	KEY_INPUT_LEFT,			"LEFT",			// ���L�[
	KEY_INPUT_UP,			"UP",			// ��L�[
	KEY_INPUT_RIGHT,		"RIGHT",		// �E�L�[
	KEY_INPUT_DOWN,			"DOWN",			// ���L�[
	KEY_INPUT_INSERT,		"INSERT",		// �C���T�[�g�L�[
	KEY_INPUT_DELETE,		"DELETE",		// �f���[�g�L�[

	KEY_INPUT_MINUS,		"-",			// �|�L�[
	KEY_INPUT_YEN,			"\\",			// ���L�[
	KEY_INPUT_PREVTRACK,	"^",			// �O�L�[
	KEY_INPUT_PERIOD,		".",			// �D�L�[
	KEY_INPUT_SLASH,		"/",			// �^�L�[
	KEY_INPUT_LALT,			"L ALT",		// ���`�k�s�L�[
	KEY_INPUT_RALT,			"R ALT",		// �E�`�k�s�L�[
	KEY_INPUT_SCROLL,		"SCROLL LOCK",	// ScrollLock�L�[
	KEY_INPUT_SEMICOLON,	";",			// �G�L�[
	KEY_INPUT_COLON,		":",			// �F�L�[
	KEY_INPUT_LBRACKET,		"[",			// �m�L�[
	KEY_INPUT_RBRACKET,		"]",			// �n�L�[
	KEY_INPUT_AT,			"@",			// ���L�[
	KEY_INPUT_BACKSLASH,	"BACK SLASH",	// �_�L�[
	KEY_INPUT_COMMA,		",",			// �C�L�[
	KEY_INPUT_KANJI,		"KANJI",		// �����L�[
	KEY_INPUT_CONVERT,		"CONVERT",		// �ϊ��L�[
	KEY_INPUT_NOCONVERT,	"NO CONVERT",	// ���ϊ��L�[
	KEY_INPUT_KANA,			"KANA",			// �J�i�L�[
	KEY_INPUT_APPS,			"APPS",			// �A�v���P�[�V�������j���[�L�[
	KEY_INPUT_CAPSLOCK,		"CAPS LOCK",	// CaspLock�L�[
	KEY_INPUT_SYSRQ,		"PRINT SCREEN",	// PrintScreen�L�[
	KEY_INPUT_PAUSE,		"PAUSE BREAK",	// PauseBreak�L�[
	KEY_INPUT_LWIN,			"L WIN",		// ���v�����L�[
	KEY_INPUT_RWIN,			"R WIN",		// �E�v�����L�[

	KEY_INPUT_NUMLOCK,		"NUM LOCK",		// �e���L�[�O
	KEY_INPUT_NUMPAD0,		"NUMPAD 0",		// �e���L�[�O
	KEY_INPUT_NUMPAD1,		"NUMPAD 1",		// �e���L�[�P
	KEY_INPUT_NUMPAD2,		"NUMPAD 2",		// �e���L�[�Q
	KEY_INPUT_NUMPAD3,		"NUMPAD 3",		// �e���L�[�R
	KEY_INPUT_NUMPAD4,		"NUMPAD 4",		// �e���L�[�S
	KEY_INPUT_NUMPAD5,		"NUMPAD 5",		// �e���L�[�T
	KEY_INPUT_NUMPAD6,		"NUMPAD 6",		// �e���L�[�U
	KEY_INPUT_NUMPAD7,		"NUMPAD 7",		// �e���L�[�V
	KEY_INPUT_NUMPAD8,		"NUMPAD 8",		// �e���L�[�W
	KEY_INPUT_NUMPAD9,		"NUMPAD 9",		// �e���L�[�X
	KEY_INPUT_MULTIPLY,		"NUMPAD *",		// �e���L�[���L�[
	KEY_INPUT_ADD,			"NUMPAD +",		// �e���L�[�{�L�[
	KEY_INPUT_SUBTRACT,		"NUMPAD -",		// �e���L�[�|�L�[
	KEY_INPUT_DECIMAL,		"NUMPAD .",		// �e���L�[�D�L�[
	KEY_INPUT_DIVIDE,		"NUMPAD /",		// �e���L�[�^�L�[
	KEY_INPUT_NUMPADENTER,	"NUMPAD ENTER",	// �e���L�[�̃G���^�[�L�[

	KEY_INPUT_F1,			"F1",			// �e�P�L�[
	KEY_INPUT_F2,			"F2",			// �e�Q�L�[
	KEY_INPUT_F3,			"F3",			// �e�R�L�[
	KEY_INPUT_F4,			"F4",			// �e�S�L�[
	KEY_INPUT_F5,			"F5",			// �e�T�L�[
	KEY_INPUT_F6,			"F6",			// �e�U�L�[
	KEY_INPUT_F7,			"F7",			// �e�V�L�[
	KEY_INPUT_F8,			"F8",			// �e�W�L�[
	KEY_INPUT_F9,			"F9",			// �e�X�L�[
	KEY_INPUT_F10,			"F10",			// �e�P�O�L�[
	KEY_INPUT_F11,			"F11",			// �e�P�P�L�[
	KEY_INPUT_F12,			"F12",			// �e�P�Q�L�[

	KEY_INPUT_A,			"A",			// �`�L�[
	KEY_INPUT_B,			"B",			// �a�L�[
	KEY_INPUT_C,			"C",			// �b�L�[
	KEY_INPUT_D,			"D",			// �c�L�[
	KEY_INPUT_E,			"E",			// �d�L�[
	KEY_INPUT_F,			"F",			// �e�L�[
	KEY_INPUT_G,			"G",			// �f�L�[
	KEY_INPUT_H,			"H",			// �g�L�[
	KEY_INPUT_I,			"I",			// �h�L�[
	KEY_INPUT_J,			"J",			// �i�L�[
	KEY_INPUT_K,			"K",			// �j�L�[
	KEY_INPUT_L,			"L",			// �k�L�[
	KEY_INPUT_M,			"M",			// �l�L�[
	KEY_INPUT_N,			"N",			// �m�L�[
	KEY_INPUT_O,			"O",			// �n�L�[
	KEY_INPUT_P,			"P",			// �o�L�[
	KEY_INPUT_Q,			"Q",			// �p�L�[
	KEY_INPUT_R,			"R",			// �q�L�[
	KEY_INPUT_S,			"S",			// �r�L�[
	KEY_INPUT_T,			"T",			// �s�L�[
	KEY_INPUT_U,			"U",			// �t�L�[
	KEY_INPUT_V,			"V",			// �u�L�[
	KEY_INPUT_W,			"W",			// �v�L�[
	KEY_INPUT_X,			"X",			// �w�L�[
	KEY_INPUT_Y,			"Y",			// �x�L�[
	KEY_INPUT_Z,			"Z",			// �y�L�[

	KEY_INPUT_0, 			"0",			// �O�L�[
	KEY_INPUT_1,			"1",			// �P�L�[
	KEY_INPUT_2,			"2",			// �Q�L�[
	KEY_INPUT_3,			"3",			// �R�L�[
	KEY_INPUT_4,			"4",			// �S�L�[
	KEY_INPUT_5,			"5",			// �T�L�[
	KEY_INPUT_6,			"6",			// �U�L�[
	KEY_INPUT_7,			"7",			// �V�L�[
	KEY_INPUT_8,			"8",			// �W�L�[
	KEY_INPUT_9,			"9",			// �X�L�[

	-1,						NULL,
};

// ���͏����̏��������s��
void InputInitialize( void )
{
	// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C������ǂݍ���
	if( !LoadInputSetting() )
	{
		// �t�@�C���̓ǂݍ��݂Ɏ��s�����ꍇ�̓f�t�H���g�ݒ�ɂ���
		SetDefaultSetting();
	}

}

// ProcessInput �p�̎����̓^�C�v�̏������s���⏕�֐�
static void ProcessInput_Assist(
	// �������ʂ�������ϐ��̃A�h���X
	int *InputState,

	// �����̓^�C�v�̃T�u���
	//    (  1:InputValue ���v���X�̒l�̏ꍇ�ɓ��͂���Ƃ���
	//      -1:InputValue ���}�C�i�X�̒l�̏ꍇ�ɓ��͂���Ƃ��� )
	int SubInfo,

	// �����͂̒l
	int InputValue
)
{
	// �T�u��� 1 �� -1 ���ŏ����𕪊�
	switch( SubInfo )
	{
	case 1:		// 1�̏ꍇ�� InputValue ���v���X�̏ꍇ�̂ݓ��͂���Ƃ���
		if( InputValue > 0 )
		{
			*InputState = InputValue;
		}
		break;

	case -1:	// -1�̏ꍇ�� InputValue ���v���X�̏ꍇ�̂ݓ��͂���Ƃ���
		if( InputValue < 0 )
		{
			*InputState = -InputValue;
		}
		break;
	}
}

// ���͏������s��
void ProcessInput( void )
{
	int              i;
	SInputTypeInfo  *ITInfo;
	int              InputState[ EInputType_Num ];
	int              PadNum;
	DINPUT_JOYSTATE *DIJState;
	int              ButtonPrevInput;
	int              Total;

	// �p�b�h�̐����擾����
	PadNum = GetJoypadNum();

	// �p�b�h�̐����v���O�������Ή����Ă��鐔��葽���ꍇ�́A�Ή����Ă��鐔�ɐ�������
	if( PadNum > MAX_GAMEPAD_NUM )
	{
		PadNum = MAX_GAMEPAD_NUM;
	}

	// �p�b�h�̓��͏��� DirectInput ����擾����
	for( i = 0; i < PadNum; i++ )
	{
		GetJoypadDirectInputState( g_PadIDTable[ i ], &g_InpSys.DirectInputState[ i ] );
	}

	// ���݂̃L�[�̓��͏�Ԃ��擾����
	GetHitKeyStateAll( g_InpSys.KeyInput );

	// �Q�[���Ŏg�p������͏����\�z����
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		// �w���͂Ȃ��x��Ԃɂ��Ă���
		InputState[ i ] = 0;

		// �Ή����Ă���L�[��������Ă�����w���͂���x�ɂ���
		if( g_InpSys.KeyInput[ ITInfo->KeyInput ] != 0 )
		{
			InputState[ i ] = DIRECTINPUT_MAX_VALUE;
		}


		// �Ή����� DirectInput �̏��^�C�v�ɂ���ď����𕪊�
		DIJState = &g_InpSys.DirectInputState[ ITInfo->PadNo ];
		switch( ITInfo->DirectInputType )
		{
		case EDInputType_X:	// �X�e�B�b�N�̂w���̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->X );
			break;

		case EDInputType_Y:	// �X�e�B�b�N�̂x���̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Y );
			break;

		case EDInputType_Z:	// �X�e�B�b�N�̂y���̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Z );
			break;

		case EDInputType_Rx:	// �X�e�B�b�N�̂w����]�̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Rx );
			break;

		case EDInputType_Ry:	// �X�e�B�b�N�̂x����]�̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Ry );
			break;

		case EDInputType_Rz:	// �X�e�B�b�N�̂y����]�̏ꍇ
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Rz );
			break;

		case EDInputType_POV:	// �����R���g���[���̏ꍇ
			// �ݒ肳��Ă�������ɓ��͂���Ă�����w���͂���x�ɂ���
			if( DIJState->POV[ ITInfo->SubInfo[ 0 ] ] == ITInfo->SubInfo[ 1 ] )
			{
				InputState[ i ] = DIRECTINPUT_MAX_VALUE;
			}
			break;

		case EDInputType_Button:	// �{�^���̏ꍇ
			// �ݒ肳��Ă���{�^����������Ă�����w���͂���x�ɂ���
			if( DIJState->Buttons[ ITInfo->SubInfo[ 0 ] ] == 128 )
			{
				InputState[ i ] = DIRECTINPUT_MAX_VALUE;
			}
			break;
		}
	}



	// �P�t���[���O�̓��͏�Ԃ��Ƃ��Ă���
	ButtonPrevInput = g_InpSys.ButtonInput;

	// ���͏�Ԃ̏�����
	g_InpSys.ButtonInput = 0;

	// �e���͂ɑΉ�����r�b�g���w���͂���x�̏ꍇ�͗��Ă�
	for( i = 0; i < EInputType_Num; i++ )
	{
		if( InputState[ i ] > 0 )
		{
			g_InpSys.ButtonInput |= 1 << i;
		}
	}

	// ����̃t���[���Łw���͂���x�ɂȂ�n�߂��r�b�g���Z�o����
	g_InpSys.ButtonEdgeInput = g_InpSys.ButtonInput & ~ButtonPrevInput;

	// �J��������p�̃X�e�B�b�N���͏�Ԃ��Z�o����
	Total = InputState[ EInputType_Camera_Right ] - InputState[ EInputType_Camera_Left ];
	g_InpSys.CameraStickInput.x = Total / ( float )DIRECTINPUT_MAX_VALUE;

	Total = InputState[ EInputType_Camera_Down  ] - InputState[ EInputType_Camera_Up   ];
	g_InpSys.CameraStickInput.y = Total / ( float )DIRECTINPUT_MAX_VALUE;

	g_InpSys.CameraStickInput.z = 0.0f;
}

// �{�^���̓��͏�Ԃ��擾����
//     �߂�l : �{�^���̓��͏��
int GetInput( void )
{
	return g_InpSys.ButtonInput;
}

// �{�^���̉����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾����
//     �߂�l : �{�^���̉����ꂽ�u�Ԃ݂̂̓��͏��
int GetEdgeInput( void )
{
	return g_InpSys.ButtonEdgeInput;
}

// �J��������p�̃X�e�B�b�N�̓��͏�Ԃ��擾����
//     �߂�l : �J��������p�̃X�e�B�b�N�̓��͏��
VECTOR GetCameraStickInput( void )
{
	return g_InpSys.CameraStickInput;
}

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C���ɕۑ�����
void SaveInputSetting( void )
{
	SBinaryFileData BinFileData;
	SInputTypeInfo *ITInfo;
	int             i;

	// �Ή�����ۑ�����t�@�C�����J��
	if( !WriteBinFile_Open( &BinFileData, INPUTCONFIG_PATH ) )
	{
		// �J���Ȃ������牽�������ɏI��
		return;
	}

	// �Q�[���̊e���͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C���ɏ����o��
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		WriteBinFile_Int( &BinFileData, ITInfo->PadNo );
		WriteBinFile_Int( &BinFileData, ITInfo->DirectInputType );
		WriteBinFile_Int( &BinFileData, ITInfo->SubInfo[ 0 ] );
		WriteBinFile_Int( &BinFileData, ITInfo->SubInfo[ 1 ] );
		WriteBinFile_Int( &BinFileData, ITInfo->KeyInput );
	}

	// �t�@�C�������
	WriteBinFile_Close( &BinFileData );
}

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C������ǂݍ���
//     �߂�l : �t�@�C���̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
bool LoadInputSetting( void )
{
	SBinaryFileData BinFileData;
	SInputTypeInfo *ITInfo;
	int             i;
	int             ParamInt;

	// �Ή�����ۑ������t�@�C�����J��
	if( !ReadBinFile_Open( &BinFileData, INPUTCONFIG_PATH ) )
	{
		// �J���Ȃ������ꍇ�͉��������ɏI��
		return false;
	}

	// �Q�[���̊e���͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C������ǂݍ���
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		ReadBinFile_Int( &BinFileData, &ITInfo->PadNo );
		ReadBinFile_Int( &BinFileData, &ParamInt );
		ITInfo->DirectInputType = ( EDInputType )ParamInt;
		ReadBinFile_Int( &BinFileData, &ITInfo->SubInfo[ 0 ] );
		ReadBinFile_Int( &BinFileData, &ITInfo->SubInfo[ 1 ] );
		ReadBinFile_Int( &BinFileData, &ITInfo->KeyInput );
	}

	// �t�@�C�������
	ReadBinFile_Close( &BinFileData );

	// �����I��
	return true;
}

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������f�t�H���g�ݒ�ɂ���
void SetDefaultSetting( void )
{
	int i;

	// �f�t�H���g�ݒ�̏����R�s�[����
	for( i = 0; i < EInputType_Num; i++ )
	{
		g_InpSys.InputTypeInfo[ i ] = g_DefaultInputTypeInfo[ i ];
	}
}

// UpdateInputTypeInfo �p�̎����̓^�C�v�̏������s���⏕�֐�
static void UpdateInputTypeInfo_Assist(
	// �L�[��p�b�h�Ȃǂ̓��͏��
	SInputTypeInfo   *ITInfo,

	// MaxInput �ɗL���Ȓl�������Ă��邩�ǂ�����ۑ����Ă���ϐ��̃A�h���X
	bool             *ValidMaxDInput,

	// �L���ȓ��͒l�̍ő�l��ۑ����Ă���ϐ��̃A�h���X
	int              *MaxDInput,

	// DirectInput �̓��͏��^�C�v
	EDInputType       DirectInputType,

	// DirectInput �̓��͒l
	int               DirectInputValue
)
{
	// DirectInput �̓��͒l���}�C�i�X���v���X���ŏ����𕪊�
	if( DirectInputValue < 0 )
	{
		// MaxDirectInputValue �ɂ܂��L���Ȓl�������Ă��Ȃ���
		// ���� MaxDirectInputValue �̒l���� DirectInputValue �̒l���傫�������`�F�b�N
		if( *ValidMaxDInput == false || *MaxDInput < -DirectInputValue )
		{
			// ���� ITInfo �ɐݒ肳��Ă�����̓^�C�v���ƈقȂ邩���`�F�b�N
			if( ITInfo->DirectInputType != DirectInputType || ITInfo->SubInfo[ 0 ] != -1 )
			{
				// �������N���A�����ꍇ�͏����X�V����
				ITInfo->DirectInputType   = DirectInputType;
				ITInfo->SubInfo[ 0 ]      = -1;
				ITInfo->SubInfo[ 1 ]      = 0;

				*ValidMaxDInput = true;
				*MaxDInput      = -DirectInputValue;
			}
		}
	}
	else
	if( DirectInputValue > 0 )
	{
		// MaxDirectInputValue �ɂ܂��L���Ȓl�������Ă��Ȃ���
		// ���� MaxDirectInputValue �̒l���� DirectInputValue �̒l���傫�������`�F�b�N
		if( *ValidMaxDInput == false || *MaxDInput < DirectInputValue )
		{
			// ���� ITInfo �ɐݒ肳��Ă�����̓^�C�v���ƈقȂ邩���`�F�b�N
			if( ITInfo->DirectInputType != DirectInputType || ITInfo->SubInfo[ 0 ] != 1 )
			{
				// �������N���A�����ꍇ�͏����X�V����
				ITInfo->DirectInputType   = DirectInputType;
				ITInfo->SubInfo[ 0 ]      = 1;
				ITInfo->SubInfo[ 1 ]      = 0;

				*ValidMaxDInput = true;
				*MaxDInput      = DirectInputValue;
			}
		}
	}
}

// ���݉�������Ă���L�[��p�b�h�̓��͏����`�F�b�N���āA���͂��������ꍇ�͏��X�V����
//     �߂�l : ���͂�����A��񂪍X�V���ꂽ���ǂ���
//              ( true:�X�V���ꂽ  false:�X�V����Ȃ����� )
bool UpdateInputTypeInfo(
	// �����X�V������͏��
	EInputType UpdateInputType
)
{
	int              i;
	int              j;
	bool             ValidMaxDInput;
	int              MaxDInput;
	int              PadNum;
	DINPUT_JOYSTATE *DIJState;
	SInputTypeInfo  *ITInfo = &g_InpSys.InputTypeInfo[ UpdateInputType ];
	bool             Result;

	// �߂�l�́w���̍X�V�����������x�ɂ��Ă���
	Result = false;

	// �ڑ�����Ă���p�b�h�̐����擾����
	PadNum = GetJoypadNum();

	// �p�b�h�̐����v���O�������Ή����Ă��鐔��葽���ꍇ�́A�Ή����Ă��鐔�ɐ�������
	if( PadNum > MAX_GAMEPAD_NUM )
	{
		PadNum = MAX_GAMEPAD_NUM;
	}

	// �wMaxDInput �ɗL���Ȓl�������Ă��邩�ǂ����x�̃t���O��|���A
	// MaxDInput �̒l�����������Ă���
	ValidMaxDInput = false;
	MaxDInput      = 0;

	// �S�Ẵp�b�h�̓��͏�Ԃ��`�F�b�N
	DIJState = g_InpSys.DirectInputState;
	for( i = 0; i < PadNum; i++, DIJState++ )
	{
		// �������͏�Ԃ��`�F�b�N( DIJState->X �� DIJState->Y �͌Œ�Ȃ̂Ń`�F�b�N���Ȃ� )
		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Z,  DIJState->Z );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Rx, DIJState->Rx );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Ry, DIJState->Ry );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Rz, DIJState->Rz );

		// �����R���g���[���̓��͏�Ԃ��`�F�b�N
		for( j = 0; j < 4; j++ )
		{
			// POV ���L���Ȓl( -1 �ȊO�̒l )�ŁA
			// ���� ITInfo �ɐݒ肳��Ă���l�ƈقȂ邩���`�F�b�N
			if( DIJState->POV[ j ] != -1 && 
				( ITInfo->DirectInputType != EDInputType_POV ||
				  ITInfo->SubInfo[ 0 ]    != j ||
				  ITInfo->SubInfo[ 1 ]    != DIJState->POV[ j ] ) )
			{
				// �������N���A�����ꍇ�͏����X�V����
				ITInfo->DirectInputType = EDInputType_POV;
				ITInfo->SubInfo[ 0 ]    = j;
				ITInfo->SubInfo[ 1 ]    = DIJState->POV[ j ];

				// �߂�l���w���̍X�V���������x�ɂ���
				Result = true;
			}
		}

		// �{�^���̓��͏�Ԃ��`�F�b�N
		for( j = 0; j < 32; j++ )
		{
			// �{�^����������Ă��āA
			// ���� ITInfo �� �ݒ肳��Ă�����ƈقȂ邩���`�F�b�N
			if( DIJState->Buttons[ j ] == 128 &&
				( ITInfo->DirectInputType != EDInputType_Button ||
				  ITInfo->SubInfo[ 0 ]    != j ) )
			{
				// �������N���A�����ꍇ�͏����X�V����
				ITInfo->DirectInputType = EDInputType_Button;
				ITInfo->SubInfo[ 0 ]    = j;
				ITInfo->SubInfo[ 1 ]    = 0;

				// �߂�l���w���̍X�V���������x�ɂ���
				Result = true;
			}
		}
	}

	// ValidMaxDirectInputValue �� true �̏ꍇ�́A�L���ȓ��͂�����A���� ITInfo ��
	// �ݒ肳��Ă���l���X�V���ꂽ�Ƃ������ƂȂ̂Ŗ߂�l���w���̍X�V���������x�ɂ���
	if( ValidMaxDInput )
	{
		Result = true;
	}

	// �L�[�̓��͏�Ԃ��`�F�b�N
	for( i = 0; i < KEY_STATE_BUFFER_SIZE; i++ )
	{
		// ���͂�����A���� ITInfo �ɐݒ肳��Ă�����ƈقȂ邩���`�F�b�N
		if( g_InpSys.KeyInput[ i ] && ITInfo->KeyInput != i )
		{
			// �������N���A�����ꍇ�͏����X�V����
			ITInfo->KeyInput = i;

			// �߂�l���w���̍X�V���������x�ɂ���
			Result = true;
		}
	}

	// ��񂪍X�V���ꂽ���ǂ�����Ԃ�
	return Result;
}

// �w��̓��͏��^�C�v�Ɋ��蓖�Ă��Ă���p�b�h�̓��͂ƃL�[�̓��̖͂��O���擾����
void GetInputTypeString(
	// ���O���擾���������͏��^�C�v
	EInputType InputType,

	// ���蓖�Ă��Ă���p�b�h���̖͂��O��������o�b�t�@�ւ̃A�h���X
	char *PadInputString,

	// ���蓖�Ă��Ă���L�[���̖͂��O��������o�b�t�@�ւ̃A�h���X
	char *KeyInputString
)
{
	SInputTypeInfo *ITInfo = &g_InpSys.InputTypeInfo[ InputType ];
	SKeyInfo       *KInfo;
	char            String[ 256 ];

	// �p�b�h�̓��͖��͐擪�Ƀp�b�h�̔ԍ���t����
	sprintf( String, "No.%d", ITInfo->PadNo );

	// �Ή����Ă���p�b�h�� DirectInput �̏��^�C�v�ɂ���ĕ���
	switch( ITInfo->DirectInputType )
	{
	case EDInputType_X:
		// X�����͂̏ꍇ�̓v���X�̏ꍇ�� RIGHT�A�}�C�i�X�̏ꍇ�� LEFT ��t����
		sprintf( PadInputString, "%s %s",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "RIGHT" : "LEFT" );
		break;

	case EDInputType_Y:
		// Y�����͂̏ꍇ�̓v���X�̏ꍇ�� DOWN�A�}�C�i�X�̏ꍇ�� UP ��t����
		sprintf( PadInputString, "%s %s",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "DOWN" : "UP" );
		break;

	case EDInputType_Z:
		// Z�����͂̏ꍇ�̓v���X�̏ꍇ�� +�A�}�C�i�X�̏ꍇ�� - ��t����
		sprintf( PadInputString, "%s %sZ",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Rx:
		// X����]���͂̏ꍇ�̓v���X�̏ꍇ�� +�A�}�C�i�X�̏ꍇ�� - ��t����
		sprintf( PadInputString, "%s %sRx",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Ry:
		// Y����]���͂̏ꍇ�̓v���X�̏ꍇ�� +�A�}�C�i�X�̏ꍇ�� - ��t����
		sprintf( PadInputString, "%s %sRy",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Rz:
		// Z����]���͂̏ꍇ�̓v���X�̏ꍇ�� +�A�}�C�i�X�̏ꍇ�� - ��t����
		sprintf( PadInputString, "%s %sRz",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_POV:
		// �����R���g���[�����͂̏ꍇ�� POV �ł��邱�ƂƁAPOV�̏��ԍ��Ɠ��͊p�x��t����
		sprintf( PadInputString, "%s POV %d Angle %d��",
			String, ITInfo->SubInfo[ 0 ], ITInfo->SubInfo[ 1 ] / 100 );
		break;

	case EDInputType_Button:
		// �{�^�����͂̏ꍇ�� BUTTON �ƃ{�^���ԍ���t����
		sprintf( PadInputString, "%s BUTTON %d",
			String, ITInfo->SubInfo[ 0 ] );
		break;
	}

	// �L�[�R�[�h�̏��e�[�u���ɖ����L�[�R�[�h�ł���ꍇ�ɔ����āA
	// �ŏ��� UNKNOWN �������Ă���
	strcpy( KeyInputString, "UNKNOWN" );

	// ��v����L�[�R�[�h�����e�[�u������T��
	for( KInfo = g_KeyInfoTable; KInfo->Name != NULL; KInfo++ )
	{
		if( KInfo->KeyInput == ITInfo->KeyInput )
		{
			strcpy( KeyInputString, KInfo->Name );
			break;
		}
	}
}


