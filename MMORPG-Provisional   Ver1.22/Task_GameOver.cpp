#include "Task_GameOver.h"
#include "DxLib.h"
#include "System.h"
#include "Sound.h"
#include <math.h>

// �wGAME OVER�x�̕�����
#define GAMEOVER_STRING					"GAME OVER"

// �Q�[���I�[�o�[�\�����t�F�[�h�C������O�ɑ҂���
#define FADE_IN_WAIT_TIME				(1.0f)

// �Q�[���I�[�o�[�\���̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME					(1.0f)

// �Q�[���I�[�o�[�\�����t�F�[�h�A�E�g����O�ɑ҂���
#define FADE_OUT_WAIT					(5.0f)

// �Q�[���I�[�o�[�\���̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME					(1.0f)

// �Q�[���I�[�o�[�\�������̏��
typedef enum _ETask_GameOverState
{
	ETask_GameOverState_FadeInWait,		// �t�F�[�h�C���҂���
	ETask_GameOverState_FadeIn,			// �t�F�[�h�C����
	ETask_GameOverState_FadeOutWait,	// �t�F�[�h�A�E�g�҂���
	ETask_GameOverState_FadeOut,		// �t�F�[�h�A�E�g��
	ETask_GameOverState_End,			// �I�����
} ETask_GameOverState;

// �Q�[���I�[�o�[�\�������p�̃f�[�^
typedef struct _STask_GameOverData
{
	// �^�X�N�̏��
	STaskInfo           TaskInfo;

	// �����̏��
	ETask_GameOverState State;

	// �ėp�J�E���^
	float               Counter;

	// �Q�[���I�[�o�[�\���̕s�����x
	int                 Opacity;
} STask_GameOverData;

// �Q�[���I�[�o�[�\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_GameOver_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �Q�[���I�[�o�[�\���̕`�揈�����s��
static void Task_GameOver_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �Q�[���I�[�o�[�\�������^�X�N�̊�{���
static STaskBaseInfo g_Task_GameOverTaskBaseInfo =
{
	8,
	8,
	Task_GameOver_Step,
	Task_GameOver_Render,
	NULL
};

// �Q�[���I�[�o�[�\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_GameOver_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;

	// �Q�[���I�[�o�[�\�������̏�Ԃɂ���ď����𕪊�
	switch( GOData->State )
	{
	case ETask_GameOverState_FadeInWait:	// �t�F�[�h�C���҂���
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����Q�[���I�[�o�[BGM�̍Đ���
			// �t�F�[�h�C�����J�n����
			Sound_PlayBGM( EBGM_GameOver );

			GOData->State   = ETask_GameOverState_FadeIn;
			GOData->Counter = 0.0f;
		}
		break;

	case ETask_GameOverState_FadeIn:		// �t�F�[�h�C����
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C��������������t�F�[�h�A�E�g�҂����J�n����
			GOData->Opacity = 255;

			GOData->State   = ETask_GameOverState_FadeOutWait;
			GOData->Counter = 0.0f;
		}
		else
		{
			// �t�F�[�h�C�����̃Q�[���I�[�o�[�\���̕s�����x���Z�o
			GOData->Opacity = ( int )( GOData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_GameOverState_FadeOutWait:	// �t�F�[�h�A�E�g�҂���
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_OUT_WAIT )
		{
			// �t�F�[�h�A�E�g�҂����Ԃ��o�߂�����t�F�[�h�A�E�g���J�n����
			GOData->State   = ETask_GameOverState_FadeOut;
			GOData->Counter = 0.0f;
		}
		break;

	case ETask_GameOverState_FadeOut:		// �t�F�[�h�A�E�g��
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g������������I����Ԃɂ���
			GOData->Opacity = 0;

			GOData->State   = ETask_GameOverState_End;
			GOData->Counter = 0.0f;
		}
		else
		{
			// �t�F�[�h�A�E�g���̃Q�[���I�[�o�[�\���̕s�����x���Z�o����
			GOData->Opacity = 255 - ( int )( GOData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_GameOverState_End:			// �I�����
		break;
	}

	// ����I��
	return true;
}

// �Q�[���I�[�o�[�\���̕`�揈�����s��
static void Task_GameOver_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;
	int                 DrawWidth;
	int                 DrawX;
	int                 DrawY;
	int                 FontHandle;

	// �Q�[���I�[�o�[�\���̕s�����x���O�̏ꍇ�͉��������I��
	if( GOData->Opacity == 0 )
	{
		return;
	}

	// �ėp�t�H���g�n���h���̎擾
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// �wGAME OVER�x�\���̕����擾����
	DrawWidth = GetDrawStringWidthToHandle(
					GAMEOVER_STRING, strlen( GAMEOVER_STRING ), FontHandle );

	// �wGAME OVER�x����ʒ��S�ɕ`�悷�邽�߂̍��W���Z�o
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2;
	DrawX = GAME_SCREEN_WIDTH  / 2 - DrawWidth           / 2;

	// �A���t�@�u�����h���[�h�ŃQ�[���I�[�o�[�\���̕s�����x��ݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, GOData->Opacity );

	// �wGAME OVER�x�̕`��
	DrawStringToHandle( DrawX, DrawY, GAMEOVER_STRING, GetColor( 255,0,0 ), FontHandle );

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �Q�[���I�[�o�[�\�����I�����������`�F�b�N����
//     �߂�l : �Q�[���I�[�o�[�\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
bool Task_GameOver_CheckEnd(
	// �Q�[���I�[�o�[�\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;

	// �����̏�Ԃ��I����Ԃ̏ꍇ�́A�������I�����Ă���̂� true ��Ԃ�
	return GOData->State == ETask_GameOverState_End;
}

// �Q�[���I�[�o�[�\�����J�n����
//     �߂�l : �Q�[���I�[�o�[�\���^�X�N���\���̂ւ̃A�h���X
STaskInfo * Task_GameOver_Start( void )
{
	STask_GameOverData *GOData;

	// �Q�[���I�[�o�[�\���̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	GOData = ( STask_GameOverData * )calloc( 1, sizeof( STask_GameOverData ) );
	if( GOData == NULL )
	{
		return NULL;
	}

	// �s�����x������������
	GOData->Opacity = 0;
	
	// �t�F�[�h�C���҂��̏�Ԃɂ���
	GOData->State = ETask_GameOverState_FadeInWait;
	GOData->Counter = 0.0f;

	// �^�X�N��o�^����
	GOData->TaskInfo.Base = &g_Task_GameOverTaskBaseInfo;
	GOData->TaskInfo.Data = GOData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &GOData->TaskInfo );

	// �^�X�N���\���̂̃A�h���X��Ԃ�
	return &GOData->TaskInfo;
}


