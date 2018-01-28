#ifndef TOOLLIB_PARAMBAR_H
#define TOOLLIB_PARAMBAR_H

// �p�����[�^�o�[�����쐬����
//     �߂�l : �p�����[�^�o�[���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolParamBar_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,

	// �\�����W
	int PositionX,
	int PositionY,

	// �o�[��\�����鑊��X���W
	int BarPositionX,

	// �o�[�̉���
	int BarWidth,

	// �\�����鐔�l�̌���
	int NumberLength,
	
	// �o�[�ő���ł���͈͂̒l�̍ŏ��l�ƍő�l
	float ParamMin,
	float ParamMax,

	// �����l
	float InitParam
);

// �p�����[�^�o�[�����폜����
extern void ToolParamBar_Delete(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �S�Ẵp�����[�^�o�[�����폜����
extern void ToolParamBar_Delete_All( void );

// �p�����[�^�o�[���Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolParamBar_SetVisibleHandle(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
);

// �p�����[�^�o�[���̕\����Ԃ�ύX����
extern void ToolParamBar_SetVisible(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �V�����\�����
	bool Visible
);

// �p�����[�^�o�[���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolParamBar_GetVisible(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �p�����[�^�o�[���Ɗ֘A�t����E�C���h�E����ݒ肷��
extern void ToolParamBar_SetWindow(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �p�����[�^�o�[���̍��W��ύX����
extern void ToolParamBar_SetPosition(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �ύX��̍��W
	int PositionX,
	int PositionY
);

// �p�����[�^�o�[���̐��l��ύX����
extern void ToolParamBar_SetParam(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �ύX��̐��l
	float Param
);

// �p�����[�^�o�[���̃o�[�ŕύX�ł��鐔�l�̍ő�l�ƍŏ��l��ύX����
extern void ToolParamBar_SetMinMax(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle,
	
	// �V�����ŏ��l�ƍő�l
	float ParamMin,
	float ParamMax
);

// �p�����[�^�o�[���̐��l�ҏW���s���Ă����ꍇ�ɖ������ŏI��������
extern void ToolParamBar_AlwaysEnd(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �p�����[�^�o�[���̐��l�ɕω������������ǂ������擾����
//     �߂�l : ���l�ɕω������������ǂ���( true:�ω����� false:�ω��Ȃ� )
extern bool ToolParamBar_CheckChange(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �p�����[�^�o�[���̌��݂̐��l���擾����
//     �߂�l : �p�����[�^�o�[�̒l
extern float ToolParamBar_GetParam(
	// �p�����[�^�o�[���n���h��
	int TParamBarHandle
);

// �S�Ẵp�����[�^�o�[���̏�Ԑ��ڏ������s��
extern void ToolParamBar_Step_All( void );

// �S�Ẵp�����[�^�o�[���̕`��ڏ������s��
extern void ToolParamBar_Draw_All( void );

#endif
