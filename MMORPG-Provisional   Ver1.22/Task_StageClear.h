#ifndef TASK_STAGECLEAR_H
#define TASK_STAGECLEAR_H

#include "Task.h"

// �X�e�[�W�N���A�\�����J�n����
//     �߂�l : �X�e�[�W�N���A�\���^�X�N���\���̂ւ̃A�h���X
extern STaskInfo * Task_StageClear_Start(    void );

// �X�e�[�W�N���A�\�����I�����������`�F�b�N����
//     �߂�l : �X�e�[�W�N���A�\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
extern bool        Task_StageClear_CheckEnd(
	// �X�e�[�W�N���A�\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
);

#endif