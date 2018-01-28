#include "Task.h"
#include <string.h>
#include <malloc.h>

// �^�X�N���X�g�ւ̒ǉ���폜�𔽉f����
static void TaskSystem_RefreshList(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	STaskInfo *NextTInfo;
	STaskInfo *TempTInfo;

	// �w�ǉ�����^�X�N�̃��X�g�x�ɓo�^����Ă���^�X�N���A�^�X�N���X�g�ɒǉ�����
	if( TSysInfo->AddTaskList != NULL )
	{
		for( TInfo = TSysInfo->AddTaskList; TInfo != NULL; TInfo = NextTInfo )
		{
			// �w�ǉ�����^�X�N�̃��X�g�x�̎��̃^�X�N�̃A�h���X��ێ����Ă���
			NextTInfo = TInfo->AddOrDelNext;

			// ��Ԑ��ڏ����̃^�X�N�̃��X�g�ɒǉ�����
			TInfo->StepNext = TSysInfo->StepTask[ TInfo->Base->StepPriority ];
			TSysInfo->StepTask[ TInfo->Base->StepPriority ] = TInfo;

			// �`�揈���̃^�X�N�̃��X�g�ɒǉ�����
			TInfo->RenderNext = TSysInfo->RenderTask[ TInfo->Base->RenderPriority ];
			TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] = TInfo;

			// �w�ǉ�����^�X�N�̃��X�g�x�̎��̃^�X�N�̃A�h���X������������
			TInfo->AddOrDelNext = NULL;
		}

		// �w�ǉ�����^�X�N�̃��X�g�x�̐擪�^�X�N�ւ̃|�C���^������������
		TSysInfo->AddTaskList = NULL;
	}

	// �w�폜����^�X�N�̃��X�g�x�ɓo�^����Ă���^�X�N���A�^�X�N���X�g����O��
	if( TSysInfo->DelTaskList != NULL )
	{
		for( TInfo = TSysInfo->DelTaskList; TInfo != NULL; TInfo = NextTInfo )
		{
			// �w�폜����^�X�N�̃��X�g�x�̎��̃^�X�N�̃A�h���X��ێ����Ă���
			NextTInfo = TInfo->AddOrDelNext;

			// �O���^�X�N����Ԑ��ڏ����̃^�X�N�̃��X�g�̐擪�����`�F�b�N
			if( TSysInfo->StepTask[ TInfo->Base->StepPriority ] == TInfo )
			{
				// �擪�������ꍇ�͐擪���O���^�X�N�̎��̃^�X�N�ɂ���
				TSysInfo->StepTask[ TInfo->Base->StepPriority ] = TInfo->StepNext;
			}
			else
			{
				// ��Ԑ��ڏ����̃^�X�N�̃��X�g�̐擪�ł͂Ȃ�������A
				// ��Ԑ��ڏ����̃^�X�N�̃��X�g����O���^�X�N������
				TempTInfo = TSysInfo->StepTask[ TInfo->Base->StepPriority ];
				while( true )
				{
					// ���X�g�̏I�[�ɗ��Ă��܂����ꍇ�̓��[�v�𔲂���
					if( TempTInfo->StepNext == NULL )
					{
						break;
					}

					// �폜�Ώۂ̃^�X�N�𔭌������ꍇ�́A���X�g����O���ă��[�v���甲����
					if( TempTInfo->StepNext == TInfo )
					{
						TempTInfo->StepNext = TInfo->StepNext;
						break;
					}

					TempTInfo = TempTInfo->StepNext;
				}
			}

			// �O���^�X�N���`�揈���̃^�X�N�̃��X�g�̐擪�����`�F�b�N
			if( TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] == TInfo )
			{
				// �擪�������ꍇ�͐擪���O���^�X�N�̎��̃^�X�N�ɂ���
				TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] = TInfo->RenderNext;
			}
			else
			{
				// �`�揈���̃^�X�N�̃��X�g�̐擪�ł͂Ȃ�������A
				// �`�揈���̃^�X�N�̃��X�g����O���^�X�N������
				TempTInfo = TSysInfo->RenderTask[ TInfo->Base->RenderPriority ];
				for(;;)
				{
					// ���X�g�̏I�[�ɗ��Ă��܂����ꍇ�̓��[�v�𔲂���
					if( TempTInfo->RenderNext == NULL )
					{
						break;
					}

					// �폜�Ώۂ̃^�X�N�𔭌������ꍇ�́A���X�g����O���ă��[�v���甲����
					if( TempTInfo->RenderNext == TInfo )
					{
						TempTInfo->RenderNext = TInfo->RenderNext;
						break;
					}

					TempTInfo = TempTInfo->RenderNext;
				}
			}

			// ��n���֐����ݒ肳��Ă���ꍇ�͌�n���֐������s����
			if( TInfo->Base->Terminate != NULL )
			{
				TInfo->Base->Terminate( TInfo );
			}

			// �e���X�g�̎��̃^�X�N�ւ̃|�C���^������������
			TInfo->AddOrDelNext = NULL;
			TInfo->StepNext     = NULL;
			TInfo->RenderNext   = NULL;

			// �f�[�^�i�[�p�Ƀ��������m�ۂ��Ă����ꍇ�͉������
			if( TInfo->Data != NULL )
			{
				free( TInfo->Data );
			}
		}

		// �w�폜����^�X�N�̃��X�g�x�̐擪�^�X�N�ւ̃|�C���^������������
		TSysInfo->DelTaskList = NULL;
	}
}

// �^�X�N�V�X�e���̏�����
void TaskSystem_Initialize(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
)
{
	// �\���̑S�̂��[������������
	memset( TSysInfo, 0, sizeof( STaskSystemInfo ) );
}

// �^�X�N�V�X�e���̌�n��
void TaskSystem_Terminate(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	int        i;

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���x�ɂ��Ă���
	TSysInfo->Run = true;

	// ���s�D�揇�ʂ̐������J��Ԃ�
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// ���s�D�揇�� i �ɓo�^����Ă���^�X�N��S�č폜����
		for( TInfo = TSysInfo->StepTask[ i ]; TInfo != NULL; TInfo = TInfo->StepNext )
		{
			TaskSystem_DelTask( TSysInfo, TInfo );
		}
	}

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���ł͂Ȃ��x�ɂ��Ă���
	TSysInfo->Run = false;

	// �폜�𔽉f����
	TaskSystem_RefreshList( TSysInfo );
}

// �^�X�N�V�X�e���̏�Ԑ��ڏ��������s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool TaskSystem_Step(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	STaskInfo *TInfo;
	int        i;

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���x�ɂ��Ă���
	TSysInfo->Run = true;

	// ���s�D�揇�ʂ̐������J��Ԃ�
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// ���s�D�揇�� i �ɓo�^����Ă���S�Ẵ^�X�N�̏�Ԑ��ڏ��������s����
		for( TInfo = TSysInfo->StepTask[ i ]; TInfo != NULL; TInfo = TInfo->StepNext )
		{
			if( !TInfo->Base->Step( TInfo, StepTime ) )
			{
				return false;
			}
		}
	}

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���ł͂Ȃ��x�ɂ��Ă���
	TSysInfo->Run = false;

	// �^�X�N�̏��������s���Ă���Ԃɍs��ꂽ�^�X�N�̓o�^��폜�����X�g�֔��f����
	TaskSystem_RefreshList( TSysInfo );

	// ����I��
	return true;
}

// �^�X�N�V�X�e���̕`�揈�������s����
void TaskSystem_Render(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	int        i;

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���x�ɂ��Ă���
	TSysInfo->Run = true;

	// ���s�D�揇�ʂ̐������J��Ԃ�
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// ���s�D�揇�� i �ɓo�^����Ă���S�Ẵ^�X�N�̕`�揈�������s����
		for( TInfo = TSysInfo->RenderTask[ i ]; TInfo != NULL; TInfo = TInfo->RenderNext)
		{
			// �w�폜����^�X�N�̃��X�g�x�ɓo�^����Ă���ꍇ�͕`�揈�����s��Ȃ�
			if( TInfo->AddOrDelNext != NULL )
			{
				continue;
			}

			TInfo->Base->Render( TInfo );
		}
	}

	// �^�X�N�̏��������s���Ă��邩�ǂ����̃t���O���w���s���ł͂Ȃ��x�ɂ��Ă���
	TSysInfo->Run = false;

	// �^�X�N�̏��������s���Ă���Ԃɍs��ꂽ�^�X�N�̓o�^��폜�����X�g�֔��f����
	TaskSystem_RefreshList( TSysInfo );
}

// �^�X�N�V�X�e���Ƀ^�X�N��ǉ�����
void TaskSystem_AddTask(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// �ǉ�����^�X�N�̏��\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	// �^�X�N�V�X�e�����́w�ǉ�����^�X�N�̃��X�g�x�Ƀ^�X�N��ǉ�����
	TInfo->AddOrDelNext   = TSysInfo->AddTaskList;
	TSysInfo->AddTaskList = TInfo;

	// �^�X�N�̏��������s���ł͂Ȃ�������A�����Ƀ^�X�N���X�g�ɔ��f����
	if( !TSysInfo->Run )
	{
		TaskSystem_RefreshList( TSysInfo );
	}
}

// �^�X�N�V�X�e���ɓo�^�����^�X�N���폜����
void TaskSystem_DelTask(
	// �^�X�N�V�X�e�����\���̂̃A�h���X
	STaskSystemInfo *TSysInfo,

	// �폜����^�X�N�̏��\���̂̃A�h���X
	STaskInfo *TInfo
)
{
	STaskInfo *TempTInfo;

	// �w�ǉ�����^�X�N�̃��X�g�x�ɒǉ�����Ă���ꍇ�́A
	// �w�ǉ�����^�X�N�̃��X�g�x����^�X�N���O��
	if( TInfo->AddOrDelNext != NULL )
	{
		// �O���^�X�N���w�ǉ�����^�X�N�̃��X�g�x�̐擪�����`�F�b�N
		if( TSysInfo->AddTaskList == TInfo )
		{
			// �擪�������ꍇ�͐擪���O���^�X�N�̎��̃^�X�N�ɂ���
			TSysInfo->AddTaskList = TInfo->AddOrDelNext;
		}
		else
		{
			// �w�ǉ�����^�X�N�̃��X�g�x�̐擪�ł͂Ȃ�������A
			// �w�ǉ�����^�X�N�̃��X�g�x����O���^�X�N������
			TempTInfo = TSysInfo->AddTaskList;
			while( true )
			{
				// ���X�g�̏I�[�ɗ��Ă��܂����ꍇ�́w�ǉ�����^�X�N�̃��X�g�x��
				// �ǉ�����Ă����̂ł͂Ȃ��w�폜����^�X�N�̃��X�g�x�ɒǉ�����Ă���
				// �Ƃ������ƂȂ̂ŁA�֐����甲����
				if( TempTInfo->AddOrDelNext == NULL )
				{
					return;
				}

				// �폜�Ώۂ̃^�X�N�𔭌������ꍇ�́A���X�g����O���ă��[�v���甲����
				if( TempTInfo->AddOrDelNext == TInfo )
				{
					TempTInfo->AddOrDelNext = TInfo->AddOrDelNext;
					break;
				}

				TempTInfo = TempTInfo->AddOrDelNext;
			}
		}

		// ��n���֐����ݒ肳��Ă���ꍇ�͌�n���֐������s����
		if( TInfo->Base->Terminate != NULL )
		{
			TInfo->Base->Terminate( TInfo );
		}

		// �w�ǉ�����^�X�N�̃��X�g�x�̎��̃^�X�N�ւ̃|�C���^������������
		TInfo->AddOrDelNext = NULL;

		// �f�[�^�i�[�p�Ƀ��������m�ۂ��Ă����ꍇ�͉������
		if( TInfo->Data != NULL )
		{
			free( TInfo->Data );
		}
	}
	else
	{
		// �^�X�N�V�X�e���Ɋ��ɓo�^����Ă���ꍇ��
		// �w�폜����^�X�N�̃��X�g�x�ɒǉ�����
		TInfo->AddOrDelNext   = TSysInfo->DelTaskList;
		TSysInfo->DelTaskList = TInfo;

		// �^�X�N�̏��������s���ł͂Ȃ�������A�����Ƀ^�X�N���X�g�ɔ��f����
		if( !TSysInfo->Run )
		{
			TaskSystem_RefreshList( TSysInfo );
		}
	}
}

