#include "Effect_Damage.h"
#include "Mathematics.h"

// �p�[�e�B�N�������ł����ō����x�ƍŒᑬ�x
#define MAX_SPEED			(750.0f)
#define MIN_SPEED			(200.0f)

// �p�[�e�B�N���̍ō��\�����ԂƍŒ�\������
#define MAX_TIME			(0.1f)
#define MIN_TIME			(0.05f)

// �p�[�e�B�N�����t�F�[�h�A�E�g���鑬�x
#define ALPHA_SPEED			(16.0f)

// �p�[�e�B�N���̍ő�T�C�Y�ƍŏ��T�C�Y
#define MAX_SIZE			(16.0f)
#define MIN_SIZE			(4.0f)

// �p�[�e�B�N���̌����̑��x
#define SUB_SPEED			(3000.0f)

// �p�[�e�B�N���̐�
#define PARTICLE_NUM		(256)

// �_���[�W�G�t�F�N�g�̃p�[�e�B�N����̏��
typedef struct _SEffect_Damage_ParticleInfo
{
	// ���W
	VECTOR                      Position;

	// �i�s����
	VECTOR                      Direction;

	// �X�s�[�h
	float                       Speed;

	// �傫��
	float                       Size;

	// �\������
	float                       VisibleTime;

	// �s�����x
	float                       Alpha;
} SEffect_Damage_ParticleInfo;

// �_���[�W�G�t�F�N�g�̏��
typedef struct _SEffect_DamageInfo
{
	// �_���[�W�G�t�F�N�g�̐F
	COLOR_U8                    Color;

	// �p�[�e�B�N���̏��
	SEffect_Damage_ParticleInfo Particle[ PARTICLE_NUM ];
} SEffect_DamageInfo;

// �_���[�W�G�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_Damage_Initialize(
	// �_���[�W�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
)
{
	// �_���[�W�G�t�F�N�g�Ŏg�p����摜�̓ǂݍ���
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\Damage.png" );

	// �摜�̓ǂݍ��݂Ɏ��s�����玸�s�I��
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// �����I��
	return true;
}

// �_���[�W�G�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Effect_Damage_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	// �G�t�F�N�g�̏��\���̂��i�[���郁�����̈�̊m��
	EInfo->SubData = malloc( sizeof( SEffect_DamageInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// ����I��
	return true;
}

// �_���[�W�G�t�F�N�g�̏�Ԑ��ڏ����֐�
void Effect_Damage_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int                          i;
	int                          ValidNum;

	// �L���ȃp�[�e�B�N���̐���������
	ValidNum = 0;

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// �s�����x���O�ȉ��̏ꍇ�͎��̃��[�v��
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// �L���ȃp�[�e�B�N���̐��𑝂₷
		ValidNum++;

		// ���x���O���傫�����ǂ����ŏ����𕪊�
		if( PInfo->Speed > 0.0f )
		{
			// ���x���O���傫��������p�[�e�B�N�����ړ�����
			PInfo->Position = VAdd( PInfo->Position,
									VScale( PInfo->Direction, PInfo->Speed * StepTime) );

			// ��������
			PInfo->Speed -= SUB_SPEED * StepTime;

			// ���x���O�ȉ��ɂȂ�����c��\�����Ԃ��O�ɂ���
			if( PInfo->Speed <= 0.0f )
			{
				PInfo->VisibleTime = 0.0f;
			}
		}

		// �\�����Ԃ��O���傫�����ǂ����ŏ����𕪊�
		if( PInfo->VisibleTime > 0.0f )
		{
			// �\�����Ԃ��O���傫��������\�����Ԃ����炷
			PInfo->VisibleTime -= StepTime;
		}
		else
		{
			// �\�����Ԃ��O�ȉ���������s�����x�����炷
			PInfo->Alpha -= ALPHA_SPEED * StepTime;
		}
	}

	// �I�����N�G�X�g������Ă��āA�L���ȃp�[�e�B�N���̐����O��������G�t�F�N�g���폜����
	if( EInfo->EndRequest || ValidNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// �_���[�W�G�t�F�N�g�̕`�揈���֐�
void Effect_Damage_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int                          i;

	// �y�o�b�t�@���g�p����ݒ�ɕύX����
	SetUseZBufferFlag( TRUE );

	// �y�o�b�t�@�ւ̏������݂͍s��Ȃ�
	SetWriteZBufferFlag( FALSE );

	// �`��F���Z�b�g
	SetDrawBright( DInfo->Color.r, DInfo->Color.g, DInfo->Color.b );

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// �s�����x���O�ȉ��̃p�[�e�B�N���͉������Ȃ�
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// ���Z�u�����h�Ńp�[�e�B�N����`��
		SetDrawBlendMode( DX_BLENDMODE_SUB, ( int )( PInfo->Alpha * 255.0f ) );
		DrawBillboard3D( PInfo->Position, 0.5f, 0.5f, PInfo->Size * PInfo->Alpha,
											0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// �s�����x���O�ȉ��̃p�[�e�B�N���͉������Ȃ�
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// ���Z�u�����h�Ńp�[�e�B�N����`��
		SetDrawBlendMode( DX_BLENDMODE_ADD_X4, ( int )( PInfo->Alpha * 255.0f ) );
		DrawBillboard3D( PInfo->Position, 0.5f, 0.5f, PInfo->Size * PInfo->Alpha,
											0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// �`��u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// �`��P�x���W���ɖ߂�
	SetDrawBright( 255,255,255 );

	// �y�o�b�t�@���g�p���Ȃ��ݒ�ɖ߂�
	SetUseZBufferFlag( FALSE );
}

// �_���[�W�G�t�F�N�g�����̏���������֐�
void Effect_Damage_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// �_���[�W�G�t�F�N�g�𔭐���������W
	VECTOR Position,

	// �_���[�W�G�t�F�N�g�̐F
	COLOR_U8 Color
)
{
	SEffect_DamageInfo          *DInfo = ( SEffect_DamageInfo * )EInfo->SubData;
	SEffect_Damage_ParticleInfo *PInfo;
	int i;

	// �G�t�F�N�g�̐F��ۑ�
	DInfo->Color = Color;

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < PARTICLE_NUM; i++, PInfo++ )
	{
		// ���W��ۑ�
		PInfo->Position    = Position;

		// �p�[�e�B�N���̔�ԕ����������_���Ō���
		PInfo->Direction.x = GetRandomFloat( 1.0f, -1.0f );
		PInfo->Direction.y = GetRandomFloat( 1.0f, -1.0f );
		PInfo->Direction.z = GetRandomFloat( 1.0f, -1.0f );

		// �p�[�e�B�N���̑��x�������_���Ō���
		PInfo->Speed       = GetRandomFloat( MAX_SPEED, MIN_SPEED );

		// �p�[�e�B�N���̕s�����x��������
		PInfo->Alpha       = 1.0f;

		// �p�[�e�B�N���̑傫���������_���Ō���
		PInfo->Size        = GetRandomFloat( MAX_SIZE, MIN_SIZE );

		// �p�[�e�B�N���̕\�����Ԃ������_���Ō���
		PInfo->VisibleTime = GetRandomFloat( MAX_TIME, MIN_TIME  );
	}
}


