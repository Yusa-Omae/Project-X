#include "Chara_Player.h"
#include "Chara_PlayerBase.h"
#include "System.h"
#include "Input.h"
#include "Camera.h"
#include <math.h>

// �U���P�R���{�ڂ́A�Q�R���{�ڂ̍U���̓��͎�t�J�n����
#define ATTACK1_NEXT_INPUT_START_TIME		(4.0f)

// �U���Q�R���{�ڂ́A�R�R���{�ڂ̍U���̓��͎�t�J�n����
#define ATTACK2_NEXT_INPUT_START_TIME		(4.0f)

// ���b�N�I������
#define LOCKON_DISTANCE						(1000.0f)

// ���b�N�I���p�x�͈�
#define LOCKON_ANGLE						(1.0f)

// �W�����v��
#define JUMP_POWER							(770.0f)

// �f�o�b�O�@�\���L���ȏꍇ�̃W�����v��
#define DEBUG_JUMP_POWER					(1400.0f)

// �W�����v���̑��x
#define JUMPMOVE_SPEED						(460.0f)

// �U����
#define ATTACK_POWER						(40)

// �f�o�b�O�@�\���L���ȏꍇ�̍U����
#define DEBUG_ATTACK_POWER					(4000)


// �v���C���[�̏��
typedef enum _EChara_PlayerState
{
	EChara_PlayerState_None,			// ���ɖ���
	EChara_PlayerState_Attack1,			// �U���P�R���{��
	EChara_PlayerState_Attack2,			// �U���Q�R���{��
	EChara_PlayerState_Attack3,			// �U���R�R���{��

	EChara_PlayerState_GuardIn,			// �K�[�h�J�n
	EChara_PlayerState_GuardLoop,		// �K�[�h��
	EChara_PlayerState_GuardOut,		// �K�[�h�I��
	EChara_PlayerState_GuardImpact,		// �K�[�h�ōU�����󂯎~�ߒ�

	EChara_PlayerState_JumpIn,			// �W�����v�J�n
	EChara_PlayerState_JumpLoop,		// �W�����v��
	EChara_PlayerState_JumpOut,			// �W�����v�I��
} EChara_PlayerState;


// �v���C���[�̏��
typedef struct _SChara_PlayerInfo
{
	// �v���C���[�̏��
	EChara_PlayerState PlayerState;

	// ���̍U�����\��ς݂��ǂ����̃t���O
	bool               NextAttackRequest;

	// �A�j���[�V�����̃L�����Z���C�x���g���`�F�b�N�ς݂��ǂ����̃t���O
	bool               AnimCancelCheck;

	// �v���C���[���U���ΏۂƂ��Ă���L�����̏��\���̂ւ̃|�C���^
	SCharaInfo *       LockOnTarget;

	// �W�����v���J�n�������_�ł̑��x
	VECTOR             JumpSpd;
} SChara_PlayerInfo;


// �v���C���[���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Player_Create(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SChara_PlayerInfo *PInfo;

	// �V�X�e���Ƀv���C���[�̃L�������\���̂̃A�h���X���Z�b�g
	System_SetPlayerCharaInfo(CInfo);

	// �v���C���[�̏��\���̂��i�[���郁�����̈�̊m��
	CInfo->SubData = malloc(sizeof(SChara_PlayerInfo));
	if (CInfo->SubData == NULL)
	{
		return false;
	}

	PInfo = (SChara_PlayerInfo *)CInfo->SubData;

	// �v���C���[�̏���������
	PInfo->NextAttackRequest = false;
	PInfo->AnimCancelCheck = false;
	PInfo->LockOnTarget = NULL;

	// �̗̓Q�[�W�̏���������
	CharaHealthGaugeSetup(&CInfo->HealthGauge, true, 1.0f);

	// �v���C���[�͏�ɑ̗̓Q�[�W��\������
	CInfo->HealthGaugeVisible = true;

	// �U���͂��Z�b�g
	CInfo->Atk = ATTACK_POWER;

	// ����I��
	return true;
}

// �v���C���[���폜�����ۂɌĂ΂��֐�
void Chara_Player_Delete(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	// �V�X�e���ɓo�^�����v���C���[�̃L�������\���̂̃A�h���X�𖳌��ɂ���
	System_SetPlayerCharaInfo(NULL);
}

// �v���C���[�̏�Ԑ��ڏ������s����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Player_Step(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;
	int    EdgeInput;
	int    Input;
	bool   MoveInputFlag;
	bool   InputLeft;
	bool   InputRight;
	bool   InputUp;
	bool   InputDown;
	bool   InputDefence;
	bool   InputJump;
	bool   InputAttack;
	VECTOR NewSpeed;
	float  NewAngle;

	// �f�o�b�O�@�\���L���ɂȂ��Ă��邩�ǂ����ōU���͂�ω�������
	if (System_GetDispDebugInfo())
	{
		CInfo->Atk = DEBUG_ATTACK_POWER;
	}
	else
	{
		CInfo->Atk = ATTACK_POWER;
	}

	// ���͏�Ԃ��擾
	EdgeInput = GetEdgeInput();
	Input = GetInput();

	InputLeft = (Input     & (1 << EInputType_Left)) != 0;
	InputRight = (Input     & (1 << EInputType_Right)) != 0;
	InputUp = (Input     & (1 << EInputType_Up)) != 0;
	InputDown = (Input     & (1 << EInputType_Down)) != 0;
	InputAttack = (EdgeInput & (1 << EInputType_Attack)) != 0;
	InputDefence = (Input     & (1 << EInputType_Defence)) != 0;
	InputJump = (EdgeInput & (1 << EInputType_Jump)) != 0;

	// �L�����N�^�[���ړ�������͂��s���Ă��邩�ǂ������擾
	MoveInputFlag = InputLeft || InputRight || InputUp || InputDown;

	// �ړ�������͂ɂ���ĐV�������x������
	NewSpeed = VGet(0.0f, 0.0f, 0.0f);
	if (InputLeft)
	{
		NewSpeed = VAdd(NewSpeed, VScale(Camera_RightDirection(), -1.0f));
	}
	else
		if (InputRight)
		{
			NewSpeed = VAdd(NewSpeed, Camera_RightDirection());
		}
	if (InputUp)
	{
		NewSpeed = VAdd(NewSpeed, Camera_FrontDirection());
	}
	else
		if (InputDown)
		{
			NewSpeed = VAdd(NewSpeed, VScale(Camera_FrontDirection(), -1.0f));
		}

	// �v���C���[���G�����b�N�I�����Ă���ꍇ�͂��̕����Ƀv���C���[�̌�����ω�������
	if (PInfo->LockOnTarget != NULL)
	{
		Chara_TargetAngleMove(CInfo, PInfo->LockOnTarget->Position);
	}

STATE_PROCESS:

	// �L�����̏�Ԃɂ���ď����𕪊�
	switch (CInfo->State)
	{
	case ECharaState_Move:		// �ړ���
								// �ړ����̏ꍇ�̓��b�N�I���͉�������
		PInfo->LockOnTarget = NULL;

		// �W�����v���̏ꍇ�͓��ɉ��������I��
		if (CInfo->JumpState)
		{
			break;
		}

		// �U���{�^����������Ă�����U����ԂɈڍs����
		if (InputAttack)
		{
			CInfo->State = ECharaState_Attack;
			PInfo->PlayerState = EChara_PlayerState_Attack1;
			PInfo->AnimCancelCheck = false;

			// ���b�N�I���ΏۂƂȂ�G�����邩��������
			PInfo->LockOnTarget = Chara_SearchTarget(CInfo, ECharaType_Enemy,
				LOCKON_DISTANCE, LOCKON_ANGLE);

			// �U���P�R���{�ڂ̃A�j���[�V�������Đ�
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack1,
				CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
			{
				return false;
			}
		}
		else
			// �K�[�h�{�^����������Ă�����K�[�h��ԂɈڍs����
			if (InputDefence)
			{
				CInfo->State = ECharaState_Defence;
				PInfo->PlayerState = EChara_PlayerState_GuardIn;

				// �K�[�h�J�n�A�j���[�V�������Đ�
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_In,
					CHARA_DEFAULT_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			else
				// �W�����v�{�^����������Ă�����W�����v��ԂɈڍs����
				if (InputJump)
				{
					CInfo->State = ECharaState_Jump;
					PInfo->PlayerState = EChara_PlayerState_JumpIn;

					// �W�����v���J�n�������_�ł̑��x��ۑ����Ă���
					PInfo->JumpSpd = CInfo->PrevAnimSpd;

					// �W�����v�J�n�A�j���[�V�������Đ�
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_In,
						CHARA_DEFAULT_CHANGE_ANIM_SPEED))
					{
						return false;
					}
				}
				else
					// ��������̈ړ����͂�����Ă���ꍇ�͌�����ύX���A����A�j���[�V�������Đ�����
					if (MoveInputFlag)
					{
						NewSpeed = VNorm(NewSpeed);
						NewAngle = atan2(NewSpeed.x, NewSpeed.z);
						Chara_SetTargetAngle(CInfo, NewAngle, true);

						if (CInfo->AnimInfo.NowAnim != ECharaAnim_Run)
						{
							if (!Chara_ChangeAnim(CInfo, ECharaAnim_Run,
								CHARA_DEFAULT_CHANGE_ANIM_SPEED))
							{
								return false;
							}
						}
					}
					else
						// �������͂�����Ă��Ȃ��ꍇ�̓j���[�g�����A�j���[�V�������Đ�����
					{
						if (CInfo->AnimInfo.NowAnim != ECharaAnim_Neutral)
						{
							if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
								CHARA_DEFAULT_CHANGE_ANIM_SPEED))
							{
								return false;
							}
			}
		}
		break;

	case ECharaState_Attack:		// �U����
									// �v���C���[�̏�Ԃɂ���ď����𕪊�
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_Attack1:	 // �U���P�R���{��
											 // �U���Q�R���{�ڂ̓��͂��\�ɂȂ鎞�Ԃ��o�߂��Ă����ԂōU�����͂�
											 // ���ꂽ��A�w���̍U�����\��ς݂��ǂ����̃t���O�x�𗧂Ă�
			if (InputAttack && CInfo->AnimInfo.NowTime > ATTACK1_NEXT_INPUT_START_TIME)
			{
				PInfo->NextAttackRequest = true;
			}

			// �A�j���[�V�����̃L�����Z���C�x���g���������Ă��āA���L�����Z���C�x���g��
			// �`�F�b�N���܂��s���Ă��Ȃ��ꍇ�͏����𕪊�
			if (CInfo->AnimInfo.Cancel && !PInfo->AnimCancelCheck)
			{
				// �L�����Z���C�x���g�̃`�F�b�N���s�������ǂ����̃t���O�𗧂Ă�
				PInfo->AnimCancelCheck = true;

				// �U���Q�R���{�ڂ𔭓�������\�񂪂���Ă�����U���Q�R���{�ڂ��J�n����
				if (PInfo->NextAttackRequest)
				{
					PInfo->AnimCancelCheck = false;
					PInfo->NextAttackRequest = false;
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack2,
						CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
					{
						return false;
					}

					PInfo->PlayerState = EChara_PlayerState_Attack2;
				}
			}
			else
				// �A�j���[�V�������I�����Ă�����ړ���ԂɈڍs����
				if (CInfo->AnimInfo.End)
				{
					CInfo->State = ECharaState_Move;
					PInfo->PlayerState = EChara_PlayerState_None;

					// �ړ���Ԃ��P�t���[�������s����
					goto STATE_PROCESS;
				}
			break;

		case EChara_PlayerState_Attack2:	// �U���Q�R���{��
											// �U���R�R���{�ڂ̓��͂��\�ɂȂ鎞�Ԃ��o�߂��Ă����ԂōU�����͂�
											// ���ꂽ��A�w���̍U�����\��ς݂��ǂ����̃t���O�x�𗧂Ă�
			if (InputAttack && CInfo->AnimInfo.NowTime > ATTACK2_NEXT_INPUT_START_TIME)
			{
				PInfo->NextAttackRequest = true;
			}

			// �A�j���[�V�����̃L�����Z���C�x���g���������Ă��āA���L�����Z���C�x���g��
			// �`�F�b�N���܂��s���Ă��Ȃ��ꍇ�͏����𕪊�
			if (CInfo->AnimInfo.Cancel && !PInfo->AnimCancelCheck)
			{
				// �L�����Z���C�x���g�̃`�F�b�N���s�������ǂ����̃t���O�𗧂Ă�
				PInfo->AnimCancelCheck = true;

				// �U���R�R���{�ڂ𔭓�������\�񂪂���Ă�����U���R�R���{�ڂ��J�n����
				if (PInfo->NextAttackRequest)
				{
					PInfo->AnimCancelCheck = false;
					PInfo->NextAttackRequest = false;
					if (!Chara_ChangeAnim(CInfo, ECharaAnim_Attack3,
						CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
					{
						return false;
					}

					PInfo->PlayerState = EChara_PlayerState_Attack3;
				}
			}
			else
				// �A�j���[�V�������I�����Ă�����ړ���ԂɈڍs����
				if (CInfo->AnimInfo.End)
				{
					CInfo->State = ECharaState_Move;
					PInfo->PlayerState = EChara_PlayerState_None;

					// �ړ���Ԃ��P�t���[�������s����
					goto STATE_PROCESS;
				}
			break;

		case EChara_PlayerState_Attack3:	// �U���R�R���{��
											// �A�j���[�V�������I�����Ă�����ړ���ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// �ړ���Ԃ��P�t���[�������s����
				goto STATE_PROCESS;
			}
			break;
		}
		break;

	case ECharaState_Defence:		// �h�䒆
									// �v���C���[�̏�Ԃɂ���ď����𕪊�
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_GuardIn:		// �K�[�h�J�n
												// �K�[�h�J�n�A�j���[�V�������I��������K�[�h����ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_GuardLoop:		// �K�[�h��
												// ���̓{�^���������ꂽ��K�[�h�I����ԂɈڍs����
			if (!InputDefence)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardOut;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Out,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_GuardOut:		// �K�[�h�I��
												// �K�[�h�I���A�j���[�V�������I��������ړ���ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// �ړ���Ԃ��P�t���[�������s����
				goto STATE_PROCESS;
			}
			break;

		case EChara_PlayerState_GuardImpact:	// �K�[�h�ōU�����󂯎~�ߒ�
												// �K�[�h�ōU�����󂯎~�߂�A�j���[�V����������������K�[�h����ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_GuardLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;
		}
		break;

	case ECharaState_Jump:		// �W�����v��
								// �v���C���[�̏�Ԃɂ���ď����𕪊�
		switch (PInfo->PlayerState)
		{
		case EChara_PlayerState_JumpIn:		// �W�����v�J�n
											// �W�����v�J�n�A�j���[�V�������I��������W�����v����ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				PInfo->PlayerState = EChara_PlayerState_JumpLoop;
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop,
					CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
				{
					return false;
				}
			}
			break;

		case EChara_PlayerState_JumpOut:	// �W�����v�I��
											// �W�����v�I���A�j���[�V�������I��������ړ���ԂɈڍs����
			if (CInfo->AnimInfo.End)
			{
				CInfo->State = ECharaState_Move;
				PInfo->PlayerState = EChara_PlayerState_None;

				// �ړ���Ԃ��P�t���[�������s����
				goto STATE_PROCESS;
			}
			break;
		}
		break;

		// ������сA�_�E���A�t�F�[�h�A�E�g�̓f�t�H���g�̃L�����N�^�[�������s��
	case ECharaState_Blow:
	case ECharaState_Down:
	case ECharaState_FadeOut:
		*DefaultProcess = true;
		return true;

	case ECharaState_Damage:	// �_���[�W��
								// �W�����v���Ƀ_���[�W�A�j���[�V�������I�������ꍇ�̓W�����v����ԂɈڍs����
		if (CInfo->AnimInfo.End && CInfo->JumpState)
		{
			CInfo->State = ECharaState_Jump;
			PInfo->PlayerState = EChara_PlayerState_JumpLoop;

			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop,
				CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
			{
				return false;
			}
		}
		else
		{
			// ����ȊO�̏ꍇ�̓f�t�H���g�̏������s��
			*DefaultProcess = true;
			return true;
		}
		break;
	}

	// �����ɗ����ꍇ�̓f�t�H���g�̏����͎��s���Ȃ�
	*DefaultProcess = false;

	// ����I��
	return true;
}

// �v���C���[���_���[�W���󂯂��ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Player_Damage(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �_���[�W
	int DamagePoint,

	// �U���������������W
	VECTOR HitPosition,

	// �U���̕���
	VECTOR AttackDirection,

	// �U����h�䂵�����ǂ�����������ϐ��̃A�h���X
	bool *Defence,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;
	float ACos;

	// �f�o�b�O�@�\���L���ɂȂ��Ă�����_���[�W���󂯂Ȃ�
	if (System_GetDispDebugInfo())
	{
		*Defence = true;
		*DefaultProcess = false;

		return true;
	}

	// �K�[�h���ł͂Ȃ��ꍇ�̓_���[�W���󂯂�
	if (CInfo->State != ECharaState_Defence)
	{
		*Defence = false;
		*DefaultProcess = true;

		return true;
	}

	// �K�[�h�����Ă���ꍇ�ł��w�㑤����U�����ꂽ��_���[�W���󂯂�
	ACos = VDot(AttackDirection, CInfo->AngleInfo.FrontDirection);
	if (ACos >= 0.0f)
	{
		*Defence = false;
		*DefaultProcess = true;

		return true;
	}

	// �K�[�h�����Đ�����
	Sound_PlaySound3D(HitPosition, PBInfo->DefenceSuccessSound, DX_PLAYTYPE_BACK);

	// �K�[�h�ōU�����󂯎~�߂���ԂɈڍs����
	PInfo->PlayerState = EChara_PlayerState_GuardImpact;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Guard_Impact,
		CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// �K�[�h���������ǂ����̃t���O�𗧂Ă�
	*Defence = true;

	// �_���[�W���󂯂Ȃ������ꍇ�̓f�t�H���g�̓�����s��Ȃ�
	*DefaultProcess = false;

	// ����I��
	return true;
}

// �v���C���[�����n�����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Player_Landed(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;

	// ������ђ���_�E�����̏ꍇ�̓f�t�H���g�̓�����s��
	if (CInfo->State == ECharaState_Blow ||
		CInfo->State == ECharaState_Down ||
		CInfo->State == ECharaState_FadeOut)
	{
		*DefaultProcess = true;
		return true;
	}

	// �������̈ړ����x���O�ɂ���
	CInfo->Spd.x = 0.0f;
	CInfo->Spd.z = 0.0f;

	// �W�����v�I����ԂɈڍs����
	CInfo->State = ECharaState_Jump;
	PInfo->PlayerState = EChara_PlayerState_JumpOut;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Out,
		CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// �����ɗ����ꍇ�̓f�t�H���g�̓���͍s��Ȃ�
	*DefaultProcess = false;

	// ����I��
	return true;
}

// �A�j���[�V�����C�x���g�u���̑��v�����������ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Player_AnimOtherEvent(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SChara_PlayerInfo     *PInfo = (SChara_PlayerInfo     *)CInfo->SubData;
	SChara_PlayerBaseInfo *PBInfo = (SChara_PlayerBaseInfo *)CInfo->BaseInfo->SubData;

	// �W�����v���̑��x���Z�b�g
	CInfo->Spd = PInfo->JumpSpd;

	// �f�o�b�O�@�\���L���ȏꍇ�͑�W�����v������
	if (System_GetDispDebugInfo())
	{
		CInfo->Spd.y = DEBUG_JUMP_POWER;
	}
	else
	{
		CInfo->Spd.y = JUMP_POWER;
	}

	// �W�����v��Ԃ��ǂ����̃t���O�𗧂Ă�
	CInfo->JumpState = true;

	// ����I��
	return true;
}


#if false
#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"Gamemain.h"
#include"Define.h"
#include"math.h"
#include"stdlib.h"
#include"Chara_Player.h"
#include"System.h"
#include"Chara.h"
#include"Camera.h"
#include "Code/Common/Mouse/Mouse.h"

#define MOVE_SPEED (-2.5f)

_PLAYER *PLAYER;
static struct _MOUSE MOUSE;

static void Player_DirectionProc(float* vAngle,float* hAngle,float* distance) {

	//�}�E�X�̉�����Ԃ̎擾
	//MOUSE.Input = GetMouseInput();
	//���݂̃}�E�X���W�̎擾
	//GetMousePoint(&MOUSE.X, &MOUSE.Y);

	Mouse_GetPositioin(&MOUSE.X, &MOUSE.Y);
	MOUSE.Wheel_Rot = Mouse_WheelValueF();

	MOUSE.X -= INIT_AREA_X2 / 2;
	MOUSE.Y -= INIT_AREA_Y2 / 2;

#ifdef __MY_DEBUG__
	if (Mouse_Press(eMouseInputBotton_Left)) {
		printfDx("��\n");
	}
	if (Mouse_Press(eMouseInputBotton_Rigth)) {
		printfDx("�E\n");
	}
	if (Mouse_Press(eMouseInputBotton_Wheel)) {
		printfDx("�z�C�[��\n");
	}
#endif


#if false //�Ƃ肠�����G���[�ɂȂ�̂ŃR�����g�A�E�g by.Syara
	//�E�B���h�E�\���ʒu�̎擾
	GetWindowRect(s_SYSTEM_INFO.WinHndl, s_SYSTEM_INFO.WinAxis);

	if (s_SYSTEM_INFO.WinAxis == NULL) return;

	//�}�E�X���W���� //�O�t���[������̈ړ��ʎZ�o������Ȃ炪���������Ȃ����Ǝv�������ǂ����ł��Ȃ������B�i���W�����ύX��j(�A�v���N����ʂɂ���č��W�ύX���Ȃ���΂Ȃ�Ȃ��B������͂Ȃ����̂��B)
	if (MOUSE.Rest_Flg == true) {
		if (MOUSE.X >= s_SYSTEM_INFO.WinAxis->right) {
			MOUSE.Rest_Flg = false;						//�}�E�X���W�����ړ��t���O���I���ɂ���B
			MOUSE.X = s_SYSTEM_INFO.WinAxis->left;		//�����ړ�
			MOUSE.Back_X = MOUSE.X;						//1�t���[���O���W�̍X�V
		}
		else if (MOUSE.X <= s_SYSTEM_INFO.WinAxis->left) {
			MOUSE.Rest_Flg = false;
			MOUSE.X = s_SYSTEM_INFO.WinAxis->right;
			MOUSE.Back_X = MOUSE.X;
		}
		if (MOUSE.Y >= s_SYSTEM_INFO.WinAxis->bottom) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->top;
			MOUSE.Back_Y = MOUSE.Y;
		}
		else if (MOUSE.Y <= s_SYSTEM_INFO.WinAxis->top) {
			MOUSE.Rest_Flg = false;
			MOUSE.Y = s_SYSTEM_INFO.WinAxis->bottom;
			MOUSE.Back_Y = MOUSE.Y;
		}
	}
	else {
		SetMousePoint(MOUSE.X, MOUSE.Y);					//�}�E�X���W�̃Z�b�g
		MOUSE.Rest_Flg = true;							//�}�E�X���W�����ړ��t���O���I�t�ɂ���B
	}
#endif	//false 

	//�}�E�X�̈ړ��ʂ��Z�o
	MOUSE.Move_X = MOUSE.X - MOUSE.Back_X;
	MOUSE.Move_Y = MOUSE.Y - MOUSE.Back_Y;
	//�}�E�X���W�ۑ�
	MOUSE.Back_X = MOUSE.X;
	MOUSE.Back_Y = MOUSE.Y;


	//�E�N���b�N��������Ă�����J�����̌�����ύX
	//if(MOUSE.Input & MOUSE_INPUT_RIGHT){
	
#if false
	//�����p�x�v�Z
	*vAngle += CAMERA_ANGLE_SPEED * MOUSE.Move_Y;
	if (*vAngle < -(DX_PI_F / 2.0f - 0.1f)) {
		*vAngle = -(DX_PI_F / 2.0f - 0.1f);
	}
	if (*vAngle > DX_PI_F / 2.0f - 0.1f) {
		*vAngle = DX_PI_F / 2.0f - 0.1f;
	}
#endif

	//�����p�x�v�Z
	*hAngle -= CAMERA_ANGLE_SPEED * MOUSE.Move_X;
	if (*hAngle < 0.0f) {
		*hAngle += DX_PI_F * 2.0f;
	}
	else if (*hAngle >= DX_PI_F * 2.0f) {
		*hAngle -= DX_PI_F * 2.0f;
	}
	//}


#if false
	//�Q�[���J�n��-1.0�܂Ŏ�����(�J�����̐�����݂������B)
	if (MOUSE.Wheel_Move_Cnt > WHEEL_CNT_INIT_NUM) {
		//�X�^�[�g������̃J�E���g
		MOUSE.Wheel_Move_Cnt -= WHEEL_MOVE_CNT;
		MOUSE.Wheel_Rot = sin(DX_PI / 2 / WHEEL_CNT_INIT_NUM * MOUSE.Wheel_Move_Cnt)*WHEEL_CNT_INIT_NUM;
	}
	else {
		//�}�E�X�z�C�[���̉�]�����擾
		//MOUSE.Wheel_Rot = GetMouseWheelRotVol();
		
	}
#endif

	

	//�}�E�X�z�C�[������]���ꂽ��J�����̈ʒu��O��Ɉړ�����B
	//���̒l
	if (MOUSE.Wheel_Rot <= 0) {
		//�z�C�[���ʁi�����_���������j//Sin�����ǉ��\��
		if (*distance <= DISTANCE_MAX) {
			*distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}	//���̒l
	}
	else if (MOUSE.Wheel_Rot >= 0) {
		if (*distance >= DISTANCE_MIN) {
			*distance -= MOUSE.Wheel_Rot * WHEEL_SPEED;
		}

	}



}


//�v���C���[�f�[�^�̏�����
void Player_Info_Initialize(){

	//�������̊m��
	_PLAYER *ix  = (_PLAYER *)malloc(sizeof(_PLAYER));

	//�������̒��g�̏�����
	if(ix != NULL){
		memset(ix,0,sizeof(_PLAYER));
	}
	//�������̃O���[�o����
	PLAYER = ix;

	//�O���[�o���ϐ��ɓn�����������ϐ�ix�̏�����
	if(ix != NULL){
		ix = NULL;
	}

	SetMousePoint(INIT_AREA_X2 / 2, INIT_AREA_Y2 / 2);

}

void PlayerUpdate(){

	float dir = Get_Chara_Rotation(model_Player);

	float isMove = true;
	float vAngle = Camera_GetVAngle();
	float hAngle = Camera_GetHAngle();
	float distance = Camera_GetDistance();

	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);

	VECTOR playerPos = PLAYER->pos;

	if(key(KEY_INPUT_W)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Up);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_S)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		//PLAYER->pos.z += cos(1.57f*dir)*MOVE_SPEED;
		moveVec.z = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Down);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_A)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Left);
		Set_Move_flg(model_Player, FALSE);
	}else if(key(KEY_INPUT_D)){
		//PLAYER->pos.x += sin(1.57f*dir)*MOVE_SPEED;
		moveVec.x = -MOVE_SPEED;
		Set_Chara_Direction(model_Player,direction_Right);
		Set_Move_flg(model_Player, FALSE);
	}else{
		Set_Move_flg(model_Player, TRUE);
		isMove = false;
	}

	Player_DirectionProc(&vAngle, &hAngle,&distance);

	if (isMove) {

		VECTOR tmpVec;
		float sinParam;
		float cosParam;
		
		float angle = hAngle;/// 180.0f * DX_PI_F;
		sinParam = sin(angle);
		cosParam = cos(angle);

		tmpVec.x = moveVec.x * cosParam - moveVec.z * sinParam;
		tmpVec.y = 0.0f;
		tmpVec.z = moveVec.x * sinParam + moveVec.z * cosParam;

		playerPos = VAdd(playerPos, tmpVec);
		


	}

	Set_Chara_Rotation(model_Player, hAngle);
	PLAYER->pos = playerPos;

	Camera_SetVAngle(vAngle);
	Camera_SetHAngle(hAngle);
	Camera_SetTargetPosition(PLAYER->pos);
	Camera_SetDistance(distance);

	//�؉��搶�ɕ�����
	//PLAYER->New_Speed = VGet(0.0f,0.0f,0.0f);
	//if(key(KEY_INPUT_W)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_S)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_A)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,VScale(Get_Right_Direction(),-1.0f));
	//	Set_Move_flg(model_Player, FALSE);
	//}else if(key(KEY_INPUT_D)){
	//	PLAYER->New_Speed = VAdd(PLAYER->New_Speed,Get_Right_Direction());
	//	Set_Move_flg(model_Player, FALSE);
	//}else{
	//	//�v���C���[Moveflg�X�V
	//	Set_Move_flg(model_Player, TRUE);
	//}

	//if(Get_Move_flg(model_Player) == FALSE){
	//	PLAYER->New_Speed = VNorm(PLAYER->New_Speed);
	//	PLAYER->Target_Angle = atan2(PLAYER->New_Speed.x,PLAYER->New_Speed.z);
	//	//������
	//	if()

	//}
}

//�v���C���[���W���󂯎��B
VECTOR Get_Player_Pos(){
	return PLAYER->pos;
}
//�v���C���[���W��+���Z
void Add_Player_Pos(VECTOR Add_Axis){
	PLAYER->pos = VAdd(PLAYER->pos,Add_Axis);
}
//�v���C���[���W��-���Z
void Sub_Player_Pos(VECTOR Sub_Axis){
	PLAYER->pos = VSub(PLAYER->pos,Sub_Axis);
}
//�v���C���[�f�[�^�̃N���A
void Player_Info_Fainalize(){
	//PLAYER�������̉��
	if(PLAYER != NULL){
		free(PLAYER);
	}
}
//�v���C���[�N���X�̃f�o�b�O�p
void Debug_Player(){
	DrawFormatString(10,0,Color_ValTbl[eCol_White],"Player_position = (%f,%f,%f)",PLAYER->pos.x,PLAYER->pos.y,PLAYER->pos.z);
}
#endif