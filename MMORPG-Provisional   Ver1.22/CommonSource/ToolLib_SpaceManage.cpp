#include "ToolLib_SpaceManage.h"
#include <malloc.h>

// �쐬�ł���X�y�[�X�Ǘ����̍ő吔
#define TOOL_SPACEMANAGE_MAX			(256)

// �X�y�[�X�Ǘ����ɓo�^�ł���X�y�[�X���̍ő吔
#define TOOL_SPACE_MAX					(256)

// �X�y�[�X�Ǘ����\����
typedef struct _SToolSpaceManage
{
	// �X�y�[�X�̐�
	int         SpaceNum;

	// �o�^����Ă���X�y�[�X���ւ̃A�h���X���i�[���Ă���|�C���^�ϐ�
	SToolSpace *Space[ TOOL_SPACE_MAX ];

	// ���̃X�y�[�X�Ɋ��蓖�Ă�X�y�[�X���W
	int         NextSpace;

	// �Œ�Ŋm�ۂ���Ă��鑍�X�y�[�X�T�C�Y
	int         TotalLockSpace;

	// ���X�y�[�X�T�C�Y
	int         TotalSpace;

	// �����Ǝ��̑��X�y�[�X�T�C�Y
	int         InitTotalSpace;
} SToolSpaceManage;

static int               g_TSpaceManageNum;
static SToolSpaceManage *g_TSpaceManage[ TOOL_SPACEMANAGE_MAX ];

// �X�y�[�X�Ǘ������쐬����
//     �߂�l : �X�y�[�X�Ǘ����n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int ToolSpaceManage_Create(
	// ���X�y�[�X�T�C�Y
	int TotalSpace
)
{
	int               NewHandle;
	SToolSpaceManage *TSpaceManage;

	// ���ɃX�y�[�X�Ǘ���񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TSpaceManageNum == TOOL_SPACEMANAGE_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TSpaceManage[ NewHandle ]; NewHandle++ ){}

	// �V�����X�y�[�X�Ǘ������i�[���邽�߂̃������̊m��
	TSpaceManage = ( SToolSpaceManage * )calloc( 1, sizeof( SToolSpaceManage ) );
	if( TSpaceManage == 0 )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TSpaceManage[ NewHandle ] = TSpaceManage;

	// �X�y�[�X�Ǘ����̐��𑝂₷
	g_TSpaceManageNum++; 

	// ����������
	TSpaceManage->SpaceNum       = 0;
	TSpaceManage->NextSpace      = 0;
	TSpaceManage->TotalLockSpace = 0;
	TSpaceManage->TotalSpace     = TotalSpace;
	TSpaceManage->InitTotalSpace = TotalSpace;

	// �n���h����Ԃ�
	return NewHandle;
}

// �X�y�[�X�Ǘ������폜����
void ToolSpaceManage_Delete(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	// �m�ۂ��Ă������������������
	free( TSpaceManage );
	g_TSpaceManage[ TSpaceManageHandle ] = 0;

	// �X�y�[�X�Ǘ����̐������炷
	g_TSpaceManageNum--;
}


// �S�ẴX�y�[�X�Ǘ������폜����
void ToolSpaceManage_Delete_All( void )
{
	int i;

	// �L���ȑS�ẴX�y�[�X�Ǘ����폜����
	for( i = 0; i < TOOL_SPACEMANAGE_MAX; i++ )
	{
		if( g_TSpaceManage[ i ] != 0 )
		{
			ToolSpaceManage_Delete( i );
		}
	}
}

// �X�y�[�X�Ǘ����ɃX�y�[�X����ǉ�����
//     �߂�l : ���̒ǉ��������������ǂ���( true:����  false:���s )
bool ToolSpaceManage_AddSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle,

	// �X�y�[�X���\���̂̃A�h���X
	SToolSpace *Space,
	
	// �g�p����X�y�[�X�T�C�Y�A-1���w�肷��Ǝc��̃X�y�[�X�S�Ă����蓖�Ă�
	int UseSpace,
	
	// �X�y�[�X���Œ�^�C�v���ǂ���( true:�Œ�^�C�v  false:�ʏ�^�C�v )
	bool LockSpace
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	// �o�^�ł���X�y�[�X���̍ő吔�ɒB���Ă����ꍇ�̓G���[
	if( TSpaceManage->SpaceNum == TOOL_SPACE_MAX )
	{
		return false;
	}

	// �g�p����X�y�[�X�T�C�Y�� -1 �������ꍇ�͎c��̑S�ẴX�y�[�X�����蓖�Ă�
	if( UseSpace == -1 )
	{
		UseSpace = TSpaceManage->TotalSpace - TSpaceManage->NextSpace;
	}

	// ����������
	Space->LockSpace    = LockSpace;
	Space->UseSpace     = UseSpace;
	Space->InitUseSpace = UseSpace;
	Space->SpaceStart   = TSpaceManage->NextSpace;

	// ���̃X�y�[�X�Ɋ��蓖�Ă�X�y�[�X�J�n�ʒu���g�p�����X�y�[�X�������ړ�����
	TSpaceManage->NextSpace += UseSpace;

	// �Œ�^�X�y�[�X�̏ꍇ�͌Œ�^�X�y�[�X�̑��T�C�Y�ɒǉ�����X�y�[�X�T�C�Y�����Z����
	if( LockSpace )
	{
		TSpaceManage->TotalLockSpace += UseSpace;
	}

	// �X�y�[�X�Ǘ����ɃX�y�[�X����ǉ�����
	TSpaceManage->Space[ TSpaceManage->SpaceNum ] = Space;
	TSpaceManage->SpaceNum ++;

	// ����I��
	return true;
}

// ���X�y�[�X�T�C�Y��ύX����
void ToolSpaceManage_ChangeTotalSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle,
	
	// ���X�y�[�X�T�C�Y
	int TotalSpace
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];
	int               FreeSpace;
	int               NextFreeSpace;
	int               i;
	int               SpaceStart;
	SToolSpace       *Space;

	// ���܂ł̑��X�y�[�X�T�C�Y�ł̌Œ�^�ł͂Ȃ��X�y�[�X�̃T�C�Y���Z�o
	FreeSpace     = TSpaceManage->InitTotalSpace - TSpaceManage->TotalLockSpace;

	// �V�������X�y�[�X�T�C�Y�ł̌Œ�^�ł͂Ȃ��X�y�[�X�̃T�C�Y���Z�o
	NextFreeSpace = TotalSpace - TSpaceManage->TotalLockSpace;

	// �X�y�[�X�̐������J��Ԃ�
	SpaceStart = 0;
	for( i = 0; i < TSpaceManage->SpaceNum; i ++ )
	{
		Space = TSpaceManage->Space[ i ];

		// �X�y�[�X�J�n�ʒu�̃Z�b�g
		Space->SpaceStart = SpaceStart;

		// �Œ�^�X�y�[�X�ł͂Ȃ��ꍇ�͑��X�y�[�X�T�C�Y�ɑ΂��銄������
		// �V�����X�y�[�X�T�C�Y�����肷��
		if( !Space->LockSpace )
		{
			Space->UseSpace = Space->InitUseSpace * NextFreeSpace / FreeSpace;
		}

		// �X�y�[�X�J�n�ʒu���ړ�����
		SpaceStart += Space->UseSpace;
	}

	// �V�������X�y�[�X�T�C�Y��ۑ�
	TSpaceManage->TotalSpace = TotalSpace;
}

// ���X�y�[�X�T�C�Y���擾����
//     �߂�l : ���X�y�[�X�T�C�Y
int ToolSpaceManage_GetTotalSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	return TSpaceManage->TotalSpace;
}

// ���̃X�y�[�X���Ɋ��蓖�Ă�J�n�ʒu���擾����
//     �߂�l : �X�y�[�X�̊J�n�ʒu
int ToolSpaceManage_GetNextSpace(
	// �X�y�[�X�Ǘ����n���h��
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	return TSpaceManage->NextSpace;
}
