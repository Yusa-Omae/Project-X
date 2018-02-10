#include "CharaHpGauge.h"
#include "DxLib.h"
#include "Mathematics.h"

// �s�����x�̕ω����x
#define ALPHA_SPEED				(3.0f)

// �x��Ė{���̗̑͒l�ɂȂ�\���̖{���̗̑͒l�ɋ߂Â����x
#define DELAY_PARAM_SPEED		(1.0f)

// �̗̓Q�[�W�̗̑͂����镔���̐F
#define GAUGE_COLOR_R			(220)
#define GAUGE_COLOR_G			(220)
#define GAUGE_COLOR_B			(0)

// �̗̓Q�[�W�̗͉̑������Ȃ��������̐F
#define GAUGE_BASE_COLOR_R		(200)
#define GAUGE_BASE_COLOR_G		(0)
#define GAUGE_BASE_COLOR_B		(0)

// �̗̓Q�[�W�̌����Ă��܂��������̐F
#define GAUGE_DELAY_COLOR_R		(175)
#define GAUGE_DELAY_COLOR_G		(165)
#define GAUGE_DELAY_COLOR_B		(0)

// �L�����N�^�[�̗̑̓Q�[�W�̏����̏������s��
void CharaHpGaugeSetup(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHpGauge *GInfo,

	// ������Ԃŕ\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,

	// �����̗̑͒l( 0.0f �` 1.0f )
	float Hp
)
{
	// �s�����x�͏����̕\����Ԃɂ���ĂP�O�O�����͂O��
	GInfo->Alpha       = Visible ? 1.0f : 0.0f;

	// �����̗̑͒l��ۑ�
	GInfo->Hp      = Hp;
	GInfo->DelayHp = GInfo->Hp;
}

// �L�����N�^�[�̗̑̓Q�[�W�̏�Ԑ��ڏ������s��
void CharaHpGaugeStep(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHpGauge *GInfo,
	
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime,

	// �\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,
	
	// �̗͒l( 0.0f �` 1.0f )
	float Hp_flow
)
{
	bool MoveEnd;

	// �̗͂�����ꍇ�͌���O�̗̑͒l������\���Ŏg�p����ϐ��ɑ������
	if( GInfo->Hp < Hp_flow)
	{
		GInfo->DelayHp = GInfo->Hp;
	}

	// �̗͒l�̕ۑ�
	GInfo->Hp = Hp_flow;

	// �̗͂��������ۂ̌���O�̗̑͂̒l��{���̗̑͂̒l�ɋ߂Â���
	MoveEnd = ParamChangeFloat( &GInfo->DelayHp, GInfo->Hp,
									StepTime, DELAY_PARAM_SPEED );

	// �̗͂̌���\�����I����Ă��邩�A�\������w��ɂȂ��Ă���ꍇ�͕s�����x��ω�������
	if( MoveEnd || Visible )
	{
		ParamChangeFloat( &GInfo->Alpha, Visible ? 1.0f : 0.0f, StepTime, ALPHA_SPEED );
	}
}

// �L�����N�^�[�̗̑̓Q�[�W�̕`�揈�����s��
void CharaHpGaugeDraw(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHpGauge *GInfo,

	// �`��w���W
	int DrawX,
	
	// �`��x���W
	int DrawY,

	// �`�悷��Q�[�W�̕�
	int DrawWidth,
	
	// �`�悷��Q�[�W�̍���
	int DrawHeight
)
{
	int Alpha;
	int Param;
	int Delay;

	// �L���ȑ̗̓Q�[�W�̏��A�h���X���n����Ȃ������ꍇ�̗͑͂O�Ƃ��ĕ\������
	if( GInfo == NULL )
	{
		Alpha = 255;
		Param = 0;
		Delay = 0;
	}
	else
	{
		// �A���t�@�l�ɎZ�o
		Alpha = ( int )( GInfo->Alpha * 255.0f );

		// �A���t�@�l���O�̏ꍇ�͉��������ɏI��
		if( Alpha <= 0 )
		{
			return;
		}

		// �̗͂̕`�敝���Z�o
		Param = ( int )( DrawWidth * GInfo->Hp );

		// �ȑO�̗̑͂̕`�敝���Z�o
		Delay = ( int )( DrawWidth * GInfo->DelayHp );
	}

	// �A���t�@�u�����h���[�h�ɐݒ�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, Alpha );

	// �̗͂̂��镔����`��
	DrawBox( DrawX,         DrawY, DrawX + Param,     DrawY + DrawHeight,
		GetColor( GAUGE_COLOR_R,       GAUGE_COLOR_G,       GAUGE_COLOR_B       ), TRUE );

	// �̗͂��ȑO������������`��
	DrawBox( DrawX + Param, DrawY, DrawX + Delay,     DrawY + DrawHeight,
		GetColor( GAUGE_DELAY_COLOR_R, GAUGE_DELAY_COLOR_G, GAUGE_DELAY_COLOR_B ), TRUE );

	// �̗͂�����������`��
	DrawBox( DrawX + Delay, DrawY, DrawX + DrawWidth, DrawY + DrawHeight,
		GetColor( GAUGE_BASE_COLOR_R,  GAUGE_BASE_COLOR_G,  GAUGE_BASE_COLOR_B  ), TRUE );

	// �u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}





