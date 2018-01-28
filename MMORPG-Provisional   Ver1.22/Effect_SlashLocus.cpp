#include "Effect_SlashLocus.h"
#include "Mathematics.h"

// �O�Ղ�\�����鎞��
#define DISPLAY_TIME		(0.1f)

// �O�Ղ��`��������W�̊Ԃ��Ԋ֐��ŕ������鐔
#define UNITVERTNUM			(8)

// �O�Ղ��`��������W�̐�
#define POSITIONNUM			(16)

// ���̋O�Ղ��`��������W�̏��
typedef struct _SEffect_SlashLocus_Position
{
	// ���̕����̍��W
	VECTOR                      NearPos;

	// ���̐n�摤�̍��W
	VECTOR                      FarPos;

	// �s�����x
	float                       Alpha;
} SEffect_SlashLocus_Position;

// ���̋O�ՃG�t�F�N�g�̏��
typedef struct _SEffect_SlashLocusInfo
{
	// �G�t�F�N�g�̐F
	COLOR_U8                    Color;

	// ���̋O�Ղ��`������L���ȍ��W�̐�
	int                         PositionNum;

	// ���̋O�Ղ��`��������W�̏��
	SEffect_SlashLocus_Position Position[ POSITIONNUM ];
} SEffect_SlashLocusInfo;


// ���̋O�ՃG�t�F�N�g�̊�{��񏉊����֐�
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Effect_SlashLocus_Initialize(
	// ���̋O�ՃG�t�F�N�g�̊�{���\���̂̃A�h���X
	SEffectBaseInfo *EBInfo
)
{
	// ���̋O�ՃG�t�F�N�g�Ŏg�p����摜�̓ǂݍ���
	EBInfo->GraphHandle = LoadGraph( "Data\\Effect\\SlashLocus.png" );

	// �摜�̓ǂݍ��݂Ɏ��s�����玸�s�I��
	if( EBInfo->GraphHandle < 0 )
	{
		return false;
	}

	// �����I��
	return true;
}

// ���̋O�ՃG�t�F�N�g���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Effect_SlashLocus_Create(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	// �G�t�F�N�g�̏��\���̂��i�[���郁�����̈�̊m��
	EInfo->SubData = malloc( sizeof( SEffect_SlashLocusInfo ) );
	if( EInfo->SubData == NULL )
	{
		return false;
	}

	// ����I��
	return true;
}

// ���̋O�ՃG�t�F�N�g�̏�Ԑ��ڏ����֐�
void Effect_SlashLocus_Step(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	int                     i;
	int                     PosNum;

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
void Effect_SlashLocus_Render(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	static VERTEX3D       VertexBuffer[ ( POSITIONNUM - 1 ) * ( 2 * UNITVERTNUM ) + 2 ];
	static unsigned short IndexBuffer[ ( POSITIONNUM - 1 ) * ( UNITVERTNUM * 6 ) ];
	VECTOR                CatmullRomDestPos[ 2 ][ ( POSITIONNUM - 1 ) * UNITVERTNUM + 1];
	VECTOR                CatmullRomSrcPos[ 2 ][ POSITIONNUM ];
	VERTEX3D             *VertP;
	unsigned short       *IndexP;
	int                   i;
	int                   j;
	int                   k;
	int                   l;
	COLOR_U8              SpcColor;
	VECTOR                Normal;
	float                 alpha;

	// ���̋O�Ղ��`������L���ȍ��W���Q��菭�Ȃ��ꍇ�͉��������ɏI��
	if( SInfo->PositionNum < 2 )
	{
		return;
	}

	// Catmull-Rom��Ԃ��g�p���č��W�ƍ��W�̊Ԃ����炩�Ɍq�����W���Z�o
	for( i = 0; i < SInfo->PositionNum; i++ )
	{
		CatmullRomSrcPos[ 0 ][ i ] = SInfo->Position[ i ].NearPos;
		CatmullRomSrcPos[ 1 ][ i ] = SInfo->Position[ i ].FarPos;
	}
	GetCatmullRomPosList( CatmullRomSrcPos[ 0 ], SInfo->PositionNum, UNITVERTNUM,
															CatmullRomDestPos[ 0 ] );
	GetCatmullRomPosList( CatmullRomSrcPos[ 1 ], SInfo->PositionNum, UNITVERTNUM,
															CatmullRomDestPos[ 1 ] );

	// ���̋O�Ղ��`������|���S���̒��_�f�[�^���쐬
	SpcColor = GetColorU8( 0,0,0,0 );
	VertP    = VertexBuffer;
	Normal   = VGet( 0.0f, 1.0f, 0.0f );
	l        = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( k = 0; k < UNITVERTNUM; k++ )
		{
			alpha = ( float )k / UNITVERTNUM;

			VertP[ 0 ].pos   = CatmullRomDestPos[ 0 ][ l ];
			VertP[ 1 ].pos   = CatmullRomDestPos[ 1 ][ l ];

			VertP[ 0 ].dif   = SInfo->Color;
			VertP[ 0 ].dif.a = ( BYTE )( ( alpha * SInfo->Position[ i + 1 ].Alpha +
							( 1.0f - alpha ) * SInfo->Position[ i ].Alpha ) * 255.0f );
			VertP[ 1 ].dif   = VertP[ 0 ].dif;
			VertP[ 0 ].spc   = SpcColor;
			VertP[ 1 ].spc   = SpcColor;
			VertP[ 0 ].u     = 0.0f;
			VertP[ 1 ].u     = 0.0f;
			VertP[ 0 ].v     = 1.0f;
			VertP[ 1 ].v     = 0.0f;
			VertP[ 0 ].su    = 0.0f;
			VertP[ 1 ].su    = 0.0f;
			VertP[ 0 ].norm  = Normal;
			VertP[ 1 ].norm  = Normal;

			VertP += 2;
			l++;
		}
	}
	VertP[ 0 ].pos   = SInfo->Position[ i ].NearPos;
	VertP[ 1 ].pos   = SInfo->Position[ i ].FarPos;
	VertP[ 0 ].dif   = SInfo->Color;
	VertP[ 0 ].dif.a = ( BYTE )( SInfo->Position[ i ].Alpha * 255.0f );
	VertP[ 1 ].dif   = VertP[ 0 ].dif;
	VertP[ 0 ].spc   = SpcColor;
	VertP[ 1 ].spc   = SpcColor;
	VertP[ 0 ].u     = 0.0f;
	VertP[ 1 ].u     = 0.0f;
	VertP[ 0 ].v     = 1.0f;
	VertP[ 1 ].v     = 0.0f;
	VertP[ 0 ].su    = 0.0f;
	VertP[ 1 ].su    = 0.0f;
	VertP[ 0 ].norm  = Normal;
	VertP[ 1 ].norm  = Normal;
	VertP += 2;

	// ���̋O�Ղ��`������|���S����`�悷�邽�߂̒��_�C���f�b�N�X���X�g���쐬
	IndexP = IndexBuffer;
	k = 0;
	for( i = 0; i < SInfo->PositionNum - 1; i++ )
	{
		for( j = 0; j < UNITVERTNUM; j++ )
		{
			IndexP[ 0 ] = k;
			IndexP[ 1 ] = k + 2;
			IndexP[ 2 ] = k + 1;
			IndexP[ 3 ] = k + 1;
			IndexP[ 4 ] = k + 2;
			IndexP[ 5 ] = k + 3;

			k += 2;
			IndexP += 6;
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
		VertexBuffer, (   SInfo->PositionNum - 1 ) * ( 2 * UNITVERTNUM ) + 2,
		IndexBuffer,  ( ( SInfo->PositionNum - 1 ) * ( UNITVERTNUM * 6 ) ) / 3,
		EInfo->BaseInfo->GraphHandle,
		TRUE
	);

	// �`��u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// �y�o�b�t�@���g�p���Ȃ��ݒ�ɖ߂�
	SetUseZBufferFlag( FALSE );
}

// ���̋O�ՃG�t�F�N�g�����̏���������֐�
void Effect_SlashLocus_Setup(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̋O�ՃG�t�F�N�g�̐F
	COLOR_U8 Color
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;

	// ���̋O�Ղ��`������L���ȍ��W�̐���������
	SInfo->PositionNum = 0;

	// �G�t�F�N�g�̐F��ۑ�
	SInfo->Color = Color;
}

// ���̋O�ՂɎg�p������W��ǉ�����
void Effect_SlashLocus_AddPosition(
	// �G�t�F�N�g���\���̂̃A�h���X
	SEffectInfo *EInfo,
	
	// ���̕����̍��W
	VECTOR NewNearPos,
	
	// ���̐n�摤�̍��W
	VECTOR NewFarPos
)
{
	SEffect_SlashLocusInfo *SInfo = ( SEffect_SlashLocusInfo * )EInfo->SubData;
	int                     i;

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
	SInfo->Position[ 0 ].NearPos = NewNearPos;
	SInfo->Position[ 0 ].FarPos  = NewFarPos;
	SInfo->Position[ 0 ].Alpha   = 1.0f;

	// �L���ȍ��W�̐�������₷
	SInfo->PositionNum++;
}

