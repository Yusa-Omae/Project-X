#include "ToolLib.h"
#include "DxLib.h"

// �T�C�Y�ύX�\�ȕ\���̈�̏����E�[X���W
#define TOOL_BASE_RIGHT_X			(350)

// �T�C�Y�ύX�\�ȕ\���̈���ő�܂ōL�������ꍇ�̉E�[�̗]��
#define TOOL_BASE_RIGHT_X_SPACE		(50)

// �T�C�Y�ύX�\�ȕ\���̈�̃T�C�Y�ύX�т̕�
#define TOOL_BASE_RIGHT_BAR_WIDTH	(4)

// �_�u���N���b�N���ꂽ�Ɣ��f����N���b�N�ƃN���b�N�̊Ԋu
#define DOUBLE_CLICK_TIME			(400)

// ToolLib�Ǘ��p�\����
typedef struct _ToolManage
{
	// �T�C�Y�ύX�\�ȕ\���̈�̉E�[X���W
	int  BaseRightX;

	// �T�C�Y�ύX�\�ȕ\���̈�̃T�C�Y�ύX�т�͂�ł��邩�ǂ���
	bool BaseRightGrip;

	// �T�C�Y�ύX�\�ȕ\���̈�̃T�C�Y�ύX�т�͂񂾂Ƃ��̉E�[X���W
	int  BaseRightGripX;

	// �T�C�Y�ύX�\�ȕ\���̈�̃T�C�Y�ύX�т�͂񂾂Ƃ��̃}�E�X��X���W
	int  BaseRightGripMouseX;

	// �}�E�X�̃{�^�����͏��
	int  MouseInput;

	// �}�E�X�̃{�^�����͏��( �����ꂽ�u�Ԃ̃{�^���̂� )
	int  MouseEdgeInput;

	// �}�E�X�̃{�^���������ꂽ�u�Ԃ̃}�E�X�J�[�\���̍��W
	int  MouseEdgeX;
	int  MouseEdgeY;

	// �}�E�X�J�[�\���̍��W
	int  MouseX;
	int  MouseY;
	
	// �}�E�X�J�[�\���̍��W�ړ���
	int  MouseMovX;
	int  MouseMovY;

	// �}�E�X�̑I���J�n���W
	int  MouseSelectStartX;
	int  MouseSelectStartY;

	// �}�E�X�̑I���I�����W
	int  MouseSelectEndX;
	int  MouseSelectEndY;

	// �_�u���N���b�N�����ꂽ���ǂ���
	bool MouseDoubleClick;

	// �}�E�X�̍��N���b�N�����ꂽ�Ƃ��̎���
	int  MouseClickTime;

	// �ʏ핶���`��p�̃t�H���g�n���h��
	int  NormalFontHandle;

	// �{�^���������`��p�̃t�H���g�n���h��
	int  ButtonFontHandle;
} ToolManage;

ToolManage g_ToolManage;


// �c�[���p�����̏��������s��
//     �߂�l : ������������Ɋ����������ǂ���( true:���튮��  false:�G���[���� )
bool Tool_Initialize( void )
{
	// �T�C�Y�ύX�\�ȕ\���̈�̉E�[X���W�̏�����
	g_ToolManage.BaseRightX = TOOL_BASE_RIGHT_X;

	// �ʏ핶���`��p�̃t�H���g�n���h���̍쐬
	g_ToolManage.NormalFontHandle = CreateFontToHandle( NULL, 12, 3, DX_FONTTYPE_EDGE );
	if( g_ToolManage.NormalFontHandle == -1 )
	{
		return false;
	}

	// �{�^���������`��p�̃t�H���g�n���h���̍쐬
	g_ToolManage.ButtonFontHandle = CreateFontToHandle( NULL, 12, 3, DX_FONTTYPE_NORMAL);
	if( g_ToolManage.ButtonFontHandle == -1 )
	{
		return false;
	}

	// ����I��
	return true;
}

// �c�[���p�����̌�n�����s��
void Tool_Terminate( void )
{
	// �S�Ẵt�@�C���I�����̍폜�������s��
	ToolFileSelect_Delete_All();

	// �S�Ẵp�����[�^�o�[���̍폜�������s��
	ToolParamBar_Delete_All();

	// �S�ẴE�C���h�E���̍폜�������s��
	ToolWindow_Delete_All();

	// �S�Ẵ{�^�����̍폜�������s��
	ToolButton_Delete_All();

	// �S�Ă̕�������͏��̍폜�������s��
	ToolStringInput_Delete_All();

	// �S�Ẵ��X�g���̍폜�������s��
	ToolList_Delete_All();

	// �S�Ă̕\����ԏ��̍폜�������s��
	ToolVisible_Delete_All();

	// �S�ẴX�y�[�X�Ǘ����̍폜�������s��
	ToolSpaceManage_Delete_All();

	// �ʏ핶���`��p�̃t�H���g�n���h���̍폜
	DeleteFontToHandle( g_ToolManage.NormalFontHandle );
	g_ToolManage.NormalFontHandle = -1;

	// �{�^���������`��p�̃t�H���g�n���h���̍폜
	DeleteFontToHandle( g_ToolManage.ButtonFontHandle );
	g_ToolManage.ButtonFontHandle = -1;
}

// �c�[���p�����̏�Ԑ��ڏ������s��
//     �߂�l : ����������Ɋ����������ǂ���( true:���튮��  false:�G���[���� )
bool Tool_Step(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
)
{
	int MouseX;
	int MouseY;
	int EdgeInput;
	int EdgeMouseX;
	int EdgeMouseY;
	int ScreenWidth;
	int ScreenHeight;

	// �}�E�X�̃{�^�����͂��擾
	g_ToolManage.MouseInput = GetMouseInput();

	// �}�E�X�̃{�^���������ꂽ�u�Ԃ̏����擾
	if( GetMouseInputLog( &EdgeInput, &EdgeMouseX, &EdgeMouseY ) != -1 )
	{
		g_ToolManage.MouseEdgeInput = EdgeInput;
		g_ToolManage.MouseInput    |= EdgeInput;

		g_ToolManage.MouseEdgeX     = EdgeMouseX;
		g_ToolManage.MouseEdgeY     = EdgeMouseY;
	}
	else
	{
		g_ToolManage.MouseEdgeInput = 0;

		g_ToolManage.MouseEdgeX     = 0;
		g_ToolManage.MouseEdgeY     = 0;
	}

	// �}�E�X�J�[�\���̍��W���擾
	GetMousePoint( &MouseX, &MouseY );

	// �O�񂩂�̍��W�ړ��ʂ��Z�o
	g_ToolManage.MouseMovX = MouseX - g_ToolManage.MouseX;
	g_ToolManage.MouseMovY = MouseY - g_ToolManage.MouseY;

	// �}�E�X�J�[�\���̍��W��ۑ�
	g_ToolManage.MouseX    = MouseX;
	g_ToolManage.MouseY    = MouseY;

	// �}�E�X�̍��{�^���������ꂽ�u�Ԃ��ǂ����ŏ����𕪊�
	if( ( g_ToolManage.MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		// �A�N�e�B�u�ȕ�������̓n���h���𖳂���Ԃɂ���
		SetActiveKeyInput( -1 );

		// �O��N���b�N����Ă��璼���ɃN���b�N���ꂽ�ꍇ�̓_�u���N���b�N���ꂽ�Ɣ��f����
		if( GetNowCount() - g_ToolManage.MouseClickTime < DOUBLE_CLICK_TIME )
		{
			g_ToolManage.MouseDoubleClick = true;
		}
		else
		{
			g_ToolManage.MouseDoubleClick = false;
		}

		// �N���b�N���ꂽ�Ƃ��̎��Ԃ�ۑ�
		g_ToolManage.MouseClickTime    = GetNowCount();

		// �I��͈͂��N���b�N���ꂽ���W�ɂ���
		g_ToolManage.MouseSelectStartX = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectStartY = g_ToolManage.MouseY;
		g_ToolManage.MouseSelectEndX   = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectEndY   = g_ToolManage.MouseY;
	}
	else
	// �}�E�X�̍��{�^���������ꂽ�u�Ԃł͂Ȃ������ꑱ���Ă���ꍇ�̓}�E�X�̑I��͈͂�
	// �I�[���W���X�V
	if( ( g_ToolManage.MouseInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		g_ToolManage.MouseSelectEndX = g_ToolManage.MouseX;
		g_ToolManage.MouseSelectEndY = g_ToolManage.MouseY;
	}
	else
	{
		// �}�E�X�̍��{�^����������Ă����莞�Ԍo�߂��Ă�����
		// �_�u���N���b�N�������������
		if( GetNowCount() - g_ToolManage.MouseClickTime > DOUBLE_CLICK_TIME )
		{
			g_ToolManage.MouseDoubleClick = false;
		}
	}

	// �T�C�Y�ύX�\�ȕ\���̈�̃T�C�Y�ύX�т�͂�ł��邩�ǂ����ŏ�����ύX
	if( g_ToolManage.BaseRightGrip )
	{
		// �͂�ł����ԂŊ����{�^����������Ă���ꍇ�͕\���̈�̃T�C�Y��
		// �}�E�X�J�[�\���̈ʒu�ɉ����ĕύX����
		if( g_ToolManage.MouseInput & MOUSE_INPUT_LEFT )
		{
			g_ToolManage.BaseRightX = g_ToolManage.BaseRightGripX +
						g_ToolManage.MouseX - g_ToolManage.BaseRightGripMouseX;

			// ��ʂ̃T�C�Y���擾����
			GetDrawScreenSize( &ScreenWidth, &ScreenHeight );

			// �T�C�Y�������T�C�Y�����������Ȃ�Ȃ��悤�ɂ���
			if( g_ToolManage.BaseRightX < TOOL_BASE_RIGHT_X )
			{
				g_ToolManage.BaseRightX = TOOL_BASE_RIGHT_X;
			}
			else
			// �T�C�Y���u��ʉ����|�]���v���傫���Ȃ�Ȃ��悤�ɂ���
			if( g_ToolManage.BaseRightX > ScreenWidth - TOOL_BASE_RIGHT_X_SPACE )
			{
				g_ToolManage.BaseRightX = ScreenWidth - TOOL_BASE_RIGHT_X_SPACE;
			}
		}
		else
		{
			// ���{�^����������Ă��Ȃ��ꍇ�͒͂�ł��邩�ǂ����̃t���O��|��
			g_ToolManage.BaseRightGrip = false;
		}
	}
	else
	{
		// �}�E�X�̍��{�^���������ꂽ�u�Ԃ̏ꍇ�̓T�C�Y�ύX�\��
		// �\���̈�̃T�C�Y�ύX�т�͂�ł��邩�`�F�b�N���āA
		// �͂�ł�����͂ݏ�Ԃɂ���
		if( ( g_ToolManage.MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			g_ToolManage.BaseRightX                             <= g_ToolManage.MouseX &&
			g_ToolManage.BaseRightX + TOOL_BASE_RIGHT_BAR_WIDTH >= g_ToolManage.MouseX )
		{
			g_ToolManage.BaseRightGrip       = true;
			g_ToolManage.BaseRightGripMouseX = g_ToolManage.MouseX;
			g_ToolManage.BaseRightGripX      = g_ToolManage.BaseRightX;
		}
	}

	// �E�C���h�E���̏�Ԑ��ڏ������s��
	ToolWindow_Step_All( StepTime );

	// �{�^�����̏�Ԑ��ڏ������s��
	ToolButton_Step_All( StepTime );

	// ��������͏��̏�Ԑ��ڏ������s��
	if( !ToolStringInput_Step_All() )
	{
		return false;
	}

	// �p�����[�^�o�[���̏�Ԑ��ڏ������s��
	ToolParamBar_Step_All();

	// ���X�g���̏�Ԑ��ڏ������s��
	ToolList_Step_All();

	// �t�@�C���I�����̏�Ԑ��ڏ������s��
	ToolFileSelect_Step_All();

	// ����I��
	return true;
}

// �c�[���p�����̕`�揈�����s��
void Tool_Draw( void )
{
	// �E�C���h�E���̕`�揈�����s��
	ToolWindow_Draw_All();

	// �{�^�����̕`�揈�����s��
	ToolButton_Draw_All();

	// ��������͏��̕`�揈�����s��
	ToolStringInput_Draw_All();

	// �p�����[�^�o�[���̕`�揈�����s��
	ToolParamBar_Draw_All();

	// ���X�g���̕`�揈�����s��
	ToolList_Draw_All();

	// �t�@�C���I�����̕`�揈�����s��
	ToolFileSelect_Draw_All();

	// �T�C�Y�ύX�\�ȕ\���̈�E�[�̃T�C�Y�ύX�т�`�悷��
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 32 );
	DrawBox(
		g_ToolManage.BaseRightX,
		0,
		g_ToolManage.BaseRightX + TOOL_BASE_RIGHT_BAR_WIDTH,
		99999,
		GetColor( 255,255,255 ),
		TRUE
	);
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 255 );
}

// �c�[���p�����̃T�C�Y�ύX�\�ȕ\���̈�̉E�[��X���W���擾����
//     �߂�l : �ϕ\���̈�E�[��X���W
int Tool_GetBaseRightX( void )
{
	return g_ToolManage.BaseRightX;
}

// �c�[���p�����ň����Ă���}�E�X�̃{�^�����͂��擾����
int Tool_GetMouseEdgeInput(
	// �����ꂽ�u�Ԃ̃}�E�X�̃{�^����������ϐ��̃A�h���X
	int *EdgeInput,

	// �}�E�X�̃{�^���������ꂽ�u�Ԃ̃}�E�X�J�[�\���̍��W��������ϐ��̃A�h���X
	int *EdgeX,
	int *EdgeY
)
{
	*EdgeInput = g_ToolManage.MouseEdgeInput;
	*EdgeX     = g_ToolManage.MouseEdgeX;
	*EdgeY     = g_ToolManage.MouseEdgeY;

	return *EdgeInput != 0;
}

// �}�E�X�J�[�\�����w��̋�`�̓��ɂ��邩�ǂ������`�F�b�N����
//     �߂�l : ��`�̓��Ƀ}�E�X�J�[�\�������邩�ǂ���
//              ( true:���ɂ���  false:�O�ɂ��� )
bool ToolStatic_CheckMouseIn(
	// �`�F�b�N�����`�̍���[���W
	int x1,
	int y1,

	// �`�F�b�N�����`�̉E���[���W
	int x2,
	int y2
)
{
	return g_ToolManage.MouseX >= x1 && g_ToolManage.MouseX <= x2 &&
		   g_ToolManage.MouseY >= y1 && g_ToolManage.MouseY <= y2;
}

// �}�E�X�J�[�\���̍��W���擾����
void ToolStatic_GetMousePosition(
	// �}�E�X�J�[�\����x���W��y���W��������ϐ��̃A�h���X
	int *x,
	int *y
)
{
	*x = g_ToolManage.MouseX;
	*y = g_ToolManage.MouseY;
}

// �}�E�X�J�[�\���̃{�^���������ꂽ�Ƃ��̍��W���擾����
void ToolStatic_GetMouseEdgePosition(
	// �}�E�X�J�[�\���̍��W��������ϐ��̃A�h���X
	int *EdgeX,
	int *EdgeY
)
{
	*EdgeX = g_ToolManage.MouseEdgeX;
	*EdgeY = g_ToolManage.MouseEdgeY;
}

// �}�E�X�J�[�\���̑O�t���[������ړ������������擾����
void ToolStatic_GetMouseMoveVecter(
	// �}�E�X�J�[�\���̈ړ�������������ϐ��̃A�h���X
	int *MoveX,
	int *MoveY
)
{
	*MoveX = g_ToolManage.MouseMovX;
	*MoveY = g_ToolManage.MouseMovY;
}

// �ʏ�t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
int ToolStatic_GetNormalFontHandle( void )
{
	return g_ToolManage.NormalFontHandle;
}

// �{�^���������p�̃t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
int ToolStatic_GetButtonFontHandle( void )
{
	return g_ToolManage.ButtonFontHandle;
}

// �}�E�X�̃{�^��������Ԃ��擾����
//     �߂�l : �}�E�X�{�^���̉�����Ԓl( GetMouseInput �̖߂�l�Ɠ����`�� )
int ToolStatic_GetMouseButton( void )
{
	return g_ToolManage.MouseInput;
}

// �}�E�X�̃{�^��������Ԃ��擾����( �����ꂽ�u�Ԃ݂̂̏�� )
//     �߂�l : �}�E�X�{�^���̉�����Ԓl( GetMouseInput �̖߂�l�Ɠ����`�� )
int ToolStatic_GetMouseEdgeButton( void )
{
	return g_ToolManage.MouseEdgeInput;
}

// �}�E�X�̍��{�^�����_�u���N���b�N���ꂽ���ǂ������擾����
//     �߂�l : ���{�^�����_�u���N���b�N���ꂽ���ǂ���
//              ( true:�_�u���N���b�N���ꂽ  false:�_�u���N���b�N����Ă��Ȃ�)
bool ToolStatic_GetMouseDoubleClick( void )
{
	bool Result;

	Result = g_ToolManage.MouseDoubleClick;

	// �_�u���N���b�N���ꂽ���ǂ����̃t���O��|��
	g_ToolManage.MouseDoubleClick = false;

	return Result;
}

// �}�E�X�̑I��͈͂̊J�n���W�ƏI�����W���擾����
void ToolStatic_GetMouseSelectStartPosition(
	// �I��͈͂̊J�n���W���i�[����ϐ��̃A�h���X
	int *StartX,
	int *StartY,

	// �I��͈͂̏I�����W���i�[����ϐ��̃A�h���X
	int *EndX,
	int *EndY
)
{
	if( StartX != NULL )
	{
		*StartX = g_ToolManage.MouseSelectStartX;
	}

	if( StartY != NULL )
	{
		*StartY = g_ToolManage.MouseSelectStartY;
	}

	if( EndX != NULL )
	{
		*EndX = g_ToolManage.MouseSelectEndX;
	}

	if( EndY != NULL )
	{
		*EndY = g_ToolManage.MouseSelectEndY;
	}
}

// �c�[�������̃{�^����`�悷��
void Tool_DrawButton(
	// �{�^����`�悷�鍶����W
	int x,
	int y,
	
	// �{�^���̕��ƍ���
	int w,
	int h,
	
	// �{�^���̍��[�Ə�[�̐F
	int LeftTopColor1,
	int LeftTopColor2,

	// �{�^���̉E�[�Ɖ��[�̐F
	int RightBottomColor1,
	int RightBottomColor2,

	// �{�^���̒��������̐F
	int CenterColor
)
{
	// ����������`��
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
	DrawBox( x + 2, y + 2, x + w - 2, y + h - 2, CenterColor, TRUE );

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	// ���[�Ə�[��`��
	DrawBox( x,     y,     x + w - 1, y + 1, LeftTopColor1, TRUE );
	DrawBox( x + 1, y + 1, x + w - 2, y + 2, LeftTopColor2, TRUE );

	DrawBox( x,     y + 1, x + 1, y + h - 1, LeftTopColor1, TRUE );
	DrawBox( x + 1, y + 2, x + 2, y + h - 2, LeftTopColor2, TRUE );

	// �E�[�Ɖ��[��`��
	DrawBox( x,     y + h - 1,  x + w, y + h,     RightBottomColor1, TRUE );
	DrawBox( x + 1, y + h - 2,  x + w, y + h - 1, RightBottomColor2, TRUE );

	DrawBox( x + w - 1, y,     x + w,     y + h - 1, RightBottomColor1, TRUE );
	DrawBox( x + w - 2, y + 1, x + w - 1, y + h - 2, RightBottomColor2, TRUE );

	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
}

// ��������̓n���h���̒��̃J�[�\���̈ʒu���擾����
int Tool_GetKeyInputCursorPos(
	// �������`�悷��X���W
	int StrDrawX,

	// �}�E�X�J�[�\����X���W
	int MouseX,

	// ��������̓n���h��
	int InputHandle,

	// �������`�悷��ۂɎg�p����t�H���g�n���h��
	int FontHandle
)
{
	int  DrawStartPos;
	char String[ 2048 ];
	int  TempMouseX;
	int  i;
	int  CharSize;
	int  TotalX;
	int  DrawWidth;

	// ������J�n�ʒu���W����݂��}�E�X�̑��΍��W���Z�o
	TempMouseX = MouseX - StrDrawX;

	// ��������̓n���h���̌��݂̓��͕�������擾
	GetKeyInputString( String, InputHandle );

	// ���͒�������̊J�n�����ʒu���擾
	DrawStartPos = GetKeyInputDrawStartPos( InputHandle );

	// �����̏I�[������܂Ń��[�v
	TotalX = 0;
	for( i = 0; String[ i + DrawStartPos ] != '\0'; i += CharSize )
	{
		// �P�����̃o�C�g�����擾
		if( MultiByteCharCheck( &String[ i + DrawStartPos ], DX_CHARSET_SHFTJIS ) )
		{
			CharSize = 2;
		}
		else
		{
			CharSize = 1;
		}

		// �P�������̃s�N�Z�������擾
		DrawWidth =
			GetDrawStringWidthToHandle( &String[ i + DrawStartPos],CharSize,FontHandle );

		// �}�E�X�J�[�\����X���W�������̗̈�ɂ���ꍇ�̓��[�v���甲����
		if( TempMouseX >= TotalX && TempMouseX < TotalX + DrawWidth )
		{
			break;
		}

		// �`�F�b�N�ʒu�𕶎��̕����������炷
		TotalX += DrawWidth;
	}

	// �}�E�X�J�[�\�������镶���̈ʒu��Ԃ�
	return DrawStartPos + i;
}


