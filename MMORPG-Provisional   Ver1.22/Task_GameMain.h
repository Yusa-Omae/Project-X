#ifndef TASK_GAMEMAIN_H
#define TASK_GAMEMAIN_H

#include "Task.h"

// �Q�[�����C���̏������J�n����
//     �߂�l : �Q�[�����C���^�X�N���\���̂ւ̃A�h���X
extern STaskInfo * Task_GameMain_Start( void );

// �Q�[�����C���̏����ɓG��|�������Ƃ�`����
extern void        Task_GameMain_AddKillEnemy(
	// �Q�[�����C���^�X�N���\���̂̃A�h���X
	STaskInfo *TInfo,

	// �|�����G�̎��ʔԍ�
	int CharaNo
);

#endif