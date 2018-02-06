#include "Task_Title.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// �^�C�g����ʂ̃f�[�^������t�H���_�̃p�X
#define TITLEDATA_DIR			"Data\\2D\\"

// �^�C�g����ʂ��t�F�[�h�C������O�ɑ҂���
#define FADE_IN_WAIT_TIME		(0.5f)

// �^�C�g����ʂ̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME			(1.0f)

// ���j���[���ڂ��t�F�[�h�C������O�ɑ҂���
#define MENU_IN_WAIT_TIME		(0.2f)

// ���j���[���ڂ̃t�F�[�h�C���ɂ����鎞��
#define MENU_IN_TIME			(0.25f)

// ���j���[���ڂ̖��łP���ɂ����鎞��
#define MENU_BRINK_TIME			(2.0f)

// �^�C�g����ʂ̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME			(1.0f)

// �^�C�g����ʂŎg�p����t�H���g�̃T�C�Y
#define FONT_SIZE				(40)

// ���j���[���ڂ̕`��J�n���W
#define MENU_X					(530)
#define MENU_Y					(450)

// ���j���[���ڈ������ɋ󂯂�X�y�[�X
#define MENU_SPACE				(64)

// �^�C�g����ʏ����̏��
typedef enum _ETask_TitleState
{
	ETask_TitleState_FadeInWait,	// �t�F�[�h�C���҂���
	ETask_TitleState_FadeIn,		// �t�F�[�h�C����
	ETask_TitleState_MenuInWait,	// ���j���[���ڃt�F�[�h�C���҂���
	ETask_TitleState_MenuIn,		// ���j���[���ڃt�F�[�h�C����
	ETask_TitleState_Select,		// ���ڑI��
	ETask_TitleState_FadeOut,		// �t�F�[�h�A�E�g��
	ETask_TitleState_End,			// �I�����
} ETask_TitleState;

// �^�C�g����ʂ̃��j���[����
typedef enum _ETask_TitleMenu
{
	ETask_TitleMenu_GameStart,		// �Q�[���X�^�[�g
	ETask_TitleMenu_KeyConfig,		// �L�[�R���t�B�O
	ETask_TitleMenu_Exit,			// �Q�[���I��

	ETask_TitleMenu_Num,			// ���ڂ̐�
} ETask_TitleMenu;

// �^�C�g����ʏ����p�̃f�[�^
typedef struct _STask_TitleData
{
	// �^�X�N�̏��
	STaskInfo	     TaskInfo;

	// �����̏��
	ETask_TitleState State;

	// �ėp�J�E���^
	float            Counter;

	// �I�����Ă��鍀��
	int              SelectPosition;

	// �^�C�g���̕s�����x
	int              TitleOpacity;

	// ���j���[�̕s�����x
	int              MenuOpacity;

	// �I�����ڂ̖��ŏ����p�J�E���^
	float            MenuBrinkCounter;

	// �^�C�g���摜
	int              TitleGraphHandle;

	// �^�C�g���̔w�i�摜
	int              BackGraphHandle;

	// ���j���[���ڗp�t�H���g
	int              MenuFontHandle;
} STask_TitleData;

// �^�C�g����ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_Title_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �^�C�g����ʂ̕`�揈�����s��
static void Task_Title_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �^�C�g����ʂ̌�n���������s��
static void Task_Title_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �^�C�g�������^�X�N�̊�{���
static STaskBaseInfo g_Task_TitleTaskBaseInfo =
{
	8,
	8,
	Task_Title_Step,
	Task_Title_Render,
	Task_Title_Terminate
};

// �^�C�g����ʊe���ڂ̖��O
static const char *g_TitleMenuTable[] =
{
	"GAME START",
	"KEY CONFIG",
	"EXIT"
};

// �^�C�g����ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_Title_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;
	int              EdgeInput;

	// ���j���[���ږ��ŗp�J�E���^�̎��Ԃ�i�߂�
	TTData->MenuBrinkCounter += StepTime;
	if( TTData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		TTData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// �^�C�g����ʏ����̏�Ԃɂ���ď����𕪊�
	switch( TTData->State )
	{
	case ETask_TitleState_FadeInWait:		// �t�F�[�h�C���҂���
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����t�F�[�h�C�����J�n����
			TTData->State   = ETask_TitleState_FadeIn;
			TTData->Counter = 0;

			// �^�C�g����ʂa�f�l�̍Đ����J�n����
			Sound_PlayBGM( EBGM_Title );
		}
		break;

	case ETask_TitleState_FadeIn:			// �t�F�[�h�C����
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_IN_TIME )
		{
			// �t�F�[�h�C�������������烁�j���[���ڃt�F�[�h�C���҂����J�n����
			TTData->TitleOpacity = 255;
			TTData->State        = ETask_TitleState_MenuInWait;
			TTData->Counter      = 0.0f;
		}
		else
		{
			// �t�F�[�h�C�����̃^�C�g���̕s�����x���Z�o
			TTData->TitleOpacity = ( int )( TTData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_TitleState_MenuInWait:		// ���j���[���ڃt�F�[�h�C���҂���
		TTData->Counter += StepTime;
		if( TTData->Counter > MENU_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂����烁�j���[���ڂ̃t�F�[�h�C�����J�n����
			TTData->State   = ETask_TitleState_MenuIn;
			TTData->Counter = 0.0f;
		}
		break;

	case ETask_TitleState_MenuIn:			// ���j���[���ڃt�F�[�h�C����
		TTData->Counter += StepTime;
		if( TTData->Counter > MENU_IN_TIME )
		{
			// �t�F�[�h�C�������������獀�ڑI�����J�n����
			TTData->MenuOpacity = 255;
			TTData->State       = ETask_TitleState_Select;
		}
		else
		{
			// �t�F�[�h�C�����̃��j���[���ڂ̕s�����x���Z�o
			TTData->MenuOpacity = ( int )( TTData->Counter * 255 / MENU_IN_TIME );
		}
		break;

	case ETask_TitleState_Select:			// ���ڑI��
		// �����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 &&
			TTData->SelectPosition > 0 )
		{
			// ��������͂�����Ă��āA����ԏ�̍��ڂ�I�����Ă��Ȃ��ꍇ��
			// �J�[�\���ړ�����炵�A�I�����ڂ����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );
			TTData->SelectPosition--;
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 &&
			TTData->SelectPosition < ETask_TitleMenu_Num - 1 )
		{
			// ���������͂�����Ă��āA����ԉ��̍��ڂ�I�����Ă��Ȃ��ꍇ��
			// �J�[�\���ړ�����炵�A�I�����ڂ�����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );
			TTData->SelectPosition++;
		}

		if( ( EdgeInput & ( 1 << EInputType_Atk ) ) != 0 )
		{
			// ����{�^���������ꂽ��I������炵�A�t�F�[�h�A�E�g���J�n����
			if( TTData->SelectPosition == ETask_TitleMenu_Exit )
			{
				System_PlayCommonSE( ECommonSE_Cancel );
			}
			else
			{
				System_PlayCommonSE( ECommonSE_Enter );
			}

			System_FadeOut();

			TTData->State   = ETask_TitleState_FadeOut;
			TTData->Counter = 0;
		}
		break;

	case ETask_TitleState_FadeOut:			// �t�F�[�h�A�E�g��
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g������������̏���

			// �ꉞ���j���[�ƃ^�C�g���̕s�����x���O�ɂ���
			TTData->MenuOpacity  = 0;
			TTData->TitleOpacity = 0;
			
			// ��Ԃ��I���ɂ���
			TTData->State = ETask_TitleState_End;

			// �I���������ڂɉ����ď����𕪊�
			switch( TTData->SelectPosition )
			{
			case ETask_TitleMenu_GameStart:
				// �Q�[���X�^�[�g���I������Ă�����Q�[���{�҂��J�n����
				if( !System_StartGameMain() )
				{
					return false;
				}
				break;

			case ETask_TitleMenu_KeyConfig:
				// �L�[�R���t�B�O���I������Ă�����L�[�R���t�B�O��ʂ��J�n����
				if( !System_StartKeyConfig() )
				{
					return false;
				}
				break;

			case ETask_TitleMenu_Exit:
				// �Q�[���I�����I������Ă�����Q�[�����I������
				System_ExitGame();
				break;
			}

			// �^�X�N���폜����
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &TTData->TaskInfo );
		}
		else
		{
			// �t�F�[�h�A�E�g���̃��j���[���ڂƃ^�C�g���̕s�����x���Z�o
			TTData->TitleOpacity =
				255 - ( int )( TTData->Counter * 255 / FADE_OUT_TIME );

			TTData->MenuOpacity  = TTData->TitleOpacity;
		}
		break;

	case ETask_TitleState_End:				// �I�����
		break;
	}

	// ����I��
	return true;
}

// �^�C�g����ʂ̕`�揈�����s��
static void Task_Title_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;
	int              SelectColor;
	int              NormalColor;
	int              DrawColor;
	int	             DrawY;
	float            t;
	int              i;
	int              Bright;

	// �^�C�g���̕s�����x���O�ł͂Ȃ��ꍇ�̂݃^�C�g����`�悷��
	if( TTData->TitleOpacity > 0 )
	{
		// �^�C�g���̔w�i�̓^�C�g���̕s�����x�ɉ����ċP�x�𒲐߂���
		Bright = TTData->TitleOpacity * 160 / 255;
		SetDrawBright( Bright/ 4, Bright, Bright / 2 );
		DrawGraph( 0, 0, TTData->BackGraphHandle, FALSE );
		SetDrawBright( 255,255,255 );

		// �A���t�@�u�����h���[�h�Ń^�C�g���̕s�����x��ݒ肵�A�^�C�g���摜��`�悷��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, TTData->TitleOpacity );
		DrawGraph( 0, 0, TTData->TitleGraphHandle, TRUE );
	}

	// ���j���[���ڂ̕s�����x���O�ł͂Ȃ��ꍇ�̂݃��j���[���ڂ�`�悷��
	if( TTData->MenuOpacity > 0 )
	{
		// �I�����Ă��鍀�ڂ̖��ł���F�̎Z�o
		t = sin( ( float )TTData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
		if( t < 0.0f ) t = -t;
		SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

		// �ʏ�̕`��F�̒l���擾
		NormalColor = GetColor( 255,255,255 );

		// �A���t�@�u�����h���[�h�Ń��j���[���ڂ̕s�����x��ݒ�
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, TTData->MenuOpacity );

		// ���j���[���ڂ̐������J��Ԃ�
		DrawY = MENU_Y;
		for( i = 0; i < ETask_TitleMenu_Num; i++ )
		{
			// ���ڂ��I������Ă��邩�ǂ����ŕ`��F��ς���
			DrawColor = TTData->SelectPosition == i ? SelectColor : NormalColor;

			// ���ڂ̕`��
			DrawStringToHandle(
				MENU_X, DrawY, g_TitleMenuTable[ i ], DrawColor, TTData->MenuFontHandle);

			// ���ڂ̕`��Y���W���P���ڕ����Ɉړ�����
			DrawY += MENU_SPACE;
		}
	}

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �^�C�g����ʂ̌�n���������s��
static void Task_Title_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;

	// �^�C�g���摜�̍폜
	DeleteGraph( TTData->TitleGraphHandle );

	// �^�C�g���̔w�i�摜�̍폜
	DeleteGraph( TTData->BackGraphHandle );

	// ���j���[���ڕ`��p�̃t�H���g���폜
	DeleteFontToHandle( TTData->MenuFontHandle );
}

// �^�C�g����ʂ̏������J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Task_Title_Start( void )
{
	STask_TitleData *TTData;

	// �^�C�g����ʂ̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	TTData = ( STask_TitleData * )calloc( 1, sizeof( STask_TitleData ) );
	if( TTData == NULL )
	{
		return false;
	}

	// �^�C�g���摜�̓ǂݍ���
	TTData->TitleGraphHandle  = LoadGraph( TITLEDATA_DIR "Swordbout_logo.png" );
	if( TTData->TitleGraphHandle == -1 )
	{
		return false;
	}

	// �^�C�g���̔w�i�摜�̓ǂݍ���
	TTData->BackGraphHandle  = LoadGraph( TITLEDATA_DIR "TitleBack.png" );
	if( TTData->BackGraphHandle == -1 )
	{
		return false;
	}

	// �^�C�g����ʂŎg�p����t�H���g�̍쐬
	TTData->MenuFontHandle    = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( TTData->MenuFontHandle == -1 )
	{
		return false;
	}

	// �^�C�g����ʂ̃t�F�[�h�C���҂��̏�Ԃɂ���
	TTData->State            = ETask_TitleState_FadeInWait;
	TTData->Counter          = 0.0f;

	// �I�����ڂ̏�����
	TTData->SelectPosition   = 0;

	// �^�C�g���ƃ��j���[���ڂ̕s�����x��������
	TTData->TitleOpacity     = 0;
	TTData->MenuOpacity      = 0;

	// �I�����ڂ̖��ŗp�J�E���^�̏�����
	TTData->MenuBrinkCounter = 0;

	// ���t�F�[�h�̉������J�n
	System_FadeIn();

	// �^�X�N��o�^����
	TTData->TaskInfo.Base     = &g_Task_TitleTaskBaseInfo;
	TTData->TaskInfo.Data     = TTData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &TTData->TaskInfo );

	// ����I��
	return true;
}


