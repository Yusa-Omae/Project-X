#ifndef TASK_STAGESTART_H
#define TASK_STAGESTART_H

#include "Task.h"

// �X�e�[�W�ԍ��\�����J�n����
//     �߂�l : �X�e�[�W�ԍ��\���^�X�N���\���̂ւ̃A�h���X
extern STaskInfo * Task_StageNumber_Start( void );

// �X�e�[�W�ԍ��\�����I�����������`�F�b�N����
//     �߂�l : �X�e�[�W�ԍ��\�����I���������邩�ǂ���
//              ( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
extern bool        Task_StageNumber_CheckEnd(
	// �X�e�[�W�ԍ��\���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo
);

#endif