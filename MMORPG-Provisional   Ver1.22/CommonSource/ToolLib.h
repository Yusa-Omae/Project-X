#ifndef __TOOLLIB__
#define __TOOLLIB__

#include "ToolLib_SpaceManage.h"
#include "ToolLib_Button.h"
#include "ToolLib_FileSelect.h"
#include "ToolLib_List.h"
#include "ToolLib_ParamBar.h"
#include "ToolLib_StringInput.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Window.h"

// �c�[�������̃E�C���h�E�̖��O�\�������̍���
#define TOOL_WINDOW_NAME_HEIGHT			(16)

// �c�[�������̃E�C���h�E�̃X�N���[���o�[�̕�
#define TOOL_SCROLLBAR_WIDTH			(16)

// �c�[���p�����̏��������s��
//     �߂�l : ������������Ɋ����������ǂ���( true:���튮��  false:�G���[���� )
extern bool Tool_Initialize( void );

// �c�[���p�����̌�n�����s��
extern void Tool_Terminate( void );

// �c�[���p�����̏�Ԑ��ڏ������s��
//     �߂�l : ����������Ɋ����������ǂ���( true:���튮��  false:�G���[���� )
extern bool Tool_Step(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
);

// �c�[���p�����̕`�揈�����s��
extern void Tool_Draw( void );

// �c�[���p�����̃T�C�Y�ύX�\�ȕ\���̈�̉E�[��X���W���擾����
//     �߂�l : �ϕ\���̈�E�[��X���W
extern int Tool_GetBaseRightX( void );

// �c�[���p�����ň����Ă���}�E�X�̃{�^�����͂��擾����
extern int Tool_GetMouseEdgeInput(
	// �����ꂽ�u�Ԃ̃}�E�X�̃{�^����������ϐ��̃A�h���X
	int *EdgeInput,

	// �}�E�X�̃{�^���������ꂽ�u�Ԃ̃}�E�X�J�[�\���̍��W��������ϐ��̃A�h���X
	int *EdgeX,
	int *EdgeY
);

#endif



