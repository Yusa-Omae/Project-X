#ifndef TOOLLIB_LIST_H
#define TOOLLIB_LIST_H

// ���X�g�����쐬����
//     �߂�l : ���X�g���n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolList_Create( void );

// ���X�g�����폜����
extern void ToolList_Delete(
	// ���X�g���n���h��
	int TListHandle
);

// �S�Ẵ��X�g�����폜����
extern void ToolList_Delete_All( void );

// ���X�g���Ɋ֘A�t����\����ԏ����Z�b�g����
extern void ToolList_SetVisibleHandle(
	// ���X�g���n���h��
	int TListHandle,

	// �\����ԏ��n���h��
	int TVisibleHandle
);

// ���X�g��������������
extern void ToolList_Initialize(
	// ���X�g���n���h��
	int TListHandle,
	
	// �֘A�t����E�C���h�E���n���h��
	int TWindowHandle
);

// ���X�g���̑I������Ă���s�̕ύX�Ȃǂ������������ǂ������擾����
//     �߂�l : �ύX�������������ǂ���( true:�ύX����������  false:�ύX�͔������Ă��Ȃ� )
extern bool ToolList_GetChange(
	// ���X�g���n���h��
	int TListHandle,

	// �ύX�������������ǂ����̏������Z�b�g���邩�ǂ���
	bool Reset
);

// ���X�g���ɍs����ǉ�����
//     �߂�l : ����ɏ������I���������ǂ���( true:����I��  false:�G���[���� )
extern bool ToolList_AddObj(
	// ���X�g���n���h��
	int TListHandle,

	// �ǉ�����s�ɕ\�����镶����
	const char *String,
	
	// �ό�����
	...
);

// ���X�g���̕\����Ԃ�ύX����
extern void ToolList_SetVisible(
	// ���X�g���n���h��
	int TListHandle,
	
	// �V�����\�����
	bool Visible
);

// ���X�g���̕\����Ԃ��擾����
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolList_GetVisible(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g���̑I���s��ύX����
extern void ToolList_SetSelectIndex(
	// ���X�g���n���h��
	int TListHandle,
	
	// �s�ԍ�
	int Index
);

// ���X�g���̑I�����Ă���s���擾����
//     �߂�l : �I�����Ă���s�̔ԍ�( �ǂ̍s���I������Ă��Ȃ��ꍇ�� -1 )
extern int ToolList_GetSelectIndex(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g���ɒǉ������s�̐����擾����
extern int ToolList_GetObjNum(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g���ɒǉ������S�Ă̍s�����킹���c�����擾����
//     �߂�l : ���X�g�̑S�s�̏c��( �s�N�Z���P�� )
extern int ToolList_GetHeight(
	// ���X�g���n���h��
	int TListHandle
);

// ���X�g���ɒǉ������S�Ă̍s�����킹���������擾����
//     �߂�l : ���X�g�̑S�s�̉���( �s�N�Z���P�� )
extern int ToolList_GetWidth(
	// ���X�g���n���h��
	int TListHandle
);

// �S�Ẵ��X�g���̏�Ԑ��ڏ������s��
extern void ToolList_Step_All( void );

// �S�Ẵ��X�g���̕`��ڏ������s��
extern void ToolList_Draw_All( void );

#endif

