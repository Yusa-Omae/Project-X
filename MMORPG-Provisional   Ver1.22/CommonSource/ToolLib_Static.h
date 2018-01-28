#ifndef TOOLLIB_STATIC_H
#define TOOLLIB_STATIC_H

// �{�^���p�t�H���g�̏c��
#define TOOL_BUTTON_FONT_HEIGHT		(12)

// �}�E�X�J�[�\�����w��̋�`�̓��ɂ��邩�ǂ������`�F�b�N����
//     �߂�l : ��`�̓��Ƀ}�E�X�J�[�\�������邩�ǂ���
//              ( true:���ɂ���  false:�O�ɂ��� )
extern bool ToolStatic_CheckMouseIn(
	// �`�F�b�N�����`�̍���[���W
	int x1,
	int y1,

	// �`�F�b�N�����`�̉E���[���W
	int x2,
	int y2
);

// �}�E�X�J�[�\���̍��W���擾����
extern void ToolStatic_GetMousePosition(
	// �}�E�X�J�[�\���̍��W��������ϐ��̃A�h���X
	int *x,
	int *y
);

// �}�E�X�J�[�\���̃{�^���������ꂽ�Ƃ��̍��W���擾����
extern void ToolStatic_GetMouseEdgePosition(
	// �}�E�X�J�[�\���̍��W��������ϐ��̃A�h���X
	int *EdgeX,
	int *EdgeY
);

// �}�E�X�J�[�\���̑O�t���[������ړ������������擾����
extern void ToolStatic_GetMouseMoveVecter(
	// �}�E�X�J�[�\���̈ړ�������������ϐ��̃A�h���X
	int *MoveX,
	int *MoveY
);

// �ʏ�t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
extern int ToolStatic_GetNormalFontHandle( void );

// �{�^���������p�̃t�H���g�n���h�����擾����
//     �߂�l : �t�H���g�n���h��
extern int ToolStatic_GetButtonFontHandle( void );

// �}�E�X�̃{�^��������Ԃ��擾����
//     �߂�l : �}�E�X�{�^���̉�����Ԓl( GetMouseInput �̖߂�l�Ɠ����`�� )
extern int ToolStatic_GetMouseButton( void );

// �}�E�X�̃{�^��������Ԃ��擾����( �����ꂽ�u�Ԃ݂̂̏�� )
//     �߂�l : �}�E�X�{�^���̉�����Ԓl( GetMouseInput �̖߂�l�Ɠ����`�� )
extern int ToolStatic_GetMouseEdgeButton( void );

// �}�E�X�̍��{�^�����_�u���N���b�N���ꂽ���ǂ������擾����
//     �߂�l : ���{�^�����_�u���N���b�N���ꂽ���ǂ���
//              ( true:�_�u���N���b�N���ꂽ  false:�_�u���N���b�N����Ă��Ȃ�)
extern bool ToolStatic_GetMouseDoubleClick( void );

// �}�E�X�̑I��͈͂̊J�n���W�ƏI�����W���擾����
extern void ToolStatic_GetMouseSelectStartPosition(
	// �I��͈͂̊J�n���W���i�[����ϐ��̃A�h���X
	int *StartX,
	int *StartY,

	// �I��͈͂̏I�����W���i�[����ϐ��̃A�h���X
	int *EndX,
	int *EndY
);

// �c�[�������̃{�^����`�悷��
extern void Tool_DrawButton(
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
);

// ��������̓n���h���̒��̃J�[�\���̈ʒu���擾����
extern int Tool_GetKeyInputCursorPos(
	// �������`�悷��X���W
	int StrDrawX,

	// �}�E�X�J�[�\����X���W
	int MouseX,

	// ��������̓n���h��
	int InputHandle,

	// �������`�悷��ۂɎg�p����t�H���g�n���h��
	int FontHandle
);

#endif
