#include "Task_KeyConfig.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// �L�[�R���t�B�O��ʂ̃f�[�^������t�H���_�̃p�X
#define KEYCONFIGDATA_DIR		"Data\\2D\\"

// �L�[�R���t�B�O��ʂ��t�F�[�h�C������O�ɑ҂���
#define FADE_IN_WAIT_TIME		(0.5f)

// �L�[�R���t�B�O��ʂ̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME			(0.5f)

// ���j���[���ڂ̖��łP���ɂ����鎞��
#define MENU_BRINK_TIME			(2.0f)

// �L�[�R���t�B�O��ʂ̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME			(0.5f)

// �L�[�R���t�B�O��ʂŎg�p����t�H���g�̃T�C�Y
#define FONT_SIZE				(32)

// �L�[�R���t�B�O��ʂ̃^�C�g����`�悷����W
#define TITLE_X					(550)
#define TITLE_Y					(32)

// ���͂̎�ނ�`�悷��x���W
#define TYPE_Y					(100)

// ���j���[���ڂ̕`��J�n���W
#define MENU_X					(200)
#define MENU_Y					(150)

// ���j���[���ڈ������ɋ󂯂�X�y�[�X
#define MENU_SPACE				(50)

// ���j���[���ڂ̃p�b�h���͂�`�悷��w���W
#define MENU_PAD_X				(550)

// ���j���[���ڂ̃L�[���͂�`�悷��w���W
#define MENU_KEY_X				(950)

// �L�[�R���t�B�O�����̏��
typedef enum _ETask_KeyConfigState
{
	ETask_KeyConfigState_FadeInWait,	// �t�F�[�h�C���҂���
	ETask_KeyConfigState_FadeIn,		// �t�F�[�h�C����
	ETask_KeyConfigState_Select,		// ���ڑI��
	ETask_KeyConfigState_FadeOut,		// �t�F�[�h�A�E�g��
	ETask_KeyConfigState_End,			// �I�����
} ETask_KeyConfigState;

// �L�[�R���t�B�O�̍���
typedef enum _ETask_KeyConfigMenu
{
	ETask_KeyConfigMenu_Attack,			// �U���{�^��
	ETask_KeyConfigMenu_Defence,		// �h��{�^��
	ETask_KeyConfigMenu_Jump,			// �W�����v�{�^��
	ETask_KeyConfigMenu_Pause,			// �|�[�Y�{�^��
	ETask_KeyConfigMenu_CameraLeft,		// �J����������
	ETask_KeyConfigMenu_CameraRight,	// �J�����E����
	ETask_KeyConfigMenu_CameraUp,		// �J���������
	ETask_KeyConfigMenu_CameraDown,		// �J����������

	ETask_KeyConfigMenu_Default,		// �ݒ���f�t�H���g�ɖ߂�
	ETask_KeyConfigMenu_Back,			// �^�C�g����ʂɖ߂�

	ETask_KeyConfigMenu_Num				// ���ڂ̐�
} ETask_KeyConfigMenu;

// �L�[�R���t�B�O�����p�̃f�[�^
typedef struct _STask_KeyConfigData
{
	// �^�X�N�̏��
	STaskInfo            TaskInfo;

	// �����̏��
	ETask_KeyConfigState State;

	// �ėp�J�E���^
	float                Counter;

	// �I�����Ă��鍀��
	int                  SelectPosition;

	// ���j���[�̕s�����x
	int                  MenuOpacity;

	// �I�����ڂ̖��ŏ����p�J�E���^
	float                MenuBrinkCounter;

	// �L�[�R���t�B�O��ʂŎg�p����t�H���g�̃n���h��
	int                  FontHandle;

	// �L�[�R���t�B�O�̔w�i�摜
	int                  BackGraphHandle;
} STask_KeyConfigData;

// �L�[�R���t�B�O�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_KeyConfig_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �L�[�R���t�B�O�̕`�揈�����s��
static void Task_KeyConfig_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �L�[�R���t�B�O�̌�n���������s��
static void Task_KeyConfig_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �L�[�R���t�B�O�����^�X�N�̊�{���
static STaskBaseInfo g_Task_KeyConfigTaskBaseInfo =
{
	8,
	8,
	Task_KeyConfig_Step,
	Task_KeyConfig_Render,
	Task_KeyConfig_Terminate
};

// �L�[�R���t�B�O�e���ڂ̖��O
static const char *g_KeyConfigMenuTable[] =
{
	"�U��",
	"�h��",
	"�W�����v",
	"�ꎞ��~",
	"�J����������",
	"�J�����E����",
	"�J���������",
	"�J����������",
	"�f�t�H���g�ݒ�ɖ߂�",
	"�^�C�g����ʂɖ߂�",
};

// �e���j���[���ڂƓ��͏��Ƃ̑Ή��e�[�u��
static EInputType g_InputTypeTable[] =
{
	EInputType_Attack,
	EInputType_Defence,
	EInputType_Jump,
	EInputType_Pause,

	EInputType_Camera_Left,
	EInputType_Camera_Right,
	EInputType_Camera_Up,
	EInputType_Camera_Down,
};

// �L�[�R���t�B�O�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_KeyConfig_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;
	int                  EdgeInput;

	// ���j���[���ږ��ŗp�J�E���^�̎��Ԃ�i�߂�
	KCData->MenuBrinkCounter += StepTime;
	if( KCData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		KCData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// �L�[�R���t�B�O��ʏ����̏�Ԃɂ���ď����𕪊�
	switch( KCData->State )
	{
	case ETask_KeyConfigState_FadeInWait:	// �t�F�[�h�C���҂���
		KCData->Counter += StepTime;
		if( KCData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����t�F�[�h�C�����J�n����
			KCData->State   = ETask_KeyConfigState_FadeIn;
			KCData->Counter = 0;
		}
		break;

	case ETask_KeyConfigState_FadeIn:		// �t�F�[�h�C����
		KCData->Counter += StepTime;
		if( KCData->Counter >= FADE_IN_TIME )
		{
			// �t�F�[�h�C�������������獀�ڑI�����J�n����
			KCData->MenuOpacity = 255;
			KCData->State       = ETask_KeyConfigState_Select;
		}
		else
		{
			// �t�F�[�h�C�����̃��j���[���ڂ̕s�����x���Z�o
			KCData->MenuOpacity = ( int )( KCData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_KeyConfigState_Select:		// ���ڑI��
		// �����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 )
		{
			// ��������͂����ꂽ��J�[�\���ړ�����炵�A�I�����ڂ����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );

			if( KCData->SelectPosition == 0 )
			{
				KCData->SelectPosition = ETask_KeyConfigMenu_Num - 1;
			}
			else
			{
				KCData->SelectPosition--;
			}
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 )
		{
			// ���������͂����ꂽ��J�[�\���ړ�����炵�A�I�����ڂ����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );

			if( KCData->SelectPosition == ETask_KeyConfigMenu_Num - 1 )
			{
				KCData->SelectPosition = 0;
			}
			else
			{
				KCData->SelectPosition++;
			}
		}

		// �I�����ڂ��u�ݒ���f�t�H���g�ɖ߂��v����ɂ��邩�ǂ����ŏ����𕪊�
		if( KCData->SelectPosition < ETask_KeyConfigMenu_Default )
		{
			// �I�����ڂ��u�ݒ���f�t�H���g�ɖ߂��v����ɂ���ꍇ�̓L�[��p�b�h��
			// ������Ԃ��`�F�b�N���āA��������Ă�����L�[�R���t�B�O�ݒ�ɔ��f������
			if( UpdateInputTypeInfo( g_InputTypeTable[ KCData->SelectPosition ] ) )
			{
				// �������������特��炷
				System_PlayCommonSE( ECommonSE_Enter );
			}
		}
		else
		{
			// �I�����ڂ��u�ݒ���f�t�H���g�ɖ߂��v�������艺�ɂ���ꍇ

			if( ( EdgeInput & ( 1 << EInputType_Attack ) ) != 0 )
			{
				// ����{�^���������ꂽ��J�[�\���̈ʒu�ɉ����ď����𕪊�
				switch( KCData->SelectPosition )
				{
				case ETask_KeyConfigMenu_Default:
					// �u�ݒ���f�t�H���g�ɖ߂��v���I������Ă����ꍇ��
					// �L�[�R���t�B�O�ݒ���f�t�H���g�ɖ߂�
					System_PlayCommonSE( ECommonSE_Enter );

					SetDefaultSetting();
					break;

				case ETask_KeyConfigMenu_Back:
					// �u�^�C�g���ɖ߂�v���I������Ă����ꍇ�̓t�F�[�h�A�E�g���J�n����
					System_PlayCommonSE( ECommonSE_Cancel );

					System_FadeOut();

					KCData->State = ETask_KeyConfigState_FadeOut;
					KCData->Counter = 0;
					break;
				}
			}
		}
		break;

	case ETask_KeyConfigState_FadeOut:		// �t�F�[�h�A�E�g��
		KCData->Counter += StepTime;
		if( KCData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g������������̏���

			// �ꉞ���j���[�̕s�����x���O�ɂ���
			KCData->MenuOpacity = 0;

			// ��Ԃ��I���ɂ���
			KCData->State = ETask_KeyConfigState_End;

			// �^�X�N���폜����
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &KCData->TaskInfo );

			// �L�[�R���t�B�O�̐ݒ��ۑ�����
			SaveInputSetting();

			// �^�C�g����ʂ��J�n����
			if( !System_StartTitle() )
			{
				return false;
			}
		}
		else
		{
			// �t�F�[�h�A�E�g���̃��j���[���ڂ̕s�����x���Z�o
			KCData->MenuOpacity = 255 - ( int )( KCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_KeyConfigState_End:			// �I�����
		break;
	}

	// ����I��
	return true;
}

// �L�[�R���t�B�O�̕`�揈�����s��
static void Task_KeyConfig_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;
	int                  SelectColor;
	int                  NormalColor;
	int                  DrawColor;
	int                  DrawY;
	float                t;
	int                  i;
	char                 PadString[ 256 ];
	char                 KeyString[ 256 ];
	int                  Bright;

	// ���j���[���ڂ̕s�����x���O�̏ꍇ�͉��������ɏI��
	if( KCData->MenuOpacity == 0 )
	{
		return;
	}

	// �I�����Ă��鍀�ڂ̖��ł���F�̎Z�o
	t = sin( ( float )KCData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
	if( t < 0.0f ) t = -t;
	SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

	// �ʏ�̕`��F�̒l���擾
	NormalColor = GetColor( 255,255,255 );

	// �L�[�R���t�B�O�̔w�i�̓��j���[���ڂ̕s�����x�ɉ����ċP�x�𒲐߂���
	Bright = KCData->MenuOpacity * 80 / 255;
	SetDrawBright( Bright, Bright, Bright );
	DrawGraph( 0, 0, KCData->BackGraphHandle, FALSE );
	SetDrawBright( 255,255,255 );

	// �A���t�@�u�����h���[�h�Ń��j���[���ڂ̕s�����x��ݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, KCData->MenuOpacity );

	// �L�[�R���t�B�O��ʂ̃^�C�g����`��
	DrawStringToHandle(
		TITLE_X, TITLE_Y, "KEY CONFIG", NormalColor, KCData->FontHandle );

	// ���͂̎�ނ̃Q�[���p�b�h�������uPAD�v�̕`��
	DrawStringToHandle(
		MENU_PAD_X, TYPE_Y, "PAD", NormalColor, KCData->FontHandle );

	// ���͂̎�ނ̃L�[�{�[�h�L�[�������uKEY�v�̕`��
	DrawStringToHandle(
		MENU_KEY_X, TYPE_Y, "KEY", NormalColor, KCData->FontHandle );

	// ���j���[���ڂ̐������J��Ԃ�
	DrawY = MENU_Y;
	for( i = 0; i < ETask_KeyConfigMenu_Num; i++ )
	{
		// ���ڂ��I������Ă��邩�ǂ����ŕ`��F��ς���
		DrawColor = KCData->SelectPosition == i ? SelectColor : NormalColor;

		// ���ږ��̕`��
		DrawStringToHandle(
			MENU_X, DrawY, g_KeyConfigMenuTable[ i ], DrawColor, KCData->FontHandle );

		// �u�f�t�H���g�̐ݒ�ɖ߂��v����̍��ڂ̏ꍇ�́A
		// �Q�[���̓��͂Ɋ��蓖�Ă��Ă���Q�[���p�b�h�̓��͂ƃL�[�̓��͂�`�悷��
		if( i < ETask_KeyConfigMenu_Default )
		{
			// �Q�[���̓��͂Ɋ��蓖�Ă��Ă���p�b�h�̓��͂ƃL�[�̓��̖͂��O���擾����
			GetInputTypeString( g_InputTypeTable[ i ], PadString, KeyString );

			// ���蓖�Ă��Ă���p�b�h�̓��͖���`�悷��
			DrawStringToHandle(
				MENU_PAD_X, DrawY, PadString, DrawColor, KCData->FontHandle );

			// ���蓖�Ă��Ă���L�[�̓��͖���`�悷��
			DrawStringToHandle(
				MENU_KEY_X, DrawY, KeyString, DrawColor, KCData->FontHandle );
		}

		// ���ڂ̕`��Y���W���P���ڕ����Ɉړ�����
		DrawY += MENU_SPACE;
	}

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �L�[�R���t�B�O�̌�n���������s��
static void Task_KeyConfig_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;

	// �L�[�R���t�B�O��ʗp�ɍ쐬�����t�H���g�n���h�����폜����
	DeleteFontToHandle( KCData->FontHandle );

	// �L�[�R���t�B�O�̔w�i�摜�̍폜
	DeleteGraph( KCData->BackGraphHandle );
}

// �L�[�R���t�B�O��ʂ��J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Task_KeyConfig_Start( void )
{
	STask_KeyConfigData *KCData;

	// �L�[�R���t�B�O��ʂ̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	KCData = ( STask_KeyConfigData * )calloc( 1, sizeof( STask_KeyConfigData ) );
	if( KCData == NULL )
	{
		return false;
	}

	// �L�[�R���t�B�O��ʂŎg�p����t�H���g�̍쐬
	KCData->FontHandle = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( KCData->FontHandle == -1 )
	{
		return false;
	}

	// �L�[�R���t�B�O��ʂ̔w�i�摜�̓ǂݍ���
	KCData->BackGraphHandle  = LoadGraph( KEYCONFIGDATA_DIR "KeyConfigBack.png" );
	if( KCData->BackGraphHandle == -1 )
	{
		return false;
	}

	// ���j���[���ڂ̃t�F�[�h�C���҂��̏�Ԃɂ���
	KCData->State            = ETask_KeyConfigState_FadeInWait;
	KCData->Counter          = 0.0f;

	// �I�����ڂ̏�����
	KCData->SelectPosition   = 0;

	// ���j���[���ڂ̕s�����x��������
	KCData->MenuOpacity      = 0;

	// �I�����ڂ̖��ŗp�J�E���^�̏�����
	KCData->MenuBrinkCounter = 0;

	// ���t�F�[�h�̉������J�n
	System_FadeIn();

	// �^�X�N��o�^����
	KCData->TaskInfo.Base = &g_Task_KeyConfigTaskBaseInfo;
	KCData->TaskInfo.Data = KCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &KCData->TaskInfo );

	// ����I��
	return true;
}


