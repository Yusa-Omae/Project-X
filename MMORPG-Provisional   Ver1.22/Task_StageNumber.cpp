#include "Task_StageNumber.h"
#include "DxLib.h"
#include "System.h"
#include "Stage.h"
#include "StageData.h"
#include <math.h>

// �X�e�[�W�ԍ��\�����t�F�[�h�C������O�ɑ҂���
#define FADE_IN_WAIT_TIME			(1.0f)

// �X�e�[�W�ԍ��\���̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME				(1.0f)

// �X�e�[�W�ԍ��t�F�[�h�C�����̈ړ��J�nY���W
#define MOVE_START_Y				(128)

// �X�e�[�W�ԍ��\�����t�F�[�h�A�E�g����O�ɑ҂���
#define FADE_OUT_WAIT				(1.0f)

// �t�F�[�h�A�E�g���ɃX�e�[�W���\������ʉ��Ɉړ����鑬�x
#define OUT_MOVE_SPEED				(256.0f)

// �X�e�[�W�ԍ��\���̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME				(1.0f)

// �X�e�[�W�ԍ��\�������̏��
typedef enum _ETask_StageNumberState
{
	ETask_StageNumberState_FadeInWait,		// �t�F�[�h�C���҂���
	ETask_StageNumberState_FadeIn,			// �t�F�[�h�C����
	ETask_StageNumberState_FadeOutWait,		// �t�F�[�h�A�E�g�҂���
	ETask_StageNumberState_FadeOut,			// �t�F�[�h�A�E�g��
	ETask_StageNumberState_End,				// �I�����
} ETask_StageNumberState;

// �X�e�[�W�ԍ��\�������p�̃f�[�^
typedef struct _STask_StageNumberData
{
	// �^�X�N�̏��
	STaskInfo              TaskInfo;

	// �����̏��
	ETask_StageNumberState State;

	// �ėp�J�E���^
	float                  Counter;

	// �X�e�[�W�ԍ���\������Y���W
	int                    DrawY;

	// �X�e�[�W�ԍ��\���̕s�����x
	int                    Opacity;
} STask_StageNumberData;

// �X�e�[�W�ԍ��\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_StageNumber_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �X�e�[�W�ԍ��\���̕`�揈�����s��
static void Task_StageNumber_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �X�e�[�W�ԍ��\�������^�X�N�̊�{���
static STaskBaseInfo g_Task_StageNumberTaskBaseInfo =
{
	8,
	8,
	Task_StageNumber_Step,
	Task_StageNumber_Render,
	NULL
};

// �X�e�[�W�ԍ��\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_StageNumber_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;
	float                  FirstSpeed;
	float                  SubSpeed;
	float                  Distance;

	// �X�e�[�W�ԍ��\�������̏�Ԃɂ���ď����𕪊�
	switch( SSData->State )
	{
	case ETask_StageNumberState_FadeInWait:		// �t�F�[�h�C���҂���
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����t�F�[�h�C�����J�n����
			SSData->State   = ETask_StageNumberState_FadeIn;
			SSData->Counter = 0.0f;
		}
		break;

	case ETask_StageNumberState_FadeIn:			// �t�F�[�h�C����
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_IN_TIME )
		{
			// �t�F�[�h�C��������������t�F�[�h�A�E�g�҂����J�n����
			SSData->DrawY   = 0;
			SSData->Opacity = 255;

			SSData->State   = ETask_StageNumberState_FadeOutWait;
			SSData->Counter = 0.0f;
		}
		else
		{
			// �t�F�[�h�C�����̃X�e�[�W�ԍ��\����Y���W�ƕs�����x���Z�o
			FirstSpeed = MOVE_START_Y * 2.0f / FADE_IN_TIME;
			SubSpeed   = FirstSpeed / FADE_IN_TIME;
			Distance   = ( FirstSpeed + FirstSpeed - SubSpeed * SSData->Counter ) 
				* SSData->Counter / 2.0f;

			SSData->DrawY   = ( int )( MOVE_START_Y - Distance );
			SSData->Opacity = ( int )( SSData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_StageNumberState_FadeOutWait:		// �t�F�[�h�A�E�g�҂���
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_OUT_WAIT )
		{
			// �t�F�[�h�A�E�g�҂����Ԃ��o�߂�����t�F�[�h�A�E�g���J�n����
			SSData->State   = ETask_StageNumberState_FadeOut;
			SSData->Counter = 0.0f;
		}
		break;

	case ETask_StageNumberState_FadeOut:			// �t�F�[�h�A�E�g��
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g����������I����Ԃɂ���
			SSData->Opacity = 0;

			SSData->State    = ETask_StageNumberState_End;
			SSData->Counter  = 0.0f;
		}
		else
		{
			// �t�F�[�h�A�E�g���̃X�e�[�W�ԍ��\����Y���W�ƕs�����x���Z�o
			SSData->DrawY   =
				-( int )( OUT_MOVE_SPEED * SSData->Counter * SSData->Counter / 2.0f );

			SSData->Opacity = 255 - ( int )( SSData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_StageNumberState_End:				// �I�����
		break;
	}

	// ����I��
	return true;
}

// �X�e�[�W�ԍ��\���̕`�揈�����s��
static void Task_StageNumber_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;
	int                    DrawWidth1;
	int                    DrawWidth2;
	int                    DrawX1;
	int                    DrawX2;
	int                    DrawY;
	char                   NumberStr[ 3 ];
	int                    FontHandle;

	// �X�e�[�W�ԍ��\���̕s�����x���O�������牽�������I��
	if( SSData->Opacity == 0 )
	{
		return;
	}

	// �ėp�t�H���g�n���h���̎擾
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// �X�e�[�W�ԍ��̕�������쐬
	if (StageData_GetLoadStageNo() >= 9) {
		
		int statgeNumber = StageData_GetLoadStageNo() + 1;

		NumberStr[0] = '0' + statgeNumber / 10;
		NumberStr[1] = '0' + statgeNumber % 10;
		NumberStr[2] = '\0';

		// �X�e�[�W�ԍ��\���S�̂̉����ƁA�wSTAGE �x������`�悵���ꍇ�̉������擾
		DrawWidth1 = GetDrawStringWidthToHandle("WAVE 00", strlen("WAVE 00"), FontHandle);
		DrawWidth2 = GetDrawStringWidthToHandle("WAVE ", strlen("WAVE "), FontHandle);

	}
	else {
		NumberStr[0] = '1' + StageData_GetLoadStageNo();
		NumberStr[1] = '\0';
		NumberStr[2] = '\0';

		// �X�e�[�W�ԍ��\���S�̂̉����ƁA�wSTAGE �x������`�悵���ꍇ�̉������擾
		DrawWidth1 = GetDrawStringWidthToHandle("WAVE 0", strlen("WAVE 0"), FontHandle);
		DrawWidth2 = GetDrawStringWidthToHandle("WAVE ", strlen("WAVE "), FontHandle);

	}

	// �X�e�[�W�ԍ���`�悷��Y���W�̎Z�o
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2 - SSData->DrawY;

	
	// �wSTAGE�x��`�悷��X���W�ƃX�e�[�W�ԍ���`�悷��X���W�̎Z�o
	DrawX1     = GAME_SCREEN_WIDTH / 2 - DrawWidth1 / 2;
	DrawX2     = DrawX1 + DrawWidth2;

	// �A���t�@�u�����h���[�h�ŃX�e�[�W�ԍ��\���̕s�����x��ݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, SSData->Opacity );

	// �wSTAGE�x�ƃX�e�[�W�ԍ��̕`��
	DrawStringToHandle( DrawX1, DrawY, "WAVE",   GetColor( 255,255,255 ), FontHandle );
	DrawStringToHandle( DrawX2, DrawY, NumberStr, GetColor( 255,  0,  0 ), FontHandle );

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �X�e�[�W�ԍ��\�����I�����������`�F�b�N����
//     �߂�l : �X�e�[�W�ԍ��\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
bool        Task_StageNumber_CheckEnd(
	// �X�e�[�W�ԍ��\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;

	// �����̏�Ԃ��I����Ԃ̏ꍇ�́A�������I�����Ă���̂� true ��Ԃ�
	return SSData->State == ETask_StageNumberState_End;
}

// �X�e�[�W�ԍ��\�����J�n����
//     �߂�l : �X�e�[�W�ԍ��\���^�X�N���\���̂ւ̃A�h���X
STaskInfo * Task_StageNumber_Start( void )
{
	STask_StageNumberData *SSData;

	// �X�e�[�W�ԍ��\���̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	SSData = ( STask_StageNumberData * )calloc( 1, sizeof( STask_StageNumberData ) );
	if( SSData == NULL )
	{
		return NULL;
	}

	// �t�F�[�h�C���҂��̏�Ԃɂ���
	SSData->State   = ETask_StageNumberState_FadeInWait;
	SSData->Counter = 0.0f;

	// �^�X�N��o�^����
	SSData->TaskInfo.Base = &g_Task_StageNumberTaskBaseInfo;
	SSData->TaskInfo.Data = SSData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &SSData->TaskInfo );

	// �^�X�N���\���̂̃A�h���X��Ԃ�
	return &SSData->TaskInfo;
}


