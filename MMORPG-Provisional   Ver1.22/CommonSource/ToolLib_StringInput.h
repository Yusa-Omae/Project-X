#ifndef TOOLLIB_STRINGINPUT_H
#define TOOLLIB_STRINGINPUT_H

// ��������͏����쐬����
//     �߂�l : ��������͏��n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolStringInput_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,
	
	// ��������͂̕\���ʒu
	int x,
	int y,

	// ���l�̂ݓ��͉\�ɂ��邩�ǂ���
	bool NumberOnly,

	// ���͂ł��鐔�l�̌���( NumberOnly �� true �̏ꍇ�̂ݗL�� )
	int NumberNum,

	// ��������͗̈�̉���( NumberOnly �� false �̏ꍇ�̂ݗL�� )
	int StringDrawWidth
);

// ��������͏����폜����
extern void ToolStringInput_Delete(
	// ��������͏��n���h��
	int TStringInputHandle
);

// �S�Ă̕�������͏����폜����
extern void ToolStringInput_Delete_All( void );

// ��������͏��Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolStringInput_SetVisibleHandle(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
);

// ��������͏��̕\����Ԃ�ύX����
extern void ToolStringInput_SetVisible(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �V�����\�����
	bool Visible
);

// ��������͏��̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolStringInput_GetVisible(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏��Ɗ֘A�t����E�C���h�E����ݒ肷��
extern void ToolStringInput_SetWindow(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// ��������͏��̕������͗̈�̍��W��ύX����
extern void ToolStringInput_SetPosition(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �ύX��̕������͗̈�̍��W
	int x,
	int y
);

// ��������͏��̓��͒��̕������ύX����
extern void ToolStringInput_SetString(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// �Z�b�g���镶���񂪊i�[���ꂽ�������A�h���X
	const char *String
);

// ��������͏��̓��͒��̕�������擾����
extern void ToolStringInput_GetString(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒��̕�������i�[���郁�����̈�̐擪�A�h���X
	char *String
);

// ��������͏��̕�������͂𖳏����ŏI������
extern void ToolStringInput_AlwaysEnd(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏��̓��͒��̕�����Ƃ��Đ����l�ɕύX����
extern void ToolStringInput_SetIntNumber(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒�������Ƃ��Đݒ肷�鐮���l
	int Number
);

// ��������͏��̓��͒��̕�����Ƃ��ĕ��������_�l�ɕύX����
extern void ToolStringInput_SetFloatNumber(
	// ��������͏��n���h��
	int TStringInputHandle,
	
	// ���͒�������Ƃ��Đݒ肷�镂�������_�l
	float Number
);

// ��������͏��̓��͒��̕�����𐮐��l�ɕϊ��������̂��擾����
//     �߂�l : ���͒��̕�����𐮐��l�ɕϊ������l
extern int ToolStringInput_GetIntNumber(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏��̓��͒��̕�����𕂓������_�l�ɕϊ��������̂��擾����
//     �߂�l : ���͒��̕�����𕂓������_�l�ɕϊ������l
extern float ToolStringInput_GetFloatNumber(
	// ��������͏��n���h��
	int TStringInputHandle
);

// ��������͏��̕�����̓��͂��I�����Ă��邩�ǂ������擾����
//     �߂�l : ���͂��I�����Ă��邩�ǂ���( true:�I�����Ă���  false:�I�����Ă��Ȃ� )
extern bool ToolStringInput_CheckEnd(
	// ��������͏��n���h��
	int TStringInputHandle,

	// ������̓��͂��I�����Ă��邩�ǂ����̃t���O�����Z�b�g���邩�ǂ���
	bool Reset
);

// ��������͏��̕�������͂��A�N�e�B�u�ɂȂ��Ă����ꍇ�ɃA�N�e�B�u��Ԃ���������
extern void ToolStringInput_ResetActive(
	// ��������͏��n���h��
	int TStringInputHandle
);

// �S�Ă̕�������͏��̏�Ԑ��ڏ������s��
//    �߂�l : ����������ɏI���������ǂ���( true:����I��  false:�G���[���� )
extern bool ToolStringInput_Step_All( void );

// �S�Ă̕�������͏��̕`��ڏ������s��
extern void ToolStringInput_Draw_All( void );

#endif

