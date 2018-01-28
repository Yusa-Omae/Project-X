#ifndef TOOLLIB_VISIBLE_H
#define TOOLLIB_VISIBLE_H

// �\����ԏ����쐬����
//     �߂�l : �\����ԏ��n���h��( ���s�����ꍇ�� -1 ���Ԃ� )
extern int ToolVisible_Create( void );

// �\����ԏ����폜����
extern void ToolVisible_Delete(
	// �\����ԏ��n���h��
	int TVisibleHandle
);

// �S�Ă̕\����ԏ����폜����
extern void ToolVisible_Delete_All( void );

// �\����ԏ��̕\����Ԃ�ύX����
extern void ToolVisible_SetVisible(
	// �\����ԏ��n���h��
	int TVisibleHandle,
	
	// �V�����\�����
	bool Visible
);

// �\����ԏ��ɐe�̕\����ԏ���ݒ肷��
extern void ToolVisible_SetParent(
	// �\����ԏ��n���h��
	int TVisibleHandle,
	
	// �e�ƂȂ�\����ԏ��n���h��
	int ParentTVisibleHandle
);

// �\����ԏ��̕\����Ԃ��擾����( �e�̕\����Ԃ��l������ )
//     �߂�l : �\�����( true:�\��  false:��\�� )
extern bool ToolVisible_GetVisible(
	// �\����ԏ��n���h��
	int TVisibleHandle
);


#endif
