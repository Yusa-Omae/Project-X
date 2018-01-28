#ifndef TOOLLIB_BUTTON_H
#define TOOLLIB_BUTTON_H

// �{�^�������쐬����
//     �߂�l : �{�^�����n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolButton_Create(
	// �n�m��ԂƂn�e�e��Ԃ��������тɐ؂�ւ��^�C�v�̃{�^�����ǂ���
	bool On_Off_SwitchType,

	// ���O�ƂȂ镶����̃A�h���X
	const char *Name,
	
	// �{�^����\�����鍶����W
	int x,
	int y,
	
	// �{�^���̕��ƍ���
	int w,
	int h
);

// �{�^�������폜����
extern void ToolButton_Delete(
	// �{�^�����n���h��
	int TButtonHandle
);

// �S�Ẵ{�^�������폜����
extern void ToolButton_Delete_All( void );

// �{�^�����Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolButton_SetVisibleHandle(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
);

// �{�^�����̖��O��ݒ肷��
//     �߂�l : ����������ɏI���������ǂ���( true:����ɏI������  false:�G���[���� )
extern bool ToolButton_SetName(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// ���O�ƂȂ镶����̃A�h���X
	const char *Name
);

// �{�^�����Ɗ֘A�t����E�C���h�E����ݒ肷��
extern void ToolButton_SetWindow(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �{�^�����̈ʒu��ύX����
extern void ToolButton_SetPosition(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �ύX��̃{�^���̍��W
	int x,
	int y
);

// �{�^�����̂n�m�^�n�e�e��Ԃ�ύX����
extern void ToolButton_SetOnOffFlag(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �ύX��̂n�m�^�n�e�e���( true:ON  false:OFF )
	bool Flag
);

// �{�^�����̂n�m�^�n�e�e��Ԃ��擾����
//     �߂�l : �n�m�^�n�e�e���( ture:ON  false:OFF )
extern bool ToolButton_GetOnOffFlag(
	// �{�^�����n���h��
	int TButtonHandle
);

// �{�^�����̃{�^�����N���b�N���ꂽ���ǂ������擾����
//     �߂�l : �{�^�����N���b�N���ꂽ���ǂ���
//              ( true:�N���b�N���ꂽ  false:�N���b�N����Ă��Ȃ� )
extern bool ToolButton_GetClick(
	// �{�^�����n���h��
	int TButtonHandle,

	// �N���b�N���ꂽ���ǂ����̏�Ԃ����Z�b�g���邩�ǂ���
	// ( true:���Z�b�g����  false:���Z�b�g���Ȃ� )
	bool Reset
);

// �{�^�����̃{�^������������Ă��邩�ǂ������擾����
//     �߂�l : �{�^������������Ă��邩�ǂ���
//              ( true:��������Ă���  false:��������Ă��Ȃ� )
extern bool ToolButton_GetBottom(
	// �{�^�����n���h��
	int TButtonHandle
);

// �{�^�����̃{�^������������Ă��邩�ǂ������擾����( ���s�[�g�@�\�t���� )
//     �߂�l : �{�^������������Ă��邩�ǂ���
//              ( true:��������Ă���  false:��������Ă��Ȃ� )
extern bool ToolButton_GetRepeatBottom(
	// �{�^�����n���h��
	int TButtonHandle
);

// �{�^�����̕\����Ԃ�ύX����
extern void ToolButton_SetVisible(
	// �{�^�����n���h��
	int TButtonHandle,
	
	// �V�����\�����
	bool Visible
);

// �{�^�����̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolButton_GetVisible(
	// �{�^�����n���h��
	int TButtonHandle
);

// �S�Ẵ{�^�����̏�Ԑ��ڏ������s��
extern void ToolButton_Step_All(
	// �i�߂鎞��( �P�� : �b )
	float StepTime
);

// �S�Ẵ{�^�����̕`��ڏ������s��
extern void ToolButton_Draw_All( void );

#endif
