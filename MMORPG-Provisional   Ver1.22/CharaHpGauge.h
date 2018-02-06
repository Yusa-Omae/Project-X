#ifndef CHARAHPGAUGE_H
#define CHARAHPGAUGE_H

// �L�����N�^�[�̗̑̓Q�[�W�̏��
typedef struct _SCharaHpGauge
{
	// �s�����x
	float Alpha;

	// �̗͒l( 0.0f �` 1.0f )
	float Hp;

	// �������ۂɎg�p����ȑO�̗̑͒l
	float DelayHp;
} SCharaHpGauge;

// �L�����N�^�[�̗̑̓Q�[�W�̏����̏������s��
extern void CharaHpGaugeSetup(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHpGauge *GInfo,

	// ������Ԃŕ\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,

	// �����̗̑͒l( 0.0f �` 1.0f )
	float Hp
);

// �L�����N�^�[�̗̑̓Q�[�W�̏�Ԑ��ڏ������s��
extern void CharaHpGaugeStep(
	// �L�����N�^�[�̗̑̓Q�[�W�̏��\���̂̃A�h���X
	SCharaHpGauge *GInfo,
	
	// ���ڂ����鎞��( �P�� : �b )
	float StepTime,

	// �\�����邩�ǂ���( true:�\������  false:��\�� )
	bool Visible,
	
	// �̗͒l( 0.0f �` 1.0f )
	float Hp
);

// �L�����N�^�[�̗̑̓Q�[�W�̕`�揈�����s��
extern void CharaHpGaugeDraw(
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
);

#endif
