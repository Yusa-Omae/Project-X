#include "Task_DebugStartMenu.h"
#include "Task.h"
#include "System.h"
#include "DxLib.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"

// �J�[�\���̕`��X���W
#define CURSOR_X		(24)

// ���j���[���ڂ̕`��J�n���W
#define MENU_X			(64)
#define MENU_Y			(54)

// ���j���[���ڈ������ɋ󂯂�X�y�[�X
#define MENU_SPACE		(36)

// �f�o�b�O�N�����j���[�̍���
typedef enum _ETask_DebugStartMenu
{
	ETask_DebugStartMenu_StageSelect,	// �X�e�[�W�I��
	ETask_DebugStartMenu_Title,			// �^�C�g�����
	ETask_DebugStartMenu_KeyConfig,		// �L�[�R���t�B�O���
	ETask_DebugStartMenu_AllStageClear,	// �S�X�e�[�W�N���A���
	ETask_DebugStartMenu_Num,			// ���ڂ̐�
} ETask_DebugStartMenu;

// �f�o�b�O�N����ʏ����p�̃f�[�^
typedef struct _STask_DebugStartMenuInfo
{
	// �^�X�N�̏��
	STaskInfo TaskInfo;

	// �I�����Ă��鍀��
	int       SelectPosition;

	// �I�����Ă���X�e�[�W�ԍ�
	int       SelectStageNo;
} STask_DebugStartMenuInfo;

// �f�o�b�O�N����ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_DebugStartMenu_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �f�o�b�O�N����ʂ̕`�揈�����s��
static void Task_DebugStartMenu_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �f�o�b�O�N����ʏ����^�X�N�̊�{���
static STaskBaseInfo g_Task_DebugStartMenuTaskBaseInfo =
{
	8,
	8,
	Task_DebugStartMenu_Step,
	Task_DebugStartMenu_Render,
	NULL
};

// �f�o�b�O�N����ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_DebugStartMenu_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_DebugStartMenuInfo *DSMData = ( STask_DebugStartMenuInfo * )TInfo->Data;
	int                       EdgeInput;
	int                       TotalStageNum;

	// �����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾
	EdgeInput = GetEdgeInput();

	// �S�X�e�[�W�����擾
	TotalStageNum = StageData_GetTotalStageNum();

	// �X�e�[�W�I�����ڂ��I������Ă���ꍇ�͏����𕪊�
	if( DSMData->SelectPosition == ETask_DebugStartMenu_StageSelect )
	{
		// ���������͂�����Ă����ꍇ�͑I���X�e�[�W���P�}�C�i�X����
		if( ( EdgeInput & ( 1 << EInputType_Left ) ) != 0 )
		{
			System_PlayCommonSE( ECommonSE_Cursor );

			DSMData->SelectStageNo--;
			if( DSMData->SelectStageNo < 0 )
			{
				DSMData->SelectStageNo = TotalStageNum - 1;
			}
		}

		// �E�������͂�����Ă����ꍇ�͑I���X�e�[�W���P�v���X����
		if( ( EdgeInput & ( 1 << EInputType_Right ) ) != 0 )
		{
			System_PlayCommonSE( ECommonSE_Cursor );

			DSMData->SelectStageNo++;
			if( DSMData->SelectStageNo == TotalStageNum )
			{
				DSMData->SelectStageNo = 0;
			}
		}
	}

	// ��������͂�����Ă����ꍇ�͑I�����ڂ����Ɉړ�����
	if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 )
	{
		System_PlayCommonSE( ECommonSE_Cursor );

		DSMData->SelectPosition--;
		if( DSMData->SelectPosition < 0 )
		{
			DSMData->SelectPosition = ETask_DebugStartMenu_Num - 1;
		}
	}

	// ��������͂�����Ă����ꍇ�͑I�����ڂ�����Ɉړ�����
	if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 )
	{
		System_PlayCommonSE( ECommonSE_Cursor );

		DSMData->SelectPosition++;
		if( DSMData->SelectPosition == ETask_DebugStartMenu_Num )
		{
			DSMData->SelectPosition = 0;
		}
	}

	// ����{�^���������ꂽ��I�����ڂɉ�������ʂɈړ�����
	if( ( EdgeInput & ( 1 << EInputType_Attack ) ) != 0 )
	{
		// ���艹��炷
		System_PlayCommonSE( ECommonSE_Enter );

		// �t�F�[�h�A�E�g���J�n����
		System_FadeOut();

		// �I�����Ă��鍀�ڂɉ����ď����𕪊�
		switch( DSMData->SelectPosition )
		{
		case ETask_DebugStartMenu_StageSelect:
			// �X�e�[�W�I�����I������Ă�����A�I�����ꂽ�X�e�[�W����Q�[���{�҂��J�n����
			System_SetStartStage( DSMData->SelectStageNo );
			if( !System_StartGameMain() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_Title:
			// �^�C�g����ʂ��J�n����
			if( !System_StartTitle() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_KeyConfig:
			// �L�[�R���t�B�O��ʂ��J�n����
			if( !System_StartKeyConfig() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_AllStageClear:
			// �S�X�e�[�W�N���A��ʂ��J�n����
			if( !System_StartAllStageClear() )
			{
				return false;
			}
			break;
		}

		// �^�X�N���폜����
		TaskSystem_DelTask( System_GetTaskSystemInfo(), &DSMData->TaskInfo );
	}

	// ����I��
	return true;
}

// �f�o�b�O�N����ʂ̕`�揈�����s��
static void Task_DebugStartMenu_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_DebugStartMenuInfo *DSMData = ( STask_DebugStartMenuInfo * )TInfo->Data;
	int                       Color;
	int                       DrawY;
	int                       FontHandle;

	// �t�H���g�n���h���̎擾
	FontHandle = System_GetFontHandle( EFontHandle_Small );

	// �����̕`��Ɏg�p����F���擾
	Color = GetColor( 255,255,255 );

	// �f�o�b�O�N�����j���[�̕������`��
	DrawStringToHandle( 0, 0, "Debug Start Menu", Color, FontHandle );

	// �e���j���[���ڂ̕`��
	DrawY = MENU_Y;

	DrawFormatStringToHandle( MENU_X, DrawY, Color, FontHandle,
										"Stage Select : %d",DSMData->SelectStageNo );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "Title Start",           Color, FontHandle );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "KeyConfig Start",       Color, FontHandle );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "All Stage Clear Start", Color, FontHandle );
	DrawY += MENU_SPACE;

	// �I�����Ă��鍀�ڂɃJ�[�\����`��
	DrawStringToHandle( CURSOR_X, MENU_Y + MENU_SPACE * DSMData->SelectPosition,
										"��", GetColor( 255,255,255 ), FontHandle );
}

// �f�o�b�O�N�����j���[�̏������J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Task_DebugStartMenu_Start( void )
{
	STask_DebugStartMenuInfo *DSMData;

	// �f�o�b�O�N����ʂ̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	DSMData = ( STask_DebugStartMenuInfo * )calloc( 1, sizeof( STask_DebugStartMenuInfo ) );
	if( DSMData == NULL )
	{
		return false;
	}

	// ���t�F�[�h�̉������J�n
	System_FadeIn();

	// �^�X�N��o�^����
	DSMData->TaskInfo.Base = &g_Task_DebugStartMenuTaskBaseInfo;
	DSMData->TaskInfo.Data = DSMData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &DSMData->TaskInfo );

	// ����I��
	return true;
}

