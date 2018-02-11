#ifndef CHARAHEALTHGAUGE_H
#define CHARAHEALTHGAUGE_H

// �L�����N�^�[�̗̑̓Q�[�W�̏��
typedef struct _SCharaHealthGauge
{
	// �s�����x
	float Alpha;

	// �̗͒l( 0.0f �` 1.0f )
	float Health;

	// �������ۂɎg�p����ȑO�̗̑͒l
	float DelayHealth;
} SCharaHealthGauge;

// �L�����N�^�[�̗̑̓Q�[�W�̏����̏������s��
extern void CharaHealthGaugeSetup(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHealthGauge *GInfo,

	// ������Ԃŕ\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,

	// �����̗̑͒l( 0.0f �` 1.0f )
	float Health
);

// �L�����N�^�[�̗̑̓Q�[�W�̏�Ԑ��ڏ������s��
extern void CharaHealthGaugeStep(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHealthGauge *GInfo,

	// ���ڂ����鎞��( �P�� : �b )
	float StepTime,

	// �\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,

	// �̗͒l( 0.0f �` 1.0f )
	float Health
);

// �L�����N�^�[�̗̑̓Q�[�W�̕`�揈�����s��
extern void CharaHealthGaugeDraw(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHealthGauge *GInfo,

	// �`��w���W
	int DrawX,

	// �`��x���W
	int DrawY,

	// �`�悷��Q�[�W�̕�
	int DrawWidth,

	// �`�悷��Q�[�W�̍���
	int DrawHeight
);

#endif