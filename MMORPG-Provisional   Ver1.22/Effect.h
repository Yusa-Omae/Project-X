#ifndef EFFECT_H
#define EFFECT_H

// �G�t�F�N�g
typedef enum _EEffect
{
	EEffect_SlashLocus = 0,		// ���̋O��
	EEffect_Damage,				// �_���[�W
	EEffect_Dead,				// ���S
	EEffect_SphereLocus,		// ���̋O��

	EEffect_Num					// �G�t�F�N�g�̐�
} EEffect;

// �G�t�F�N�g�̊�{���
typedef struct _SEffectBaseInfo
{
	// �G�t�F�N�g���g�p����摜�̃n���h��
	int              GraphHandle;
} SEffectBaseInfo;

// �G�t�F�N�g�̏��
typedef struct _SEffectInfo
{
	// ���̍\���̂��g�p���Ă��邩�ǂ����̃t���O
	bool             UseFlag;

	// �G�t�F�N�g�̏I�����N�G�X�g������Ă��邩�ǂ����̃t���O
	bool             EndRequest;

	// �G�t�F�N�g
	EEffect          Effect;

	// �G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *BaseInfo;

	// �G�t�F�N�g�ʂ̃f�[�^���i�[���Ă��郁�����̈�̃A�h���X
	void *           SubData;
} SEffectInfo;

// �G�t�F�N�g�����̏�����������
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Effect_Initialize( void );

// �G�t�F�N�g�����̌�n��������
extern void Effect_Terminate( void );

// �G�t�F�N�g���쐬����
//     �߂�l : �쐬�����G�t�F�N�g�̏��\���̂̃A�h���X
extern SEffectInfo *Effect_Create(
	// �쐬����G�t�F�N�g
	EEffect Effect
);

// �G�t�F�N�g�ɑ΂��ďI�����N�G�X�g������
extern void Effect_EndRequest(
	// �G�t�F�N�g�̏��\���̂̃A�h���X
	SEffectInfo *EInfo
);

// �G�t�F�N�g���폜����
extern void Effect_Delete(
	// �G�t�F�N�g�̏��\���̂̃A�h���X
	SEffectInfo *EInfo
);

// �S�ẴG�t�F�N�g���폜����
extern void Effect_DeleteAll( void );

// �S�ẴG�t�F�N�g�̏�Ԑ��ڏ��������s����
extern void Effect_StepAll(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �S�ẴG�t�F�N�g�̕`�揈�������s����
extern void Effect_RenderAll( void );


#endif
