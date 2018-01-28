#include "Effect_SphereLocus.h"
#include "Mathematics.h"
#include <math.h>

// �O�Ղ�\�����鎞��
#define DISPLAY_TIME		(0.15f)

// �O�Ղ��`��������W�̊Ԃ��Ԋ֐��ŕ������鐔
#define UNITVERTNUM			(4)

// ����\�����钸�_�̐�
#define CIRCLEVERTNUM		(16)

// �O�Ղ��`��������W�̐�
#define POSITIONNUM			(32)

// ���̋O�Ղ��`��������W�̏��
typedef struct _SEffect_SphereLocus_Position
{
	// ���̒��S���W
	VECTOR                       CenterPos;

	// ���̌��������肷�邽�߂̍��W
	VECTOR                       DirectionPos;

	// �s�����x
	float                        Alpha;
} SEffect_SphereLocus_Position;

// ���̋O�ՃG�t�F�N�g�̏��
typedef struct _SEffect_SphereLocusInfo
{
	// �G�t�F�N�g�̐F
	COLOR_U8                     Color; 

	// ���̑傫��
	float                        SphereSize;

	// ���̋O�Ղ��`������L���ȍ��W�̐�
	int                          PositionNum;

	// ���̋O�Ղ��`��������W�̏��
	SEffect_SphereLocus_Position Position[ POSITIONNUM ];
} SEffect_SphereLocusInfo;

// ���̕`��Ŏg�p���� Sin, Cos �̒l��\�ߌv�Z���Ċi�[���Ă������߂̔z��
static float g_EffSphereSinCos[ CIRCLEVERTNUM ][ 2 ];

// ���̋O�ՃG�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_SphereLocus_Initialize(
	// ���̋O�ՃG�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
)
{
	int i;

	// ���̋O�ՃG�t�F�N�g�Ŏg�p����摜�̓ǂݍ���
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\SphereLocus.png" );

	// �摜�̓ǂݍ��݂Ɏ��s�����玸�s�I��
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// ���̒��_���W�`���Ɏg�p���� Sin, Cos �̒l��\�ߎZ�o���Ă���
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		g_EffSphereSinCos[ i ][ 0 ] = ( float )cos( DX_TWO_PI_F / CIRCLEVERTNUM * i );
		g_EffSphereSinCos[ i ][ 1 ] = ( float )sin( DX_TWO_PI_F / CIRCLEVERTNUM * i );
	}

	// �����I��
	return true;
}

// ���̋O�ՃG�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Effect_SphereLocus_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	// �G�t�F�N�g�̏��\���̂��i�[���郁�����̈�̊m��
	EInfo->SubData = malloc( sizeof( SEffect_SphereLocusInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// ����I��
	return true;
}

// ���̋O�ՃG�t�F�N�g�̏�Ԑ��ڏ����֐�
void Effect_SphereLocus_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	int                      i;
	int                      PosNum;

	// �ǉ�����Ă�����W�̐������J��Ԃ�
	PosNum = SInfo->PositionNum;
	for( i = 0; i < PosNum; i++ )
	{
		// ���W�̕s�����x������������
		SInfo->Position[ i ].Alpha -= StepTime * ( 1.0f / DISPLAY_TIME );

		// ���W�̕s�����x���O�ȉ��ɂȂ��Ă�����L���ȍ��W�̐������炷
		if( SInfo->Position[ i ].Alpha <= 0.0f )
		{
			SInfo->Position[ i ].Alpha = 0.0f;
			SInfo->PositionNum--;
		}
	}

	// �I�����N�G�X�g������Ă��āA�L���ȍ��W�̐����O��������G�t�F�N�g���폜����
	if( EInfo->EndRequest && SInfo->PositionNum == 0 )
	{
		Effect_Delete( EInfo );
	}
}

// ���̋O�ՃG�t�F�N�g�̕`�揈���֐�
void Effect_SphereLocus_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	static VERTEX3D       VertexBuffer[ 
							1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) +
							( POSITIONNUM - 1 ) * ( CIRCLEVERTNUM * UNITVERTNUM ) +
							CIRCLEVERTNUM ];
	static unsigned short IndexBuffer[ 
							CIRCLEVERTNUM * 3 +
							CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) * 6 +
							( POSITIONNUM - 1 ) * ( CIRCLEVERTNUM * 6 * UNITVERTNUM ) ];
	VECTOR                CatmullRomDestPos[ 2][(POSITIONNUM - 1) * UNITVERTNUM + 1 + 1];
	VECTOR                CatmullRomSrcPos[ 2 ][ POSITIONNUM ];
	float                 ScalingSinCos[ CIRCLEVERTNUM ][ 2 ];
	VERTEX3D             *VertP;
	unsigned short       *IndexP;
	int                   i;
	int                   j;
	int                   k;
	int                   l;
	int                   m;
	COLOR_U8              DifColor;
	COLOR_U8              SpcColor;
	VECTOR                Normal;
	VECTOR                xvec;
	VECTOR                yvec;
	VECTOR                zvec;
	int                   p2ind;
	float                 alpha;
	float                 x, y, z;
	float                 xd, yd, zd;

	// ���̋O�Ղ��`������L���ȍ��W���Q��菭�Ȃ��ꍇ�͉��������ɏI��
	if( SInfo->PositionNum < 2 )
	{
		return;
	}

	// ���̃T�C�Y�ɃX�P�[�����O���� Sin, Cos �̒l���Z�o
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		ScalingSinCos[ i ][ 0 ] = g_EffSphereSinCos[ i ][ 0 ] * SInfo->SphereSize;
		ScalingSinCos[ i ][ 1 ] = g_EffSphereSinCos[ i ][ 1 ] * SInfo->SphereSize;
	}

	// Catmull-Rom��Ԃ��g�p���č��W�ƍ��W�̊Ԃ����炩�Ɍq�����W���Z�o
	for( i = 0; i < SInfo->PositionNum; i++ )
	{
		CatmullRomSrcPos[ 0 ][ i ] = SInfo->Position[ i ].CenterPos;
		CatmullRomSrcPos[ 1 ][ i ] = SInfo->Position[ i ].DirectionPos;
	}
	GetCatmullRomPosList( CatmullRomSrcPos[ 0 ], SInfo->PositionNum, UNITVERTNUM,
														CatmullRomDestPos[ 0 ] );
	GetCatmullRomPosList( CatmullRomSrcPos[ 1 ], SInfo->PositionNum, UNITVERTNUM,
														CatmullRomDestPos[ 1 ] );

	CatmullRomDestPos[ 0 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1 ] =
							SInfo->Position[ SInfo->PositionNum - 1 ].CenterPos;
	CatmullRomDestPos[ 1 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1 ] =
							SInfo->Position[ SInfo->PositionNum - 1 ].DirectionPos;

	// ���̋O�Ղ��`������|���S���̒��_�f�[�^���쐬
	DifColor     = SInfo->Color;
	SpcColor     = GetColorU8( 0,0,0,0 );
	VertP        = VertexBuffer;
	Normal       = VGet( 0.0f, 1.0f, 0.0f );

	zvec         = VSub( CatmullRomDestPos[ 0 ][ 1 ], CatmullRomDestPos[ 0 ][ 0 ] );
	yvec         = VSub( CatmullRomDestPos[ 1 ][ 0 ], CatmullRomDestPos[ 0 ][ 0 ] );
	xvec         = VNorm( VCross( yvec, zvec ) );
	yvec         = VNorm( VCross( zvec, xvec ) );
	zvec         = VNorm( zvec );

	// ��[�̔����̓��̂P���_�̃f�[�^���Z�b�g
	VertP->pos   = VAdd( CatmullRomDestPos[ 0 ][ 0 ], VScale( zvec, -1.0f ) );
	VertP->dif   = DifColor;
	VertP->dif.a = 0;
	VertP->spc   = SpcColor;
	VertP->u     = 0.0f;
	VertP->v     = 0.0f;
	VertP->su    = 0.0f;
	VertP->sv    = 0.0f;
	VertP->norm  = Normal;
	VertP++;

	// ��[�̔����̓��̂P���_�ȊO�̕����̃f�[�^���Z�b�g
	for( i = 0; i < CIRCLEVERTNUM; i++ )
	{
		for( j = 1; j < CIRCLEVERTNUM / 4; j++ )
		{
			x            =  g_EffSphereSinCos[ j ][ 1 ];
			y            =  0.0f;
			z            = -g_EffSphereSinCos[ j ][ 0 ];

			xd           = x * g_EffSphereSinCos[i][0] - y * g_EffSphereSinCos[i][1];
			yd           = x * g_EffSphereSinCos[i][1] + y * g_EffSphereSinCos[i][0];
			zd           = z;

			VertP->pos   = CatmullRomDestPos[ 0 ][ 0 ];
			VertP->pos   = VAdd( VertP->pos, VScale( xvec, xd * SInfo->SphereSize ) );
			VertP->pos   = VAdd( VertP->pos, VScale( yvec, yd * SInfo->SphereSize ) );
			VertP->pos   = VAdd( VertP->pos, VScale( zvec, zd * SInfo->SphereSize ) );
			VertP->dif   = DifColor;
			VertP->dif.a = 0;
			VertP->spc   = SpcColor;
			VertP->u     = ( float )i / CIRCLEVERTNUM;
			VertP->v     = 0.0f;
			VertP->su    = 0.0f;
			VertP->sv    = 0.0f;
			VertP->norm  = Normal;
			VertP++;
		}
	}

	// �O�Ղ���`�������~�������̒��_�f�[�^���Z�b�g
	l = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( k = 0; k < UNITVERTNUM; k++ )
		{
			alpha      = ( float )k / UNITVERTNUM;
			DifColor.a = ( ( SInfo->PositionNum - 1 ) * UNITVERTNUM - l  ) * 255
									/ ( ( SInfo->PositionNum - 1 ) * UNITVERTNUM );

			zvec = VSub( CatmullRomDestPos[ 0 ][ l + 1 ], CatmullRomDestPos[ 0 ][ l ] );
			yvec = VSub( CatmullRomDestPos[ 1 ][ l ],     CatmullRomDestPos[ 0 ][ l ] );
			xvec = VNorm( VCross( yvec, zvec ) );
			yvec = VNorm( VCross( zvec, xvec ) );

			for( m = 0; m < CIRCLEVERTNUM; m++ )
			{
				VertP->pos  = CatmullRomDestPos[ 0 ][ l ];
				VertP->pos  = VAdd( VertP->pos, VScale( xvec, ScalingSinCos[ m ][ 0 ] ));
				VertP->pos  = VAdd( VertP->pos, VScale( yvec, ScalingSinCos[ m ][ 1 ] ));
				VertP->dif  = DifColor;
				VertP->spc  = SpcColor;
				VertP->u    = ( float )m / CIRCLEVERTNUM;
				VertP->v    = 0.0f;
				VertP->su   = 0.0f;
				VertP->sv   = 0.0f;
				VertP->norm = Normal;

				VertP++;
			}

			l++;
		}
	}

	// �O�Ղ���`�������~�������̏I�[�����̒��_�f�[�^���Z�b�g
	DifColor.a = 0;

	zvec       = VSub( CatmullRomDestPos[ 0 ][ l     ], CatmullRomDestPos[ 0 ][ l - 1 ]);
	yvec       = VSub( CatmullRomDestPos[ 1 ][ l - 1 ], CatmullRomDestPos[ 0 ][ l - 1 ]);
	xvec       = VNorm( VCross( yvec, zvec ) );
	yvec       = VNorm( VCross( zvec, xvec ) );

	for( m = 0; m < CIRCLEVERTNUM; m++ )
	{
		VertP->pos  = CatmullRomDestPos[ 0 ][ l ];
		VertP->pos  = VAdd( VertP->pos, VScale( xvec, ScalingSinCos[ m ][ 0 ] ) );
		VertP->pos  = VAdd( VertP->pos, VScale( yvec, ScalingSinCos[ m ][ 1 ] ) );
		VertP->dif  = DifColor;
		VertP->spc  = SpcColor;
		VertP->u    = ( float )m / CIRCLEVERTNUM;
		VertP->v    = 0.0f;
		VertP->su   = 0.0f;
		VertP->sv   = 0.0f;
		VertP->norm = Normal;

		VertP++;
	}

	// ���̋O�Ղ��`������|���S����`�悷�邽�߂̒��_�C���f�b�N�X���X�g���쐬

	// ��[�̔����̓��̂P���_���܂ޕ����̃|���S�����`�����钸�_�C���f�b�N�X���X�g���쐬
	p2ind  = 1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 );
	IndexP = IndexBuffer;
	k = 0;
	for( i = 0; i < CIRCLEVERTNUM - 1; i++ )
	{
		IndexP[ 0 ] = 0;
		IndexP[ 1 ] = k + 1 + ( CIRCLEVERTNUM / 4 - 1 );
		IndexP[ 2 ] = k + 1;
		IndexP += 3;
		k += CIRCLEVERTNUM / 4 - 1;
	}
	IndexP[ 0 ] = 0;
	IndexP[ 1 ] =     1;
	IndexP[ 2 ] = k + 1;
	IndexP += 3;

	// ��[�̔����̓��̂P���_���܂܂Ȃ������̃|���S����
	// �`�����钸�_�C���f�b�N�X���X�g���쐬
	k = 0;
	for( i = 0; i < CIRCLEVERTNUM - 1; i++ )
	{
		for( j = 0; j < ( CIRCLEVERTNUM / 4 - 1 ) - 1; j++ )
		{
			IndexP[ 0 ] = k + 1 +   j;
			IndexP[ 1 ] = k + 1 +   j       + ( CIRCLEVERTNUM / 4 - 1 );
			IndexP[ 2 ] = k + 1 + ( j + 1 ) + ( CIRCLEVERTNUM / 4 - 1 );

			IndexP[ 3 ] = k + 1 +   j;
			IndexP[ 4 ] = k + 1 + ( j + 1 ) + ( CIRCLEVERTNUM / 4 - 1 );
			IndexP[ 5 ] = k + 1 + ( j + 1 );

			IndexP += 6;
		}
		IndexP[ 0 ] = k + 1 +   j;
		IndexP[ 1 ] = k + 1 +   j       + ( CIRCLEVERTNUM / 4 - 1 );
		IndexP[ 2 ] = p2ind + i + 1;

		IndexP[ 3 ] = k + 1 +   j;
		IndexP[ 4 ] = p2ind + i + 1;
		IndexP[ 5 ] = p2ind + i;

		IndexP += 6;
		k += ( CIRCLEVERTNUM / 4 - 1 );
	}

	// ��[�̔����̓��̂P���_���܂܂Ȃ������̈�ԊO���̕����̃|���S����
	// �`�����钸�_�C���f�b�N�X���X�g���쐬
	{
		for( j = 0; j < ( CIRCLEVERTNUM / 4 - 1 ) - 1; j++ )
		{
			IndexP[ 0 ] = k + 1 +   j;
			IndexP[ 1 ] =     1 +   j      ;
			IndexP[ 2 ] =     1 + ( j + 1 );

			IndexP[ 3 ] = k + 1 +   j;
			IndexP[ 4 ] =     1 + ( j + 1 );
			IndexP[ 5 ] = k + 1 + ( j + 1 );

			IndexP += 6;
		}
		IndexP[ 0 ] = k + 1 +   j;
		IndexP[ 1 ] =     1 +   j;
		IndexP[ 2 ] = p2ind;

		IndexP[ 3 ] = k + 1 +   j;
		IndexP[ 4 ] = p2ind;
		IndexP[ 5 ] = p2ind + i;

		IndexP += 6;
	}

	// �O�Ղ���`�������~�������̃|���S�����`�����钸�_�C���f�b�N�X���X�g���쐬
	k = p2ind;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( j = 0; j < UNITVERTNUM; j++ )
		{
			for( m = 0; m < CIRCLEVERTNUM - 1; m++ )
			{
				IndexP[ 0 ] = k + m;
				IndexP[ 1 ] = k + m + 1;
				IndexP[ 2 ] = k + m + 1 + CIRCLEVERTNUM;

				IndexP[ 3 ] = k + m;
				IndexP[ 4 ] = k + m + 1 + CIRCLEVERTNUM;
				IndexP[ 5 ] = k + m +     CIRCLEVERTNUM;

				IndexP += 6;
			}
			IndexP[ 0 ] = k + m;
			IndexP[ 1 ] = k;
			IndexP[ 2 ] = k +     CIRCLEVERTNUM;

			IndexP[ 3 ] = k + m;
			IndexP[ 4 ] = k +     CIRCLEVERTNUM;
			IndexP[ 5 ] = k + m + CIRCLEVERTNUM;

			IndexP += 6;

			k += CIRCLEVERTNUM;
		}
	}

	// �y�o�b�t�@���g�p����ݒ�ɕύX����
	SetUseZBufferFlag( TRUE );

	// �y�o�b�t�@�ւ̏������݂͍s��Ȃ�
	SetWriteZBufferFlag( FALSE );

	// ���Z�u�����h�`��ɐݒ�
	SetDrawBlendMode( DX_BLENDMODE_ADD, 255 );

	// �O�Ղ̃|���S����`��
	DrawPolygonIndexed3D(
		VertexBuffer,
		1 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) +
		( SInfo->PositionNum - 1 ) * ( CIRCLEVERTNUM * UNITVERTNUM ) + CIRCLEVERTNUM,
		IndexBuffer,
		( CIRCLEVERTNUM * 3 + CIRCLEVERTNUM * ( CIRCLEVERTNUM / 4 - 1 ) * 6 +
		( SInfo->PositionNum - 1 ) * ( CIRCLEVERTNUM * 6 * UNITVERTNUM ) ) / 3,
		EInfo->BaseInfo->GraphHandle,
		TRUE
	);

	// �`��u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// �y�o�b�t�@���g�p���Ȃ��ݒ�ɖ߂�
	SetUseZBufferFlag( FALSE );
}

// ���̋O�ՃG�t�F�N�g�����̏���������֐�
void Effect_SphereLocus_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// �G�t�F�N�g�̐F
	COLOR_U8 Color,
	
	// ���̑傫��
	float SphereSize
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;

	// �L���ȍ��W�̐���������
	SInfo->PositionNum = 0;

	// ���̑傫����ۑ�
	SInfo->SphereSize  = SphereSize;

	// �G�t�F�N�g�̐F��ۑ�
	SInfo->Color       = Color;
}

// ���̋O�ՂɎg�p������W��ǉ�����
void Effect_SphereLocus_AddPosition(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̒��S���W
	VECTOR NewCenterPos,

	// ���̌��������肷�邽�߂̍��W
	VECTOR NewDirectionPos
)
{
	SEffect_SphereLocusInfo *SInfo = ( SEffect_SphereLocusInfo * )EInfo->SubData;
	int i;

	// ���ɒǉ��ł���ő吔�ɒB���Ă��܂��Ă���ꍇ�͍ő吔������Ȃ����ɂ���
	if( SInfo->PositionNum >= POSITIONNUM )
	{
		SInfo->PositionNum--;
	}

	// ���ɓo�^����Ă�����W������ړ�����
	for( i = SInfo->PositionNum - 1; i >= 0; i-- )
	{
		SInfo->Position[ i + 1 ] = SInfo->Position[ i ];
	}

	// �z��̐擪�ɍ��W��ǉ�
	SInfo->Position[ 0 ].CenterPos     = NewCenterPos;
	SInfo->Position[ 0 ].DirectionPos  = NewDirectionPos;
	SInfo->Position[ 0 ].Alpha         = 1.0f;

	// �L���ȍ��W�̐�������₷
	SInfo->PositionNum++;
}
