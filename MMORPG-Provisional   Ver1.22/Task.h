#ifndef TASK_H
#define TASK_H

// �^�X�N�̎��s�D�揇�ʂ̍ő�l - 1
#define TASK_PRIORITY_MAX		(16)

// �^�X�N�̊�{���
typedef struct _STaskBaseInfo
{
	// ��Ԑ��ڏ����̎��s�D�揇��
	int StepPriority;

	// �`�揈���̎��s�D�揇��
	int RenderPriority;

	// ��Ԑ��ڏ������s���֐��ւ̃|�C���^
	bool(*Step)(struct _STaskInfo *TInfo, float StepTime);

	// �`�揈�����s���֐��ւ̃|�C���^
	void(*Render)(struct _STaskInfo *TInfo);

	// ��n���������s���֐��ւ̃|�C���^
	void(*Terminate)(struct _STaskInfo *TInfo);
} STaskBaseInfo;

// �^�X�N�̏��
typedef struct _STaskInfo
{
	// �^�X�N�̊�{���ւ̃|�C���^
	STaskBaseInfo *     Base;

	// �^�X�N���g�p����f�[�^�ւ̃|�C���^
	void *              Data;

	// ��Ԑ��ڏ����̎��s�D�揇�ʂ������^�X�N�̃��X�g�̎��̃^�X�N�ւ̃|�C���^
	struct _STaskInfo * StepNext;

	// �`�揈���̎��s�D�揇�ʂ������^�X�N�̃��X�g�̎��̃^�X�N�ւ̃|�C���^
	struct _STaskInfo * RenderNext;

	// �ǉ����͍폜����^�X�N�̃��X�g�̎��̃^�X�N�ւ̃|�C���^
	struct _STaskInfo * AddOrDelNext;
} STaskInfo;

// �^�X�N�V�X�e�����
typedef struct _STaskSystemInfo
{
	// �^�X�N�̏��������s�����ǂ����̃t���O
	bool                Run;

	// ��Ԑ��ڏ����̃^�X�N�̃��X�g�̊e�D�揇�ʂ̐擪�^�X�N�ւ̃|�C���^
	STaskInfo *         StepTask[TASK_PRIORITY_MAX];

	// �`�揈���̃^�X�N�̃��X�g�̊e�D�揇�ʂ̐擪�^�X�N�ւ̃|�C���^
	STaskInfo *         RenderTask[TASK_PRIORITY_MAX];

	// �ǉ�����^�X�N�̃��X�g�̐擪�^�X�N�ւ̃|�C���^
	STaskInfo *         AddTaskList;

	// �폜����^�X�N�̃��X�g�̐擪�^�X�N�ւ̃|�C���^
	STaskInfo *         DelTaskList;
} STaskSystemInfo;

// �^�X�N�V�X�e���̏�����
extern void TaskSystem_Initialize(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
);

// �^�X�N�V�X�e���̌�n��
extern void TaskSystem_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
);

// �^�X�N�V�X�e���̏�Ԑ��ڏ��������s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool TaskSystem_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �^�X�N�V�X�e���̕`�揈�������s����
extern void TaskSystem_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
);

// �^�X�N�V�X�e���Ƀ^�X�N��ǉ�����
extern void TaskSystem_AddTask(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// �ǉ�����^�X�N�̏��\���̂̃A�h���X
	STaskInfo *TInfo
);

// �^�X�N�V�X�e���ɓo�^�����^�X�N���폜����
extern void TaskSystem_DelTask(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// �폜����^�X�N�̏��\���̂̃A�h���X
	STaskInfo *TInfo
);

#endif
