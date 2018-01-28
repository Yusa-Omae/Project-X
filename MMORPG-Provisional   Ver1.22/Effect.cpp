#include "Effect.h"
#include "Effect_SlashLocus.h"
#include "Effect_Damage.h"
#include "Effect_Dead.h"
#include "Effect_SphereLocus.h"

// ���������邱�Ƃ̂ł���G�t�F�N�g�̍ő吔
#define EFFECT_MAXNUM			(256)

// �G�t�F�N�g�̊�{���
static SEffectBaseInfo g_EffectBaseInfo[ EEffect_Num ];

// �G�t�F�N�g�̏��
static SEffectInfo     g_EffectInfo[ EFFECT_MAXNUM ];

// �G�t�F�N�g�ʂ̏����𕪂��邽�߂̃R�[���o�b�N�֐����
typedef struct _SEffectFunction
{
	// ���������ɌĂ΂��֐��ւ̃|�C���^
	bool ( *Initialize )( SEffectBaseInfo *EBInfo );

	// ��n�����ɌĂ΂��֐��ւ̃|�C���^
	void ( *Terminate  )( SEffectBaseInfo *EBInfo );

	// �쐬���ɌĂ΂��֐��ւ̃|�C���^
	bool ( *Create     )( SEffectInfo *    EInfo );

	// �폜���ɌĂ΂��֐��ւ̃|�C���^
	void ( *Delete     )( SEffectInfo *    EInfo );

	// ��Ԑ��ڎ��ɌĂ΂��֐��ւ̃|�C���^
	void ( *Step       )( SEffectInfo *    EInfo, float StepTime );

	// �`�掞�ɌĂ΂��֐��ւ̃|�C���^
	void ( *Render     )( SEffectInfo *    EInfo );
} SEffectFunction;

// �G�t�F�N�g�ʂ̏����𕪂��邽�߂̃R�[���o�b�N�֐��e�[�u��
static SEffectFunction g_EffectFunctionTable[ EEffect_Num ] =
{
	//EEffect_SlashLocus
	{ Effect_SlashLocus_Initialize,  NULL,                    Effect_SlashLocus_Create,
	  NULL,                          Effect_SlashLocus_Step,  Effect_SlashLocus_Render },

	// EEffect_Damage
	{ Effect_Damage_Initialize,      NULL,                    Effect_Damage_Create,
	  NULL,                          Effect_Damage_Step,      Effect_Damage_Render     },

	// EEffect_Dead
	{ Effect_Dead_Initialize,        Effect_Dead_Terminate,   Effect_Dead_Create,
	  Effect_Dead_Delete,            Effect_Dead_Step,        Effect_Dead_Render       },

	// EEffect_SphereLocus
	{ Effect_SphereLocus_Initialize, NULL,                    Effect_SphereLocus_Create,
	  NULL,                          Effect_SphereLocus_Step, Effect_SphereLocus_Render},
};

// �G�t�F�N�g�����̏�����������
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_Initialize( void )
{
	int              i;
	SEffectInfo     *EInfo;
	SEffectBaseInfo *EBInfo;

	// �G�t�F�N�g�̐������J��Ԃ�
	EBInfo = g_EffectBaseInfo;
	for( i = 0; i < EEffect_Num; i++, EBInfo++ )
	{
		// �G�t�F�N�g�Ŏg�p����摜�n���h���̒l��������
		EBInfo->GraphHandle = -1;

		// �G�t�F�N�g�ʂ̏������֐�������ꍇ�͌Ă�
		if( g_EffectFunctionTable[ i ].Initialize != NULL )
		{
			if( !g_EffectFunctionTable[ i ].Initialize( EBInfo ) )
			{
				return false;
			}
		}
	}

	// �S�ẴG�t�F�N�g���\���̂́w�g�p�����ǂ����x�̃t���O��|��
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		EInfo->UseFlag = false;
	}

	// �����I��
	return true;
}

// �G�t�F�N�g�����̌�n��������
void Effect_Terminate( void )
{
	int              i;
	SEffectBaseInfo *EBInfo;

	// �G�t�F�N�g��S�č폜����
	Effect_DeleteAll();

	// �G�t�F�N�g�̐������J��Ԃ�
	EBInfo = g_EffectBaseInfo;
	for( i = 0; i < EEffect_Num; i++, EBInfo++ )
	{
		// �G�t�F�N�g�ʂ̌�n���֐�������ꍇ�͌Ă�
		if( g_EffectFunctionTable[ i ].Terminate != NULL )
		{
			g_EffectFunctionTable[ i ].Terminate( EBInfo );
		}

		// �G�t�F�N�g�Ŏg�p���Ă����摜�n���h�����폜
		if( EBInfo->GraphHandle != -1 )
		{
			DeleteGraph( EBInfo->GraphHandle );
			EBInfo->GraphHandle = -1;
		}
	}
}

// �G�t�F�N�g���쐬����
//     �߂�l : �쐬�����G�t�F�N�g�̏��\���̂̃A�h���X
SEffectInfo *Effect_Create(
	// �쐬����G�t�F�N�g
	EEffect Effect
)
{
	int          i;
	SEffectInfo *EInfo;

	// �g�p����Ă��Ȃ��G�t�F�N�g���\���̂�T��
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			break;
		}
	}

	// �S�Ă̍\���̂��g�p����Ă��܂��Ă�����G���[
	if( i == EFFECT_MAXNUM )
	{
		return NULL;
	}

	// �\���̂��w�g�p���x�ɂ���
	EInfo->UseFlag    = true;

	// �I�����N�G�X�g������Ă��邩�ǂ����̃t���O��|��
	EInfo->EndRequest = false;

	// ���̃G�t�F�N�g����ۑ�
	EInfo->Effect   = Effect;

	// �G�t�F�N�g�̊�{���̃A�h���X���Z�b�g
	EInfo->BaseInfo   = &g_EffectBaseInfo[ Effect ];

	// �G�t�F�N�g�ʂ̃f�[�^���i�[���Ă��郁�����̈�̃A�h���X��ۑ�����|�C���^��������
	EInfo->SubData    = NULL;

	// �G�t�F�N�g�ʂ̍쐬���ɌĂԊ֐��̌Ăяo��
	if( !g_EffectFunctionTable[ Effect ].Create( EInfo ) )
	{
		return NULL;
	}

	// �����I��
	return EInfo;
}

// �G�t�F�N�g�ɑ΂��ďI�����N�G�X�g������
void Effect_EndRequest(
	// �G�t�F�N�g�̏��\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	// �I�����N�G�X�g������Ă��邩�ǂ����̃t���O�𗧂Ă�
	EInfo->EndRequest = true;
}

// �G�t�F�N�g���폜����
void Effect_Delete(
	// �G�t�F�N�g�̏��\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	// ���ɍ\���̂��g�p����Ă��Ȃ������牽�������ɏI��
	if( !EInfo->UseFlag )
	{
		return;
	}

	// �폜���Ɏ��s����֐������݂���ꍇ�͎��s����
	if( g_EffectFunctionTable[ EInfo->Effect ].Delete != NULL )
	{
		g_EffectFunctionTable[ EInfo->Effect ].Delete( EInfo );
	}

	// �G�t�F�N�g�ʂ̏�񂪂������ꍇ�̓��������������
	if( EInfo->SubData != NULL )
	{
		free( EInfo->SubData );
		EInfo->SubData = NULL;
	}

	// �\���̂��g�p���Ă��邩�ǂ����̃t���O��|��
	EInfo->UseFlag = false;
}

// �S�ẴG�t�F�N�g���폜����
void Effect_DeleteAll( void )
{
	int          i;
	SEffectInfo *EInfo;

	// �S�Ă̍\���̂ɑ΂��č폜�������s��
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		Effect_Delete( EInfo );
	}
}

// �S�ẴG�t�F�N�g�̏�Ԑ��ڏ��������s����
void Effect_StepAll(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	int          i;
	SEffectInfo *EInfo;

	// �S�Ă̗L���ȃG�t�F�N�g�ɑ΂��ď�Ԑ��ڏ������s��
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			continue;
		}

		g_EffectFunctionTable[ EInfo->Effect ].Step( EInfo, StepTime );
	}
}

// �S�ẴG�t�F�N�g�̕`�揈�������s����
void Effect_RenderAll( void )
{
	int          i;
	SEffectInfo *EInfo;

	// �S�Ă̗L���ȃG�t�F�N�g�ɑ΂��ď�`�揈�����s��
	EInfo = g_EffectInfo;
	for( i = 0; i < EFFECT_MAXNUM; i++, EInfo++ )
	{
		if( !EInfo->UseFlag )
		{
			continue;
		}

		g_EffectFunctionTable[ EInfo->Effect ].Render( EInfo );
	}
}
