#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_SpaceManage.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include <string.h>

// �쐬�ł���E�C���h�E���̍ő吔
#define TOOL_WINDOW_MAX				(256)

// �T�C�Y�ύX�\�ȕ\���̈���g�p����ꍇ�̕\���̈�E�[����󂯂�X�y�[�X�̃T�C�Y
#define DRAWAREA_BASERIGHT_SPACE	(20)

// �X�N���[���o�[����p�̃{�^���̃T�C�Y
#define SCROLLBAR_BUTTON_SIZE		(8)

// �X�N���[���o�[�̕�
#define SCROLLBAR_WIDTH				(8)

// �X�N���[���o�[���{�^���������đ��삷��ꍇ�̈ړ����x
#define SCROLLBAR_SPEED				(180)

// �E�C���h�E�\����
typedef struct _SToolWindow
{
	// �X�y�[�X�Ǘ����̃n���h��
	int               TSpaceManageHandle;

	// �\����ԏ��̃n���h��
	int               TVisibleHandle;

	// �{�^���̖��O�̕����񂪊i�[����Ă��郁�����A�h���X��ۑ����Ă���|�C���^
	char             *Name;

	// �E�C���h�E�̏��\���̈�
	RECT              InfoArea;

	// �E�C���h�E�̃N���C�A���g�̈�̃T�C�Y
	SIZE              ClientSize;

	// �E�C���h�E�̃N���C�A���g�̈�̓��e��`�悷��ۂ̊�_�ƂȂ鍶����W
	// ( �X�N���[���o�[�ɂ��E�C���h�E�����̕\���ʒu���ύX����鏈���Ɏg�p )
	int               DrawLeftUpX;
	int               DrawLeftUpY;

	// �\���ʒu�����肷��X�N���[�����W
	int               HScroll;
	int               VScroll;

	// �X�N���[���o�[��͂�ł��邩�ǂ���
	bool              HScrollGrips;
	bool              VScrollGrips;

	// �\�����
	bool              Visible;

	// �Ō�Ƀ{�^�������X�N���[������������
	int               LastButtonBottomTime;

	// ���ꂼ��㉺���E�X�N���[���{�^���������ꂽ��Ԃ��ǂ���
	// ( true:������Ă���  false:������Ă��Ȃ�)
	bool              UpButBottom;
	bool              DownButBottom;
	bool              LeftButBottom;
	bool              RightButBottom;

	// ���ꂼ��㉺���E�X�N���[���{�^���̋�`���Ƀ}�E�X�J�[�\�������邩�ǂ���
	bool              UpButIn;
	bool              DownButIn;
	bool              LeftButIn;
	bool              RightButIn;

	// ���O�\�������̃X�y�[�X���
	SToolSpace        NameSpace;

	// ���\�������̃X�y�[�X���
	SToolSpace        InfoSpace;

	// �����X�N���[���o�[�̃X�y�[�X���
	SToolSpace        HScrollBarSpace;
} SToolWindow;

static int          g_TWindowNum;
static SToolWindow *g_TWindow[ TOOL_WINDOW_MAX ];

// �X�y�[�X�����g�p���Ă���ꍇ�ɃX�y�[�X���Ɋ�Â������\���̈�̍X�V���s��
static void ToolWindow_UpdateInfoArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �E�C���h�E���̏�Ԑ��ڏ������s��
static void ToolWindow_Step(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �i�߂鎞��( �P�� : �b )
	float StepTime
);

// �E�C���h�E������ʂɕ`�悷��
static void ToolWindow_Draw(
	// �E�C���h�E���n���h��
	int TWindowHandle
);


// �E�C���h�E�����쐬����
//     �߂�l : �E�C���h�E���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
int  ToolWindow_Create(
	// �X�y�[�X�Ǘ����n���h��( �K�v�̂Ȃ��ꍇ�� -1 ��n�� )
	int TSpaceManageHandle,
	
	// �c��̃X�y�[�X��S�Ďg�p���邩�ǂ���( true ��n���ƑS�Ďg�p���� )
	// ( TSpaceManageHandle �� -1 �ȊO�̏ꍇ�̂ݗL�� )
	bool SpaceLast,

	// �Œ�^�C�v�̃X�y�[�X���ǂ���
	// ( TSpaceManageHandle �� -1 �ȊO�̏ꍇ�̂ݗL�� )
	bool LockSpace,
	
	// �����X�N���[���o�[�p�̃X�y�[�X���m�ۂ��邩�ǂ���
	// ( TSpaceManageHandle �� -1 �ȊO�̏ꍇ�̂ݗL�� )
	bool HScrollSpace,

	// �E�C���h�E�̖��O
	const char *Name,

	// �E�C���h�E�̍��[���W
	int DrawX,
	
	// �E�C���h�E�̍��[Y���W
	// ( TSpaceManageHandle �� -1 �̏ꍇ�̂ݗL�� )
	int DrawY,

	// �E�C���h�E�̕�
	// ( TSpaceManageHandle �� -1 �̏ꍇ�̂ݗL�� )
	int DrawW,
	
	// �E�C���h�E�̍���
	// ( TSpaceManageHandle �� -1 �̏ꍇ�̂ݗL�� )
	int DrawH,
	
	// �E�C���h�E�̃N���C�A���g�̈�̕�
	int ClientW,
	
	// �E�C���h�E�̃N���C�A���g�̈�̍���
	int ClientH
)
{
	int          NewHandle;
	SToolWindow *TWindow;

	// ���ɃE�C���h�E��񂪍ő吔�ɒB���Ă���ꍇ�̓G���[�I��
	if( g_TWindowNum == TOOL_WINDOW_MAX )
	{
		return -1;
	}

	// �g�p����Ă��Ȃ��z��v�f��T��
	for( NewHandle = 0; g_TWindow[ NewHandle ]; NewHandle++ ){}

	// �V�����E�C���h�E�����i�[���邽�߂̃������̊m��
	TWindow = ( SToolWindow * )calloc( 1, sizeof( SToolWindow ) );
	if( TWindow == NULL )
	{
		return -1;
	}

	// �m�ۂ����������̈��ۑ�
	g_TWindow[ NewHandle ] = TWindow;

	// �E�C���h�E���̐��𑝂₷
	g_TWindowNum++; 

	// �X�y�[�X�Ǘ����n���h����ۑ�
	TWindow->TSpaceManageHandle = TSpaceManageHandle;

	// �X�y�[�X�Ǘ����n���h�����L���ȏꍇ�̓E�C���h�E�̃X�y�[�X����ǉ�����
	if( TSpaceManageHandle >= 0 )
	{
		// ���O�\�������̃X�y�[�X��ǉ�
		if( !ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->NameSpace,
				TOOL_WINDOW_NAME_HEIGHT,
				true ) )
		{
			return -1;
		}

		// ���\���̈�̃X�y�[�X��ǉ�
		if( !ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->InfoSpace,
				SpaceLast ?
					ToolSpaceManage_GetTotalSpace( TSpaceManageHandle ) - 
					ToolSpaceManage_GetNextSpace(  TSpaceManageHandle ) -
					TOOL_SCROLLBAR_WIDTH : DrawH,
				LockSpace ) )
		{
			return -1;
		}

		// �����X�N���[���o�[�p�̃X�y�[�X���m�ۂ���w�肪����ꍇ��
		// �����X�N���[���o�[�p�̃X�y�[�X��ǉ�����
		if( HScrollSpace )
		{
			ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->HScrollBarSpace,
				TOOL_SCROLLBAR_WIDTH,
				true
			);
		}
	}

	// ����������
	TWindow->TVisibleHandle  = -1;

	TWindow->InfoArea.left    = DrawX;
	TWindow->InfoArea.right   = DrawX + DrawW;
	if( TWindow->TSpaceManageHandle >= 0 )
	{
		TWindow->InfoArea.top = TWindow->InfoSpace.SpaceStart;
	}
	else
	{
		TWindow->InfoArea.top = DrawY;
	}
	TWindow->InfoArea.bottom  = TWindow->InfoArea.top + DrawH;
	TWindow->ClientSize.cx    = ClientW;
	TWindow->ClientSize.cy    = ClientH;

	if( !ToolWindow_SetName( NewHandle, Name ) )
	{
		return -1;
	}

	TWindow->DrawLeftUpX      = 0;
	TWindow->DrawLeftUpY      = 0;
	TWindow->HScroll          = 0;
	TWindow->VScroll          = 0;
	TWindow->Visible          = true;

	// �n���h����Ԃ�
	return NewHandle;
}

// �E�C���h�E�����폜����
void ToolWindow_Delete(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// ���O�ۑ��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
	if( TWindow->Name )
	{
		free( TWindow->Name );
		TWindow->Name = NULL;
	}

	// �m�ۂ��Ă������������������
	free( TWindow );
	g_TWindow[ TWindowHandle ] = NULL;

	// �E�C���h�E���̐������炷
	g_TWindowNum--;
}

// �S�ẴE�C���h�E�����폜����
void ToolWindow_Delete_All( void )
{
	int i;

	// �L���ȑS�ẴE�C���h�E���폜����
	for( i = 0; i < TOOL_WINDOW_MAX; i++ )
	{
		if( g_TWindow[ i ] != NULL )
		{
			ToolWindow_Delete( i );
		}
	}
}

// �E�C���h�E���Ɋ֘A�t����\����ԏ����Z�b�g����
void ToolWindow_SetVisibleHandle( 
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// �\����ԏ��n���h��
	int TVisibleHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	TWindow->TVisibleHandle = TVisibleHandle;
}

// �X�y�[�X�����g�p���Ă���ꍇ�ɃX�y�[�X���Ɋ�Â������\���̈�̍X�V���s��
static void ToolWindow_UpdateInfoArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �X�y�[�X�Ǘ����n���h���������ȏꍇ�͉������Ȃ�
	if( TWindow->TSpaceManageHandle < 0 )
	{
		return;
	}

	// ���\���̈���X�y�[�X���Ɋ�Â��ĎZ�o
	TWindow->InfoArea.right  = Tool_GetBaseRightX() - DRAWAREA_BASERIGHT_SPACE;
	TWindow->InfoArea.top    = TWindow->InfoSpace.SpaceStart;
	TWindow->InfoArea.bottom = TWindow->InfoArea.top + TWindow->InfoSpace.UseSpace;
}

// �E�C���h�E�������� SetDrawArea �ŕ`��̈�𐧌�����ݒ���s��
void ToolWindow_SetupDrawArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// SetDrawArea ���g�p���Ď��ۂɕ`��̈�̐����ݒ���s��
	SetDrawArea(
		TWindow->InfoArea.left,
		TWindow->InfoArea.top,
		TWindow->InfoArea.right,
		TWindow->InfoArea.bottom
	);
}

// �E�C���h�E���̏��\���̈���擾����
//     �߂�l : �`��̈�̋�`
RECT ToolWindow_GetInfoArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	return TWindow->InfoArea;
}

// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
// �N���C�A���g�̈�̕`�挴�_�ƂȂ�X�N���[�����W���擾����
void ToolWindow_GetDrawLeftUpPos(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// ���W��������ϐ��̃A�h���X
	int *x,
	int *y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	if( x )
	{
		*x = -TWindow->DrawLeftUpX + TWindow->InfoArea.left;
	}

	if( y )
	{
		*y = -TWindow->DrawLeftUpY + TWindow->InfoArea.top;
	}
}

// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
// �N���C�A���g�̈�̕`�挴�_�ƂȂ���W���擾����
void ToolWindow_GetClientArea(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// ���W��������ϐ��̃A�h���X
	int *x,
	int *y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	if( x )
	{
		*x = TWindow->DrawLeftUpX;
	}

	if( y )
	{
		*y = TWindow->DrawLeftUpY;
	}
}

// �E�C���h�E���̃N���C�A���g�̈�̃T�C�Y��ݒ肷��
void ToolWindow_SetClientSize(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �V�����N���C�A���g�̈�̃T�C�Y
	int w,
	int h
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          ScrollW;
	int          ScrollH;

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// ���݂̃N���C�A���g�̈�̃T�C�Y�Ɠ����ꍇ�͉��������ɏI��
	if( TWindow->ClientSize.cx == w && TWindow->ClientSize.cy == h ) 
	{
		return;
	}

	// �V�����N���C�A���g�̈�̃T�C�Y��ۑ�
	TWindow->ClientSize.cx = w;
	TWindow->ClientSize.cy = h;

	// ���\���̈�̕��ƍ������Z�o
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// �X�N���[���o�[�̐����ړ��͈͂Ɛ����ړ��͈͂��Z�o
	ScrollW = InfoAreaW - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );
	ScrollH = InfoAreaH - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );

	// �V�����N���C�A���g�̈�̃T�C�Y��
	// ���\���̈��苷���ꍇ�̓X�N���[���o�[�̈ʒu�𒲐�
	if( TWindow->ClientSize.cx > InfoAreaW )
	{
		if( TWindow->DrawLeftUpX + InfoAreaW > TWindow->ClientSize.cx )
		{
			TWindow->DrawLeftUpX = TWindow->ClientSize.cx - InfoAreaW;
		}

		TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
								( TWindow->ClientSize.cx - InfoAreaW );
	}
	else
	{
		TWindow->DrawLeftUpX = 0;
		TWindow->HScroll     = 0;
	}

	if( TWindow->ClientSize.cy > InfoAreaH )
	{
		if( TWindow->DrawLeftUpY + InfoAreaH > TWindow->ClientSize.cy )
		{
			TWindow->DrawLeftUpY = TWindow->ClientSize.cy - InfoAreaH;
		}
		TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
								( TWindow->ClientSize.cy - InfoAreaH );
	}
	else
	{
		TWindow->DrawLeftUpY = 0;
		TWindow->VScroll     = 0;
	}
}

// �E�C���h�E���̖��O��ݒ肷��
//     �߂�l : ����������ɏI���������ǂ���( true:����ɏI������  false:�G���[���� )
bool ToolWindow_SetName(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// ���O�ƂȂ镶����̃A�h���X
	const char *Name
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// ���ɖ��O�p�Ƀ��������m�ۂ��Ă����ꍇ�͉������
	if( TWindow->Name )
	{
		free( TWindow->Name );
		TWindow->Name = NULL;
	}

	// ���O�̃A�h���X���L���ȏꍇ�̓��������m�ۂ��ĕۑ�����
	if( Name != NULL )
	{
		TWindow->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TWindow->Name == NULL )
		{
			return false;
		}
		strcpy( TWindow->Name, Name );
	}

	// ����I��
	return true;
}

// �E�C���h�E���̕\����Ԃ�ύX����
void ToolWindow_SetVisible(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �V�����\�����
	bool Visible
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �V�����\����Ԃ�ۑ�����
	TWindow->Visible = Visible;
}

// �E�C���h�E���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
bool ToolWindow_GetVisible(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// �\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( !TWindow->Visible )
	{
		return false;
	}

	// �\����ԏ��n���h�����o�^����Ă��āA
	// ���\����ԏ��n���h���̕\����Ԃ� false �̏ꍇ�� false ��Ԃ�
	if( TWindow->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TWindow->TVisibleHandle ) )
	{
		return false;
	}

	// �����ɂ��Ă������\���ł͂Ȃ��Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// �w��̍��W���E�C���h�E�̏��\���̈�ɓ����Ă��邩�ǂ����𒲂ׂ�
//     �߂�l : �w��̍��W���E�C���h�E�̕`��̈�ɓ����Ă��邩�ǂ���
//              ( true:�����Ă���   false:�����Ă��Ȃ� )
bool ToolWindow_CheckInfoAreaIn(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// �E�C���h�E�̕`��̈�ɓ����Ă��邩�ǂ������ׂ���W
	int x,
	int y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	return TWindow->InfoArea.left <= x && TWindow->InfoArea.right  >= x &&
	       TWindow->InfoArea.top  <= y && TWindow->InfoArea.bottom >= y;
}

// �E�C���h�E���̏�Ԑ��ڏ������s��
static void ToolWindow_Step(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �i�߂鎞��( �P�� : �b )
	float StepTime
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          ScrollH;
	int          ScrollW;
	int          DeltaTime;
	int          ScrollSize;
	bool         SizeOverX;
	bool         SizeOverY;
	int          MouseInput;
	int          MouseEdgeInput;
	int          MouseX;
	int          MouseY;
	int          MouseMoveX;
	int          MouseMoveY;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolWindow_GetVisible( TWindowHandle ) )
	{
		return;
	}

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// �}�E�X�J�[�\���̈ʒu���擾
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// �}�E�X�J�[�\���̈ړ��ʂ��擾
	ToolStatic_GetMouseMoveVecter( &MouseMoveX, &MouseMoveY );

	// �}�E�X�̓��͏�Ԃ��擾
	MouseInput     = ToolStatic_GetMouseButton();
	MouseEdgeInput = ToolStatic_GetMouseEdgeButton();

	// ���\���̈�̕��ƍ������Z�o
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// �N���C�A���g�̈�̃T�C�Y�����\���̈�̃T�C�Y���傫�����ǂ����𒲂ׂĂ���
	SizeOverX = InfoAreaW < TWindow->ClientSize.cx;
	SizeOverY = InfoAreaH < TWindow->ClientSize.cy;

	// �X�N���[���o�[�̐����ړ��͈͂Ɛ����ړ��͈͂��Z�o
	ScrollW = InfoAreaW - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );
	ScrollH = InfoAreaH - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );

	// �X�N���[���o�[�̏㉺���E�{�^���̕\���̈�̒��ɃJ�[�\�������邩�ǂ����𒲂ׂ�
	TWindow->UpButIn =
		SizeOverY &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE
		);

	TWindow->DownButIn = 
		SizeOverY &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom
		);

	TWindow->LeftButIn = 
		SizeOverX &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.left,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH
		);

	TWindow->RightButIn = 
		SizeOverX &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH
		);

	// �O���Ԑ��ڏ������s���Ă���̌o�ߎ��Ԃ��Z�o����
	DeltaTime                     = GetNowCount() - TWindow->LastButtonBottomTime;

	// ���݂̎��Ԃ�ۑ����Ă���
	TWindow->LastButtonBottomTime = GetNowCount();

	// �o�ߎ��Ԃ���X�N���[���o�[���ړ�����ꍇ�̈ړ��ʂ��Z�o����
	ScrollSize                    = DeltaTime * SCROLLBAR_SPEED / 1000;
	if( ScrollSize == 0 )
	{
		ScrollSize = SCROLLBAR_SPEED / 60;
	}

	// �}�E�X�̍��{�^����������Ă��Ȃ��ꍇ�̓X�N���[���֌W�̃t���O��S�ď��������ďI��
	if( ( MouseInput & MOUSE_INPUT_LEFT ) == 0 )
	{
		TWindow->HScrollGrips   = false;
		TWindow->VScrollGrips   = false;
		TWindow->RightButBottom = false;
		TWindow->LeftButBottom  = false;
		TWindow->UpButBottom    = false;
		TWindow->DownButBottom  = false;
		return;
	}

	// �N���C�A���g�̈�̉��������\���̈�̉������傫���ꍇ��
	// �������̃X�N���[���o�[�̏������s��
	if( SizeOverX )
	{
		// ���{�^���������ꂽ�u�ԂŁA���X�N���[���{�^���̕\���̈���Ƀ}�E�X�J�[�\����
		// �������ꍇ�̓X�N���[���{�^���������Ă��邩�ǂ����̃t���O�𗧂Ă�
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
		{
			if( TWindow->LeftButIn  )
			{
				TWindow->LeftButBottom  = true;
			}

			if( TWindow->RightButIn )
			{
				TWindow->RightButBottom = true;
			}
		}

		// �X�N���[���{�^����������Ă����ꍇ�̓X�N���[���ʒu���ړ�����
		if( TWindow->LeftButIn || TWindow->RightButIn )
		{
			if( TWindow->LeftButBottom )
			{
				TWindow->DrawLeftUpX -= ScrollSize;
				if( TWindow->DrawLeftUpX < 0 )
				{
					TWindow->DrawLeftUpX = 0;
				}

				TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
										( TWindow->ClientSize.cx - InfoAreaW );
			}
			else
			if( TWindow->RightButBottom )
			{
				TWindow->DrawLeftUpX += ScrollSize;
				if( TWindow->DrawLeftUpX > TWindow->ClientSize.cx - InfoAreaW )
				{
					TWindow->DrawLeftUpX = TWindow->ClientSize.cx - InfoAreaW;
				}

				TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
										( TWindow->ClientSize.cx - InfoAreaW );
			}
		}
		else
		// �X�N���[���o�[�̂܂݂�͂�ł���ꍇ�̓}�E�X�J�[�\���̈ړ��ʂ�
		// ���킹�ăX�N���[���ʒu���ړ�����
		if( TWindow->HScrollGrips )
		{
			TWindow->HScroll += MouseMoveX;
			if( TWindow->HScroll < 0 )
			{
				TWindow->HScroll = 0;
			}

			if( TWindow->HScroll > ScrollW )
			{
				TWindow->HScroll = ScrollW;
			}

			TWindow->DrawLeftUpX = 
				TWindow->HScroll * ( TWindow->ClientSize.cx - InfoAreaW ) / ScrollW;
		}
		else
		// �X�N���[���o�[�̗̈���ɃJ�[�\��������A�����{�^���������ꂽ�u�Ԃ̏ꍇ��
		// �X�N���[���o�[�֌W�̏���������
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			ToolStatic_CheckMouseIn(
				TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.bottom,
				TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH ) )
		{
			// �J�[�\���̊O���N���b�N�����ꍇ�͂P��ʕ��ړ�����
			if( MouseX < TWindow->InfoArea.left + SCROLLBAR_BUTTON_SIZE +
											TWindow->HScroll )
			{
				TWindow->HScroll -= InfoAreaW * ScrollW / TWindow->ClientSize.cx;
				if( TWindow->HScroll < 0 )
				{
					TWindow->HScroll = 0;
				}
			}
			else
			if( MouseX > TWindow->InfoArea.left + SCROLLBAR_BUTTON_SIZE +
											TWindow->HScroll + SCROLLBAR_WIDTH )
			{
				TWindow->HScroll += InfoAreaW * ScrollW / TWindow->ClientSize.cx;
				if( TWindow->HScroll > ScrollW )
				{
					TWindow->HScroll = ScrollW;
				}
			}
			else
			{
				// �܂݂��N���b�N�����ꍇ�͂܂ݒ͂ݏ�Ԃɂ���
				TWindow->HScrollGrips = true;
			}

			// �X�N���[���ʒu����X�N���[���ʒu���l�������`���_�ʒu���Z�o
			TWindow->DrawLeftUpX = TWindow->HScroll *
									( TWindow->ClientSize.cx - InfoAreaW ) / ScrollW;
		}
	}

	// �N���C�A���g�̈�̏c�������\���̈�̏c�����傫���ꍇ��
	// �c�����̃X�N���[���o�[�̏������s��
	if( SizeOverY )
	{
		// ���{�^���������ꂽ�u�ԂŁA���X�N���[���{�^���̕\���̈���Ƀ}�E�X�J�[�\����
		// �������ꍇ�̓X�N���[���{�^���������Ă��邩�ǂ����̃t���O�𗧂Ă�
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
		{
			if( TWindow->UpButIn )
			{
				TWindow->UpButBottom   = true;
			}

			if( TWindow->DownButIn )
			{
				TWindow->DownButBottom = true;
			}
		}

		// �X�N���[���{�^����������Ă����ꍇ�̓X�N���[���ʒu���ړ�����
		if( TWindow->UpButIn || TWindow->DownButIn )
		{
			if( TWindow->UpButBottom )
			{
				TWindow->DrawLeftUpY -= ScrollSize;
				if( TWindow->DrawLeftUpY < 0 )
				{
					TWindow->DrawLeftUpY = 0;
				}

				TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
										( TWindow->ClientSize.cy - InfoAreaH );
			}
			else
			if( TWindow->DownButBottom )
			{
				TWindow->DrawLeftUpY += ScrollSize;
				if( TWindow->DrawLeftUpY > TWindow->ClientSize.cy - InfoAreaH )
				{
					TWindow->DrawLeftUpY = TWindow->ClientSize.cy - InfoAreaH;
				}

				TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
										( TWindow->ClientSize.cy - InfoAreaH );
			}
		}
		else
		// �X�N���[���o�[�̂܂݂�͂�ł���ꍇ�̓}�E�X�J�[�\���̈ړ��ʂ�
		// ���킹�ăX�N���[���ʒu���ړ�����
		if( TWindow->VScrollGrips )
		{
			TWindow->VScroll += MouseMoveY;
			if( TWindow->VScroll < 0 )
			{
				TWindow->VScroll = 0;
			}

			if( TWindow->VScroll > ScrollH )
			{
				TWindow->VScroll = ScrollH;
			}

			TWindow->DrawLeftUpY =
				TWindow->VScroll * ( TWindow->ClientSize.cy - InfoAreaH ) / ScrollH;
		}
		else
		// �X�N���[���o�[�̗̈���ɃJ�[�\��������A�����{�^���������ꂽ�u�Ԃ̏ꍇ��
		// �X�N���[���o�[�֌W�̏���������
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			ToolStatic_CheckMouseIn(
				TWindow->InfoArea.right,
				TWindow->InfoArea.top    + SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
				TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE ) )
		{
			// �܂݂̊O���N���b�N�����ꍇ�͂P��ʕ��ړ�����
			if( MouseY < TWindow->InfoArea.top + SCROLLBAR_BUTTON_SIZE +
											TWindow->VScroll )
			{
				TWindow->VScroll -= InfoAreaH * ScrollH / TWindow->ClientSize.cy;
				if( TWindow->VScroll < 0 )
				{
					TWindow->VScroll = 0;
				}
			}
			else
			if( MouseY > TWindow->InfoArea.top + SCROLLBAR_BUTTON_SIZE +
											TWindow->VScroll + SCROLLBAR_WIDTH )
			{
				TWindow->VScroll += InfoAreaH * ScrollH / TWindow->ClientSize.cy;
				if( TWindow->VScroll > ScrollH )
				{
					TWindow->VScroll = ScrollH;
				}
			}
			else
			{
				// �܂݂��N���b�N�����ꍇ�͂܂ݒ͂ݏ�Ԃɂ���
				TWindow->VScrollGrips = true;
			}

			// �X�N���[���ʒu����X�N���[���ʒu���l�������`���_�ʒu���Z�o
			TWindow->DrawLeftUpY = TWindow->VScroll *
									( TWindow->ClientSize.cy - InfoAreaH ) / ScrollH;
		}
	}
}

// �E�C���h�E������ʂɕ`�悷��
static void ToolWindow_Draw(
	// �E�C���h�E���n���h��
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          DrawWidth;
	bool         SizeOverX;
	bool         SizeOverY;
	bool         DownButInBottom;
	bool         UpButInBottom;
	bool         LeftButInBottom;
	bool         RightButInBottom;

	// �\����Ԃ� false �̏ꍇ�͉��������I��
	if( !ToolWindow_GetVisible( TWindowHandle ) )
	{
		return;
	}

	// �X�y�[�X��񂪕ύX����Ă���\��������̂ŏ��\���̈�̍X�V
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// �X�N���[���o�[�̃{�^���̏�ɃJ�[�\��������A
	// ���{�^����������Ă����Ԃ��ǂ����𒲂ׂĂ���
	DownButInBottom  = TWindow->DownButBottom  && TWindow->DownButIn;
	UpButInBottom    = TWindow->UpButBottom    && TWindow->UpButIn;
	LeftButInBottom  = TWindow->LeftButBottom  && TWindow->LeftButIn;
	RightButInBottom = TWindow->RightButBottom && TWindow->RightButIn;

	// ���\���̈�̕��ƍ������Z�o
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// �N���C�A���g�̈�̃T�C�Y�����\���̈�̃T�C�Y���傫�����ǂ����𒲂ׂĂ���
	SizeOverX = InfoAreaW < TWindow->ClientSize.cx;
	SizeOverY = InfoAreaH < TWindow->ClientSize.cy;

	// ���O���ݒ肳��Ă���ꍇ�͖��O��\������
	if( TWindow->Name )
	{
		// ���O��\������E�C���h�E�㕔��`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.left  - 1,
			TWindow->InfoArea.top   - TOOL_WINDOW_NAME_HEIGHT,
			TWindow->InfoArea.right + 1,
			TWindow->InfoArea.top   - 1,
			GetColor( 255,255,255 ),
			TRUE
		);

		// ���O�̕������`��
		DrawWidth = GetDrawStringWidthToHandle(
					TWindow->Name, -1, ToolStatic_GetButtonFontHandle() );

		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawStringToHandle(
			TWindow->InfoArea.left + ( InfoAreaW - DrawWidth ) / 2,
			TWindow->InfoArea.top - TOOL_WINDOW_NAME_HEIGHT +
							( TOOL_WINDOW_NAME_HEIGHT - TOOL_BUTTON_FONT_HEIGHT ) / 2,
			TWindow->Name,
			GetColor( 0,0,0 ),
			ToolStatic_GetButtonFontHandle()
		);
	}

	// ���\���̈�̘g��`��
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	DrawBox(
		TWindow->InfoArea.left   - 1,
		TWindow->InfoArea.top    - 1,
		TWindow->InfoArea.right  + 1 + ( SizeOverY ? TOOL_SCROLLBAR_WIDTH : 0 ),
		TWindow->InfoArea.bottom + 1 + ( SizeOverX ? TOOL_SCROLLBAR_WIDTH : 0 ),
		GetColor( 255,255,255 ),
		FALSE
	);

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 64 );

	DrawBox(
		TWindow->InfoArea.left   ,
		TWindow->InfoArea.top    ,
		TWindow->InfoArea.right  + ( SizeOverY ? TOOL_SCROLLBAR_WIDTH : 0 ),
		TWindow->InfoArea.bottom + ( SizeOverX ? TOOL_SCROLLBAR_WIDTH : 0 ),
		GetColor( 0,0,0 ),
		TRUE
	);

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	// �N���C�A���g�̈�̏c�������\���̈�̏c�����傫���ꍇ��
	// �c�����̃X�N���[���o�[��`�悷��
	if( SizeOverY )
	{
		// �o�[�̏㉺�ړ�����{�^����`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE,
			GetColor( 255,255,255 ),
			UpButInBottom ? TRUE : FALSE
		);

		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom,
			GetColor( 255,255,255 ),
			DownButInBottom ? TRUE : FALSE
		);

		DrawTriangle(
			TWindow->InfoArea.right + 2,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE - 2, 
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.top   + 2, 
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH - 2,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE - 2,
			UpButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		DrawTriangle(
			TWindow->InfoArea.right  + 2,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE + 2, 
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.bottom - 2,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH - 2,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE + 2,
			DownButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		// �o�[�̘g��`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top    + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			GetColor( 255,255,255 ),
			TRUE
		);

		// �o�[�̌��݂̃X�N���[���ʒu�������܂ݕ�����`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top   + TWindow->VScroll + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + TWindow->VScroll +
								SCROLLBAR_BUTTON_SIZE + SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);
	}

	// �N���C�A���g�̈�̉��������\���̈�̉������傫���ꍇ��
	// �������̃X�N���[���o�[��`�悷��
	if( SizeOverX )
	{
		// �o�[�̍��E�ړ�����{�^����`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

		DrawBox(
			TWindow->InfoArea.left,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			LeftButInBottom ? TRUE : FALSE
		);

		DrawBox(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			RightButInBottom ? TRUE : FALSE
		);

		DrawTriangle(
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE - 2,
			TWindow->InfoArea.bottom + 2,
			TWindow->InfoArea.left   + 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE - 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH - 2,
			LeftButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		DrawTriangle(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE + 2,
			TWindow->InfoArea.bottom + 2,
			TWindow->InfoArea.right  - 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE + 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH - 2,
			RightButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		// �o�[�̘g��`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);

		// �o�[�̌��݂̃X�N���[���ʒu�������܂ݕ�����`��
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawBox(
			TWindow->InfoArea.left   + TWindow->HScroll + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + TWindow->HScroll +
									SCROLLBAR_BUTTON_SIZE + SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);
	}

	// �`��u�����h���[�h�����ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
}

// �S�ẴE�C���h�E���̏�Ԑ��ڏ������s��
void ToolWindow_Step_All(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
)
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TWindowNum; i++ )
	{
		if( g_TWindow[ i ] == NULL )
		{
			continue;
		}

		ToolWindow_Step( i, StepTime );
		j++;
	}
}

// �S�ẴE�C���h�E���̕`��ڏ������s��
void ToolWindow_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TWindowNum; i++ )
	{
		if( g_TWindow[ i ] == NULL )
		{
			continue;
		}

		ToolWindow_Draw( i );
		j++;
	}
}

