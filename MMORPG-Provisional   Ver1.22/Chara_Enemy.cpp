#include "Chara_Enemy.h"
#include "DxLib.h"
#include "Chara_EnemyBase.h"
#include "Chara_Player.h"
#include "System.h"
#include "Stage.h"
#include "StageData.h"
#include "Task_GameMain.h"
#include <math.h>

// �f�t�H���g�̓G�̃A�j���[�V�����ύX���x
#define DEFAULT_CHANGE_ANIM_SPEED			(5.0f)

// �v���C���[�̑��݂��C�t�����邽�߂̃��b�Z�[�W�̎��ʔԍ�
#define PLAYER_NEAR_DISTANCE_MESSAGE		(1)

// �_���[�W���󂯂��ۂɎ��͂ɋ��钇�ԂɃv���C���[�̑��݂��C�t�����鋗��
#define PLAYER_NEAR_MESSAGE_SEND_DISTANCE	(2000.0f)

#define GET_MIN_Gold 200

// �G�̏��
typedef enum _EChara_EnemyState
{
	EChara_EnemyState_Idle,						// ���ɉ������Ă��Ȃ����
	EChara_EnemyState_IdleMove,					// ���ɉ��������ړ���
	EChara_EnemyState_IdleAngleMove_BeginWait,	// ���ɉ������������ύX�J�n�O
	EChara_EnemyState_IdleAngleMove_AngleMove,	// ���ɉ������������ύX��
	EChara_EnemyState_IdleAngleMove_AfterWait,	// ���ɉ������������ύX��
	EChara_EnemyState_AttackMoveWait,			// �U���ړ��J�n�O
	EChara_EnemyState_AttackMove,				// �U���ړ���
	EChara_EnemyState_Attack,					// �U����
	EChara_EnemyState_AttackAfter,				// �U����
	EChara_EnemyState_Damage,					// �_���[�W���A�N�V������
	EChara_EnemyState_DamageAfter,				// �_���[�W���A�N�V������
} EChara_EnemyState;

// �G�̍U�����
typedef struct _SChara_EnemyAttackInfo
{
	// �U�����s�������͈̔͂Ƀv���C���[�����邩�ǂ���
	bool                   AttackDistance;

	// �U�����s�������͈̔͂Ƀv���C���[�����邩�ǂ���( ���������̂ݍl�� )
	bool                   AttackDistance_XZ;
} SChara_EnemyAttackInfo;

// �G�̏��
typedef struct _SChara_EnemyInfo
{
	// �G�̊�{���\���̂̃A�h���X
	SChara_EnemyBaseInfo  *BaseInfo;

	// �v���C���[�ɋC�t�������ɂȂ��Ă���̌o�ߎ��Ԍv���p�ϐ�
	float                  PlayerNoticeDistanceCounter;

	// �v���C���[�ɋC�t�������ɂȂ��Ă���̌o�ߎ��Ԍv���p�ϐ�( ���������̂ݍl�� )
	float                  PlayerNoticeDistanceCounter_XZ;

	// �v���C���[�����͂ɋ���Ƃ������b�Z�[�W���͂������ǂ���
	bool                   PlayerNearDistanceMessage;

	// �v���C���[�������Ă��邩�ǂ���
	bool                   SeePlayer;

	// �v���C���[�������Ă��鎞�Ԍv���p�ϐ�
	float                  SeePlayerCounter;

	// �v���C���[�ɋC�t���Ă��邩�ǂ���
	bool                   NoticePlayer;

	// �v���C���[�ɋC�t���Ă��邩�ǂ���( �U�����p )
	bool                   NoticePlayer_AttackMode;

	// �v���C���[�̋������
	VECTOR                 PlayerDirection;

	// �v���C���[�̋������( ���������̂ݍl�� )
	VECTOR                 PlayerDirection_XZ;

	// �U���^�C�v
	int                    AttackType;

	// �e�U���^�C�v�̏��
	SChara_EnemyAttackInfo AttackInfo[ENEMY_ATTACK_MAX_NUM];

	// �G�̏��
	EChara_EnemyState      EnemyState;

	// ���Ԍv���p�ϐ�
	float                  TimeCounter;

	// �҂���
	float                  WaitTime;

	// �ڕW�Ƃ������
	float                  TargetAngle;

	// �|���ꂽ�ꍇ�ɃV�X�e���ɓ|���ꂽ���Ƃ�`�������ǂ���
	bool                   KillCounterAdd;
} SChara_EnemyInfo;

// �G�̏����ɕK�v�ȏ��̃Z�b�g�A�b�v���s��
static void Chara_Enemy_Info_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �G�́u���ɉ������Ă��Ȃ���ԁv�̋��ʏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Idle_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ��ԕύX�������������ǂ�����ۑ�����t���O�̃A�h���X
	bool *ChangeState
);

// �G�́u�U���ړ���ԁv�̋��ʏ������s��
//     �߂�l : ��ԕύX�������������ǂ���
//              ( true : ��ԕύX����������  false : ��ԕύX�͔������Ă��Ȃ� )
static bool Chara_Enemy_State_AttackMove_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ��ԕύX�������������ǂ�����ۑ�����t���O�̃A�h���X
	bool *ChangeState
);

// �G�́u�U����ԁv�̋��ʏ������s��
static void Chara_Enemy_State_Attack_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u���ɉ������Ă��Ȃ���ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Idle_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u���ɉ��������ړ����v�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_IdleMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u���ɉ������������ύX��ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_IdleAngleMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u�U���ړ��J�n�O��ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackMoveWait_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u�U���ړ�����ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u�U������ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Attack_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u�U�����ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackAfter_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);

// �G���u�_���[�W���ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_DamageAfter_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
);



// �G���쐬���ꂽ�ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Enemy_Create(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SChara_EnemyInfo *EInfo;

	CInfo->Gold = GetRand(250)+ GET_MIN_Gold;
	// �G�̏��\���̂��i�[���郁�����̈�̊m��
	CInfo->SubData = malloc(sizeof(SChara_EnemyInfo));
	if (CInfo->SubData == NULL)
	{
		return false;
	}

	EInfo = (SChara_EnemyInfo *)CInfo->SubData;

	// �돉����
	memset(EInfo, 0, sizeof(SChara_EnemyInfo));

	// �G�̊�{���\���̂̃A�h���X���Z�b�g
	EInfo->BaseInfo = (SChara_EnemyBaseInfo *)CInfo->BaseInfo->SubData;

	// �ŏ��́u���ɉ������Ă��Ȃ���ԁv�ɂ���
	if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G�̏����ɕK�v�ȏ��̃Z�b�g�A�b�v���s��
static void Chara_Enemy_Info_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SCharaInfo           *PCInfo;
	VECTOR                EnemyToPlayerVec;
	VECTOR                EnemyToPlayerVec_XZ;
	int                   StageCollModel;
	MV1_COLL_RESULT_POLY  CollPoly;
	int                   i;
	VECTOR                PlayerDirection;
	VECTOR                PlayerDirection_XZ;
	float                 PlayerDistance;
	float                 PlayerDistance_XZ;
	VECTOR                PlayerHeadPosition;
	VECTOR                HeadPosition;
	int                   StageObjectNum;
	int                   ObjectCollModel;

	// �X�e�[�W�p�R���W�����R�c���f���̃n���h�����擾
	StageCollModel = StageData_GetCollisionModelHandle();

	// �X�e�[�W�I�u�W�F�N�g�̐����擾����
	StageObjectNum = StageData_GetObjectNum();

	// �v���C���[�̃L�������\���̂̃A�h���X���擾
	PCInfo = System_GetPlayerCharaInfo();

	// �v���C���[�����邩�ǂ����ŏ����𕪊�
	if (PCInfo == NULL)
	{
		// �v���C���[�����Ȃ��ꍇ�́w�v���C���[�����������ǂ����x�Ȃǂ̏�������������
		EInfo->PlayerNoticeDistanceCounter = 0.0f;
		EInfo->PlayerNoticeDistanceCounter_XZ = 0.0f;
		EInfo->SeePlayer = false;
		EInfo->SeePlayerCounter = 0.0f;
		EInfo->NoticePlayer = false;
		EInfo->NoticePlayer_AttackMode = false;
	}
	else
	{
		// �G����v���C���[�ւ̃x�N�g�����Z�o
		EnemyToPlayerVec = VSub(PCInfo->Position, CInfo->Position);
		EnemyToPlayerVec_XZ = EnemyToPlayerVec;
		EnemyToPlayerVec_XZ.y = 0.0f;

		// �G����v���C���[�ւ̐��K���x�N�g�����Z�o
		PlayerDirection = VNorm(EnemyToPlayerVec);
		PlayerDirection_XZ = VNorm(EnemyToPlayerVec_XZ);

		// �G����v���C���[�܂ł̋������Z�o
		PlayerDistance = VSize(EnemyToPlayerVec);
		PlayerDistance_XZ = VSize(EnemyToPlayerVec_XZ);

		// �v���C���[�̋��������ۑ�
		EInfo->PlayerDirection = PlayerDirection;
		EInfo->PlayerDirection_XZ = PlayerDirection_XZ;

		// �v���C���[�ɋC�t�������ɋ���ꍇ�̓v���C���[��
		// �C�t�������ɂȂ��Ă���̎��Ԃ�i�߂�
		if (PlayerDistance < EInfo->BaseInfo->NoticeDistance)
		{
			EInfo->PlayerNoticeDistanceCounter += StepTime;
		}
		else
		{
			EInfo->PlayerNoticeDistanceCounter = 0.0f;
		}

		// �v���C���[�ɋC�t�������ɋ���ꍇ�̓v���C���[��
		// �C�t�������ɂȂ��Ă���̎��Ԃ�i�߂�( ���������̂ݍl�� )
		if (PlayerDistance_XZ < EInfo->BaseInfo->NoticeDistance)
		{
			EInfo->PlayerNoticeDistanceCounter_XZ += StepTime;
		}
		else
		{
			EInfo->PlayerNoticeDistanceCounter_XZ = 0.0f;
		}

		// �G�̎��E�͈̔͂Ƀv���C���[�����邩�𒲂ׂ�
		if (cos(EInfo->BaseInfo->SeeAngleRange) <
			VDot(CInfo->AngleInfo.FrontDirection, PlayerDirection_XZ))
		{
			// �G�̎��E�͈̔͂Ƀv���C���[������ꍇ�͊ԂɎՕ������������𒲂ׂ�

			// �v���C���[�ƓG�̓��̈ʒu���擾
			PlayerHeadPosition = MV1GetFramePosition(PCInfo->ModelHandle,
				MV1SearchFrame(PCInfo->ModelHandle, "head"));
			HeadPosition = MV1GetFramePosition(CInfo->ModelHandle,
				MV1SearchFrame(CInfo->ModelHandle, "head"));

			// ���݂��̓����̊ԂɎՂ���̂����邩�ǂ����𒲂ׂ�
			CollPoly = MV1CollCheck_Line(StageCollModel, -1, HeadPosition,
				PlayerHeadPosition);
			if (CollPoly.HitFlag == FALSE)
			{
				for (i = 0; i < StageObjectNum; i++)
				{
					ObjectCollModel = StageData_GetObjectCollisionModelHandle(i);

					if (ObjectCollModel == -1)
					{
						continue;
					}

					CollPoly = MV1CollCheck_Line(ObjectCollModel, -1, HeadPosition,
						PlayerHeadPosition);

					if (CollPoly.HitFlag)
					{
						break;
					}
				}
			}

			// �����ꍇ�͓G����v���C���[��������Ƃ�������
			EInfo->SeePlayer = CollPoly.HitFlag == FALSE;
		}
		else
		{
			EInfo->SeePlayer = false;
		}

		// �G����v���C���[�������Ă���ꍇ�͌�����悤�ɂȂ��Ă���̎��Ԃ�i�߂�
		if (EInfo->SeePlayer)
		{
			EInfo->SeePlayerCounter += StepTime;
		}
		else
		{
			EInfo->SeePlayerCounter = 0.0f;
		}

		// �U�����[�h�ł͂Ȃ��ꍇ�Ƀv���C���[�̑��݂ɋC�t�������������Ă��邩���`�F�b�N
		if ((EInfo->SeePlayerCounter            > EInfo->BaseInfo->NoticeTime_See &&
			PlayerDistance_XZ                  < EInfo->BaseInfo->NoticeDistance_See) ||
			EInfo->PlayerNoticeDistanceCounter    > EInfo->BaseInfo->NoticeTime_Distance ||
			EInfo->PlayerNoticeDistanceCounter_XZ > EInfo->BaseInfo->NoticeTime_Distance)
		{
			// �����Ă���ꍇ�̓v���C���[�ɋC�t���Ă��邩�ǂ����̃t���O�𗧂Ă�
			EInfo->NoticePlayer = true;
		}
		else
		{
			EInfo->NoticePlayer = false;
		}

		// �퓬���[�h�Ńv���C���[�̑��݂ɋC�t�������������Ă��邩���`�F�b�N
		if (EInfo->SeePlayer ||
			PlayerDistance    < EInfo->BaseInfo->NoticeDistance_AttackMode ||
			PlayerDistance_XZ < EInfo->BaseInfo->NoticeDistance_AttackMode)
		{
			// �����Ă���ꍇ�̓v���C���[�ɋC�t���Ă��邩�ǂ����̃t���O�𗧂Ă�
			EInfo->NoticePlayer_AttackMode = true;
		}
		else
		{
			EInfo->NoticePlayer_AttackMode = false;
		}

		// �v���C���[�����͂ɋ���Ƃ������b�Z�[�W���͂��Ă����疳������
		// �v���C���[�ɋC�t���Ă��邱�Ƃɂ���
		if (EInfo->PlayerNearDistanceMessage)
		{
			EInfo->NoticePlayer = true;
			EInfo->NoticePlayer_AttackMode = true;

			// ���b�Z�[�W���͂������Ƃ������t���O��|��
			EInfo->PlayerNearDistanceMessage = false;
		}

		// �e�U���^�C�v�́u�U�����s�������ɋ��邩�ǂ����v�̃t���O���X�V
		for (i = 0; i < EInfo->BaseInfo->AttackNum; i++)
		{
			EInfo->AttackInfo[i].AttackDistance =
				PlayerDistance    < EInfo->BaseInfo->AttackInfo[i].AttackDistance;

			EInfo->AttackInfo[i].AttackDistance_XZ =
				PlayerDistance_XZ < EInfo->BaseInfo->AttackInfo[i].AttackDistance;
		}
	}
}

// �G�̏�Ԑ��ڏ������s����ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Enemy_Step(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime,

	// �f�t�H���g�̏������s�����ǂ����̃t���O�̃A�h���X
	bool *DefaultProcess
)
{
	SChara_EnemyInfo *EInfo = (SChara_EnemyInfo *)CInfo->SubData;
	bool              ChangeState;

	// �G�̏����ɕK�v�ȏ��̃Z�b�g�A�b�v���s��
	Chara_Enemy_Info_Setup(CInfo, EInfo, StepTime);

	// �L�����̏�Ԃ��ړ������ǂ����ŏ����𕪊�
	if (CInfo->State != ECharaState_Move)
	{
		// �ړ����ł͂Ȃ��ꍇ
		switch (CInfo->State)
		{
		case ECharaState_Damage:	// �_���[�W��
									// �G�̏�Ԃ��_���[�W���ɂ���
			EInfo->EnemyState = EChara_EnemyState_Damage;
			break;

		case ECharaState_FadeOut:	// �t�F�[�h�A�E�g��
									// �܂��V�X�e���ɓ|���ꂽ���Ƃ�`���Ă��Ȃ��ꍇ�͓`����
			Chara_Player_AddMoney(CInfo->Gold*(1 + CInfo->Gold_Per));
			if (!EInfo->KillCounterAdd)
			{
				EInfo->KillCounterAdd = true;
				Task_GameMain_AddKillEnemy(
					System_GetGameMainTaskInfo(), CInfo->CharaNo);
			}
			break;

		default:					// ����ȊO�̏ꍇ
									// �G�̏�Ԃ́u���ɉ������Ă��Ȃ��v�ɂ���
			EInfo->EnemyState = EChara_EnemyState_Idle;
			break;
		}

		// �ړ����ȊO�̏ꍇ�͔ėp�̎��Ԍv���p�ϐ�������������
		EInfo->TimeCounter = 0.0f;
	}
	else
	{
		// �ėp�̎��Ԍv���p�ϐ��̎��Ԃ�i�߂�
		EInfo->TimeCounter += StepTime;

		// �G�̏�Ԃɂ���ď����𕪊�
		switch (EInfo->EnemyState)
		{
		case EChara_EnemyState_Idle:	// ���ɉ������Ă��Ȃ����
										// ���ɉ������Ă��Ȃ���Ԃ̋��ʏ��������s����
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// ��莞�Ԃ��o�߂�����u���ɉ��������ړ����v�Ɉڍs
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_IdleMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_IdleMove:	// ���ɉ��������ړ���
											// ���ɉ������Ă��Ȃ���Ԃ̋��ʏ��������s����
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// ��莞�Ԃ��o�߂��Ċ��A�j���[�V�������I��������
			// �u���ɓ�Ղ����������ύX�J�n�O�v�Ɉڍs
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (CInfo->AnimInfo.End)
				{
					if (!Chara_Enemy_State_IdleAngleMove_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
			}
			else
			{
				// ��莞�Ԃ��o�߂���܂ŃA�j���[�V�������I�����Ă���������
				CInfo->AnimInfo.End = false;
			}
			break;

		case EChara_EnemyState_IdleAngleMove_BeginWait:		// ���ɉ������������ύX�J�n�O
															// ���ɉ������Ă��Ȃ���Ԃ̋��ʏ��������s����
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// ��莞�Ԃ��o�߂�����u���ɉ������������ύX���v�Ɉڍs
			if (EInfo->TimeCounter > EInfo->BaseInfo->IdleAngleMoveBeginWait)
			{
				EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_AngleMove;

				Chara_SetTargetAngle(CInfo, EInfo->TargetAngle, true);
			}
			break;

		case EChara_EnemyState_IdleAngleMove_AngleMove:		// ���ɉ������������ύX��
															// �����ύX������������u���ɉ������������ύX��v�Ɉڍs
			if (CInfo->AngleInfo.AngleMoveEnd)
			{
				EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_AfterWait;
				EInfo->TimeCounter = 0.0f;
			}
			break;

		case EChara_EnemyState_IdleAngleMove_AfterWait:		// ���ɉ������������ύX��
															// ���ɉ������Ă��Ȃ���Ԃ̋��ʏ��������s����
			if (!Chara_Enemy_State_Idle_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// ��莞�Ԍo�߂�����u���ɉ������Ă��Ȃ���ԁv�Ɉڍs
			if (EInfo->TimeCounter > EInfo->BaseInfo->IdleAngleMoveAfterWait)
			{
				if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackMoveWait:		// �U���ړ��J�n�O
													// �퓬�ړ���Ԃ̋��ʏ������s��
			if (!Chara_Enemy_State_AttackMove_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// ��莞�Ԍo�߂�����u�U���ړ����v�Ɉڍs����
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackMove:			// �U���ړ���
													// �퓬�ړ���Ԃ̋��ʏ������s��
			if (!Chara_Enemy_State_AttackMove_Common(CInfo, EInfo, &ChangeState))
			{
				return false;
			}
			if (ChangeState)
			{
				break;
			}

			// �v���C���[���U���ł��鋗���ɗ�����u�U�����v�Ɉڍs����
			if (EInfo->AttackInfo[EInfo->AttackType].AttackDistance_XZ)
			{
				if (!Chara_Enemy_State_Attack_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_Attack:				// �U����
													// �U����Ԃ̋��ʏ������s��
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// �U���A�j���[�V�������I��������u�U����v�Ɉڍs����
			if (CInfo->AnimInfo.End)
			{
				if (!Chara_Enemy_State_AttackAfter_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;

		case EChara_EnemyState_AttackAfter:			// �U����
													// �U����Ԃ̋��ʏ������s��
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// ��莞�Ԃ��o�߂����珈���𕪊�
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				// �v���C���[���G�̋C�t���ʒu�ɋ���ꍇ�́u�U���ړ����v�ɁA
				// ���Ȃ��ꍇ�́u���ɉ������Ă��Ȃ���ԁv�Ɉڍs����
				if (EInfo->NoticePlayer_AttackMode)
				{
					if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
				else
				{
					if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
					{
						return false;
					}
				}
			}
			break;

		case EChara_EnemyState_Damage:				// �_���[�W���A�N�V������
													// �����ɂ��鎞�̓_���[�W�������I����Ă���̂ŁA
													// �_���[�W���A�N�V������Ɉڍs����
			if (!Chara_Enemy_State_DamageAfter_Setup(CInfo, EInfo))
			{
				return false;
			}
			break;

		case EChara_EnemyState_DamageAfter:			// �_���[�W���A�N�V������
													// �U����Ԃ̋��ʏ������s��
			Chara_Enemy_State_Attack_Common(CInfo, EInfo);

			// ��莞�Ԍo�߂��Ă�����u�U���ړ����v�Ɉڍs����
			if (EInfo->TimeCounter > EInfo->WaitTime)
			{
				if (!Chara_Enemy_State_AttackMove_Setup(CInfo, EInfo))
				{
					return false;
				}
			}
			break;
		}
	}

	// �v���O�����ɂ��ړ����s�����ǂ����ŏ����𕪊�
	if (EInfo->BaseInfo->ProgramMove)
	{
		// �u���ɉ��������ړ����v���́u�U���ړ����v�̏ꍇ�̓v���O�����ɂ��ړ����s��
		if (EInfo->EnemyState == EChara_EnemyState_IdleMove ||
			EInfo->EnemyState == EChara_EnemyState_AttackMove)
		{
			CInfo->ProgramMove = true;
		}
		else
		{
			CInfo->ProgramMove = false;
		}
	}

	// ��{�I�Ƀf�t�H���g�̏������s��
	*DefaultProcess = true;

	// ����I��
	return true;
}

// �G���_���[�W���󂯂��ۂɌĂ΂��֐�
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_Enemy_Damage(
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
	// �U���͕K��������
	*Defence = false;

	// �f�t�H���g�̏������s��
	*DefaultProcess = true;

	// ���͂Ƀv���C���[�ɋC�t���Ă��Ȃ��G��������C�t������
	Chara_SendMessage(CInfo, CInfo->Position,
		PLAYER_NEAR_MESSAGE_SEND_DISTANCE, PLAYER_NEAR_DISTANCE_MESSAGE);

	// ����I��
	return true;
}


// �G�����b�Z�[�W�������Ă����Ƃ��ɌĂ΂��֐�
bool Chara_Enemy_MessageRecv(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���b�Z�[�W�𑗂��Ă����L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *SendCInfo,

	// ���b�Z�[�W���ʗp�̒l
	int Message
)
{
	SChara_EnemyInfo *EInfo = (SChara_EnemyInfo *)CInfo->SubData;

	// �v���C���[���߂��ɋ��邱�Ƃ�m�点�郁�b�Z�[�W�������ꍇ��
	// ���b�Z�[�W���͂������ǂ����̃t���O�𗧂Ă�
	if (Message == PLAYER_NEAR_DISTANCE_MESSAGE)
	{
		EInfo->PlayerNearDistanceMessage = true;
	}

	// ����I��
	return true;
}

// �G�́u���ɉ������Ă��Ȃ���ԁv�̋��ʏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Idle_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ��ԕύX�������������ǂ�����ۑ�����t���O�̃A�h���X
	bool *ChangeState
)
{
	// �v���C���[�ɋC�t������u�U���ړ��J�n�O�v�Ɉڍs����
	if (EInfo->NoticePlayer)
	{
		if (!Chara_Enemy_State_AttackMoveWait_Setup(CInfo, EInfo))
		{
			return false;
		}

		// ��ԕύX�����������̂� true ��������
		*ChangeState = true;
	}
	else
	{
		// ��ԕύX���������Ȃ������̂� false ��������
		*ChangeState = false;
	}

	// ����I��
	return true;
}

// �G�́u�U���ړ���ԁv�̋��ʏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackMove_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo,

	// ��ԕύX�������������ǂ�����ۑ�����t���O�̃A�h���X
	bool *ChangeState
)
{
	// �v���C���[������������u���ɉ������Ă��Ȃ���ԁv�Ɉڍs����
	if (!EInfo->NoticePlayer_AttackMode)
	{
		if (!Chara_Enemy_State_Idle_Setup(CInfo, EInfo))
		{
			return false;
		}

		// ��ԕύX�����������̂� true ��������
		*ChangeState = true;
	}
	else
	{
		// �v���C���[�̋�������Ɍ�����ύX����
		Chara_SetTargetDirection(CInfo, EInfo->PlayerDirection_XZ, true, false);
		CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;

		// ��ԕύX���������Ă��Ȃ��̂� false ��������
		*ChangeState = false;
	}

	// ����I��
	return true;
}

// �G�́u�U����ԁv�̋��ʏ������s��
static void Chara_Enemy_State_Attack_Common(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	// �u�U�����������Ă��������ω������邩�ǂ����v�̃t���O���|��Ă���ꍇ�͉������Ȃ�
	if (!EInfo->BaseInfo->AttackInfo[EInfo->AttackType].IsAttackAngleChange)
	{
		return;
	}

	// �v���C���[�̋�������Ɍ�����ύX����
	Chara_SetTargetDirection(CInfo, EInfo->PlayerDirection_XZ, true, false);
	CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;
}

// �G���u���ɉ������Ă��Ȃ���ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Idle_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_Idle;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->IdleMoveStartWait);
	if (CInfo->AnimInfo.NowAnim != ECharaAnim_Neutral)
	{
		if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
		{
			return false;
		}
	}

	// ����I��
	return true;
}

// �G���u���ɉ��������ړ����v�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_IdleMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_IdleMove;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->IdleMoveTime);
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Walk, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G���u���ɉ������������ύX��ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_IdleAngleMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	float MoveAngle;

	EInfo->EnemyState = EChara_EnemyState_IdleAngleMove_BeginWait;
	EInfo->TimeCounter = 0.0f;

	// �ύX��̌����̓����_��
	MoveAngle = GetRandomFloat(DX_PI_F, 1.0f);
	if (GetRand(1000) > 500)
	{
		MoveAngle = -MoveAngle;
	}

	EInfo->TargetAngle = CInfo->AngleInfo.NowAngle + MoveAngle;
	CInfo->AngleInfo.AngleSpeed = EInfo->BaseInfo->AngleChangeSpeed;
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G���u�U���ړ��J�n�O��ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackMoveWait_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackMoveWait;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->SeeAttackMoveStartWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G���u�U���ړ�����ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackMove_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackMove;
	EInfo->TimeCounter = 0.0f;

	// �ǂ̍U���^�C�v�ɂȂ邩�̓����_��
	EInfo->AttackType = GetRand(EInfo->BaseInfo->AttackNum - 1);

	// �U���͍͂U���^�C�v���ɐݒ肳��Ă���l�ɂ���
	CInfo->Atk = EInfo->BaseInfo->AttackInfo[EInfo->AttackType].Atk;

	if (!Chara_ChangeAnim(
		CInfo, EInfo->BaseInfo->AttackInfo[EInfo->AttackType].IsAttackMoveWalk ?
		ECharaAnim_Walk : ECharaAnim_Run, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G���u�U������ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_Attack_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	static ECharaAnim AttackAnimTable[] =
	{
		ECharaAnim_Attack1,
		ECharaAnim_Attack2,
		ECharaAnim_Attack3
	};

	EInfo->EnemyState = EChara_EnemyState_Attack;

	// �U���^�C�v�ɂ���čĐ�����A�j���[�V������ύX
	if (!Chara_ChangeAnim(CInfo, AttackAnimTable[EInfo->AttackType],
		DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;

	}

	// ����I��
	return true;
}

// �G���u�U�����ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_AttackAfter_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_AttackAfter;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->AttackRepeatWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}

// �G���u�_���[�W���ԁv�Ɉڍs����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Enemy_State_DamageAfter_Setup(
	// �L�����N�^�[���\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �G�̏��\���̂̃A�h���X
	SChara_EnemyInfo *EInfo
)
{
	EInfo->EnemyState = EChara_EnemyState_DamageAfter;
	EInfo->TimeCounter = 0.0f;
	EInfo->WaitTime = GetRandomFloatS(&EInfo->BaseInfo->DamageAfterMoveWait);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}

	// ����I��
	return true;
}


#if false
//�G�l�~�[�̍U���A�ړ��AAI�A����p�Ȃ�
#include"DxLib.h"







//�G�l�~�[�p�����[�^�ݒ�
void Enemy_Initialize(){


}

//�G�l�~�[AI�ݒ�
void Enemy_Ai(){


}
#endif