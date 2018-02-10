#ifndef INPUT_H
#define INPUT_H

#include "DxLib.h"

// ���͏��
typedef enum _EInputType
{
	EInputType_Left,				// �������͍�
	EInputType_Right,				// �������͉E
	EInputType_Up,					// �������͏�
	EInputType_Down,				// �������͉�

	EInputType_Camera_Left,			// �J�����p�������͍�
	EInputType_Camera_Right,		// �J�����p�������͉E
	EInputType_Camera_Up,			// �J�����p�������͏�
	EInputType_Camera_Down,			// �J�����p�������͉�

	EInputType_Attack,				// �U���{�^��
	EInputType_Defence,				// �h��{�^��
	EInputType_Jump,				// �W�����v�{�^��
	EInputType_Pause,				// �|�[�Y�{�^��

	EInputType_Num,					// ���̓^�C�v�̐�
} EInputType;

// ���͏����̏��������s��
extern void   InputInitialize( void );

// ���͏������s��
extern void   ProcessInput( void );

// �{�^���̓��͏�Ԃ��擾����
//     �߂�l : �{�^���̓��͏��
extern int    GetInput( void );

// �{�^���̉����ꂽ�u�Ԃ݂̂̓��͏�Ԃ��擾����
//     �߂�l : �{�^���̉����ꂽ�u�Ԃ݂̂̓��͏��
extern int    GetEdgeInput( void );

// �J��������p�̃X�e�B�b�N�̓��͏�Ԃ��擾����
//     �߂�l : �J��������p�̃X�e�B�b�N�̓��͏��
extern VECTOR GetCameraStickInput( void );

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C���ɕۑ�����
extern void   SaveInputSetting( void );

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������t�@�C������ǂݍ���
//     �߂�l : �t�@�C���̓ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool   LoadInputSetting( void );

// �Q�[���ł̓��͂ƃL�[��p�b�h�Ȃǂ̓��͂Ƃ̑Ή������f�t�H���g�ݒ�ɂ���
extern void   SetDefaultSetting( void );

// ���݉�������Ă���L�[��p�b�h�̓��͏����`�F�b�N���āA���͂��������ꍇ�͏��X�V����
//     �߂�l : ���͂�����A��񂪍X�V���ꂽ���ǂ���
//              ( true:�X�V���ꂽ  false:�X�V����Ȃ����� )
extern bool   UpdateInputTypeInfo(
	// �����X�V������͏��
	EInputType UpdateInputType
);

// �w��̓��͏��^�C�v�Ɋ��蓖�Ă��Ă���p�b�h�̓��͂ƃL�[�̓��̖͂��O���擾����
extern void   GetInputTypeString(
	// ���O���擾���������͏��^�C�v
	EInputType InputType,

	// ���蓖�Ă��Ă���p�b�h���̖͂��O��������o�b�t�@�ւ̃A�h���X
	char *PadInputString,

	// ���蓖�Ă��Ă���L�[���̖͂��O��������o�b�t�@�ւ̃A�h���X
	char *KeyInputString
);


#endif
