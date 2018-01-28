#ifndef TASK_PAUSEMENU_H
#define TASK_PAUSEMENU_H

#include "Task.h"

// �|�[�Y���j���[�̃��j���[����
typedef enum _ETask_PauseMenu
{
	ETask_PauseMenu_Continue,		// �Q�[���ĊJ
	ETask_PauseMenu_BackToTitle,	// �^�C�g����ʂɖ߂�
	ETask_PauseMenu_GameEnd,		// �Q�[���I��

	ETask_PauseMenu_Num,			// ���ڂ̐�
} ETask_PauseMenu;

// �|�[�Y���j���[�̏������J�n����
//     �߂�l : �|�[�Y���j���[�^�X�N���\���̂ւ̃A�h���X
extern STaskInfo * Task_PauseMenu_Start( void );

// �|�[�Y���j���[���I�����������`�F�b�N����
//     �߂�l : �|�[�Y���j���[���I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
extern bool Task_PauseMenu_CheckEnd(
	// �|�[�Y���j���[�^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
);

// �|�[�Y���j���[�őI�����ꂽ���ڂ��擾����
//     �߂�l : �I�����ꂽ���ڂ̗񋓌^�̒l
extern ETask_PauseMenu Task_PauseMenu_GetChoiceIndex(
	// �|�[�Y���j���[�^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
);

#endif