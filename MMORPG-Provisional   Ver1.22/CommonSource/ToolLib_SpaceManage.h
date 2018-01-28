#ifndef TOOLLIB_SPACEMANAGE_H
#define TOOLLIB_SPACEMANAGE_H

// �X�y�[�X���\����
typedef struct _SToolSpace
{
	// �X�y�[�X���Œ�^�C�v���ǂ���( true:�Œ�^�C�v  false:�ʏ�^�C�v )
	bool LockSpace;

	// �g�p����X�y�[�X�T�C�Y
	int  UseSpace;

	// �g�p����X�y�[�X�̊J�n�ʒu
	int  SpaceStart;

	// ���������̃X�y�[�X�T�C�Y
	int  InitUseSpace;
} SToolSpace;

// �X�y�[�X�Ǘ������쐬����
//     �߂�l : �X�y�[�X�Ǘ����n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolSpaceManage_Create(
	// ���X�y�[�X�T�C�Y
	int TotalSpace
);

// �X�y�[�X�Ǘ������폜����
extern void ToolSpaceManage_Delete(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
);

// �S�ẴX�y�[�X�Ǘ������폜����
extern void ToolSpaceManage_Delete_All( void );

// �X�y�[�X�Ǘ����ɃX�y�[�X����ǉ�����
//     �߂�l : ���̒ǉ��������������ǂ���( true:����  false:���s )
extern bool ToolSpaceManage_AddSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle,

	// �X�y�[�X���\���̂̃A�h���X
	SToolSpace *Space,
	
	// �g�p����X�y�[�X�T�C�Y�A-1���w�肷��Ǝc��̃X�y�[�X�S�Ă����蓖�Ă�
	int UseSpace,
	
	// �X�y�[�X���Œ�^�C�v���ǂ���( true:�Œ�^�C�v  false:�ʏ�^�C�v )
	bool LockSpace
);

// ���X�y�[�X�T�C�Y��ύX����
extern void ToolSpaceManage_ChangeTotalSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle,
	
	// ���X�y�[�X�T�C�Y
	int TotalSpace
);

// ���X�y�[�X�T�C�Y���擾����
//     �߂�l : ���X�y�[�X�T�C�Y
extern int ToolSpaceManage_GetTotalSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
);

// ���̃X�y�[�X���Ɋ��蓖�Ă�J�n�ʒu���擾����
//     �߂�l : �X�y�[�X�̊J�n�ʒu
extern int ToolSpaceManage_GetNextSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
);

#endif
