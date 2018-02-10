#include "Task_PauseMenu.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// �|�[�Y���j���[�̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME			(0.5f)

// ���j���[���ڂ̖��łP���ɂ����鎞��
#define MENU_BRINK_TIME			(2.0f)

// �|�[�Y���j���[�̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME			(0.5f)

// �|�[�Y���j���[�Ŏg�p����t�H���g�̃T�C�Y
#define FONT_SIZE				(40)

// ���j���[���ڂ̕`��J�n���W
#define MENU_X					(460)
#define MENU_Y					(250)

// ���j���[���ڈ������ɋ󂯂�X�y�[�X
#define MENU_SPACE				(76)

// �|�[�Y���j���[�����̏��
typedef enum _ETask_PauseMenuState
{
	ETask_PauseMenuState_FadeIn,		// �t�F�[�h�C����
	ETask_PauseMenuState_Select,		// ���ڑI��
	ETask_PauseMenuState_FadeOut,		// �t�F�[�h�A�E�g��
	ETask_PauseMenuState_End,			// �I�����
} ETask_PauseMenuState;

// �|�[�Y���j���[�����p�̃f�[�^
typedef struct _STask_PauseMenuData
{
	// �^�X�N�̏��
	STaskInfo	     TaskInfo;

	// �����̏��
	ETask_PauseMenuState State;

	// �ėp�J�E���^
	float            Counter;

	// �I�����Ă��鍀��
	int              SelectPosition;

	// ���j���[�̕s�����x
	int              MenuOpacity;

	// �I�����ڂ̖��ŏ����p�J�E���^
	float            MenuBrinkCounter;

	// ���j���[���ڗp�t�H���g
	int              MenuFontHandle;
} STask_PauseMenuData;

// �|�[�Y���j���[�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_PauseMenu_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �|�[�Y���j���[�̕`�揈�����s��
static void Task_PauseMenu_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �|�[�Y���j���[�̌�n���������s��
static void Task_PauseMenu_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �^�C�g�������^�X�N�̊�{���
static STaskBaseInfo g_Task_PauseMenuTaskBaseInfo =
{
	8,
	8,
	Task_PauseMenu_Step,
	Task_PauseMenu_Render,
	Task_PauseMenu_Terminate
};

// �|�[�Y���j���[�e���ڂ̖��O
static const char *g_PauseMenuTable[] =
{
	"�Q�[���ĊJ",
	"�^�C�g����ʂɖ߂�",
	"�Q�[�����I������"
};

// �|�[�Y���j���[�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_PauseMenu_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;
	int                  EdgeInput;

	// ���j���[���ږ��ŗp�J�E���^�̎��Ԃ�i�߂�
	PTData->MenuBrinkCounter += StepTime;
	if( PTData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		PTData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// �|�[�Y���j���[�����̏�Ԃɂ���ď����𕪊�
	switch( PTData->State )
	{
	case ETask_PauseMenuState_FadeIn:			// �t�F�[�h�C����
		PTData->Counter += StepTime;
		if( PTData->Counter > FADE_IN_TIME )
		{
			// �t�F�[�h�C�������������獀�ڑI�����J�n����
			PTData->MenuOpacity = 255;
			PTData->State       = ETask_PauseMenuState_Select;
		}
		else
		{
			// �t�F�[�h�C�����̃��j���[���ڂ̕s�����x���Z�o
			PTData->MenuOpacity = ( int )( PTData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_PauseMenuState_Select:			// ���ڑI��
		// �����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 &&
			PTData->SelectPosition > 0 )
		{
			// ��������͂�����Ă��āA����ԏ�̍��ڂ�I�����Ă��Ȃ��ꍇ��
			// �J�[�\���ړ�����炵�A�I�����ڂ����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );
			PTData->SelectPosition--;
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 &&
			PTData->SelectPosition < ETask_PauseMenu_Num - 1 )
		{
			// ���������͂�����Ă��āA����ԉ��̍��ڂ�I�����Ă��Ȃ��ꍇ��
			// �J�[�\���ړ�����炵�A�I�����ڂ�����Ɉړ�����
			System_PlayCommonSE( ECommonSE_Cursor );
			PTData->SelectPosition++;
		}

		if( ( EdgeInput & ( 1 << EInputType_Atk ) ) != 0 ||
			( EdgeInput & ( 1 << EInputType_Pause  ) ) != 0 )
		{
			// ����{�^�����|�[�Y�{�^���������ꂽ��I������炵�A
			// �t�F�[�h�A�E�g���J�n����
			if( PTData->SelectPosition == ETask_PauseMenu_Continue )
			{
				System_PlayCommonSE( ECommonSE_Cancel );
			}
			else
			{
				System_PlayCommonSE( ECommonSE_Enter );
			}

			// �Q�[���I�����^�C�g���֖߂邪�I�����ꂽ���ʂ̃t�F�[�h�A�E�g���J�n����
			if( PTData->SelectPosition == ETask_PauseMenu_GameEnd ||
				PTData->SelectPosition == ETask_PauseMenu_BackToTitle )
			{
				System_FadeOut();
			}

			PTData->State   = ETask_PauseMenuState_FadeOut;
			PTData->Counter = 0;
		}
		break;

	case ETask_PauseMenuState_FadeOut:			// �t�F�[�h�A�E�g��
		// �I�����ꂽ���ڂɂ���ď����𕪊�
		switch( PTData->SelectPosition )
		{
		case ETask_PauseMenu_Continue:
			PTData->Counter += StepTime;
			if( PTData->Counter > FADE_OUT_TIME )
			{
				// �t�F�[�h�A�E�g������������̏���

				// �ꉞ���j���[�ƃ^�C�g���̕s�����x���O�ɂ���
				PTData->MenuOpacity  = 0;
			
				// ��Ԃ��I���ɂ���
				PTData->State = ETask_PauseMenuState_End;
			}
			else
			{
				// �t�F�[�h�A�E�g���̃��j���[���ڂƃ^�C�g���̕s�����x���Z�o
				PTData->MenuOpacity =
					255 - ( int )( PTData->Counter * 255 / FADE_OUT_TIME );
			}
			break;

		case ETask_PauseMenu_BackToTitle:
		case ETask_PauseMenu_GameEnd:
			if( !System_CheckFade() )
			{
				// ��Ԃ��I���ɂ���
				PTData->State = ETask_PauseMenuState_End;
			}
			break;
		}
		break;

	case ETask_PauseMenuState_End:				// �I�����
		break;
	}

	// ����I��
	return true;
}

// �|�[�Y���j���[�̕`�揈�����s��
static void Task_PauseMenu_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;
	int                  SelectColor;
	int                  NormalColor;
	int                  DrawColor;
	int	                 DrawY;
	float                t;
	int                  i;

	// ���j���[���ڂ̕s�����x���O�ł͂Ȃ��ꍇ�̂݃��j���[���ڂ�`�悷��
	if( PTData->MenuOpacity > 0 )
	{
		// ��ʑS�̂𔼓����̍��ŕ���
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, PTData->MenuOpacity / 2 );
		DrawBox( 0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, GetColor( 0,0,0 ), TRUE );

		// �I�����Ă��鍀�ڂ̖��ł���F�̎Z�o
		t = sin( ( float )PTData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
		if( t < 0.0f ) t = -t;
		SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

		// �ʏ�̕`��F�̒l���擾
		NormalColor = GetColor( 255,255,255 );

		// �A���t�@�u�����h���[�h�Ń��j���[���ڂ̕s�����x��ݒ�
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, PTData->MenuOpacity );

		// ���j���[���ڂ̐������J��Ԃ�
		DrawY = MENU_Y;
		for( i = 0; i < ETask_PauseMenu_Num; i++ )
		{
			// ���ڂ��I������Ă��邩�ǂ����ŕ`��F��ς���
			DrawColor = PTData->SelectPosition == i ? SelectColor : NormalColor;

			// ���ڂ̕`��
			DrawStringToHandle(
				MENU_X, DrawY, g_PauseMenuTable[ i ], DrawColor, PTData->MenuFontHandle);

			// ���ڂ̕`��Y���W���P���ڕ����Ɉړ�����
			DrawY += MENU_SPACE;
		}
	}

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �|�[�Y���j���[�̌�n���������s��
static void Task_PauseMenu_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	// ���j���[���ڕ`��p�̃t�H���g���폜
	DeleteFontToHandle( PTData->MenuFontHandle );
}


// �|�[�Y���j���[���I�����������`�F�b�N����
//     �߂�l : �|�[�Y���j���[���I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
bool Task_PauseMenu_CheckEnd(
	// �|�[�Y���j���[�^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	// �����̏�Ԃ��I����Ԃ̏ꍇ�́A�������I�����Ă���̂� true ��Ԃ�
	return PTData->State == ETask_PauseMenuState_End;
}

// �|�[�Y���j���[�őI�����ꂽ���ڂ��擾����
//     �߂�l : �I�����ꂽ���ڂ̗񋓌^�̒l
ETask_PauseMenu Task_PauseMenu_GetChoiceIndex(
	// �|�[�Y���j���[�^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	return ( ETask_PauseMenu )PTData->SelectPosition;
}

// �|�[�Y���j���[�̏������J�n����
//     �߂�l : �|�[�Y���j���[�^�X�N���\���̂ւ̃A�h���X
STaskInfo * Task_PauseMenu_Start( void )
{
	STask_PauseMenuData *PTData;

	// �|�[�Y���j���[�̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	PTData = ( STask_PauseMenuData * )calloc( 1, sizeof( STask_PauseMenuData ) );
	if( PTData == NULL )
	{
		return NULL;
	}

	// �|�[�Y���j���[�Ŏg�p����t�H���g�̍쐬
	PTData->MenuFontHandle   = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( PTData->MenuFontHandle == -1 )
	{
		return NULL;
	}

	// �|�[�Y���j���[�̃t�F�[�h�C�����ɂ���
	PTData->State            = ETask_PauseMenuState_FadeIn;
	PTData->Counter          = 0.0f;

	// �I�����ڂ̏�����
	PTData->SelectPosition   = 0;

	// ���j���[���ڂ̕s�����x��������
	PTData->MenuOpacity      = 0;

	// �I�����ڂ̖��ŗp�J�E���^�̏�����
	PTData->MenuBrinkCounter = 0;

	// �^�X�N��o�^����
	PTData->TaskInfo.Base     = &g_Task_PauseMenuTaskBaseInfo;
	PTData->TaskInfo.Data     = PTData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &PTData->TaskInfo );

	// ����I��
	return &PTData->TaskInfo;
}


