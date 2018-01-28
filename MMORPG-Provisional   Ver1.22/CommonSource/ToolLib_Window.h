#ifndef TOOLLIB_WINDOW_H
#define TOOLLIB_WINDOW_H

#include "DxLib.h"

// �E�C���h�E�����쐬����
//     �߂�l : �E�C���h�E���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int  ToolWindow_Create(
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
);

// �E�C���h�E�����폜����
extern void ToolWindow_Delete(
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �S�ẴE�C���h�E�����폜����
extern void ToolWindow_Delete_All( void );

// �E�C���h�E���Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolWindow_SetVisibleHandle( 
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// �\����ԏ��n���h��
	int TVisibleHandle
);

// �E�C���h�E�������� SetDrawArea �ŕ`��̈�𐧌�����ݒ���s��
extern void ToolWindow_SetupDrawArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �E�C���h�E���̏��\���̈���擾����
//     �߂�l : �`��̈�̋�`
extern RECT ToolWindow_GetInfoArea(
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
// �N���C�A���g�̈�̕`�挴�_�ƂȂ�X�N���[�����W���擾����
extern void ToolWindow_GetDrawLeftUpPos(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// ���W��������ϐ��̃A�h���X
	int *x,
	int *y
);

// �E�C���h�E���̃X�N���[���o�[�̏�Ԃ��l������
// �N���C�A���g�̈�̕`�挴�_�ƂȂ���W���擾����
extern void ToolWindow_GetClientArea(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// ���W��������ϐ��̃A�h���X
	int *x,
	int *y
);

// �E�C���h�E���̃N���C�A���g�̈�̃T�C�Y��ݒ肷��
extern void ToolWindow_SetClientSize(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �V�����N���C�A���g�̈�̃T�C�Y
	int w,
	int h
);

// �E�C���h�E���̖��O��ݒ肷��
//     �߂�l : ����������ɏI���������ǂ���( true:����ɏI������  false:�G���[���� )
extern bool ToolWindow_SetName(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// ���O�ƂȂ镶����̃A�h���X
	const char *Name
);

// �E�C���h�E���̕\����Ԃ�ύX����
extern void ToolWindow_SetVisible(
	// �E�C���h�E���n���h��
	int TWindowHandle,
	
	// �V�����\�����
	bool Visible
);

// �E�C���h�E���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolWindow_GetVisible(
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �w��̍��W���E�C���h�E�̏��\���̈�ɓ����Ă��邩�ǂ����𒲂ׂ�
//     �߂�l : �w��̍��W���E�C���h�E�̕`��̈�ɓ����Ă��邩�ǂ���
//              ( true:�����Ă���   false:�����Ă��Ȃ� )
extern bool ToolWindow_CheckInfoAreaIn(
	// �E�C���h�E���n���h��
	int TWindowHandle,

	// �E�C���h�E�̕`��̈�ɓ����Ă��邩�ǂ������ׂ���W
	int x,
	int y
);

// �S�ẴE�C���h�E���̏�Ԑ��ڏ������s��
extern void ToolWindow_Step_All(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
);

// �S�ẴE�C���h�E���̕`��ڏ������s��
extern void ToolWindow_Draw_All( void );


#endif


