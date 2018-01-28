#include "Task_AllStageClear.h"
#include "DxLib.h"
#include "Task.h"
#include "System.h"
#include <math.h>

// �S�X�e�[�W�N���A��ʂ̃f�[�^������t�H���_�̃p�X
#define ALLSTAGECLEARDATA_DIR		"Data\\2D\\"

// �S�X�e�[�W�N���A��ʂ��t�F�[�h�C������O�ɑ҂���
#define FADE_IN_WAIT_TIME			(1.0f)

// �S�X�e�[�W�N���A��ʂ̃t�F�[�h�C���ɂ����鎞��
#define FADE_IN_TIME				(1.0f)

// �S�X�e�[�W�N���A��ʂ��t�F�[�h�A�E�g����O�ɑ҂���
#define FADE_OUT_WAIT				(8.1f)

// �S�X�e�[�W�N���A��ʂ̃t�F�[�h�A�E�g�ɂ����鎞��
#define FADE_OUT_TIME				(1.0f)

// �㉺�ɗh��镶���\���Ŏg�p���� sin �̊p�x�ω����x
#define ANGLE_SPEED					(1.5f)

// �㉺�ɗh��镶���\���� sin �ɓn���p�x�̂P�����ӂ�̕ύX�p�x
#define ONE_CHAR_ANGLE				(0.5f)

// �㉺�ɗh��镶���\���̂P�������̒ǉ��X�y�[�X
#define ONE_CHAR_SPACE				(16)

// �㉺�ɗh��镶���\���̐G�ꕝ
#define STRING_MOVE_Y_HEIGHT		(32.0f)


// �S�X�e�[�W�N���A��ʏ����̏��
typedef enum _ETask_AllStageClearState
{
	ETask_AllStageClearState_FadeInWait,	// �t�F�[�h�C���҂���
	ETask_AllStageClearState_FadeIn,		// �t�F�[�h�C����
	ETask_AllStageClearState_FadeOutWait,	// �t�F�[�h�A�E�g�҂���
	ETask_AllStageClearState_FadeOut,		// �t�F�[�h�A�E�g��
	ETask_AllStageClearState_End,			// �I�����
} ETask_AllStageClearState;

// �S�X�e�[�W�N���A��ʏ����p�̃f�[�^
typedef struct _STask_AllStageClearData
{
	// �^�X�N�̏��
	STaskInfo                TaskInfo;

	// �����̏��
	ETask_AllStageClearState State;

	// �ėp�J�E���^
	float                    Counter;

	// �㉺�ɗh��镶���\���Ŏg�p���� sin �ɓn���p�x
	float                    SinAngle;

	// �S�X�e�[�W�N���A�\���̕s�����x
	int                      Opacity;

	// �w�i�摜
	int                      BackGraphHandle;
} STask_AllStageClearData;

// �S�X�e�[�W�N���A��ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_AllStageClear_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �S�X�e�[�W�N���A��ʂ̕`�揈�����s��
static void Task_AllStageClear_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �S�X�e�[�W�N���A��ʂ̌�n���������s��
static void Task_AllStageClear_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
);

// �S�X�e�[�W�N���A��ʏ����^�X�N�̊�{���
static STaskBaseInfo g_Task_AllStageClearTaskBaseInfo =
{
	8,
	8,
	Task_AllStageClear_Step,
	Task_AllStageClear_Render,
	Task_AllStageClear_Terminate
};

// �S�X�e�[�W�N���A��ʂ̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Task_AllStageClear_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;

	// �㉺�ɗh��镶���\���Ŏg�p���� sin �ɓn���p�x�̕ύX
	ASCData->SinAngle += ANGLE_SPEED * StepTime;
	if( ASCData->SinAngle > DX_TWO_PI_F )
	{
		ASCData->SinAngle -= DX_TWO_PI_F;
	}

	// �S�X�e�[�W�N���A�\�������̏�Ԃɂ���ď����𕪊�
	switch( ASCData->State )
	{
	case ETask_AllStageClearState_FadeInWait:	// �t�F�[�h�C���҂���
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C���҂����Ԃ��o�߂�����t�F�[�h�C�����J�n����
			ASCData->State   = ETask_AllStageClearState_FadeIn;
			ASCData->Counter = 0.0f;

			// �S�X�e�[�W�N���A��ʂa�f�l�̍Đ����J�n����
			Sound_PlayBGM( EBGM_AllStageClear );
		}
		break;

	case ETask_AllStageClearState_FadeIn:		// �t�F�[�h�C����
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_IN_WAIT_TIME )
		{
			// �t�F�[�h�C��������������t�F�[�h�A�E�g�҂����J�n����
			ASCData->Opacity = 255;

			ASCData->State   = ETask_AllStageClearState_FadeOutWait;
			ASCData->Counter = 0.0f;
		}
		else
		{
			// �t�F�[�h�C�����̑S�X�e�[�W�N���A�\���̕s�����x���Z�o
			ASCData->Opacity = ( int )( ASCData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_AllStageClearState_FadeOutWait:	// �t�F�[�h�A�E�g�҂���
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_OUT_WAIT )
		{
			// �t�F�[�h�A�E�g�҂����Ԃ��o�߂�����t�F�[�h�A�E�g���J�n����
			System_FadeOut();

			ASCData->State   = ETask_AllStageClearState_FadeOut;
			ASCData->Counter = 0.0f;
		}
		break;

	case ETask_AllStageClearState_FadeOut:		// �t�F�[�h�A�E�g��
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_OUT_TIME )
		{
			// �t�F�[�h�A�E�g������������̏���

			// �ꉞ�S�X�e�[�W�N���A�\���̕s�����x���O�ɂ���
			ASCData->Opacity = 0;

			// ��Ԃ��I���ɂ���
			ASCData->State   = ETask_AllStageClearState_End;
			ASCData->Counter = 0.0f;

			// �^�C�g����ʂ��J�n����
			if( !System_StartTitle() )
			{
				return false;
			}

			// �^�X�N���폜����
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &ASCData->TaskInfo );
		}
		else
		{
			// �t�F�[�h�A�E�g���̑S�X�e�[�W�N���A�\���̕s�����x���Z�o
			ASCData->Opacity = 255 - ( int )( ASCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_AllStageClearState_End:			// �I�����
		break;
	}

	// ����I��
	return true;
}

// �S�X�e�[�W�N���A��ʂ̕`�揈�����s��
static void Task_AllStageClear_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;
	int                      DrawWidth;
	int                      DrawX;
	int                      DrawY;
	char                     DrawStr[ 2 ];
	int                      FontHandle;
	float                    Angle;
	int                      i;
	int                      CharNum;
	const char              *DispString = "ALL STAGE CLEAR!";
	int                      Bright;

	// �S�X�e�[�W�N���A�\���̕s�����x���O�̏ꍇ�͉��������I��
	if( ASCData->Opacity == 0 )
	{
		return;
	}

	// �w�i�͕\���̕s�����x�ɉ����ċP�x�𒲐߂���
	Bright = ASCData->Opacity * 160 / 255;
	SetDrawBright( Bright, Bright, Bright );
	DrawGraph( 0, 0, ASCData->BackGraphHandle, FALSE );
	SetDrawBright( 255,255,255 );

	// �ėp�t�H���g�n���h���̎擾
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// �����̐����擾
	CharNum   = strlen( DispString );

	// �S�X�e�[�W�N���A�\���̕`�敝���Z�o
	DrawWidth = GetDrawStringWidthToHandle( DispString, CharNum, FontHandle ) 
												+ ( CharNum - 1 ) * ONE_CHAR_SPACE;

	// �`��J�nX���W�̎Z�o
	DrawX     = ( GAME_SCREEN_WIDTH - DrawWidth ) / 2;

	// �A���t�@�u�����h���[�h�őS�X�e�[�W�N���A�\���̕s�����x��ݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, ASCData->Opacity );

	// �S�X�e�[�W�N���A�\���̕����̐������J��Ԃ�
	Angle = ASCData->SinAngle;
	for( i = 0; i < CharNum; i++ )
	{
		// �P�����������o����������̍쐬
		DrawStr[ 0 ] = DispString[ i ];
		DrawStr[ 1 ] = '\0';

		// ������`�悷��Y���W�̎Z�o
		DrawY = ( int )( ( GAME_SCREEN_HEIGHT - FONTHANDLE_BIG_SIZE ) / 2 
										+ sin( Angle ) * STRING_MOVE_Y_HEIGHT );

		// �����̕`��
		DrawStringToHandle( DrawX, DrawY, DrawStr, GetColor( 255,255,128 ), FontHandle );

		// �P�������`��X���W���ړ�����
		DrawX += GetDrawStringWidthToHandle( DrawStr, strlen( DrawStr ), FontHandle ) 
																		+ ONE_CHAR_SPACE;

		// �P�������㉺�ɗh���Y���W�̎Z�o�Ɏg�p���� sin �ɓn���p�x��ύX����
		Angle += ONE_CHAR_ANGLE;
	}

	// �u�����h���[�h��W���̏�Ԃɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// �S�X�e�[�W�N���A��ʂ̌�n���������s��
static void Task_AllStageClear_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;

	// �w�i�摜�̍폜
	DeleteGraph( ASCData->BackGraphHandle );
}

// �S�X�e�[�W�N���A��ʂ̏������J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Task_AllStageClear_Start( void )
{
	STask_AllStageClearData *ASCData;

	// �S�X�e�[�W�N���A��ʂ̏����Ŏg�p����f�[�^���i�[���郁�����̈�̊m��
	ASCData = ( STask_AllStageClearData * )calloc( 1, sizeof( STask_AllStageClearData ) );
	if( ASCData == NULL )
	{
		return false;
	}

	// �w�i�摜�̓ǂݍ���
	ASCData->BackGraphHandle  = LoadGraph( ALLSTAGECLEARDATA_DIR "AllStageClearBack.png" );
	if( ASCData->BackGraphHandle == -1 )
	{
		return false;
	}

	// �S�X�e�[�W�N���A�\���̕s�����x��������
	ASCData->Opacity = 0;

	// �S�X�e�[�W�N���A��ʂ̃t�F�[�h�C���҂��̏�Ԃɂ���
	ASCData->State    = ETask_AllStageClearState_FadeInWait;
	ASCData->Counter  = 0.0f;

	// ���t�F�[�h�̉������J�n
	System_FadeIn();

	// �^�X�N��o�^����
	ASCData->TaskInfo.Base = &g_Task_AllStageClearTaskBaseInfo;
	ASCData->TaskInfo.Data = ASCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &ASCData->TaskInfo );

	// ����I��
	return true;
}


