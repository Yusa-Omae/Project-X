#ifndef TASK_GAMEOVER_H
#define TASK_GAMEOVER_H

#include "Task.h"

// �Q�[���I�[�o�[�\�����J�n����
//     �߂�l : �Q�[���I�[�o�[�\���^�X�N���\���̂ւ̃A�h���X
extern STaskInfo * Task_GameOver_Start(    void );

// �Q�[���I�[�o�[�\�����I�����������`�F�b�N����
//     �߂�l : �Q�[���I�[�o�[�\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
extern bool        Task_GameOver_CheckEnd(
	// �Q�[���I�[�o�[�\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
);

#endif