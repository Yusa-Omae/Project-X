#include "Task_GameMain.h"
#include "DxLib.h"
#include "System.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"
#include "Camera.h"
#include "Chara.h"
#include "GameMainRender.h"
#include "Chara_Player.h"
#include "Task_StageNumber.h"
#include "Task_StageClear.h"
#include "Task_GameOver.h"
#include "Task_PauseMenu.h"
#include "Code/Scene/Adventure/Adventure.h"
#include "Code/Scene/Shop/Shop.h"
#include <math.h>

// �X�e�[�W�J�n������O�ɑ҂���
#define STAGE_START_WAIT		(0.1f)

// �X�e�[�W�N���A�������J�n����O�ɑ҂���
#define STAGE_CLEAR_WAIT		(1.5f)

// �S�X�e�[�W�N���A�������J�n����O�ɑ҂���
#define STAGE_ALLCLEAR_WAIT		(1.0f)

// �Q�[���I�[�o�[�������J�n����O�ɑ҂���
#define GAMEOVER_WAIT			(0.1f)

// �v���C���[�̗̑̓Q�[�W��\��������W
#define PC_HEALTH_X				(140)
#define PC_HEALTH_Y				(32)

// �v���C���[�̗̑̓Q�[�W�̕��ƍ���
#define PC_HEALTH_SIZE_X		(400)
#define PC_HEALTH_SIZE_Y		(16)

// �v���C���[�̗̑̓Q�[�W�́wPLAYER�x�ƕ\��������W
#define PC_NAME_X				(32)
#define PC_NAME_Y				(28)
#define PC_NAME_STRING			"PLAYER"

// �|���G�̐���\��������W
#define ENEMY_NUM_X				(32)
#define ENEMY_NUM_Y				(GAME_SCREEN_HEIGHT - 56)

// �Q�[�����C�������̏��
typedef enum _ETask_GameMainState
{
	
	ETask_GameMainState_Adventure_EndWait,	//��b�p�[�g�I���҂�
	ETask_GameMainState_StageNumber,		// �X�e�[�W�ԍ��\����

	ETask_GameMainState_StageStart_Wait,	// �X�e�[�W�J�n�҂���
	ETask_GameMainState_Stage_FadeIn,		// �X�e�[�W�t�F�[�h�C����

	ETask_GameMainState_InGame,				// �Q�[����
	ETask_GameMainState_PauseMenu,			// �|�[�Y���j���[��
	ETask_GameMainState_StageClear_Wait,	// �X�e�[�W�N���A�����J�n�҂���
	ETask_GameMainState_StageClear_String,	// �X�e�[�W�N���A�����\����
	ETask_GameMainState_StageClear_FadeOut,	// �X�e�[�W�N���A��t�F�[�h�A�E�g��

	ETask_GameMainState_Shop_Start,			//�V���b�v���J��
	ETask_GameMainState_Shop_End,			//�V���b�v�I���҂�
	ETask_GameMainState_Shop_FadeOut,		//�V���b�v�I����t�F�[�h�A�E�g��

	ETask_GameMainState_StageAllClear_Wait,	// �S�X�e�[�W�N���A�����J�n�҂���
	ETask_GameMainState_GameOver_Wait,		// �Q�[���I�[�o�[�����J�n�҂���
	ETask_GameMainState_GameOver,			// �Q�[���I�[�o�[�\����
	ETask_GameMainState_GameOver_FadeOut,	// �Q�[���I�[�o�[��t�F�[�h�A�E�g��
	ETask_GameMainState_End,				// �I�����
} ETask_GameMainState;

// �Q�[�����C�������p�̃f�[�^
typedef struct _STask_GameMainData
{
	// �^�X�N�̏��
	STaskInfo           TaskInfo;

	// �����̏��
	ETask_GameMainState State;

	// �ėp�J�E���^
	float               Counter;

	// �Q�[�����C���̏�Ԑ��ڏ������s�����ǂ����̃t���O
	bool                EnableGameMainStepProcess;

	// �Q�[�����C���̕`�揈�����s�����ǂ����̃t���O
	bool                EnableGameMainRenderProcess;

	// �|�����G�̐�
	int                 KillEnemyNum;

	// �|���ׂ��G��|�������ǂ����̃t���O( true:�|����  false:�|���Ă��Ȃ� )
	bool                KillTargetCharaKill;

	// �v���C���[�̗͕̑\���Ȃǂ��s�����ǂ����̃t���O( true:�\������  false:�\�����Ȃ� )
	bool                DrawHUD;

	// �X�e�[�W�ԍ��\���^�X�N���\���̂ւ̃|�C���^
	STaskInfo *         StageNumberTaskInfo;

	// �X�e�[�W�N���A�\���^�X�N���\���̂ւ̃|�C���^
	STaskInfo *         StageClearTaskInfo;

	// �Q�[���I�[�o�[�\���^�X�N���\���̂ւ̃|�C���^
	STaskInfo *         GameOverTaskInfo;

	// �|�[�Y���j���[�^�X�N���\���̂ւ̃|�C���^
	STaskInfo *         PauseMenuTaskInfo;

	// ��b�p�[�g�^�X�N���\���̂ւ̃|�C���^
	STaskInfo *         AdventureTaskInfo;

	//�V���b�v�^�X�N���\���̂ւ̃|�C���^
	STaskInfo *			ShopTaskInfo;

} STask_GameMainData;

// �Q�[�����C���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_GameMain_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �Q�[�����C���̕`�揈�����s��
static void Task_GameMain_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �Q�[�����C�������^�X�N�̊�{���
static STaskBaseInfo g_Task_GameMainTaskBaseInfo =
{
	7,
	7,
	Task_GameMain_Step,
	Task_GameMain_Render,
	NULL
};

// �Q�[�����C���̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_GameMain_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;

	// �e��Q�[�����C���p�̏�Ԑ��ڏ������s��
	if( GMData->EnableGameMainStepProcess )
	{
		if( !Stage_Step(         StepTime ) )
		{
			return false;
		}

		if( !Chara_StepAll(      StepTime ) )
		{
			return false;
		}

		Effect_StepAll(     StepTime );
		Camera_Step(        StepTime );
	}

	// �Q�[�����C�������̏�Ԃɂ���ď����𕪊�
	switch( GMData->State )
	{

	case ETask_GameMainState_Adventure_EndWait:
		if (!System_CheckFade())
		{
			if (Task_Adventure_IsDelete() == true) {
				TaskSystem_DelTask(

					//��b�^�X�N�폜
					System_GetTaskSystemInfo(), GMData->AdventureTaskInfo);


				// �ŏ��̃X�e�[�W�̃Z�b�g�A�b�v���s��
				if (!Stage_Setup(System_GetStartStage()))
				{
					return false;
				}

				System_FadeOut();

				GMData->State = ETask_GameMainState_StageStart_Wait;
			}
		}
		break;
	case ETask_GameMainState_StageStart_Wait:	// �X�e�[�W�J�n�҂���
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_START_WAIT )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����t�F�[�h�C�����J�n����
			System_FadeIn();

			// �t�F�[�h�C��������������X�e�[�W�ԍ��\�����J�n����
			GMData->StageNumberTaskInfo = Task_StageNumber_Start();
			if (GMData->StageNumberTaskInfo == NULL)
			{
				return false;
			}

			//�v���C���[�̗̑͂Ȃǂ�\������
			GMData->DrawHUD = true;

			GMData->State   = ETask_GameMainState_Stage_FadeIn;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_Stage_FadeIn:	// �X�e�[�W�t�F�[�h�C����
		if( !System_CheckFade() )
		{
			
			GMData->State   = ETask_GameMainState_StageNumber;
			GMData->Counter = 0.0f;
		}
		break;
	case ETask_GameMainState_StageNumber:	// �X�e�[�W�ԍ��\����
		if( Task_StageNumber_CheckEnd( GMData->StageNumberTaskInfo ) )
		{
			// �X�e�[�W�ԍ��\������������������Q�[�����������J�n����

			// �X�e�[�W�ԍ��\���^�X�N�̍폜
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->StageNumberTaskInfo );
			GMData->StageNumberTaskInfo = NULL;

			GMData->State   = ETask_GameMainState_InGame;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_InGame:				// �Q�[����
		if( System_GetPlayerCharaInfo() == NULL )
		{
			// �v���C���[�����Ȃ��Ȃ�����|���ꂽ�Ƃ������ƂȂ̂�
			// �Q�[���I�[�o�[�������J�n����
			GMData->State   = ETask_GameMainState_GameOver_Wait;
			GMData->Counter = 0.0f;
		}
		else
		{
			// �X�e�[�W�̃N���A�����ɂ���ď����𕪊�
			switch( StageData_GetStageClearCondition() )
			{
			case EStageClearCondition_CharaKillNum:
				// �w�萔�̓G��|������N���A�̏ꍇ
				if( GMData->KillEnemyNum >= StageData_GetKillCharaNum() )
				{
					// �w�萔�̓G��|������N���A�����J�n�҂����J�n����
					GMData->State   = ETask_GameMainState_StageClear_Wait;
					GMData->Counter = 0.0f;
				}
				break;

			case EStageClearCondition_TargetCharaKill:
				// �w��̓G��|������N���A�̏ꍇ
				if( GMData->KillTargetCharaKill )
				{
					// �w��̓G��|�����炻��ȊO�̓G���|�������Ƃɂ���
					if( !Chara_TagetTypeKill( ECharaType_Enemy ) ||
					    !Chara_TagetTypeKill( ECharaType_EnemyShot ) )
					{
						return false;
					}

					// �X�e�[�W�N���A�J�n�҂����J�n����
					GMData->State   = ETask_GameMainState_StageClear_Wait;
					GMData->Counter = 0.0f;
				}
				break;
			}

			// ��Ԃ��ύX����Ă��Ȃ��āA���|�[�Y�{�^���������ꂽ��
			// �|�[�Y���j���[��\������
			if( GMData->State == ETask_GameMainState_InGame &&
				( GetEdgeInput() & ( 1 << EInputType_Pause ) ) != 0 )
			{
				// �Q�[���̏�Ԑ��ڏ������~�߂�
				GMData->EnableGameMainStepProcess = false;

				// ����炷
				System_PlayCommonSE( ECommonSE_Enter );

				// �|�[�Y���j���[�������J�n����
				GMData->PauseMenuTaskInfo = Task_PauseMenu_Start();
				if( GMData->PauseMenuTaskInfo == NULL )
				{
					return false;
				}

				GMData->State   = ETask_GameMainState_PauseMenu;
				GMData->Counter = 0.0f;
			}
		}
		break;

	case ETask_GameMainState_PauseMenu:			// �|�[�Y���j���[��
		if( Task_PauseMenu_CheckEnd( GMData->PauseMenuTaskInfo ) )
		{
			// �|�[�Y���j���[������������I�����ꂽ���ڂɂ���ď����𕪊򂷂�
			switch( Task_PauseMenu_GetChoiceIndex( GMData->PauseMenuTaskInfo ) )
			{
			case ETask_PauseMenu_Continue:		// �Q�[���ĊJ
				// �Q�[���̏�Ԑ��ڏ������ĊJ����
				GMData->EnableGameMainStepProcess = true;

				// �Q�[�����ɖ߂�
				GMData->State   = ETask_GameMainState_InGame;
				GMData->Counter = 0.0f;
				break;

			case ETask_PauseMenu_BackToTitle:	// �^�C�g����ʂɖ߂�
				// �^�C�g����ʏ������J�n����
				if( !System_StartTitle() )
				{
					return false;
				}

				// �S�ẴL�������폜����
				Chara_AllDelete();

				// �I����Ԃɂ���
				GMData->State   = ETask_GameMainState_End;
				GMData->Counter = 0.0f;

				// �^�X�N���폜����
				TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
				break;

			case ETask_PauseMenu_GameEnd:		// �Q�[���I��
				// �Q�[���I�����I������Ă�����Q�[�����I������
				System_ExitGame();

				// �S�ẴL�������폜����
				Chara_AllDelete();

				// �I����Ԃɂ���
				GMData->State   = ETask_GameMainState_End;
				GMData->Counter = 0.0f;

				// �^�X�N���폜����
				TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
				break;
			}

			// �|�[�Y���j���[�����^�X�N���폜
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->PauseMenuTaskInfo );
			GMData->PauseMenuTaskInfo = NULL;
		}
		break;

	case ETask_GameMainState_StageClear_Wait:	// �X�e�[�W�N���A�����J�n�҂���
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_CLEAR_WAIT )
		{
			// �X�e�[�W�N���A�J�n�҂����Ԃ��o�߂�����X�e�[�W�N���A�\�����J�n����
			Sound_PlayBGM( EBGM_StageClear );
			GMData->StageClearTaskInfo = Task_StageClear_Start();
			if( GMData->StageClearTaskInfo == NULL )
			{
				return false;
			}

			GMData->State   = ETask_GameMainState_StageClear_String;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_StageClear_String:	// �X�e�[�W�N���A�����\����
		if( Task_StageClear_CheckEnd( GMData->StageClearTaskInfo ) )
		{
			// �X�e�[�W�N���A�\������������������t�F�[�h�A�E�g���J�n����

			// �X�e�[�W�N���A�\�������^�X�N���폜����
			TaskSystem_DelTask( 
				System_GetTaskSystemInfo(), GMData->StageClearTaskInfo );
			GMData->StageClearTaskInfo = NULL;

			System_FadeOut();

			GMData->State   = ETask_GameMainState_StageClear_FadeOut;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_StageClear_FadeOut:	// �X�e�[�W�N���A��t�F�[�h�A�E�g��
		if( !System_CheckFade() )
		{

			/*
			
				�����ŁA�V���b�v�����ނ��^�X�N�Ŏ��s������B

				�P�[�X�𑝂₵�A�V���b�v�ł̍w�����I���������𔻒f����P�[�X����ǉ�����

				
			
			
			*/

			// �X�e�[�W�N���A��t�F�[�h�A�E�g������������A��U�L������S�č폜����
			Chara_AllDelete();

			// �S�X�e�[�W�N���A�������ǂ����ŏ����𕪊�
			//if( StageData_GetTotalStageNum() == StageData_GetLoadStageNo() + 1 )
			if (STAGE_MAX_NUM == StageData_GetLoadStageNo() + 1)
			{
				// �S�X�e�[�W�N���A�����ꍇ�͑S�X�e�[�W�N���A�\���J�n�҂����J�n����
				GMData->EnableGameMainStepProcess = false;
				GMData->EnableGameMainRenderProcess = false;
				System_FadeIn();
				GMData->DrawHUD = false;

				GMData->State   = ETask_GameMainState_StageAllClear_Wait;
				GMData->Counter = 0.0f;
			}
			else
			{
			


				GMData->KillEnemyNum        = 0;
				GMData->KillTargetCharaKill = false;

				GMData->State   = ETask_GameMainState_Shop_Start;
				GMData->Counter = 0.0f;
			}

			

		}
		break;
	case ETask_GameMainState_Shop_Start:
		
		/*
			�����ŃV���b�v�^�X�N����
		
		*/

		GMData->ShopTaskInfo = Task_Shop_Start();
		if (GMData->ShopTaskInfo == NULL) {
			return false;
		}


		System_FadeIn();
		GMData->State = ETask_GameMainState_Shop_End;
		GMData->Counter = 0.0f;
		
		break;
	case ETask_GameMainState_Shop_End:
		if (Task_Shop_IsExit()) {
					
			
			System_FadeOut();
						

			GMData->State = ETask_GameMainState_Shop_FadeOut;
			GMData->Counter = 0.0f;

		}
		break;
	case ETask_GameMainState_Shop_FadeOut:

		if (!System_CheckFade()) {

			//�V���b�v�^�X�N�̍폜������
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->ShopTaskInfo);
			GMData->ShopTaskInfo = NULL;

			// ���̃X�e�[�W������ꍇ�́A���̃X�e�[�W�̃Z�b�g�A�b�v���s���A
			// ���̌�X�e�[�W�J�n�҂����J�n����
			if (!Stage_Setup(StageData_GetLoadStageNo() + 1))
			{
				return false;
			}

			GMData->State = ETask_GameMainState_StageStart_Wait;
			GMData->Counter = 0.0f;

		}

		break;
	case ETask_GameMainState_StageAllClear_Wait:	// �S�X�e�[�W�N���A�����J�n�҂���
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_ALLCLEAR_WAIT )
		{
			// �S�X�e�[�W�N���A�\���J�n�҂����Ԃ��o�߂�����S�X�e�[�W�N���A�\�����J�n����
			if( !System_StartAllStageClear() )
			{
				return false;
			}

			// �I����Ԃɂ���
			GMData->State   = ETask_GameMainState_End;
			GMData->Counter = 0.0f;

			// �^�X�N���폜����
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
		}
		break;

	case ETask_GameMainState_GameOver_Wait:		// �Q�[���I�[�o�[�����J�n�҂���
		GMData->Counter += StepTime;
		if( GMData->Counter > GAMEOVER_WAIT )
		{
			// �Q�[���I�[�o�[�����J�n�҂����Ԃ��o�߂�����Q�[���I�[�o�[�\�����J�n����
			GMData->GameOverTaskInfo = Task_GameOver_Start();
			if( GMData->GameOverTaskInfo == NULL )
			{
				return false;
			}

			GMData->State   = ETask_GameMainState_GameOver;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_GameOver:		// �Q�[���I�[�o�[�\����
		if( Task_GameOver_CheckEnd( GMData->GameOverTaskInfo ) )
		{
			// �Q�[���I�[�o�[�\��������������t�F�[�h�A�E�g���J�n����

			// �Q�[���I�[�o�[�\���^�X�N���폜
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->GameOverTaskInfo );
			GMData->GameOverTaskInfo = NULL;

			System_FadeOut();

			GMData->State   = ETask_GameMainState_GameOver_FadeOut;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_GameOver_FadeOut:	// �Q�[���I�[�o�[��t�F�[�h�A�E�g��
		if( !System_CheckFade() )
		{
			// �t�F�[�h�A�E�g������������^�C�g����ʏ������J�n����
			if( !System_StartTitle() )
			{
				return false;
			}

			// �S�ẴL�������폜����
			Chara_AllDelete();

			// �I����Ԃɂ���
			GMData->State   = ETask_GameMainState_End;
			GMData->Counter = 0.0f;

			// �^�X�N���폜����
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
		}
		break;

	case ETask_GameMainState_End:				// �I�����
		break;
	}

	// ����I��
	return true;
}

// �Q�[�����C���̕`�揈�����s��
static void Task_GameMain_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;
	SCharaInfo         *PCInfo;
	int                 SmallFontHandle;
	int                 MidiumFontHandle;

	// �Q�[�����C���p�̕`�揈�����s��
	if( GMData->EnableGameMainRenderProcess )
	{
		GameMainRender_Render();
	}

	// �r�L�[��������Ă�����Q�[����ʂ̃X�N���[���V���b�g��ۑ�����
//	if( CheckHitKey( KEY_INPUT_S ) != 0 )
//	{
//		SaveDrawScreen( 0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,
//													"ScreenShot.bmp" );
//	}

	// �̗͕\���Ȃǂ��s�����ǂ����̃t���O���|��Ă���ꍇ�͉��������I��
	if( !GMData->DrawHUD )
	{
		return;
	}

	// �`��u�����h���[�h��W���ݒ�ɂ���
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// �ėp�t�H���g�n���h�����擾����
	SmallFontHandle  = System_GetFontHandle( EFontHandle_Small );
	MidiumFontHandle = System_GetFontHandle( EFontHandle_Midium );

	// �N���A�������w�G���w�萔�|���x�������ꍇ�́A�|���ׂ��G�̎c�萔��`�悷��
	if( StageData_GetStageClearCondition() == EStageClearCondition_CharaKillNum )
	{
		DrawFormatStringToHandle(
			ENEMY_NUM_X, ENEMY_NUM_Y, GetColor( 255,64,64 ), MidiumFontHandle,
			"ENEMY �~ %d", StageData_GetKillCharaNum() - GMData->KillEnemyNum
		);
	}

	// �wPLAYER�x��`�悷��
	DrawStringToHandle( PC_NAME_X, PC_NAME_Y, PC_NAME_STRING, GetColor( 255,255,255 ), SmallFontHandle );

	// �v���C���[�̗͕̑\���̘g������`�悷��
	DrawBox(
		PC_HEALTH_X - 4,                    PC_HEALTH_Y - 4,
		PC_HEALTH_X + PC_HEALTH_SIZE_X + 4, PC_HEALTH_Y + PC_HEALTH_SIZE_Y + 4,
		GetColor( 200,200,200 ),
		TRUE
	);

	// �v���C���[�̏����擾����
	PCInfo = System_GetPlayerCharaInfo();

	// �v���C���[������ꍇ�̓v���C���[�̗̑͂�`�悷��
	// �v���C���[�����Ȃ��ꍇ�̓v���C���[�̗̑͂͂O�Ƃ��ĕ`�悷��
	CharaHealthGaugeDraw(
		PCInfo != NULL ? &PCInfo->HealthGauge : NULL,
		PC_HEALTH_X,      PC_HEALTH_Y,
		PC_HEALTH_SIZE_X, PC_HEALTH_SIZE_Y
	);

	// �f�o�b�O����\�����邩�ǂ����̃t���O�������Ă�����f�o�b�O�\�����s��
	if( System_GetDispDebugInfo() )
	{
		SCharaInfo *CInfo;

		CInfo = System_GetPlayerCharaInfo();

		Stage_CollisionRender();

		if( CInfo != NULL )
		{
			DrawFormatString( 0, GAME_SCREEN_HEIGHT - 32, GetColor( 255,255,255 ),
				"Position:%.2f,%.2f,%.2f TargetAngle:%.2f NowAngle:%.2f",
				CInfo->Position.x, CInfo->Position.y, CInfo->Position.z,
				CInfo->AngleInfo.TargetAngle, CInfo->AngleInfo.NowAngle );
		}
	}
}

// �Q�[�����C���̏����ɓG��|�������Ƃ�`����
void Task_GameMain_AddKillEnemy(
	// �Q�[�����C���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo,

	// �|�����G�̎��ʔԍ�
	int CharaNo
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;

	// �|�����G�̐��𑝂₷
	GMData->KillEnemyNum++;

	// �|�����G���A�N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�������ꍇ�́A
	// �|���ׂ��G��|�������ǂ����̃t���O�𗧂Ă�
	if( StageData_GetKillTargetChara() == CharaNo )
	{
		GMData->KillTargetCharaKill = true;
	}
}

// �Q�[�����C���̏������J�n����
//     �߂�l : �Q�[�����C���^�X�N���\���̂ւ̃A�h���X
STaskInfo * Task_GameMain_Start( void )
{
	STask_GameMainData *GMData;

	// �Q�[�����C���̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	GMData = ( STask_GameMainData * )calloc( 1, sizeof( STask_GameMainData ) );
	if( GMData == NULL )
	{
		return NULL;
	}

	//��b�^�X�N����
	GMData->AdventureTaskInfo = Task_Adventure_Start();
	if (GMData->AdventureTaskInfo == NULL) {
		return false;
	}

	// �Q�[�����C���̏������s�����ǂ����̃t���O�𗧂Ă�
	GMData->EnableGameMainStepProcess   = true;
	GMData->EnableGameMainRenderProcess = true;

	// �|�����G�̐��ƁA�|���ׂ��G��|�������ǂ����̃t���O������������
	GMData->KillEnemyNum        = 0;
	GMData->KillTargetCharaKill = false;

	// �v���C���[�̗͕̑\���Ȃǂ��s�����ǂ����̃t���O�𗧂Ă�
	GMData->DrawHUD             = false;

	// �X�e�[�W�J�n�҂���Ԃɂ���
	//GMData->State               = ETask_GameMainState_StageStart_Wait;
	GMData->State = ETask_GameMainState_Adventure_EndWait;
	GMData->Counter             = 0.0f;

	// �^�X�N��o�^����
	GMData->TaskInfo.Base = &g_Task_GameMainTaskBaseInfo;
	GMData->TaskInfo.Data = GMData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );

	System_FadeIn();

	// �^�X�N���\���̂̃A�h���X��Ԃ�
	return &GMData->TaskInfo;
}




