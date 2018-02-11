#include "Chara_Player.h"
#include "Chara_PlayerBase.h"
#include "System.h"
#include "Input.h"
#include "Camera.h"
#include "Code/AppData/Item/ItemData.h"
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
#define ATTACK_POWER						(15)

// �f�o�b�O�@�\���L���ȏꍇ�̍U����
#define DEBUG_ATTACK_POWER					(4000)

int Save_Item[10];
int Save_Gold;


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

SCharaInfo* s_PlayerInfo;



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

	CInfo->Gold = 9999999999;

	//�A�C�e���̏���������
	for (int i = 0; i < 10; i++) {
		CInfo->Item_Db[i] = -1;
		CInfo->ItemHav[i] = -1;
	}

	//�L�����N�^�[�\���̂�ݒ�
	s_PlayerInfo = CInfo;

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
		int Crt_Decision = GetRand(100);
		//�N���e�B�J���������1.25�{����
		if (Crt_Decision <= (CInfo->Crt + CInfo->Add_Crt) * 100) {
			CInfo->Atk = (ATTACK_POWER + CInfo->Add_Atk)*1.25;
		}else{
			CInfo->Atk = ATTACK_POWER + CInfo->Add_Atk;
		}
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
					CHARA_DEFAULT_CHANGE_ANIM_SPEED  * (1 + CInfo->Add_Spd)))
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
						CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
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
								CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
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
								CHARA_DEFAULT_CHANGE_ANIM_SPEED * (1 + CInfo->Add_Spd)))
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


/*
�L�����N�^�[�ɃA�C�e����ݒ肷��
*/
void Chara_Player_SetItem(int haveId, int itmeIdex) {
	if (s_PlayerInfo == NULL) return;
	if (haveId < 0 || haveId >= 10) return;

	s_PlayerInfo->ItemHav[haveId] = itmeIdex;

}


/*
�L�����N�^�[�ɃA�C�e����ݒ肷��
return -1�ŃG���[�@0�ȏ�ŃA�C�e��
*/
int Chara_Player_GetItem(int haveId) {
	if (s_PlayerInfo == NULL) return -1;
	if (haveId < 0 || haveId >= 10) return -1;

	return s_PlayerInfo->ItemHav[haveId];

}


/*
�L�����N�^�[�̂�����ݒ肷��
*/
void Chara_Player_SetMoney(int gold) {
	if (s_PlayerInfo == NULL) return;

	s_PlayerInfo->Gold = gold;
}

//�L�����N�^�[�ɂ��������Z����B
void Chara_Player_AddMoney(int gold) {
	if (s_PlayerInfo == NULL) return;

	s_PlayerInfo->Gold += gold;
}

/*
�L�����N�^�[�̂�����ԋp����
return �������z
*/
int Chara_Player_GetMoney() {
	if (s_PlayerInfo == NULL) return 0;

	return s_PlayerInfo->Gold;
}

//�A�C�e���̃p�����[�^�����v����
void Player_Add_Item_sum() {
	if (s_PlayerInfo == NULL) return;

	ITEM_PARAM_DATA_t ItemData;

	for (int i = 0; i < 10; i++) {
		int ItemHave = s_PlayerInfo->ItemHav[i];

		ItemData_GetItemData(ItemHave, &ItemData);

		if (ItemHave >= 0 && ItemHave < ITEM_PARAM_DATA_NUM){

			s_PlayerInfo->Add_Health += ItemData.Health;
			s_PlayerInfo->Add_Atk += ItemData.Attack;
			s_PlayerInfo->Add_Def += ItemData.Def;
			s_PlayerInfo->Add_Crt += ItemData.Critical;
			s_PlayerInfo->Add_Spd += ItemData.Spd;
			s_PlayerInfo->Abp_Heal += ItemData.Absorption;
			s_PlayerInfo->Auto_Heal += ItemData.AutoHeal;
			s_PlayerInfo->Gold_Per += ItemData.GoldPrice;

		}
	}
}

//�A�C�e���̑ϋv�l��ݒ�(��)
int Set_Player_Item_Durable(int BuyItem) {
	if (s_PlayerInfo == NULL) return -1;

	ITEM_PARAM_DATA_t ItemData,NewItemData;
	bool Item_Load_flg = true;

	for (int i = 0; i < 10; i++) {

		int ItemHave = s_PlayerInfo->ItemHav[i];

		ItemData_GetItemData(ItemHave, &ItemData);
		ItemData_GetItemData(BuyItem, &NewItemData);
		
		if (ItemHave >= 0 && ItemHave < ITEM_PARAM_DATA_NUM) {
			for (int j = 0; j < 4; j++) {
				if (ItemData.Evol[j] == BuyItem) {
					//�����̕�����i�������ꍇ
					s_PlayerInfo->Item_Db[i] = NewItemData.Durable - ItemData.Durable + s_PlayerInfo->Item_Db[i];
					s_PlayerInfo->ItemHav[i] = BuyItem;
					Item_Load_flg = true;
					break;

				}else{
					Item_Load_flg = false;
				}
			}

		}
		if (Item_Load_flg == false){
			//�V�����������ꍇ
			s_PlayerInfo->Item_Db[i] = NewItemData.Durable;
			s_PlayerInfo->ItemHav[i] = BuyItem;
			Item_Load_flg = true;
			break;
		}else if (ItemHave == -1) {
			//�V�����������ꍇ
			s_PlayerInfo->Item_Db[i] = NewItemData.Durable;
			s_PlayerInfo->ItemHav[i] = BuyItem;
			return -1;
			break;
		}
	}

}