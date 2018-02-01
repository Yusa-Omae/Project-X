#include "Task_StageClear.h"
#include "DxLib.h"
#include "System.h"
#include "Sound.h"
#include "Stage.h"
#include "StageData.h"
#include <math.h>

// �X�e�[�W�N���A�\���̏o���O�ɑ҂���
#define SCREEN_IN_WAIT_TIME			(1.0f)

// �X�e�[�W�ԍ��o�����̈ړ��J�nY���W
#define MOVE_START_Y				(500)

// �X�e�[�W�N���A�\���̏o�����̏���Y���x
#define SCREEN_IN_Y_SPEED			(-3000.0f)

// �X�e�[�W�N���A�\���̏o������Y�����x
#define SCREEN_IN_Y_ACCEL			(6000.0f)

// �X�e�[�W�N���A�\�����t�F�[�h�A�E�g����O�ɑ҂���
#define FADE_OUT_WAIT				(2.0f)

// �X�e�[�W�N���A�\���̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME				(1.0f)

// �X�e�[�W�N���A�\�������̏��
typedef enum _ETask_StageClearState
{
	ETask_StageClearState_ScreenInWait,		// �o���҂���
	ETask_StageClearState_ScreenIn,			// �o����
	ETask_StageClearState_FadeOutWait,		// �t�F�[�h�A�E�g�҂���
	ETask_StageClearState_FadeOut,			// �t�F�[�h�A�E�g��
	ETask_StageClearState_End,				// �I�����
} ETask_StageClearState;

// �X�e�[�W�N���A�\�������p�̃f�[�^
typedef struct _STask_StageClearData
{
	// �^�X�N�̏��
	STaskInfo             TaskInfo;

	// �����̏��
	ETask_StageClearState State;

	// �ėp�J�E���^
	float                 Counter;

	// �X�e�[�W�N���A�̕������`�悷��Y���W
	float                 StringY;

	// �X�e�[�W�N���A�̕������`�悷��Y���W�̑��x
	float                 StringYSpeed;

	// �X�e�[�W�N���A�\���̕s�����x
	int                   StringOpacity;
} STask_StageClearData;

// �X�e�[�W�N���A�\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_StageClear_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �X�e�[�W�N���A�\���̕`�揈�����s��
static void Task_StageClear_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �X�e�[�W�N���A�\�������^�X�N�̊�{���
static STaskBaseInfo g_Task_StageClearTaskBaseInfo =
{
	8,
	8,
	Task_StageClear_Step,
	Task_StageClear_Render,
	NULL
};

// �X�e�[�W�N���A�\���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_StageClear_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;
	float                 PrevStringYSpeed;

	// �X�e�[�W�N���A�\�������̏�Ԃɂ���ď����𕪊�
	switch( SCData->State )
	{
	case ETask_StageClearState_ScreenInWait:		// �o���҂���
		SCData->Counter += StepTime;
		if( SCData->Counter > SCREEN_IN_WAIT_TIME )
		{
			// �o���҂����Ԃ��o�߂�����o�����J�n����
			SCData->State   = ETask_StageClearState_ScreenIn;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_ScreenIn:			// �o����
		// Y���W�̑��x�ɏ]���ĕ\��Y���W���ړ�����
		PrevStringYSpeed = SCData->StringYSpeed;
		SCData->StringYSpeed += StepTime * SCREEN_IN_Y_ACCEL;
		SCData->StringY      += 
			( PrevStringYSpeed + SCData->StringYSpeed ) * StepTime / 2.0f;

		// ����������~�ʒu�ɓ��B���Ă�����t�F�[�h�A�E�g�҂����J�n����
		if( SCData->StringY > 0.0f && SCData->StringYSpeed > 0.0f )
		{
			SCData->StringY = 0.0f;

			SCData->State   = ETask_StageClearState_FadeOutWait;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_FadeOutWait:		// �t�F�[�h�A�E�g�҂���
		SCData->Counter += StepTime;
		if( SCData->Counter > FADE_OUT_WAIT )
		{
			// �t�F�[�h�A�E�g�҂����Ԃ��o�߂�����t�F�[�h�A�E�g���J�n����
			SCData->State   = ETask_StageClearState_FadeOut;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_FadeOut:			// �t�F�[�h�A�E�g��
		SCData->Counter += StepTime;
		if( SCData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g������������I����Ԃɂ���
			SCData->StringOpacity = 0;

			SCData->State         = ETask_StageClearState_End;
			SCData->Counter  = 0.0f;
		}
		else
		{
			// �t�F�[�h�A�E�g���̃X�e�[�W�N���A�\���̕s�����x���Z�o
			SCData->StringOpacity =
				255 - ( int )( SCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_StageClearState_End:				// �I�����
		break;
	}

	// ����I��
	return true;
}

// �X�e�[�W�N���A�\���̕`�揈�����s��
static void Task_StageClear_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;
	int                   DrawWidth1;
	int                   DrawWidth2;
	int                   DrawWidth3;
	int                   DrawX1;
	int                   DrawX2;
	int                   DrawX3;
	int                   DrawY;
	char                  NumberStr[ 2 ];
	int                   FontHandle;

	// �X�e�[�W�N���A�\���̕s�����x���O�������牽�������I��
	if( SCData->StringOpacity == 0 )
	{
		return;
	}

	// �ėp�t�H���g�n���h���̎擾
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// �X�e�[�W�ԍ��̕�������쐬
	NumberStr[ 0 ] = '1' + StageData_GetLoadStageNo();
	NumberStr[ 1 ] = '\0';

	// �X�e�[�W�N���A�\���S�̂̉����ƁA�wSTAGE �x������`�悵���ꍇ�̉����ƁA
	// �w CLEAR!�x������`�悵���ꍇ�̉������擾
	DrawWidth1 = GetDrawStringWidthToHandle( "WAVE ",  strlen( "WAVE " ), FontHandle );
	DrawWidth2 = GetDrawStringWidthToHandle( NumberStr, strlen( NumberStr ),FontHandle );
	DrawWidth3 = GetDrawStringWidthToHandle( " CLEAR!", strlen( " CLEAR!" ),FontHandle );

	// �wSTAGE�x�ƃX�e�[�W�ԍ��ƁwCLEAR!�x�̕`��X���W���Z�o
	DrawX1 = GAME_SCREEN_WIDTH  / 2 - ( DrawWidth1 + DrawWidth2 + DrawWidth3 ) / 2;
	DrawX2 = DrawX1 + DrawWidth1;
	DrawX3 = DrawX2 + DrawWidth2;

	// �X�e�[�W�N���A�\���̕`��Y���W�̎Z�o
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2 + ( int )SCData->StringY;

	// �A���t�@�u�����h���[�h�ŃX�e�[�W�N���A�\���̕s�����x��ݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, SCData->StringOpacity );

	// �wSTAGE�x�ƃX�e�[�W�ԍ��ƁwCLEAR!�x�̕`��
	DrawStringToHandle( DrawX1, DrawY, "WAVE",   GetColor( 255,255,255 ), FontHandle );
	DrawStringToHandle( DrawX2, DrawY, NumberStr, GetColor( 255,0  ,0   ), FontHandle );
	DrawStringToHandle( DrawX3, DrawY, " CLEAR!", GetColor( 255,255,255 ), FontHandle );

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �X�e�[�W�N���A�\�����I�����������`�F�b�N����
//     �߂�l : �X�e�[�W�N���A�\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
bool Task_StageClear_CheckEnd(
	// �X�e�[�W�N���A�\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;

	// �����̏�Ԃ��I����Ԃ̏ꍇ�́A�������I�����Ă���̂� true ��Ԃ�
	return SCData->State == ETask_StageClearState_End;
}

// �X�e�[�W�N���A�\�����J�n����
//     �߂�l : �X�e�[�W�N���A�\���^�X�N���\���̂ւ̃A�h���X
STaskInfo * Task_StageClear_Start( void )
{
	STask_StageClearData *SCData;

	// �X�e�[�W�N���A�\���̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	SCData = ( STask_StageClearData * )calloc( 1, sizeof( STask_StageClearData ) );
	if( SCData == NULL )
	{
		return NULL;
	}

	// �X�e�[�W�N���A�\���̏���Y���W�Ə���Y���x��ݒ�
	SCData->StringY       = MOVE_START_Y;
	SCData->StringYSpeed  = SCREEN_IN_Y_SPEED;

	// �X�e�[�W�N���A�\���̕s�����x��ݒ�
	SCData->StringOpacity = 255;

	// �X�e�[�W�N���A�\���o���҂���Ԃɂ���
	SCData->State         = ETask_StageClearState_ScreenInWait;
	SCData->Counter       = 0.0f;

	// �^�X�N��o�^����
	SCData->TaskInfo.Base = &g_Task_StageClearTaskBaseInfo;
	SCData->TaskInfo.Data = SCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &SCData->TaskInfo );

	// �^�X�N���\���̂̃A�h���X��Ԃ�
	return &SCData->TaskInfo;
}


