#include "Effect_Dead.h"
#include "Mathematics.h"
#include <math.h>

// �|���S���̖ʐςɑ΂���p�[�e�B�N���쐮��
#define CREATE_RATE			(0.025f)

// �p�[�e�B�N���㏸�҂����Ԃ̍ő�ƍŏ�
#define MAX_UPWAIT			(1.0f)
#define MIN_UPWAIT			(0.01f)

// �p�[�e�B�N���㏸���x�̍ő�ƍŏ�
#define MAX_UPSPEED			(400.0f)
#define MIN_UPSPEED			(100.0f)

// �p�[�e�B�N���̏㏸�����x
#define UPACCEL				(500.0f)

// �p�[�e�B�N����]�^�����x�̍ő�ƍŏ�
#define MAX_ANGLESPEED		(6.0f)
#define MIN_ANGLESPEED		(1.5f)

// �p�[�e�B�N�����W����̐����ő勗���̍ő�ƍŏ�
#define MAX_H_DISTANCE		(100.0f)
#define MIN_H_DISTANCE		(20.0f)

// �p�[�e�B�N�����W���琅�������ɗ���Ă������x
#define H_DISTANCE_SPEED	(200.0f)

// �p�[�e�B�N���̍ő�\�����Ԃƍŏ��\������
#define MAX_VISIBLETIME		(0.5f)
#define MIN_VISIBLETIME		(0.2f)

// �p�[�e�B�N���̕s�����x�㏸���x�ƌ������x
#define ALPHA_UP_SPEED		(16.0f)
#define ALPHA_DOWN_SPEED	(1.0f)

// �p�[�e�B�N���̍ő�T�C�Y�ƍŏ��T�C�Y
#define MAX_SIZE			(16.0f)
#define MIN_SIZE			(4.0f)

// �p�[�e�B�N���̔������̊g�呬�x
#define SIZERATE_SPEED		(3.0f)

// �v�Z�덷���l�����ė]���Ɋm�ۂ���p�[�e�B�N�����̐�
#define ADD_PARTICLE_NUM	(10)


// ���S�G�t�F�N�g�̃|���S����̏��
typedef struct _SEffect_Dead_PolygonInfo
{
	// �|���S���ʐ�
	float                      AreaSize;

	// �|���S���̒��_�O���璸�_�P�ւ̃x�N�g��
	VECTOR                     Vec1;

	// �|���S���̒��_�O���璸�_�Q�ւ̃x�N�g��
	VECTOR                     Vec2;

	// �|���S���̒��_�O�̍��W
	VECTOR                     Pos;
} SEffect_Dead_PolygonInfo;

// ���S�G�t�F�N�g�̃p�[�e�B�N����̏��
typedef struct _SEffect_Dead_ParticleInfo
{
	// ���W
	VECTOR                     Position;

	// �㏸�҂�����
	float                      UpWait;

	// �㏸�ő呬�x
	float                      UpMaxSpeed;

	// �㏸���x
	float                      UpSpeed;

	// �p�x
	float                      Angle;

	// �p���x
	float                      AngleSpeed;

	// �p�[�e�B�N�����W����̍ő吅������
	float                      H_DistanceMax;

	// �p�[�e�B�N�����W����̐�������
	float                      H_Distance;

	// �\������
	float                      VisibleTime;

	// �s�����x
	float                      Alpha;

	// �傫��
	float                      Size;

	// �傫����
	float                      SizeRate;
} SEffect_Dead_ParticleInfo;

// ���S�G�t�F�N�g�̏��
typedef struct _SEffect_DeadInfo
{
	// �G�t�F�N�g�̐F
	COLOR_U8                   Color;

	// �p�[�e�B�N���̐�
	int                        ParticleNum;

	// �p�[�e�B�N���̏����i�[���Ă��郁�����̈�̃A�h���X
	SEffect_Dead_ParticleInfo *Particle;
} SEffect_DeadInfo;

static int                       g_PolyInfoBufferSize;
static SEffect_Dead_PolygonInfo *g_PolyInfoBuffer;

// ���S�G�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_Dead_Initialize(
	// ���S�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
)
{
	// ���S�G�t�F�N�g�Ŏg�p����摜�̓ǂݍ���
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\Dead.png" );

	// �摜�̓ǂݍ��݂Ɏ��s�����玸�s�I��
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// �����I��
	return true;
}

// ���S�G�t�F�N�g�̊�{����n���֐�
void Effect_Dead_Terminate(
	// ���S�G�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
)
{
	// �|���S�����i�[�p�Ɋm�ۂ��Ă����������̈���������
	if( g_PolyInfoBuffer != NULL )
	{
		free( g_PolyInfoBuffer );
		g_PolyInfoBuffer = NULL;
	}
}

// ���S�G�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Effect_Dead_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo *DInfo;

	// �G�t�F�N�g�̏��\���̂��i�[���郁�����̈�̊m��
	EInfo->SubData = malloc( sizeof( SEffect_DeadInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	DInfo = ( SEffect_DeadInfo * )EInfo->SubData;

	// �p�[�e�B�N���̏����i�[���郁�����̈�̃A�h���X��ۑ�����ϐ�������������
	DInfo->Particle    = NULL;

	// ����I��
	return true;
}

// ���S�G�t�F�N�g���폜���ꂽ�ۂɌĂ΂��֐�
void Effect_Dead_Delete(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;

	// �p�[�e�B�N���̏����i�[���Ă����������̈���������
	if( DInfo->Particle != NULL )
	{
		free( DInfo->Particle );
		DInfo->Particle = NULL;
	}
}

// ���S�G�t�F�N�g�̏�Ԑ��ڏ����֐�
void Effect_Dead_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	int                        ValidNum;

	// �L���ȃp�[�e�B�N���̐���������
	ValidNum = 0;

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < DInfo->ParticleNum; i++, PInfo++ )
	{
		// �s�����x���O�ȉ��̏ꍇ�͎��̃��[�v��
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// �L���ȃp�[�e�B�N���̐��𑝂₷
		ValidNum++;

		// �傫������ 1.0f �����̏ꍇ�͑傫�������グ��
		if( PInfo->SizeRate < 0.99999f )
		{
			PInfo->SizeRate += StepTime * SIZERATE_SPEED;
			if( PInfo->SizeRate > 1.0f )
			{
				PInfo->SizeRate = 1.0f;
			}
		}
		else
		{
			// �p�x��ύX
			PInfo->Angle += StepTime * PInfo->AngleSpeed;

			// �\���Ԃ��O���傫�����ǂ����ŏ����𕪊�
			if( PInfo->VisibleTime > 0.0f )
			{
				// �\�����Ԃ��O���傫���ꍇ�͕\�����Ԃ����炷
				PInfo->VisibleTime -= StepTime;
			}
			else
			{
				// �\�����Ԃ��O�ȉ���������s�����x�����炷
				PInfo->Alpha -= StepTime * ALPHA_DOWN_SPEED;
			}

			// �㏸�҂����Ԃ��O���傫�����ǂ����ŏ����𕪊�
			if( PInfo->UpWait > 0.0f )
			{
				// �㏸�҂����Ԃ��O���傫��������㏸�҂����Ԃ����炷
				PInfo->UpWait -= StepTime;
			}
			else
			{
				// �㏸�҂����Ԃ��O�ȉ��̏ꍇ

				// �㏸���x���グ��
				if( PInfo->UpSpeed < PInfo->UpMaxSpeed )
				{
					PInfo->UpSpeed += StepTime * UPACCEL;
					if( PInfo->UpSpeed > PInfo->UpMaxSpeed )
					{
						PInfo->UpSpeed = PInfo->UpMaxSpeed;
					}
				}

				// �p�[�e�B�N�����㏸������
				PInfo->Position.y += PInfo->UpSpeed * StepTime;

				// �p�[�e�B�N�����W����̐��������̋��������Z
				if( PInfo->H_Distance < PInfo->H_DistanceMax )
				{
					PInfo->H_Distance += StepTime * H_DISTANCE_SPEED;
					if( PInfo->H_Distance > PInfo->H_DistanceMax )
					{
						PInfo->H_Distance = PInfo->H_DistanceMax;
					}
				}
			}
		}
	}

	// �I�����N�G�X�g������Ă��āA�L���ȃp�[�e�B�N���̐����O��������G�t�F�N�g���폜����
	if( EInfo->EndRequest || ValidNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// ���S�G�t�F�N�g�̕`�揈���֐�
void Effect_Dead_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	VECTOR                     DrawPos;

	// �y�o�b�t�@���g�p����ݒ�ɕύX����
	SetUseZBufferFlag( TRUE );

	// �y�o�b�t�@�ւ̏������݂͍s��Ȃ�
	SetWriteZBufferFlag( FALSE );

	// �`��F���Z�b�g
	SetDrawBright( DInfo->Color.r, DInfo->Color.g, DInfo->Color.b );

	// �p�[�e�B�N���̐������J��Ԃ�
	PInfo = DInfo->Particle;
	for( i = 0; i < DInfo->ParticleNum; i++, PInfo++ )
	{
		// �s�����x���O�ȉ��̃p�[�e�B�N���͉������Ȃ�
		if( PInfo->Alpha <= 0.0f )
		{
			continue;
		}

		// ���Z�u�����h�ɐݒ�
		SetDrawBlendMode( DX_BLENDMODE_SUB, ( int )( PInfo->Alpha * 255.0f ) );

		// �p�[�e�B�N����`�悷����W�̎Z�o
		DrawPos = PInfo->Position;
		DrawPos.x += sin( PInfo->Angle ) * PInfo->H_Distance;
		DrawPos.z += cos( PInfo->Angle ) * PInfo->H_Distance;

		// �p�[�e�B�N����`��
		DrawBillboard3D( DrawPos, 0.5f, 0.5f, PInfo->Size * PInfo->SizeRate,
									0.0f, EInfo->BaseInfo->GraphHandle, TRUE );
	}

	// �`��u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// �`��P�x���W���ɖ߂�
	SetDrawBright( 255,255,255 );

	// �y�o�b�t�@���g�p���Ȃ��ݒ�ɖ߂�
	SetUseZBufferFlag( FALSE );
}

// ���S�G�t�F�N�g�����̏���������֐�
//     �߂�l : �����������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_Dead_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,

	// ���S�G�t�F�N�g�����s����R�c���f���̃n���h��
	int ModelHandle,

	// ���S�G�t�F�N�g�̐F
	COLOR_U8 Color
)
{
	SEffect_DeadInfo          *DInfo = ( SEffect_DeadInfo * )EInfo->SubData;
	SEffect_Dead_ParticleInfo *PInfo;
	int                        i;
	float                      CreateNum;
	VECTOR                    *Pos2;
	VECTOR                    *Pos3;
	MV1_REF_POLYGONLIST        RefMesh;
	SEffect_Dead_PolygonInfo  *PolyInfo;
	float                      TotalAreaSize;
	int                        TempParticleNum;
	float                      Rate1;
	float                      Rate2;

	// �G�t�F�N�g�̐F��ۑ�
	DInfo->Color = Color;

	// �G�t�F�N�g�����s����R�c���f���̎Q�Ɨp���b�V�����Z�b�g�A�b�v
	MV1RefreshReferenceMesh( ModelHandle, -1, TRUE, TRUE );

	// �Q�Ɨp���b�V�����擾
	RefMesh = MV1GetReferenceMesh( ModelHandle, -1, TRUE, TRUE );

	// �|���S���̐��������Ċ��Ɋm�ۂ��Ă���|���S�����i�[�p�̃������̈��
	// ���肫��Ȃ��ꍇ�̓������̈���Ċm�ۂ���
	if( g_PolyInfoBufferSize < RefMesh.PolygonNum )
	{
		// ���Ɋm�ۂ��Ă����������̈悪����ꍇ�͉��
		if( g_PolyInfoBuffer != NULL )
		{
			free( g_PolyInfoBuffer );
			g_PolyInfoBuffer = NULL;
		}

		// �|���S�����i�[�p�̃������̈���m��
		g_PolyInfoBuffer = ( SEffect_Dead_PolygonInfo * )malloc(
							sizeof( SEffect_Dead_PolygonInfo ) * RefMesh.PolygonNum );

		// �������̈�̊m�ۂɎ��s�����ꍇ�͎��s�I��
		if( g_PolyInfoBuffer == NULL )
		{
			return false;
		}

		// �m�ۂ����������̈�ɓ���|���S�����̐���ۑ�
		g_PolyInfoBufferSize = RefMesh.PolygonNum;
	}

	// �|���S���̏����\�z����
	PolyInfo   = g_PolyInfoBuffer;
	TotalAreaSize  = 0.0f;
	for( i = 0; i < RefMesh.PolygonNum; i++, PolyInfo++ )
	{
		// �|���S���̍��W���擾
		PolyInfo->Pos  = RefMesh.Vertexs[  RefMesh.Polygons[ i ].VIndex[ 0 ] ].Position;
		Pos2           = &RefMesh.Vertexs[ RefMesh.Polygons[ i ].VIndex[ 1 ] ].Position;
		Pos3           = &RefMesh.Vertexs[ RefMesh.Polygons[ i ].VIndex[ 2 ] ].Position;

		// �|���S���̍��W�O������W�P����W�Q�ւ̃x�N�g�����Z�o
		PolyInfo->Vec1 = VSub( *Pos2, PolyInfo->Pos );
		PolyInfo->Vec2 = VSub( *Pos3, PolyInfo->Pos );

		// �|���S���̖ʐς��Z�o
		PolyInfo->AreaSize = VSize( VCross( PolyInfo->Vec1, PolyInfo->Vec2 ) ) / 2.0f;

		// �|���S���̖ʐς̍��v�ɉ��Z
		TotalAreaSize += PolyInfo->AreaSize;
	}

	// �m�ۂ��郁�����̈�Ɋi�[����p�[�e�B�N�����p�̐����Z�o
	TempParticleNum = ( int )( TotalAreaSize * CREATE_RATE ) + ADD_PARTICLE_NUM;

	// �p�[�e�B�N�������i�[���郁�����̈�̊m��
	DInfo->Particle    = ( SEffect_Dead_ParticleInfo * )malloc(
								TempParticleNum * sizeof( SEffect_Dead_ParticleInfo ) );

	// �������̈�̊m�ۂɎ��s������G���[�I��
	if( DInfo->Particle == NULL )
	{
		return false;
	}

	// �|���S���̐������J��Ԃ�
	CreateNum = 0.0f;
	PInfo      = DInfo->Particle;
	PolyInfo   = g_PolyInfoBuffer;
	DInfo->ParticleNum = 0;
	for( i = 0; i < RefMesh.PolygonNum; i++, PolyInfo++ )
	{
		// �p�[�e�B�N���쐬���J�E���^�Ƀ|���S���ʐς�����̃p�[�e�B�N���̐������Z����
		CreateNum += PolyInfo->AreaSize * CREATE_RATE;

		// �p�[�e�B�N���쐬���J�E���^��1.0f�ȏ�Ȃ烋�[�v
		while( CreateNum >= 1.0f )
		{
			// �p�[�e�B�N���쐬���J�E���^����1.0f������
			CreateNum        -= 1.0f;

			// �p�[�e�B�N���̍��W���|���S���\�ʏォ�烉���_���Ō���
			Rate1              = GetRandomFloat( 1.0f,         0.0f );
			Rate2              = GetRandomFloat( 1.0f - Rate1, 0.0f );
			PInfo->Position    = VAdd( PolyInfo->Pos,
				VAdd( VScale( PolyInfo->Vec1, Rate1 ), VScale( PolyInfo->Vec2, Rate2 )));

			// �p�[�e�B�N���̏㏸�҂����Ԃ������_���Ō���
			PInfo->UpWait      = GetRandomFloat( MAX_UPWAIT,  MIN_UPWAIT  );

			// �p�[�e�B�N���̏㏸�ő呬�x�������_���Ō���
			PInfo->UpMaxSpeed  = GetRandomFloat( MAX_UPSPEED, MIN_UPSPEED );

			// �p�[�e�B�N���̏㏸���x��������
			PInfo->UpSpeed     = 0.0f;

			// �p�[�e�B�N���̏����p�x�������_���Ō���
			PInfo->Angle       = GetRandomFloat( DX_TWO_PI_F,    0.0f );

			// �p�[�e�B�N���̊p���x�������_���Ō���
			PInfo->AngleSpeed  = GetRandomFloat( MAX_ANGLESPEED, MIN_ANGLESPEED );

			// �����̊m���Ŋp���x�̌����𔽓]
			if( GetRand( 100 ) < 50 )
			{
				PInfo->AngleSpeed = -PInfo->AngleSpeed;
			}

			// �p�[�e�B�N�����W����̍ő吅�������������_���Ō���
			PInfo->H_DistanceMax = GetRandomFloat( MAX_H_DISTANCE, MIN_H_DISTANCE );

			// �p�[�e�B�N�����W����̐���������������
			PInfo->H_Distance    = 0.0f;

			// �p�[�e�B�N���̕\�����Ԃ������_���Ō���
			PInfo->VisibleTime = GetRandomFloat( MAX_VISIBLETIME, MIN_VISIBLETIME );

			// �p�[�e�B�N���̕s�����x��������
			PInfo->Alpha       = 1.0f;

			// �p�[�e�B�N���̑傫���������_���Ō���
			PInfo->Size        = GetRandomFloat( MAX_SIZE, MIN_SIZE );

			// �p�[�e�B�N���̑傫������������
			PInfo->SizeRate    = 0.0f;

			// �|�C���^�̃A�h���X���p�[�e�B�N������i�߂�
			PInfo++;

			// �p�[�e�B�N���̐�������₷
			DInfo->ParticleNum++;
		}
	}

	// �����I��
	return true;
}


