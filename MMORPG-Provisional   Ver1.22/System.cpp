#include "System.h"
#include "DxLib.h"
#include "Chara.h"
#include "Effect.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"
#include "GameMainRender.h"
#include "Camera.h"
#include "Task_GameMain.h"
#include "Task_Title.h"
#include "Task_KeyConfig.h"
#include "Task_AllStageClear.h"
#include "Task_DebugStartMenu.h"
#include "Mathematics.h"
#include "Code/Common/Mouse/Mouse.h"

// �t�F�[�h�A�E�g�A�t�F�[�h�C���̑��x
#define FADE_SPEED			(1.0f)

// �P�t���[���̍ő厞��
#define MAX_DELTA_TIME		(1.0f / 120.0f)

// ��x�ɏ�������ő�t���[����
#define MAX_FRAME_NUM		(8)

// �ҏW�ł��鐮���p�����[�^�̍ő吔
#define DEBUG_INT_MAXNUM	(20)

// �V�X�e���̏��
typedef struct _SSystemInfo
{
	// ��O�̃t���[���̎���
	LONGLONG        PrevTime;

	// ��Ԑ��ڏ����Ő��ڂ����鎞��
	float           StepTime;

	// �P�t���[���ŏ�Ԑ��ڏ������s����
	int             StepNum;

	// �\�t�g���I�����邩�ǂ����̃t���O
	bool            ExitGame;

	// �v���C���[�L�����̃L�������\���̂̃A�h���X���i�[����|�C���^�ϐ�
	SCharaInfo *    PlayerCharaInfo;

	// �Q�[���J�n���̃X�e�[�W
	int             StartStage;

	// ���I�ҏW�����鐮���p�����[�^�̐�
	int             DebugParamIntNum;

	// ���I�ҏW�����鐮���p�����[�^�z��̐擪�A�h���X���i�[����|�C���^�ϐ�
	SParamInt *     DebugParamInt;

	// �f�o�b�O����\�����邩�ǂ����̃t���O
	bool            DispDebugInfo;

	// �^�X�N�V�X�e�����
	STaskSystemInfo TaskSystemInfo;

	// �\�t�g�S�̂Ŏg�p����t�H���g�n���h��
	int             FontHandle[EFontHandle_Num];

	// ���L���ʉ��̃n���h��
	int				CommonSEHandle[ECommonSE_Num];

	// �t�F�[�h�������s���Ă��邩�ǂ����̃t���O
	bool            Fade;

	// �t�F�[�h�A�E�g��Ԃ��ǂ����̃t���O
	bool            FadeOut;

	// �t�F�[�h�A�E�g��
	float           FadeOutRate;

	// �O��t���[�����[�g�\���p�Ƀt���[���J�E���g�����Z�b�g��������
	LONGLONG        BackDispFrameTime;

	// �t���[�����[�g�\���p�̃t���[���J�E���^
	int             FrameCounter;

	// �t���[�����[�g�\���ŕ\������t���[�����[�g
	int             DispFrameCount;

	// �Q�[�����C���^�X�N�̃^�X�N���\���̂̃A�h���X���i�[����|�C���^�ϐ�
	STaskInfo *     GameMainTaskInfo;

	// �Ꮘ�����׃��[�h���ǂ����̃t���O
	bool			LowSpecMode;
} SSystemInfo;

// ���L���ʉ��̃t�@�C����
static const char *g_CommonSEFileName[ECommonSE_Num] =
{
	"system_alert.wav",
	"system_cancel.wav",
	"system_counter.wav",
	"system_cursor.wav",
	"system_enter.wav",
};

// �V�X�e���̏��
static SSystemInfo g_SystemInfo;

// ��Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool System_Step(
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime
);

// �`�揈�����s��
static void System_Render(void);

// �������������s��
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
static bool System_Initialize(void);

// ���C�����[�v�����s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool System_MainLoop(void);

// ��n�����s��
static void System_Terminate(void);

// �t�F�[�h�����̏�Ԑ��ڏ������s��
static void System_FadeStep(
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime
);

// �t�F�[�h�����̕`����s��
static void System_FadeRender(void);

// �f�o�b�O�֌W�̏�Ԑ��ڏ������s��
static void System_DebugStep(void);

// �f�o�b�O�֌W�̕`�揈�����s��
static void System_DebugDraw(void);

// �t�F�[�h�����̏�Ԑ��ڏ������s��
static void System_FadeStep(
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime
)
{
	// �t�F�[�h�������s���Ă��邩�ǂ����̃t���O���|��Ă���ꍇ�͉��������I��
	if (!g_SystemInfo.Fade)
	{
		return;
	}

	// �t�F�[�h�A�E�g���̕ύX
	if (ParamChangeFloat(&g_SystemInfo.FadeOutRate,
		g_SystemInfo.FadeOut ? 1.0f : 0.0f, StepTime, FADE_SPEED))
	{
		// �l�̕ύX������������t�F�[�h�A�E�g���s���Ă��邩�ǂ����̃t���O��|��
		g_SystemInfo.Fade = false;
	}
}

// �t�F�[�h�����̕`����s��
static void System_FadeRender(void)
{
	int Alpha;

	// �t�F�[�h�A�E�g������s�����x���v�Z
	Alpha = (int)(g_SystemInfo.FadeOutRate * 255.0f);

	// �s�����x���O�ȏ�Ȃ�`��
	if (Alpha > 0)
	{
		// ��ʑS�̂Ƀt�F�[�h�A�E�g�p�̍����l�p��`��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawBox(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, GetColor(0, 0, 0), TRUE);
	}
}

// �f�o�b�O�֌W�̏�Ԑ��ڏ������s��
static void System_DebugStep(void)
{
	int EditParamInt;
	int EditParamIntAdd;

	// �f�o�b�O����\�����邩�ǂ����̃t���O���|��Ă��邩�ǂ����ŏ����𕪊�
	if (!g_SystemInfo.DispDebugInfo)
	{
		// \�L�[�������ꂽ��f�o�b�O���\�����n�m�ɂ���
		if (GetInputChar(TRUE) == '\\')
		{
			g_SystemInfo.DispDebugInfo = true;
		}
	}
	else
	{
		EditParamInt = -1;

		// �����ꂽ�L�[�ɂ���ď����𕪊�
		switch (GetInputChar(TRUE))
		{
		case '\\':	// �f�o�b�O�\�����n�e�e�ɂ���
			g_SystemInfo.DispDebugInfo = false;
			break;

			// �����p�����[�^�̓��I�ҏW����
		case '1': EditParamInt = 0; EditParamIntAdd = 1; break;
		case 'q': EditParamInt = 0; EditParamIntAdd = -1; break;
		case '2': EditParamInt = 1; EditParamIntAdd = 1; break;
		case 'w': EditParamInt = 1; EditParamIntAdd = -1; break;
		case '3': EditParamInt = 2; EditParamIntAdd = 1; break;
		case 'e': EditParamInt = 2; EditParamIntAdd = -1; break;
		case '4': EditParamInt = 3; EditParamIntAdd = 1; break;
		case 'r': EditParamInt = 3; EditParamIntAdd = -1; break;
		case '5': EditParamInt = 4; EditParamIntAdd = 1; break;
		case 't': EditParamInt = 4; EditParamIntAdd = -1; break;
		case '6': EditParamInt = 5; EditParamIntAdd = 1; break;
		case 'y': EditParamInt = 5; EditParamIntAdd = -1; break;
		case '7': EditParamInt = 6; EditParamIntAdd = 1; break;
		case 'u': EditParamInt = 6; EditParamIntAdd = -1; break;
		case '8': EditParamInt = 7; EditParamIntAdd = 1; break;
		case 'i': EditParamInt = 7; EditParamIntAdd = -1; break;
		case '9': EditParamInt = 8; EditParamIntAdd = 1; break;
		case 'o': EditParamInt = 8; EditParamIntAdd = -1; break;
		case '0': EditParamInt = 9; EditParamIntAdd = 1; break;
		case 'p': EditParamInt = 9; EditParamIntAdd = -1; break;

		case 'a': EditParamInt = 10; EditParamIntAdd = 1; break;
		case 'z': EditParamInt = 10; EditParamIntAdd = -1; break;
		case 's': EditParamInt = 11; EditParamIntAdd = 1; break;
		case 'x': EditParamInt = 11; EditParamIntAdd = -1; break;
		case 'd': EditParamInt = 12; EditParamIntAdd = 1; break;
		case 'c': EditParamInt = 12; EditParamIntAdd = -1; break;
		case 'f': EditParamInt = 13; EditParamIntAdd = 1; break;
		case 'v': EditParamInt = 13; EditParamIntAdd = -1; break;
		case 'g': EditParamInt = 14; EditParamIntAdd = 1; break;
		case 'b': EditParamInt = 14; EditParamIntAdd = -1; break;
		case 'h': EditParamInt = 15; EditParamIntAdd = 1; break;
		case 'n': EditParamInt = 15; EditParamIntAdd = -1; break;
		case 'j': EditParamInt = 16; EditParamIntAdd = 1; break;
		case 'm': EditParamInt = 16; EditParamIntAdd = -1; break;
		case 'k': EditParamInt = 17; EditParamIntAdd = 1; break;
		case ',': EditParamInt = 17; EditParamIntAdd = -1; break;
		case 'l': EditParamInt = 18; EditParamIntAdd = 1; break;
		case '.': EditParamInt = 18; EditParamIntAdd = -1; break;
		case ';': EditParamInt = 19; EditParamIntAdd = 1; break;
		case '/': EditParamInt = 19; EditParamIntAdd = -1; break;
		}

		// ���I�ҏW�����p�ɐ����p�����[�^���ݒ肳��Ă���ꍇ�͕ҏW�������s��
		if (g_SystemInfo.DebugParamInt != NULL &&
			EditParamInt >= 0 &&
			g_SystemInfo.DebugParamIntNum > EditParamInt)
		{
			g_SystemInfo.DebugParamInt[EditParamInt].Param += EditParamIntAdd;
		}
	}
}

// �f�o�b�O�֌W�̕`�揈�����s��
static void System_DebugDraw(void)
{
	int                i;
	static const char *KeyName[] =
	{
		"1/q", "2/w", "3/e", "4/r", "5/t", "6/y", "7/u", "8/i", "9/o", "0/p",
		"a/z", "s/x", "d/c", "f/v", "g/b", "h/n", "j/m", "k/,", "l/.", ";//"
	};

	// �f�o�b�O�\�������邩�ǂ����̃t���O���|��Ă���ꍇ�͉��������ɏI��
	if (!g_SystemInfo.DispDebugInfo)
	{
		return;
	}

	// ��Ԑ��ڏ����P�񓖂���̎��Ԃ�`��
	DrawFormatString(0, GAME_SCREEN_HEIGHT - 16, GetColor(255, 255, 255),
		"StepTime:%fsec", g_SystemInfo.StepTime);

	// ���I�ҏW�̑ΏۂɂȂ��Ă��鐮���p�����[�^�̕`��
	for (i = 0; i < g_SystemInfo.DebugParamIntNum; i++)
	{
		DrawFormatString(
			0, 64 + i * 16,
			GetColor(255, 255, 255),
			"%s %20s:%d",
			KeyName[i],
			g_SystemInfo.DebugParamInt[i].Name,
			g_SystemInfo.DebugParamInt[i].Param
		);
	}
}

// �������������s��
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
static bool System_Initialize(void)
{
	int  i;
	char FilePath[MAX_PATH];

	// �E�C���h�E���[�h�ŋN�����邩�m�F����
	if (MessageBox(NULL, "�E�C���h�E���[�h�ŋN�����܂����H", "��ʃ��[�h�m�F", MB_YESNO) == IDYES)
	{
		// �u�͂��v���I�����ꂽ�ꍇ�̓E�C���h�E���[�h�ŋN��
		ChangeWindowMode(TRUE);
	}

	// �Ꮘ�����׃��[�h�ŋN�����邩�m�F����
	if (MessageBox(NULL, "�Ꮘ�����׃��[�h�ŋN�����܂����H", "�������׃��[�h�m�F", MB_YESNO) == IDYES)
	{
		// �u�͂��v���I�����ꂽ�ꍇ�͒Ꮘ�����׃��[�h�t���O�𗧂Ă�
		g_SystemInfo.LowSpecMode = true;
	}

	// �Q�[����ʂ̉𑜓x��ݒ�
	SetGraphMode(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, 32);

	// �E�C���h�E�̃^�C�g����ݒ�
	SetMainWindowText("Sword Bout");

	// �Ꮘ�����׃��[�h���ǂ�����XAudio���g�p���邩�ǂ����̐ݒ��ύX����
	SetEnableXAudioFlag(g_SystemInfo.LowSpecMode ? FALSE : TRUE);

	// �R�c�T�E���h�̂P���[�g��������̒l���Z�b�g
	Set3DSoundOneMetre(100.0f);

	// �c�w���C�u������������
	if (DxLib_Init() == -1)
	{
		return false;
	}

	// �t�H���g�n���h���̍쐬
	g_SystemInfo.FontHandle[EFontHandle_Small] = CreateFontToHandle(
		NULL, FONTHANDLE_SMALL_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	g_SystemInfo.FontHandle[EFontHandle_Midium] = CreateFontToHandle(
		NULL, FONTHANDLE_MIDIUM_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	g_SystemInfo.FontHandle[EFontHandle_Big] = CreateFontToHandle(
		NULL, FONTHANDLE_BIG_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	for (i = 0; i < EFontHandle_Num; i++)
	{
		if (g_SystemInfo.FontHandle[i] == -1)
		{
			return false;
		}
	}

	// ���L���ʉ��̓ǂݍ���
	for (i = 0; i < ECommonSE_Num; i++)
	{
		sprintf(FilePath, "Data\\Sound\\SE\\Common\\%s", g_CommonSEFileName[i]);
		g_SystemInfo.CommonSEHandle[i] = LoadSoundMem(FilePath);
		if (g_SystemInfo.CommonSEHandle[i] == -1)
		{
			return false;
		}
	}

	// �v���C���[�L�����̃L�������\���̂̃A�h���X���i�[����|�C���^�ϐ��̏�����
	g_SystemInfo.PlayerCharaInfo = NULL;

	// �t�F�[�h�A�E�g�֌W�̕ϐ���������
	g_SystemInfo.FadeOut = true;
	g_SystemInfo.Fade = false;
	g_SystemInfo.FadeOutRate = 1.0f;

	// �^�X�N�V�X�e���̏�����
	TaskSystem_Initialize(&g_SystemInfo.TaskSystemInfo);

	// �T�E���h�����̏�����
	if (!Sound_Initialize())
	{
		return false;
	}

	// �Q�[�����C���̕`�揈���̏�����
	if (!GameMainRender_Initialize())
	{
		return false;
	}

	// ���͏����̏�����
	InputInitialize();

	// �L�����N�^�[�̏����̏�����
	if (!Chara_Initialize())
	{
		return false;
	}

	// �G�t�F�N�g�̏����̏�����
	if (!Effect_Initialize())
	{
		return false;
	}

	// �X�e�[�W�̏����̏�����
	if (!Stage_Initialize())
	{
		return false;
	}

	// �J�����̏����̏�����
	Camera_Initialize();

#ifdef DEBUG_START_MODE
	// �f�o�b�O�X�^�[�g���[�h�̏ꍇ�̓f�o�b�O���j���[����J�n����
	if (!System_StartDebugStartMenu())
	{
		return false;
	}
#else
	// �ʏ탂�[�h�̏ꍇ�̓^�C�g�����j���[����J�n����
	if (!System_StartTitle())
	{
		return false;
	}
#endif

	// �t���[�����[�g�v���p�̕ϐ��̏�����
	g_SystemInfo.BackDispFrameTime = GetNowHiPerformanceCount();
	g_SystemInfo.FrameCounter = 0;
	g_SystemInfo.DispFrameCount = 0;

	// �����I��
	return true;
}

// ���C�����[�v�����s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool System_MainLoop(void)
{
	LONGLONG NowTime;
	int      i;

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �ŏ��̃t���[���̏�Ԑ��ڎ��ԂȂǂ��Z�b�g
	g_SystemInfo.StepTime = MAX_DELTA_TIME;
	g_SystemInfo.PrevTime = GetNowHiPerformanceCount();
	g_SystemInfo.StepNum = 1;

	// ���C�����[�v
	while (ProcessMessage() == 0)
	{
		// ��Ԑ��ڏ������s���񐔕����[�v
		for (i = 0; i < g_SystemInfo.StepNum; i++)
		{
			Mouse_Update();

			// �G�X�P�[�v�L�[��������邩�A
			// �\�t�g���I�����邩�ǂ����̃t���O�������Ă����烋�[�v�𔲂���
			if (CheckHitKey(KEY_INPUT_ESCAPE) == 1 || g_SystemInfo.ExitGame)
			{
				g_SystemInfo.ExitGame = true;
				break;
			}

#ifdef _DEBUG
			// �f�o�b�O�֌W�̏�Ԑ��ڏ������s��
			System_DebugStep();
#endif

			// ��Ԑ��ڏ������s��
			if (!System_Step(g_SystemInfo.StepTime))
			{
				return false;
			}
		}

		// �\�t�g���I�����邩�ǂ����̃t���O�������Ă����烋�[�v�𔲂���
		if (g_SystemInfo.ExitGame)
		{
			break;
		}

		// �`�揈�����s��
		System_Render();

		// �r�L�[��������Ă�����Q�[����ʂ̃X�N���[���V���b�g��ۑ�����
		if (CheckHitKey(KEY_INPUT_S) != 0)
		{
			SaveDrawScreen(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,
				"ScreenShot.bmp");
		}

		// ����ʂ̓��e��\��ʂɔ��f������
		ScreenFlip();

		// ���݂̎��Ԃ��擾
		NowTime = GetNowHiPerformanceCount();

		// �O��擾�������Ԃ���̌o�ߎ��Ԃ��Z�o
		g_SystemInfo.StepTime = (NowTime - g_SystemInfo.PrevTime) / 1000000.0f;

		// ��Ԑ��ڏ������s���񐔂��Z�o
		g_SystemInfo.StepNum = (int)(g_SystemInfo.StepTime / MAX_DELTA_TIME);

		// ��Ԑ��ڏ������s���񐔂��O��̏ꍇ�͂P��ɂ���
		if (g_SystemInfo.StepNum == 0)
		{
			g_SystemInfo.StepNum = 1;
		}
		else
		{
			// �O��ł͂Ȃ��ꍇ�͏�Ԑ��ڂŐi�߂鎞�Ԃ���Ԑ��ڂ��s���񐔂Ŋ���
			g_SystemInfo.StepTime /= g_SystemInfo.StepNum;

			// ������Ԑ��ڂ��s���񐔂��ő�l�𒴂��Ă�����ő�l�ɗ}����
			if (g_SystemInfo.StepNum > MAX_FRAME_NUM)
			{
				g_SystemInfo.StepNum = MAX_FRAME_NUM;
			}
		}

		// ����̎��Ԃ�ۑ�����
		g_SystemInfo.PrevTime = NowTime;

		// �t���[�����[�g�v���p�̃J�E���^���P����
		g_SystemInfo.FrameCounter++;

		// �O��t���[�����[�g���X�V���Ă���P�b�o�߂�����\���p�t���[���J�E���g���X�V����
		if (NowTime - g_SystemInfo.BackDispFrameTime > 1000000)
		{
			// �\���p�t���[���J�E���g���X�V
			g_SystemInfo.DispFrameCount = g_SystemInfo.FrameCounter;

			// �v���p�t���[���J�E���^��������
			g_SystemInfo.FrameCounter = 0;

			// �\���p�t���[���J�E���g���X�V�������Ԃ��X�V
			g_SystemInfo.BackDispFrameTime = NowTime;
		}

		// ��ʂ�����������
		ClearDrawScreen();
	}

	// ����I��
	return true;
}

// ��Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool System_Step(
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime
)
{
	// ���͏������s��
	ProcessInput();

	// �t�F�[�h�����̏�Ԑ��ڂ��s��
	System_FadeStep(StepTime);

	// �^�X�N�V�X�e���̏�Ԑ��ڏ������s��
	if (!TaskSystem_Step(&g_SystemInfo.TaskSystemInfo, StepTime))
	{
		return false;
	}

	// ����I��
	return true;
}

// �`�揈�����s��
static void System_Render(void)
{
	// �^�X�N�V�X�e���̕`�揈�����s��
	TaskSystem_Render(&g_SystemInfo.TaskSystemInfo);

	// �y�o�b�t�@�͎g�p���Ȃ��ݒ�ɂ���
	SetUseZBufferFlag(FALSE);

	// �t�F�[�h�����̕`����s��
	System_FadeRender();

	// �t���[�����[�g����ʉE���ɕ`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawFormatString(GAME_SCREEN_WIDTH - 84, GAME_SCREEN_HEIGHT - 20,
		GetColor(255, 255, 255), "FPS:%d", (int)g_SystemInfo.DispFrameCount);

	// �f�o�b�O�֌W�̕`�揈�����s��
	System_DebugDraw();
}

// ��n�����s��
static void System_Terminate(void)
{
	int i;

	// �T�E���h�����̌�n��
	Sound_Terminate();

	// �L�����N�^�[�̏����̌�n��
	Chara_Terminate();

	// �G�t�F�N�g�̏����̌�n��
	Effect_Terminate();

	// �Q�[�����C���`�揈���̌�n��
	GameMainRender_Terminate();

	// �t�H���g�n���h���̍폜
	for (i = 0; i < EFontHandle_Num; i++)
	{
		DeleteFontToHandle(g_SystemInfo.FontHandle[i]);
		g_SystemInfo.FontHandle[i] = -1;
	}

	// ���L���ʉ��̍폜
	for (i = 0; i < ECommonSE_Num; i++)
	{
		DeleteSoundMem(g_SystemInfo.CommonSEHandle[i]);
		g_SystemInfo.CommonSEHandle[i] = -1;
	}

	// �c�w���C�u�����̏I������
	DxLib_End();
}

// �\�t�g�̏��������s����
//     �߂�l : ����ɏ����������������ǂ���
//              ( true : ����ɏ�������������   false : �G���[���������� )
bool System_Main(void)
{
	// �������������s��
	if (!System_Initialize())
	{
		return false;
	}

	// ���C�����[�v�������s��
	if (!System_MainLoop())
	{
		return false;
	}

	// ��n�����s��
	System_Terminate();

	// ����I��
	return true;
}

// �f�o�b�O�N�����j���[���J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool System_StartDebugStartMenu(void)
{
	if (!Task_DebugStartMenu_Start())
	{
		return false;
	}

	// ����I��
	return true;
}

// �Q�[�����C�����J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool System_StartGameMain(void)
{
	g_SystemInfo.GameMainTaskInfo = Task_GameMain_Start();
	if (g_SystemInfo.GameMainTaskInfo == NULL)
	{
		return false;
	}

	// ����I��
	return true;
}

// �^�C�g����ʂ��J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool System_StartTitle(void)
{
	if (!Task_Title_Start())
	{
		return false;
	}

	// ����I��
	return true;
}

// �L�[�R���t�B�O��ʂ��J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool System_StartKeyConfig(void)
{
	if (!Task_KeyConfig_Start())
	{
		return false;
	}

	// ����I��
	return true;
}

// �S�X�e�[�W�N���A���o���J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool System_StartAllStageClear(void)
{
	if (!Task_AllStageClear_Start())
	{
		return false;
	}

	// ����I��
	return true;
}

// �Q�[�����I������
void System_ExitGame(void)
{
	g_SystemInfo.ExitGame = true;
}

// ��ʂ��t�F�[�h�A�E�g������
void System_FadeOut(void)
{
	g_SystemInfo.FadeOut = true;
	g_SystemInfo.Fade = true;
}

// ��ʂ��t�F�[�h�C������
void System_FadeIn(void)
{
	g_SystemInfo.FadeOut = false;
	g_SystemInfo.Fade = true;
}

// ��ʂ̃t�F�[�h���������ǂ������擾����
//     �߂�l : �t�F�[�h���������ǂ���
//              ( true : �t�F�[�h������   false : �t�F�[�h�����͍s���Ă��Ȃ� )
bool System_CheckFade(void)
{
	return g_SystemInfo.Fade;
}

// ���L���ʉ����Đ�����
void System_PlayCommonSE(
	// ���ʉ�
	ECommonSE CommonSE
)
{
	PlaySoundMem(g_SystemInfo.CommonSEHandle[CommonSE], DX_PLAYTYPE_BACK);
}

// �v���C���[�L�����̃L�������\���̂̃A�h���X���擾����
//     �߂�l : �v���C���[�L�����̃L�������\���̂̃A�h���X
SCharaInfo *System_GetPlayerCharaInfo(void)
{
	return g_SystemInfo.PlayerCharaInfo;
}

// �v���C���[�L�����̃L�������\���̂̃A�h���X��o�^����
void System_SetPlayerCharaInfo(
	// �v���C���[�L�����̃L�������\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	g_SystemInfo.PlayerCharaInfo = CInfo;
}

// �w��T�C�Y�̃t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
int System_GetFontHandle(
	// �t�H���g�n���h���^�C�v
	EFontHandle FontType
)
{
	return g_SystemInfo.FontHandle[FontType];
}

// �Q�[���J�n���̃X�e�[�W��ݒ肷��
void System_SetStartStage(
	// �X�e�[�W�ԍ�
	int StageNo
)
{
	g_SystemInfo.StartStage = StageNo;
}

// �Q�[���J�n���̃X�e�[�W���擾����
//     �߂�l : �Q�[���J�n���̃X�e�[�W�ԍ�
int System_GetStartStage(void)
{
	return g_SystemInfo.StartStage;
}

// �^�X�N�V�X�e�����\���̂̃A�h���X���擾����
//     �߂�l : �^�X�N�V�X�e�����\���̂̃A�h���X
STaskSystemInfo *System_GetTaskSystemInfo(void)
{
	return &g_SystemInfo.TaskSystemInfo;
}

// �Q�[�����C���^�X�N�̃^�X�N���\���̂̃A�h���X���擾����
//     �߂�l : �Q�[�����C���^�X�N�̃^�X�N���\���̂̃A�h���X
STaskInfo * System_GetGameMainTaskInfo(void)
{
	return g_SystemInfo.GameMainTaskInfo;
}

// �f�o�b�O����\�����邩�ǂ����̃t���O���擾����
//     �߂�l : �f�o�b�O����\�����邩�ǂ���
//              ( true : �f�o�b�O����\������  false : �f�o�b�O����\�����Ȃ� )
bool System_GetDispDebugInfo(void)
{
	return g_SystemInfo.DispDebugInfo;
}

// �Ꮘ�����׃��[�h���ǂ������擾����
bool System_GetLowSpecMode(void)
{
	return g_SystemInfo.LowSpecMode;
}

// ���I�ҏW�����鐮���p�����[�^��o�^����
void System_SetDebugParamInt(
	// �����p�����[�^�z��̐擪�A�h���X
	SParamInt *ParamInt,

	// �����p�����[�^�̐�
	int ParamNum
)
{
	g_SystemInfo.DebugParamInt = ParamInt;
	g_SystemInfo.DebugParamIntNum = ParamNum;

	// ���I�ɕҏW�ł���ő吔�𒴂��Ă�����l��␳����
	if (g_SystemInfo.DebugParamIntNum > DEBUG_INT_MAXNUM)
	{
		g_SystemInfo.DebugParamIntNum = DEBUG_INT_MAXNUM;
	}
}




/*
#include<math.h>
#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"


bool Debug_flg;

//SYSTEM_INFO�͊����̃��C�u�������Ő錾����Ă����̂Ŗ��O��ς��܂��� by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO;


void Set_Debug_Mode(){

	if(Debug_flg == 0){
		Debug_flg = 1;
	}else{
		Debug_flg = 0;
	}
}

bool Get_Debug_mode(){
	return Debug_flg;
}

void SystemInitialize(){

	// �E�C���h�E���[�h�ŋN�����邩�m�F����
	//if( MessageBox( NULL, "�E�C���h�E���[�h�ŋN�����܂����H", "��ʃ��[�h�m�F", MB_YESNO ) == IDYES )
	//{
	//	// �u�͂��v���I�����ꂽ�ꍇ�̓E�C���h�E���[�h�ŋN��
	//	ChangeWindowMode( TRUE );
	//}
	//if(MessageBox(NULL,"�f�o�b�O���[�h�ŋN�����܂����H","�f�o�b�O�m�F",MB_YESNO) == IDYES){
	//	Set_Debug_Mode();
	//}else{
	//	Debug_flg = 0;
	//}

	//�Q�[���I���t���O�̃I�t
	s_SYSTEM_INFO.Exit_Game = true;
	//�E�B���h�E�n���h���̎擾
	s_SYSTEM_INFO.WinHndl = GetMainWindowHandle();


}

void SystemMain(){

}

void Set_Exit_Game(){

	s_SYSTEM_INFO.Exit_Game = false;
}

bool Get_Exit_Game(){
	return s_SYSTEM_INFO.Exit_Game;
}

////�t�F�[�h�C���A�E�g�֐�	true:FadeIN		false:FadeOut
//void Fade_io(int Color,int Fade_Flg){
//
//	SYSTEM_INFO.Fade_Out_Count = 0;
//	if(Fade_Flg == true){			
//		for(int i = 255;0 < i;i -= 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}else{
//		for(int i = 0;255 > i;i += 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}
//}

HWND GetConsoleHwnd(){

	return NULL;

}
*/