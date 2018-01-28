#ifndef SYSTEM_H
#define SYSTEM_H

#include "DxLib.h"
#include "Task.h"
#include "Chara.h"

// �Q�[����ʂ̕�
#define GAME_SCREEN_WIDTH			(1280)

// �Q�[����ʂ̍���
#define GAME_SCREEN_HEIGHT			(720)

// ���t�H���g�̃T�C�Y
#define FONTHANDLE_SMALL_SIZE		(24)

// ���t�H���g�̃T�C�Y
#define FONTHANDLE_MIDIUM_SIZE		(32)

// ��t�H���g�̃T�C�Y
#define FONTHANDLE_BIG_SIZE			(80)

// �t�H���g�n���h��
typedef enum _EFontHandle
{
	EFontHandle_Small,		// ���t�H���g�n���h��
	EFontHandle_Midium,		// ���t�H���g�n���h��
	EFontHandle_Big,		// ��t�H���g�n���h��

	EFontHandle_Num			// �t�H���g�n���h���̐�
} EFontHandle;

// ���L���ʉ�
typedef enum _ECommonSE
{
	ECommonSE_Alert,		// �x��
	ECommonSE_Cancel,		// �L�����Z��
	ECommonSE_Counter,		// �J�E���g
	ECommonSE_Cursor,		// �J�[�\��
	ECommonSE_Enter,		// ����

	ECommonSE_Num,			// ���L���ʉ��̐�
} ECommonSE;

// �����p�����[�^���
typedef struct _SParamInt
{
	// �����p�����[�^
	int         Param;

	// �p�����[�^��
	const char *Name;
} SParamInt;

// �\�t�g�̏��������s����
//     �߂�l : ����ɏ����������������ǂ���
//              ( true : ����ɏ�������������   false : �G���[���������� )
extern bool System_Main(void);

// �f�o�b�O�N�����j���[���J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool System_StartDebugStartMenu(void);

// �Q�[�����C�����J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool System_StartGameMain(void);

// �^�C�g����ʂ��J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool System_StartTitle(void);

// �L�[�R���t�B�O��ʂ��J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool System_StartKeyConfig(void);

// �S�X�e�[�W�N���A���o���J�n����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool System_StartAllStageClear(void);

// �Q�[�����I������
extern void System_ExitGame(void);

// ��ʂ��t�F�[�h�A�E�g������
extern void System_FadeOut(void);

// ��ʂ��t�F�[�h�C������
extern void System_FadeIn(void);

// ��ʂ̃t�F�[�h���������ǂ������擾����
//     �߂�l : �t�F�[�h���������ǂ���
//              ( true : �t�F�[�h������   false : �t�F�[�h�����͍s���Ă��Ȃ� )
extern bool System_CheckFade(void);

// ���L���ʉ����Đ�����
extern void System_PlayCommonSE(
	// ���ʉ�
	ECommonSE CommonSE
);

// �v���C���[�L�����̃L�������\���̂̃A�h���X���擾����
//     �߂�l : �v���C���[�L�����̃L�������\���̂̃A�h���X
extern SCharaInfo *System_GetPlayerCharaInfo(void);

// �v���C���[�L�����̃L�������\���̂̃A�h���X��o�^����
extern void System_SetPlayerCharaInfo(
	// �v���C���[�L�����̃L�������\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �w��T�C�Y�̃t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
extern int System_GetFontHandle(
	// �t�H���g�n���h���^�C�v
	EFontHandle FontType
);

// �Q�[���J�n���̃X�e�[�W��ݒ肷��
extern void System_SetStartStage(
	// �X�e�[�W�ԍ�
	int StageNo
);

// �Q�[���J�n���̃X�e�[�W���擾����
//     �߂�l : �Q�[���J�n���̃X�e�[�W�ԍ�
extern int System_GetStartStage(void);

// �^�X�N�V�X�e�����\���̂̃A�h���X���擾����
//     �߂�l : �^�X�N�V�X�e�����\���̂̃A�h���X
extern STaskSystemInfo *System_GetTaskSystemInfo(void);

// �Q�[�����C���^�X�N�̃^�X�N���\���̂̃A�h���X���擾����
//     �߂�l : �Q�[�����C���^�X�N�̃^�X�N���\���̂̃A�h���X
extern STaskInfo *System_GetGameMainTaskInfo(void);

// �f�o�b�O����\�����邩�ǂ����̃t���O���擾����
//     �߂�l : �f�o�b�O����\�����邩�ǂ���
//              ( true : �f�o�b�O����\������  false : �f�o�b�O����\�����Ȃ� )
extern bool System_GetDispDebugInfo(void);

// �Ꮘ�����׃��[�h���ǂ������擾����
extern bool System_GetLowSpecMode(void);

// ���I�ҏW�����鐮���p�����[�^��o�^����
extern void System_SetDebugParamInt(
	// �����p�����[�^�z��̐擪�A�h���X
	SParamInt *ParamInt,

	// �����p�����[�^�̐�
	int ParamNum
);




void Set_Exit_Game();
bool Get_Exit_Game();
void SystemInitialize();
void SystemMain();
//void Fade_io(int Color,int Fade_Flg);


enum eCol{
	eCol_White,
	eCol_EmeraldGreen,
	eCol_Black,
	eCol_Max,
};

//�J���[�e�[�u��0:��1:�G�������h�O���[��2:��
const int Color_ValTbl[eCol_Max] = {
	GetColor(255,255,255),
	GetColor(0,255,127),
	GetColor(0,0,0)
};

//�V�X�e���n���̍\����
struct _SYSTEM_INFO_t{	//_SYSTEM_INFO�͊����̃��C�u�����Ŏg�p����Ă����̂Ŗ��O��ύX���܂��� by.Syara

	//�Q�[�����I�����邩�ǂ���
	bool Exit_Game;
	//�S�̂̃t�F�[�h�A�E�g�Ŏg�p����J�E���g�ϐ�
	int Fade_Out_Count;
	//Window�̃n���h��
	HWND WinHndl;
	//Window�̍��W�l
	LPRECT WinAxis;
	//Window�̃^�C�g��
	char WinTitle;

};

#endif