#ifndef TOOLLIB_FILESELECT_H
#define TOOLLIB_FILESELECT_H

// �t�@�C���I�����������쐬����
//     �߂�l : �t�@�C���I���������n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int  ToolFileSelect_Create(
	// ���O�̕����񂪊i�[���ꂽ�������A�h���X
	const char *Name,
	
	// �t�@�C���I�������̕\����������W
	int x,
	int y
);

// �t�@�C���I�����������폜����
extern void ToolFileSelect_Delete(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
);

// �S�Ẵt�@�C���I�����������폜����
extern void ToolFileSelect_Delete_All( void );

// �t�@�C���I���������Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolFileSelect_SetVisibleHandle(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �\����ԏ��n���h��
	int TVisibleHandle
);

// �t�@�C���I���������Ɗ֘A�t����E�C���h�E����ݒ肷��
extern void ToolFileSelect_SetWindow(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �E�C���h�E���n���h��
	int TWindowHandle
);

// �t�@�C���I���������̕\���ʒu��ύX����
extern void ToolFileSelect_SetPosition(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �ύX��̍��W
	int x,
	int y
);

// �t�@�C���I���������̕\����Ԃ�ύX����
extern void ToolFileSelect_SetVisible(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �V�����\�����
	bool Visible
);

// �t�@�C���I���������̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolFileSelect_GetVisible(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle
);

// �t�@�C���I���������̑I���������������Ă��邩�ǂ������擾����
//     �߂�l : �������������Ă��邩�ǂ���( true:�������Ă���  false:�܂��I����Ă��Ȃ� )
extern bool ToolFileSelect_CheckEnd(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,

	// �������������Ă��邩�ǂ����̃t���O��|�����ǂ���
	bool Reset
);

// �t�@�C���I���������̃t�@�C���p�X��ݒ肷��
extern void ToolFileSelect_SetPath(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �t�@�C���p�X�����񂪊i�[���ꂽ�������̈�̐擪�A�h���X
	const char *FilePath
);

// �t�@�C���I���������̃t�@�C���p�X���擾����
extern void ToolFileSelect_GetPath(
	// �t�@�C���I���������n���h��
	int TFileSelectHandle,
	
	// �t�@�C���p�X���i�[����o�b�t�@�̐擪�A�h���X
	char *DestBuffer
);

// �S�Ẵt�@�C���I���������̏�Ԑ��ڏ������s��
extern void ToolFileSelect_Step_All( void );

// �S�Ẵt�@�C���I���������̕`��ڏ������s��
extern void ToolFileSelect_Draw_All( void );

#endif


