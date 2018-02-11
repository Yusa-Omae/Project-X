#include "Chara.h"
#include "Stage.h"
#include "StageData.h"
#include "System.h"
#include "Sound.h"
#include "Mathematics.h"
#include "Effect.h"
#include "Effect_SlashLocus.h"
#include "Effect_Damage.h"
#include "Effect_Dead.h"
#include "Effect_SphereLocus.h"
#include <math.h>
#include <stdlib.h>

#include "Chara_Player.h"
#include "Chara_Enemy.h"
#include "Chara_Bee.h"
#include "Chara_Golem.h"
#include "Chara_Needle.h"

// �L�����̍ő吔
#define CHARA_MAXNUM					(256)

// ��������R���W�����|���S���̍ő吔
#define CHARA_MAX_HITCOLL				(2048)

// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define CHARA_ENUM_DEFAULT_SIZE			(200.0f)

// ���𓥂݊O�������̃W�����v��
#define CHARA_FALL_UP_POWER				(60.0f)

// ���n���Ă���ۂ̏��Ƃ̓����蔻��̍��W�̕␳�l
#define CHARA_HIT_FLOOR_Y_ADJUST_WALK	(-40.0f)

// �W�����v���Ă���ۂ̏��Ƃ̓����蔻��̍��W�̕␳�l
#define CHARA_HIT_FLOOR_Y_ADJUST_JUMP	(-1.0f)

// �ǉ����o�������̍ő厎�s��
#define CHARA_HIT_TRYNUM				(16)

// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
#define CHARA_HIT_SLIDE_DISTANCE		(5.0f)

// �L�����N�^�[���m�œ��������Ƃ��̉����o������
#define CHARA_HIT_PUSH_POWER			(12.0f)

// �d��
#define CHARA_GRAVITY					(980.0f * 2.0f)

// �L�������t�F�[�h�A�E�g���鑬�x
#define CHARA_DOWN_FADEOUT_SPEED		(2.0f)

// �L������������ԃf�t�H���g���x
#define CHARA_DEFAULT_BLOWSPEED			(900.0f)

// �L������������ԍۂ̍���
#define CHARA_DEFAULT_BLOWHEIGHT		(76.514f)

// �ۉe�������鍂��
#define CHARA_SHADOW_HEIGHT				(700.0f)

// ��x�ɕ`��ł���ۉe�|���S���̐�
#define CHARA_SHADOW_POLYGON_NUM		(16)

// �̗̓Q�[�W�̕�
#define HP_GAUGE_WIDTH				(64)

// �̗̓Q�[�W�̍���
#define HP_GAUGE_HEIGHT				(4)

// �X�e�[�W�R���W�����^�C�v
typedef enum _EStageCollType
{
	EStageCollType_Wall,			// ��
	EStageCollType_Floor,			// ��
	EStageCollType_Ceiling,			// �V��

	EStageCollType_Num				// �R���W�����^�C�v�̐�
} EStageCollType;

// �L�����ʂ̏����𕪂��邽�߂̃R�[���o�b�N�֐����
typedef struct _SCharaFunction
{
	// �쐬���ɌĂ΂��֐��ւ̃|�C���^
	bool(*Create)(SCharaInfo *CInfo);

	// �폜���ɌĂ΂��֐��ւ̃|�C���^
	void(*Delete)(SCharaInfo *CInfo);

	// ��Ԑ��ڎ��ɌĂ΂��֐��ւ̃|�C���^
	bool(*Step)(SCharaInfo *CInfo, float StepTime, bool *DefaultProcess);

	// �_���[�W���󂯂��ۂɌĂ΂��֐��ւ̃|�C���^
	bool(*Damage)(SCharaInfo *CInfo, ECharaAttack_DamageType DamageType,
		int DamagePoint, VECTOR HitPosition, VECTOR AttackDirection,
		bool *Defence, bool *DefaultProcess);

	// ���n���ɌĂ΂��֐��ւ̃|�C���^
	bool(*Landed)(SCharaInfo *CInfo, bool *DefaultProcess);

	// �A�j���[�V�����́u���̑��v�C�x���g�����������ۂɌĂ΂��֐��ւ̃|�C���^
	bool(*AnimOtherEvent)(SCharaInfo *CInfo);

	// ���b�Z�[�W�������Ă����Ƃ��ɌĂ΂��֐��ւ̃|�C���^
	bool(*MessageRecv)(SCharaInfo *CInfo, SCharaInfo *SendCInfo, int Message);
} SCharaFunction;

// �L�����̏����S�̂Ŏg�p������
typedef struct _SCharaSystemInfo
{
	// �ۉe�`��p�̉摜�n���h��
	int  ShadowHandle;
} SCharaSystemInfo;

// �L�����̏����S�̂Ŏg�p������
static SCharaSystemInfo g_CharaSystemInfo;

// �L�����̏��
static SCharaInfo g_CharaInfo[CHARA_MAXNUM];

// �L�����ʂ̏����𕪂��邽�߂̃R�[���o�b�N�֐��e�[�u��
static SCharaFunction g_CharaFunctionTable[EChara_Num] =
{
	// EChara_Player
	{ Chara_Player_Create, Chara_Player_Delete, Chara_Player_Step,
	Chara_Player_Damage, Chara_Player_Landed, Chara_Player_AnimOtherEvent,
	NULL },

	// EChara_Goblin
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                NULL,
	Chara_Enemy_MessageRecv },

	// EChara_Bee
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                Chara_Bee_AnimOtherEvent,
	Chara_Enemy_MessageRecv },

	// EChara_Golem
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                Chara_Golem_AnimOtherEvent,
	Chara_Enemy_MessageRecv },

	// EChara_RedGoblin
	{ Chara_Enemy_Create,  NULL,                Chara_Enemy_Step,
	Chara_Enemy_Damage,  NULL,                NULL,
	Chara_Enemy_MessageRecv },

	// EChara_Needle
	{ Chara_Needle_Create, NULL,                NULL,
	NULL,                NULL,                NULL,
	NULL },

};

// �L�����̏�Ԑ��ڏ��������s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Step(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �L�����̍U�������𐄈ڂ�����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �L�������m�̓����蔻�菈�����s��
static void Chara_Collision(
	// �����蔻����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// CInfo ���ړ����悤�Ƃ��Ă���x�N�g��
	VECTOR *MoveVector,

	// CInfo �� MoveVector �ړ�����ꍇ�ɓ����邩�ǂ������`�F�b�N����
	// �Ώۂ̃L�����̏��\���̂̃A�h���X
	SCharaInfo *CheckCInfo
);

// �X�e�[�W���f���Ƃ̓����蔻������Ȃ���L�������ړ�������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Move(
	// �ړ�������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �ړ��x�N�g��
	VECTOR MoveVector
);

// �L�����̈ړ������𐄈ڂ�����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_MoveStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �L�����̕`����s��
static void Chara_Render(
	// �`�悷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̊ۉe��`�悷��
static void Chara_ShadowRender(
	// �ۉe��`�悷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̂Q�c�`����s��
static void Chara_2D_Render(
	// �Q�c�`����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̗����������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Fall(
	// �����������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̒��n�������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Landed(
	// ���n�������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̐�����я������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Blow(
	// ������я������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̓|�ꏈ�����s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Down(
	// �|�ꏈ�����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̃t�F�[�h�A�E�g�������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_FadeOut(
	// �t�F�[�h�A�E�g�������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̌�������������������
static void Chara_InitializeAngle(
	// ��������������������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �����̌���
	float Angle
);

// �L�����̌��������̏�Ԑ��ڂ��s��
static void Chara_AngleStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �L�����̃A�j���[�V��������������������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_InitializeAnim(
	// �A�j���[�V��������������������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�����̃A�j���[�V���������̏�Ԑ��ڂ��s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AnimStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �u�U���ɓ��������L������X�e�[�W�̏��v�����Z�b�g����
static void Chara_AttackHitInfo_Reset(
	// ���Z�b�g����U���ɓ��������L�����̏��\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo
);

// �u�U���ɓ��������L������X�e�[�W�̏��v�ɐV���ɍU���ɓ��������L������ǉ�����
//     �߂�l : �ǉ����悤�Ƃ����L���������ɒǉ�����Ă������ǂ���
//              ( false : ���ɒǉ�����Ă���   true : �V���ɒǉ����� )
static bool Chara_AttackHitInfo_AddChara(
	// ����ǉ�����u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// ���ɒǉ�����L�����̏��\���̂̃A�h���X
	SCharaInfo *NewHitCInfo
);

// �u�U���ɓ��������L������X�e�[�W�̏��v�̃X�e�[�W�Ɠ��������ꍇ�̏������s��
static void Chara_AttackHitInfo_ProcessStageHit(
	// �������s���u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// �U�������������ӏ��̃}�e���A���^�C�v
	EMaterialType MaterialType,

	// �U���������������W
	VECTOR HitPosition,

	// �U���Ɏg�p���ꂽ����
	ECharaWeapon Weapon
);

// �u�U���ɓ��������L������X�e�[�W�̏��v�̃L�����Ɠ��������ꍇ�̏������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackHitInfo_ProcessCharaHit(
	// �������s���u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// �U�������������L�����̏��\���̂̃A�h���X
	SCharaInfo *HitCInfo,

	// �U���������������W
	VECTOR HitPosition,

	// �U���̕���
	VECTOR AttackDirection,

	// �U���̃_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �U���Ɏg�p���ꂽ����
	ECharaWeapon Weapon,

	// �U���ɂ���Ď󂯂�_���[�W
	int DamagePoint
);

// �L�����̍U���������Z�b�g����
static void Chara_AttackInfo_Reset(
	// ���Z�b�g����L�����̍U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo
);

// �L�����̍U�����ɍU������p�̍��W��ǉ�����
static void Chara_AttackInfo_AddPosition(
	// ���W��ǉ�����U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo,

	// �V���ɒǉ�����N�_�ƂȂ�t���[���ɋ߂����_�̍��W
	VECTOR NewNearPos,

	// �V���ɒǉ�����N�_�ƂȂ�t���[�����牓�����_�̍��W
	VECTOR NewFarPos
);

// �L�����̍U�����ɂ��U��������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackInfo_Check(
	// ����Ɏg�p����L�����̍U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo,

	// �����蔻��̌`��
	ECharaAttackFormType FormType,

	// �U�����s���Ă���L�����̏��\���̂̃A�h���X
	SCharaInfo *AttackCInfo,

	// �U���Ώۂ̃L�����^�C�v
	ECharaType TargetType,

	// �U�������������ꍇ�̃_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �U���Ɏg�p����镐��
	ECharaWeapon Weapon,

	// �U�������������ꍇ�̃_���[�W
	int DamagePoint,

	// �����蔻��̌`�� ECharaAttackFormType_Sphere �������ꍇ�̋��̑傫��
	float SphereSize
);


// �L�����̏����̏�����������
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
bool Chara_Initialize(void)
{
	int         i;
	SCharaInfo *CInfo;

	// �L�����̊�{���̏�����
	if (!CharaBase_Initialize())
	{
		return false;
	}

	// �S�ẴL�������\���̂́w�g�p�����ǂ����x�̃t���O��|��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		CInfo->UseFlag = false;
	}

	// �ۉe�`��p�̉摜��ǂݍ���
	g_CharaSystemInfo.ShadowHandle = LoadGraph(CHARADATA_DIR "Shadow.png");
	if (g_CharaSystemInfo.ShadowHandle == -1)
	{
		return false;
	}

	// �����I��
	return true;
}

// �L�����̏����̌�n��������
void Chara_Terminate(void)
{
	// �S�ẴL�������폜����
	Chara_AllDelete();

	// �L�����̊�{���̌�n��
	CharaBase_Terminate();
}

// �V�����L�������쐬����
//     �߂�l : �L�����̍쐬�ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
bool Chara_Create(
	// �o��������L����
	EChara Chara,

	// �o��������L�����̏����ʒu
	VECTOR Position,

	// �o��������L�����̏�������
	float Angle,

	// �o��������L�����̎��ʗp�ԍ�
	int CharaNo
)
{
	int                   i;
	int                   j;
	SCharaInfo           *CInfo;
	const SCharaBaseInfo *CBInfo;

	// �쐬����L�����̊�{�����擾����
	CBInfo = CharaBase_GetInfo(Chara);

	// �g�p����Ă��Ȃ��L�������\���̂�T��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (!CInfo->UseFlag)
		{
			break;
		}
	}

	// �S�Ă̍\���̂��g�p����Ă��܂��Ă�����G���[
	if (i == CHARA_MAXNUM)
	{
		return false;
	}

	// �\���̂��w�g�p���x�ɂ���
	CInfo->UseFlag = true;

	// �쐬����L�������g�p����R�c���f���n���h�����쐬����
	CInfo->ModelHandle = MV1DuplicateModel(CBInfo->ModelHandle);
	if (CInfo->ModelHandle == -1)
	{
		return false;
	}

	// ���[�g�t���[���̍��W�ϊ��͖����ɂ��Ă���
	MV1SetFrameUserLocalMatrix(CInfo->ModelHandle, CBInfo->RootFrameIndex, MGetIdent());

	// �R�c���f���̈ʒu���Z�b�g
	MV1SetPosition(CInfo->ModelHandle, Position);

	// ����������
	CInfo->Chara = Chara;
	CInfo->CharaNo = CharaNo;
	CInfo->BaseInfo = CBInfo;
	CInfo->State = ECharaState_Move;
	CInfo->Position = Position;
	CInfo->OnMaterialType = EMaterialType_Grass;
	CInfo->DamageDirection = VGet(0.0f, 0.0f, 0.0f);
	CInfo->OpacityRate = 1.0f;
	CInfo->JumpState = false;
	CInfo->ProgramMove = false;
	CInfo->Spd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevAnimSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->PrevFixSpd = VGet(0.0f, 0.0f, 0.0f);
	CInfo->Health = CBInfo->Health;

	// ���������̏�����
	Chara_InitializeAngle(CInfo, Angle);

	// �A�j���[�V���������̏�����
	if (!Chara_InitializeAnim(CInfo))
	{
		return false;
	}

	// �̗̓Q�[�W�̕\���ݒ��������
	CInfo->HealthGaugeVisible = false;
	CInfo->HealthGaugeHideDelay = 0.0f;
	CharaHealthGaugeSetup(&CInfo->HealthGauge, false, 1.0f);

	// �U������������
	for (j = 0; j < CHARA_ATTACK_MAX_NUM; j++)
	{
		Chara_AttackInfo_Reset(&CInfo->AttackInfo[j]);
	}

	// �L�����ʂ̏��ւ̃|�C���^��������
	CInfo->SubData = NULL;

	// �L�����ʂ̍쐬���ɌĂԊ֐��̌Ăяo��
	if (!g_CharaFunctionTable[Chara].Create(CInfo))
	{
		return false;
	}

	// �����p�����[�^�̍X�V
	Chara_AngleParamRefresh(CInfo);

	// �����I��
	return true;
}

// �w��̃L�������폜����
void Chara_Delete(
	// �폜����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	int i;

	// ���ɍ\���̂��g�p����Ă��Ȃ������牽�������ɏI��
	if (!CInfo->UseFlag)
	{
		return;
	}

	// �폜���Ɏ��s����֐������݂���ꍇ�͎��s����
	if (g_CharaFunctionTable[CInfo->Chara].Delete != NULL)
	{
		g_CharaFunctionTable[CInfo->Chara].Delete(CInfo);
	}

	// �L�����ʂ̏�񂪂������ꍇ�̓��������������
	if (CInfo->SubData != NULL)
	{
		free(CInfo->SubData);
		CInfo->SubData = NULL;
	}

	// �U������������
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++)
	{
		if (!CInfo->AttackInfo[i].Enable)
		{
			continue;
		}

		Chara_AttackInfo_Reset(&CInfo->AttackInfo[i]);
	}

	// �R�c���f���n���h�����폜����
	MV1DeleteModel(CInfo->ModelHandle);
	CInfo->ModelHandle = -1;

	// �\���̂��g�p���Ă��邩�ǂ����̃t���O��|��
	CInfo->UseFlag = false;
}

// �S�ẴL�������폜����
void Chara_AllDelete(void)
{
	int         i;
	SCharaInfo *CInfo;

	// �S�Ă̍\���̂ɑ΂��č폜�������s��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		Chara_Delete(CInfo);
	}
}

// �L�����̕`����s��
static void Chara_Render(
	// �`�悷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	int    i;
	MATRIX FrameMat;
	VECTOR CapsulePos1;
	VECTOR CapsulePos2;
	int    WeaponModelHandle;
	int    SubModelHandle;

	// �����x������ꍇ�͂y�o�b�t�@�ւ̏������݂��s��Ȃ�
	if (CInfo->OpacityRate < 1.0f)
	{
		MV1SetWriteZBuffer(CInfo->ModelHandle, FALSE);
	}

	// �R�c���f���̕`�擧���x��ݒ�
	MV1SetOpacityRate(CInfo->ModelHandle, CInfo->OpacityRate);

	// �R�c���f���̕`��
	MV1DrawModel(CInfo->ModelHandle);

	// ����𑕔����Ă���ꍇ�͕���R�c���f����`�悷��
	if (CInfo->BaseInfo->EquipWeapon != ECharaWeapon_Num &&
		CInfo->BaseInfo->EquipWeaponAttachFrameIndex != -1)
	{
		// ����R�c���f���n���h�����擾
		WeaponModelHandle =
			CharaBase_GetWeaponInfo(CInfo->BaseInfo->EquipWeapon)->ModelHandle;

		// ������A�^�b�`����t���[���̃��[�J�������[���h�ϊ��s����擾����
		FrameMat = MV1GetFrameLocalWorldMatrix(
			CInfo->ModelHandle, CInfo->BaseInfo->EquipWeaponAttachFrameIndex);

		// �����x������ꍇ�͂y�o�b�t�@�ւ̏������݂��s��Ȃ�
		if (CInfo->OpacityRate < 1.0f)
		{
			MV1SetWriteZBuffer(WeaponModelHandle, FALSE);
		}

		// �R�c���f���̕`�擧���x��ݒ�
		MV1SetOpacityRate(WeaponModelHandle, CInfo->OpacityRate);

		// ����R�c���f���Ɏ擾�����ϊ��s����Z�b�g����
		MV1SetMatrix(WeaponModelHandle, FrameMat);

		// ����R�c���f����`��
		MV1DrawModel(WeaponModelHandle);

		// �y�o�b�t�@�ւ̏������ݐݒ�ⓧ���x�����ɖ߂�
		MV1SetWriteZBuffer(WeaponModelHandle, TRUE);
		MV1SetOpacityRate(WeaponModelHandle, 1.0f);
	}

	// �T�u�R�c���f��������ꍇ�̓T�u�R�c���f����`�悷��
	for (i = 0; i < CInfo->BaseInfo->SubModelNum; i++)
	{
		// �T�u�R�c���f���n���h�����擾
		SubModelHandle = CInfo->BaseInfo->SubModel[i].ModelHandle;

		// �T�u�R�c���f�����A�^�b�`����t���[���̃��[�J�������[���h�ϊ��s����擾����
		FrameMat = MV1GetFrameLocalWorldMatrix(
			CInfo->ModelHandle, CInfo->BaseInfo->SubModel[i].AttachFrameIndex);

		// �����x������ꍇ�͂y�o�b�t�@�ւ̏������݂��s��Ȃ�
		if (CInfo->OpacityRate < 1.0f)
		{
			MV1SetWriteZBuffer(SubModelHandle, FALSE);
		}

		// �R�c���f���̕`�擧���x��ݒ�
		MV1SetOpacityRate(SubModelHandle, CInfo->OpacityRate);

		// �T�u�R�c���f���n���h���Ɏ擾�����ϊ��s����Z�b�g����
		MV1SetMatrix(SubModelHandle, FrameMat);

		// �T�u�R�c���f����`��
		MV1DrawModel(SubModelHandle);

		// �y�o�b�t�@�ւ̏������ݐݒ�ⓧ���x�����ɖ߂�
		MV1SetWriteZBuffer(SubModelHandle, TRUE);
		MV1SetOpacityRate(SubModelHandle, 1.0f);
	}

	// �f�o�b�O����`�悷�邩�ǂ������`�F�b�N
	if (System_GetDispDebugInfo())
	{
		// �f�o�b�O���`��

		// �y�o�b�t�@���g�p�A�y�o�b�t�@�ւ̏������݂���A���C�e�B���O�����ɐݒ�
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
		SetUseLighting(FALSE);

		// �_���[�W����̃J�v�Z���`���`��
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->DamageHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->DamageHitInfo.Width,
			10, GetColor(255, 255, 0), GetColor(0, 0, 0), FALSE);

		// �L�����N�^�[���m�̓����蔻��̃J�v�Z���`���`��
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->CharaHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->CharaHitInfo.Width,
			10, GetColor(0, 255, 255), GetColor(0, 0, 0), FALSE);

		// �����X�e�[�W�Ƃ̓����蔻��̏㑤�̃J�v�Z���`���`��
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);

		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->StageHitTopInfo.Width,
			10, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);

		// �����X�e�[�W�Ƃ̓����蔻��̉����̃J�v�Z���`���`��
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);
		DrawCapsule3D(CapsulePos1, CapsulePos2, CInfo->BaseInfo->StageHitBottomInfo.Width,
			10, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);

		// �y�o�b�t�@���g�p���Ȃ��A�y�o�b�t�@�ւ̏������݂����Ȃ��A���C�e�B���O����ɐݒ�
		SetUseLighting(TRUE);
		SetWriteZBufferFlag(FALSE);
		SetUseZBufferFlag(FALSE);
	}
}

// �L�����̊ۉe��`�悷��
static void Chara_ShadowRender(
	// �ۉe��`�悷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	int                      i;
	int                      j;
	int                      k;
	int                      StageObjectNum;
	int                      CollisionModelHandle;
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];
	int                      HitNum;
	MV1_COLL_RESULT_POLY    *HitRes;
	VERTEX3D                 Vertex[3 * CHARA_SHADOW_POLYGON_NUM];
	VERTEX3D                *VertP;
	VECTOR                   SlideVec;
	int                      PolyNum;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���
	// ( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �X�e�[�W�I�u�W�F�N�g�̐����擾����
	StageObjectNum = StageData_GetObjectNum();

	// ���������I�u�W�F�N�g�̐���������
	HitNum = 0;

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitDim[HitNum] = MV1CollCheck_Capsule(
		StageData_GetCollisionModelHandle(), -1, CInfo->Position,
		VAdd(CInfo->Position, VGet(0.0f, -CHARA_SHADOW_HEIGHT, 0.0f)),
		CInfo->BaseInfo->ShadowSize
	);
	if (HitDim[HitNum].HitNum != 0)
	{
		HitNum++;
	}
	else
	{
		MV1CollResultPolyDimTerminate(HitDim[HitNum]);
	}

	// �L�����N�^�[�̎��͂ɂ���R���W�����I�u�W�F�N�g�̃|���S�����擾����
	for (i = 0; i < StageObjectNum; i++)
	{
		CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

		if (CollisionModelHandle != -1)
		{
			HitDim[HitNum] = MV1CollCheck_Capsule(
				CollisionModelHandle, -1, CInfo->Position,
				VAdd(CInfo->Position, VGet(0.0f, -CHARA_SHADOW_HEIGHT, 0.0f)),
				CInfo->BaseInfo->ShadowSize
			);
			if (HitDim[HitNum].HitNum != 0)
			{
				HitNum++;
			}
			else
			{
				MV1CollResultPolyDimTerminate(HitDim[HitNum]);
			}
		}
	}

	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	for (i = 1; i < CHARA_SHADOW_POLYGON_NUM * 3; i++)
	{
		Vertex[i] = Vertex[0];
	}

	// �`�悷��|���S���̐��ƒ��_�f�[�^�̃A�h���X��������
	PolyNum = 0;
	VertP = Vertex;

	// �����蔻�茋�ʏ��̐������J��Ԃ�
	for (k = 0; k < HitNum; k++)
	{
		// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
		HitRes = HitDim[k].Dim;
		for (i = 0; i < HitDim[k].HitNum; i++, HitRes++)
		{
			// �|���S�����d�Ȃ�Ȃ��悤�ɂ���ׂɂ��炷�x�N�g�����Z�o
			SlideVec = VScale(HitRes->Normal, 0.5f);

			// �|���S���̒��_�̐�( 3�� )�����J��Ԃ�
			for (j = 0; j < 3; j++)
			{
				// �|���S���̍��W�͒n�ʃ|���S���̍��W
				VertP->pos = HitRes->Position[j];

				// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
				VertP->pos = VAdd(VertP->pos, SlideVec);

				// �|���S���̕s�����x��ݒ肷��
				if (HitRes->Position[j].y > CInfo->Position.y - CHARA_SHADOW_HEIGHT)
				{
					VertP->dif.a = (BYTE)(200 * (1.0f - fabs(HitRes->Position[j].y -
						CInfo->Position.y) / CHARA_SHADOW_HEIGHT));
				}
				else
				{
					VertP->dif.a = 0;
				}

				// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
				VertP->u = (HitRes->Position[j].x - CInfo->Position.x) /
					(CInfo->BaseInfo->ShadowSize * 2.0f) + 0.5f;

				VertP->v = (HitRes->Position[j].z - CInfo->Position.z) /
					(CInfo->BaseInfo->ShadowSize * 2.0f) + 0.5f;

				// ���_�̃A�h���X���C���N�������g
				VertP++;
			}

			// �|���S���̐��𑝂₷
			PolyNum++;

			// �����|���S���̍ő吔�ɂȂ��Ă�����|���S����`��
			if (PolyNum == CHARA_SHADOW_POLYGON_NUM)
			{
				// �e�|���S����`��
				DrawPolygon3D(Vertex, PolyNum, g_CharaSystemInfo.ShadowHandle, TRUE);

				// �J�E���^�ƃ|�C���^�����Z�b�g����
				PolyNum = 0;
				VertP = Vertex;
			}
		}
	}

	// �����`�悷��|���S�����c���Ă�����`�悷��
	if (PolyNum > 0)
	{
		// �e�|���S����`��
		DrawPolygon3D(Vertex, PolyNum, g_CharaSystemInfo.ShadowHandle, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	for (k = 0; k < HitNum; k++)
	{
		MV1CollResultPolyDimTerminate(HitDim[k]);
	}

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	// �y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(FALSE);
}

// �L�����̂Q�c�`����s��
static void Chara_2D_Render(
	// �Q�c�`����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	VECTOR ScreenPosition;
	int    DrawX;
	int    DrawY;

	// �v���C���[�̏ꍇ�͉��������I��
	if (CInfo->BaseInfo->Type == ECharaType_Player)
	{
		return;
	}

	// �̗̓Q�[�W��`�悷����W���擾
	ScreenPosition = ConvWorldPosToScreenPos(
		VAdd(CInfo->Position, CInfo->BaseInfo->HealthGaugePosition));

	// ��ʂ̒��ɉf���Ă��Ȃ������牽�������ɏI��
	if (ScreenPosition.z < 0.0f || ScreenPosition.z > 1.0f)
	{
		return;
	}

	// �̗̓Q�[�W�̕`��
	DrawX = (int)(ScreenPosition.x - HP_GAUGE_WIDTH / 2);
	DrawY = (int)(ScreenPosition.y - HP_GAUGE_HEIGHT / 2);
	CharaHealthGaugeDraw(
		&CInfo->HealthGauge, DrawX, DrawY, HP_GAUGE_WIDTH, HP_GAUGE_HEIGHT);
}

// �S�ẴL�����̏�Ԑ��ڏ��������s����
bool Chara_StepAll(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	int         i;
	SCharaInfo *CInfo;

	// �S�Ă̗L���ȃL�����ɑ΂��ď�Ԑ��ڏ������s��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			if (!Chara_Step(CInfo, StepTime))
			{
				return false;
			}
		}
	}

	// �S�Ă̗L���ȃL�����ɑ΂��č��W�ړ��������s��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			if (!Chara_MoveStep(CInfo, StepTime))
			{
				return false;
			}
		}
	}

	// ����I��
	return true;
}

// �S�ẴL�����̕`�揈�����s��
void Chara_RenderAll(
	// �`�惂�[�h
	ECharaRenderMode RenderMode,

	// �`�惂�[�h�� ECharaRenderMode_Always �ȊO�̏ꍇ�Ɏg�p������r�p����
	float CompareDistance
)
{
	int         i;
	SCharaInfo *CInfo;
	float       CompareDistanceSquare;
	SCharaInfo *PCInfo;
	VECTOR      PlayerPosition;

	// �v���C���[�̈ʒu���擾����
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo == NULL)
	{
		// �v���C���[�����Ȃ��ꍇ�͕`�惂�[�h���w��ɕ`�悷��x�ɐݒ�
		RenderMode = ECharaRenderMode_Always;
	}
	else
	{
		PlayerPosition = PCInfo->Position;
	}

	// ��r�p�������悵�Ă���
	CompareDistanceSquare = CompareDistance * CompareDistance;

	// �`�惂�[�h�ɂ���ď����𕪊�
	switch (RenderMode)
	{
	case ECharaRenderMode_Always:			// ��ɕ`�悷��ꍇ
											// �L���ȃL�����𖳏����ŕ`�悷��
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Less:	// �w��̋����ȓ��ł���Ε`�悷��ꍇ
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// �v���C���[�Ƃ̋������w��̋����ȏ�̏ꍇ�͕`�悵�Ȃ�
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) >
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Greater:	// �w��̋����ȏ�ł���Ε`�悷��ꍇ
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// �v���C���[�Ƃ̋������w��̋����ȓ��̏ꍇ�͕`�悵�Ȃ�
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) <=
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_Render(CInfo);
		}
		break;
	}
}

// �S�ẴL�����̊ۉe�̕`�揈�����s��
void Chara_ShadowRenderAll(
	// �`�惂�[�h
	ECharaRenderMode RenderMode,

	// �`�惂�[�h�� ECharaRenderMode_Always �ȊO�̏ꍇ�Ɏg�p������r�p����
	float CompareDistance
)
{
	int         i;
	SCharaInfo *CInfo;
	float       CompareDistanceSquare;
	SCharaInfo *PCInfo;
	VECTOR      PlayerPosition;

	// �v���C���[�̈ʒu���擾����
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo == NULL)
	{
		// �v���C���[�����Ȃ��ꍇ�͕`�惂�[�h���w��ɕ`�悷��x�ɐݒ�
		RenderMode = ECharaRenderMode_Always;
	}
	else
	{
		PlayerPosition = PCInfo->Position;
	}

	// ��r�p�������悵�Ă���
	CompareDistanceSquare = CompareDistance * CompareDistance;

	// �`�惂�[�h�ɂ���ď����𕪊�
	switch (RenderMode)
	{
	case ECharaRenderMode_Always:			// ��ɕ`�悷��ꍇ
											// �L���ȑS�ẴL�����̊ۉe��`�悷��
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (CInfo->UseFlag)
			{
				Chara_ShadowRender(CInfo);
			}
		}
		break;


	case ECharaRenderMode_Distance_Less:	// �w��̋����ȓ��ł���Ε`�悷��ꍇ
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// �v���C���[�Ƃ̋������w��̋����ȏ�̏ꍇ�͕`�悵�Ȃ�
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) >
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_ShadowRender(CInfo);
		}
		break;

	case ECharaRenderMode_Distance_Greater:	// �w��̋����ȏ�ł���Ε`�悷��ꍇ
		CInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
		{
			if (!CInfo->UseFlag)
			{
				continue;
			}

			// �v���C���[�Ƃ̋������w��̋����ȓ��̏ꍇ�͕`�悵�Ȃ�
			if (VSquareSize(VSub(PlayerPosition, CInfo->Position)) <=
				CompareDistanceSquare)
			{
				continue;
			}

			Chara_ShadowRender(CInfo);
		}
		break;
	}
}

// �S�ẴL�����̂Q�c�`�揈�����s��
void Chara_2D_RenderAll(void)
{
	int         i;
	SCharaInfo *CInfo;

	// �L���ȑS�ẴL�����̂Q�c�`�揈�����s��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		if (CInfo->UseFlag)
		{
			Chara_2D_Render(CInfo);
		}
	}
}

// �w��̃^�C�v�̃L������|��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_TagetTypeKill(
	// �|�������L�����^�C�v
	ECharaType Type
)
{
	int         i;
	SCharaInfo *CInfo;

	// �S�ẴL�����ɑ΂��ď��������s
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		// �\���̂��L���ł͖��������牽�������Ɏ��̃��[�v��
		if (!CInfo->UseFlag)
		{
			continue;
		}

		// ���S�������J�n���Ă����牽�������Ɏ��̃��[�v��
		if (CInfo->BaseInfo->Type != Type ||
			CInfo->State == ECharaState_Blow ||
			CInfo->State == ECharaState_Down ||
			CInfo->State == ECharaState_FadeOut)
		{
			continue;
		}

		// �̗͂��O�ɂ���
		CInfo->Health = 0;

		// ���Ƀ_���[�W���󂯂��������Z�b�g
		CInfo->DamageDirection = VScale(CInfo->AngleInfo.FrontDirection, -1.0f);

		// ������΂����Ԃɂ���
		if (!Chara_Blow(CInfo))
		{
			return false;
		}

		// �̗̓Q�[�W���\���ɂ���
		CInfo->HealthGaugeHideDelay = 0.0f;
	}

	// ����I��
	return true;
}

// �w��L�����̎��͂ɋ���w��^�C�v�̃L������T��
//     �߂�l : �����Ō��������L�����̏��\���̂̃A�h���X
SCharaInfo * Chara_SearchTarget(
	// ������ƂȂ�L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �T���L�����̃^�C�v
	ECharaType SearchCharaType,

	// �T������( CInfo �̋���ʒu����ǂꂾ���̋����͈̔͂��������邩 )
	float SearchDistance,

	// �T���p�x�͈�( CInfo �̌����Ă����������ǂ͈̔͂��������邩 )
	float SearchAngle
)
{
	int         i;
	SCharaInfo *TCInfo;
	float       Distance;
	VECTOR      TargetVector;
	VECTOR      Direction;
	float       Angle;
	float       MinDistance;
	SCharaInfo *MinCInfo;

	// ���������L�����̏���������
	MinCInfo = NULL;
	MinDistance = 0.0f;

	// �L�������\���̂̐��������[�v
	TCInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, TCInfo++)
	{
		// �����Ώۂ̍\���̂��g�p����Ă��Ȃ������玟�̃��[�v��
		if (!TCInfo->UseFlag)
		{
			continue;
		}

		// �����Ώۂ�������̃L�����������ꍇ�͎��̃��[�v��
		if (TCInfo == CInfo)
		{
			continue;
		}

		// �����Ώۂ̃L�����^�C�v�ł͂Ȃ������玟�̃��[�v��
		if (TCInfo->BaseInfo->Type != SearchCharaType)
		{
			continue;
		}

		// ���Ɏ���ł����玟�̃��[�v��
		if (TCInfo->Health <= 0)
		{
			continue;
		}

		// �����Ώۂւ̋�����p�x���Z�o
		TargetVector = VSub(TCInfo->Position, CInfo->Position);
		TargetVector.y = 0.0f;
		Distance = VSize(TargetVector);
		Direction = VScale(TargetVector, 1.0f / Distance);
		Angle = acos(VDot(Direction, CInfo->AngleInfo.FrontDirection));

		// �����Ώۂւ̋�����p�x���͈͊O�̏ꍇ�͎��̃��[�v��
		if (SearchAngle < Angle || SearchDistance < Distance)
		{
			continue;
		}

		// ���܂łɏ����ɍ����L���������Ȃ��������A
		// �Ⴕ���͍��܂ŏ����ɍ������L������苗�����߂��ꍇ�͕ۑ�
		if (MinCInfo == NULL || MinDistance > Distance)
		{
			MinCInfo = TCInfo;
			MinDistance = Distance;
		}
	}

	// �������ʂ�Ԃ�
	return MinCInfo;
}

// �w����W�̎��͂ɋ���L�����N�^�[�Ƀ��b�Z�[�W�𑗂�
void Chara_SendMessage(
	// ���b�Z�[�W�𑗂�L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���b�Z�[�W�𑗂��ƂȂ���W
	VECTOR BasePosition,

	// ���b�Z�[�W�𑗂�͈�
	float SendDistance,

	// ���b�Z�[�W�̎��ʔԍ�
	int Message
)
{
	int         i;
	SCharaInfo *TCInfo;
	float       Distance;
	VECTOR      TargetVector;

	// �L�������\���̂̐��������[�v
	TCInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, TCInfo++)
	{
		// �����Ώۂ̍\���̂��g�p����Ă��Ȃ������玟�̃��[�v��
		if (!TCInfo->UseFlag)
		{
			continue;
		}

		// �����Ώۂ�������̃L�����������ꍇ�͎��̃��[�v��
		if (TCInfo == CInfo)
		{
			continue;
		}

		// ���Ɏ���ł����玟�̃��[�v��
		if (TCInfo->Health <= 0)
		{
			continue;
		}

		// �����Ώۂւ̋������Z�o
		TargetVector = VSub(TCInfo->Position, BasePosition);
		TargetVector.y = 0.0f;
		Distance = VSize(TargetVector);

		// �����Ώۂւ̋�����p�x���͈͊O�̏ꍇ�͎��̃��[�v��
		if (SendDistance < Distance)
		{
			continue;
		}

		// ���b�Z�[�W���󂯎�����ۂɌĂяo���֐�������ꍇ�͌Ăяo��
		if (g_CharaFunctionTable[TCInfo->Chara].MessageRecv != NULL)
		{
			g_CharaFunctionTable[TCInfo->Chara].MessageRecv(TCInfo, CInfo, Message);
		}
	}
}

// �L�����̏�Ԑ��ڏ��������s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Step(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	VECTOR BlowMoveVec;
	bool   DefaultProcess;

	// �L�����̌����̏�Ԑ��ڏ������s��
	Chara_AngleStep(CInfo, StepTime);

	// �L�����ɃA�j���[�V�����̏�Ԑ��ڏ������s��
	if (!Chara_AnimStep(CInfo, StepTime))
	{
		return false;
	}

	// �L�����ʂ̏�Ԑ��ڏ����p�֐�������ꍇ�͎��s����
	if (g_CharaFunctionTable[CInfo->Chara].Step != NULL)
	{
		if (!g_CharaFunctionTable[CInfo->Chara].Step(CInfo, StepTime, &DefaultProcess))
		{
			return false;
		}
	}
	else
	{
		DefaultProcess = true;
	}

	// �f�t�H���g�̏������s�����ǂ����ŕ���
	if (DefaultProcess)
	{
		// �L�����̏�Ԃɉ����ď����𕪊�
		switch (CInfo->State)
		{
		case ECharaState_Damage:			// �_���[�W��
											// �_���[�W�A�j���[�V�������I�����Ă��Ȃ������牽�������I��
			if (!CInfo->AnimInfo.End)
			{
				break;
			}

			// �ʏ�ړ����Ɉڍs
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}
			CInfo->State = ECharaState_Move;
			break;

		case ECharaState_Blow:				// ������ђ�
											// ������я�Ԃɂ���ď����𕪊�
			switch (CInfo->BlowState)
			{
			case ECharaBlowState_In:		// ������я�Ԉڍs�A�j���[�V������
											// �A�j���[�V�������I�����Ă��Ȃ������牽�������I��
				if (!CInfo->AnimInfo.End)
				{
					break;
				}

				// �L�����N�^�[���W�����v��Ԃɂ���
				CInfo->Position.y += CHARA_DEFAULT_BLOWHEIGHT;
				CInfo->JumpState = true;

				// ������уx�N�g�����Z�o
				BlowMoveVec = VScale(CInfo->DamageDirection, CHARA_DEFAULT_BLOWSPEED);
				CInfo->Spd.x = BlowMoveVec.x;
				CInfo->Spd.z = BlowMoveVec.z;

				// �ڒn�҂����Ɉڍs����
				if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_Loop, -1.0f))
				{
					return false;
				}
				CInfo->BlowState = ECharaBlowState_WaitLanding;
				break;

			case ECharaBlowState_Out:		// �ڒn���Ă���̃A�j���[�V������
											// �A�j���[�V�������I�����Ă��Ȃ������牽�������I��
				if (!CInfo->AnimInfo.End)
				{
					break;
				}

				// �t�F�[�h�A�E�g��ԂɈڍs
				if (!Chara_FadeOut(CInfo))
				{
					return false;
				}
				break;
			}
			break;

		case ECharaState_Down:				// �|�ꒆ
											// �|��A�j���[�V�������I�����Ă��Ȃ������牽�������ɏI��
			if (!CInfo->AnimInfo.End)
			{
				break;
			}

			// �t�F�[�h�A�E�g��ԂɈڍs
			if (!Chara_FadeOut(CInfo))
			{
				return false;
			}
			break;

		case ECharaState_FadeOut:			// �t�F�[�h�A�E�g��
											// �s�����x��������
			CInfo->OpacityRate -= CHARA_DOWN_FADEOUT_SPEED * StepTime;

			// ���S�ɓ����ɂȂ�����L�������폜����
			if (CInfo->OpacityRate < 0.0f)
			{
				CInfo->OpacityRate = 0.0f;
				Chara_Delete(CInfo);
				return true;
			}
			break;
		}
	}

	// �U�����̏�Ԑ��ڏ������s��
	if (!Chara_AttackStep(CInfo, StepTime))
	{
		return false;
	}

	// �̗̓Q�[�W�\���t���O���|��Ă���ꍇ�́w��\���ɂ���̂�҂��ԁx�����炷
	if (!CInfo->HealthGaugeVisible)
	{
		ParamChangeFloat(&CInfo->HealthGaugeHideDelay, 0.0f, StepTime, 1.0f);
	}

	// �̗̓Q�[�W�̏�Ԑ��ڏ������s��
	CharaHealthGaugeStep(
		&CInfo->HealthGauge,
		StepTime,
		CInfo->HealthGaugeVisible || CInfo->HealthGaugeHideDelay > 0.0f,
		(float)CInfo->Health / CInfo->BaseInfo->Health
	);

	// ����I��
	return true;
}

// �L�����̍U�������𐄈ڂ�����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SCharaAttackInfo               *AInfo;
	const SCharaAttackPositionInfo *AFInfo;
	int                             i;
	VECTOR                          NewNearPos;
	VECTOR                          NewFarPos;
	MATRIX                          AttackPosMatrix;

	// �U�����\���̂̐��������[�v
	AInfo = CInfo->AttackInfo;
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++, AInfo++)
	{
		// �\���̂��g�p����Ă��Ȃ������牽�������Ɏ��̃��[�v��
		if (!AInfo->Enable)
		{
			continue;
		}

		// �U���ʒu���̎擾
		AFInfo = &CInfo->BaseInfo->AttackPosInfo[AInfo->AttackPosIndex];

		// �U������̋N�_�ƂȂ�R�c���f�����̃t���[���̃��[�J�������[���h�ϊ��s����擾
		AttackPosMatrix =
			MV1GetFrameLocalWorldMatrix(CInfo->ModelHandle, AFInfo->StartFrameIndex);

		// �U������Ŏg�p����Q�_���Z�o
		NewNearPos = VTransform(VGet(0.0f, 0.0f, 0.0f), AttackPosMatrix);
		NewFarPos = VTransform(AFInfo->EndLocalPosition, AttackPosMatrix);

		// �U�����ɐV�����Q�_��ǉ�����
		Chara_AttackInfo_AddPosition(AInfo, NewNearPos, NewFarPos);

		// �U������̎��s
		if (!Chara_AttackInfo_Check(
			AInfo, AFInfo->FormType, CInfo,
			CInfo->BaseInfo->Type == ECharaType_Player ?
			ECharaType_Enemy : ECharaType_Player,
			ECharaAttack_DamageType_Cut,
			CInfo->BaseInfo->EquipWeapon,
			CInfo->Atk,
			AFInfo->SphereSize
		))
		{
			return false;
		}

		// �O�ՃG�t�F�N�g�𔭐������Ă��Ȃ������甭��������
		if (AInfo->LocusEffect == NULL)
		{
			// �U������̌`��ɂ���ď����𕪊�
			switch (AFInfo->FormType)
			{
			case ECharaAttackFormType_Poly:		// �|���S��
												// �|���S���O�ՃG�t�F�N�g���쐬
				AInfo->LocusEffect = Effect_Create(EEffect_SlashLocus);
				if (AInfo->LocusEffect == NULL)
				{
					return false;
				}
				Effect_SlashLocus_Setup(AInfo->LocusEffect, AFInfo->EffectColor);
				break;

			case ECharaAttackFormType_Sphere:	// ��
												// ���O�ՃG�t�F�N�g���쐬
				AInfo->LocusEffect = Effect_Create(EEffect_SphereLocus);
				if (AInfo->LocusEffect == NULL)
				{
					return false;
				}
				Effect_SphereLocus_Setup(
					AInfo->LocusEffect, AFInfo->EffectColor, AFInfo->SphereSize);
				break;
			}
		}

		// �U������̌`��ɉ������O�՗p���W�̒ǉ����s��
		switch (AFInfo->FormType)
		{
		case ECharaAttackFormType_Poly:
			Effect_SlashLocus_AddPosition(AInfo->LocusEffect, NewNearPos, NewFarPos);
			break;

		case ECharaAttackFormType_Sphere:
			Effect_SphereLocus_AddPosition(AInfo->LocusEffect, NewNearPos, NewFarPos);
			break;
		}
	}

	// �����ɂȂ����U�����̌�n�����s��
	AInfo = CInfo->AttackInfo;
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++, AInfo++)
	{
		// �\���̂��L���������ꍇ�͉����������̃��[�v��
		if (AInfo->Enable)
		{
			continue;
		}

		// �U���������Z�b�g����
		Chara_AttackInfo_Reset(AInfo);
	}

	// ����I��
	return true;
}

// �L�������m�̓����蔻�菈�����s��
static void Chara_Collision(
	// �����蔻����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// CInfo ���ړ����悤�Ƃ��Ă���x�N�g��
	VECTOR *MoveVector,

	// CInfo �� MoveVector �ړ�����ꍇ�ɓ����邩�ǂ������`�F�b�N����
	// �Ώۂ̃L�����̏��\���̂̃A�h���X
	SCharaInfo *CheckCInfo
)
{
	VECTOR ChkChToChVec;
	VECTOR PushVec;
	VECTOR ChPosition;
	float  Distance;
	float  TotalWidth;
	float  TempY;
	VECTOR CapsulePos1;
	VECTOR CapsulePos2;
	VECTOR CheckCapsulePos1;
	VECTOR CheckCapsulePos2;

	// �L�������m�̕��̍��v���Z�o
	TotalWidth =
		CInfo->BaseInfo->CharaHitInfo.Width + CheckCInfo->BaseInfo->CharaHitInfo.Width;

	// �ړ���̃L���� CInfo �̍��W���Z�o
	ChPosition = VAdd(CInfo->Position, *MoveVector);

	// �e�L�����̓����蔻��p�̃J�v�Z�����W���Z�o
	CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->CharaHitInfo,
		&ChPosition, &CapsulePos1, &CapsulePos2);
	CharaBase_GetHitCapsulePos(&CheckCInfo->BaseInfo->CharaHitInfo,
		&CheckCInfo->Position, &CheckCapsulePos1, &CheckCapsulePos2);

	// �������Ă��邩����
	if (HitCheck_Capsule_Capsule(
		CapsulePos1, CapsulePos2,
		CInfo->BaseInfo->CharaHitInfo.Width,
		CheckCapsulePos1, CheckCapsulePos2,
		CheckCInfo->BaseInfo->CharaHitInfo.Width) == TRUE)
	{
		// �������Ă����� CInfo �� CheckCInfo ���痣��鏈��������

		// CheckCInfo ���� CInfo �ւ̃x�N�g�����Z�o
		ChkChToChVec = VSub(ChPosition, CheckCInfo->Position);

		// �x���͌��Ȃ�
		ChkChToChVec.y = 0.0f;

		// ��l�̋������Z�o
		Distance = VSize(ChkChToChVec);

		// CheckCInfo ���� CInfo �ւ̃x�N�g���𐳋K��
		PushVec = VScale(ChkChToChVec, 1.0f / Distance);

		// �����o���������Z�o�A������l�̋��������l�̑傫�����������l�ɉ����o���͂�
		// �����ė���Ă��܂��ꍇ�́A�҂����肭���������Ɉړ�����
		if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f)
		{
			TempY = ChPosition.y;
			ChPosition = VAdd(CheckCInfo->Position, VScale(PushVec, TotalWidth));

			// �x���W�͕ω������Ȃ�
			ChPosition.y = TempY;
		}
		else
		{
			// �����o��
			ChPosition = VAdd(ChPosition, VScale(PushVec, CHARA_HIT_PUSH_POWER));
		}
	}

	// �����蔻�菈����̈ړ��x�N�g�����Z�b�g
	*MoveVector = VSub(ChPosition, CInfo->Position);
}

// �X�e�[�W���f���Ƃ̓����蔻������Ȃ���L�������ړ�������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Move(
	// �ړ�������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �ړ��x�N�g��
	VECTOR MoveVector
)
{
	int                      i;
	int                      j;
	int                      k;

	// ���������Ɉړ��������ǂ����̃t���O
	// ( false : �ړ����Ă��Ȃ�  true:�ړ����� )
	bool                     MoveFlag;

	// �|���S���ɓ����������ǂ������L�����Ă����ϐ�
	// ( false : �������Ă��Ȃ�  true : �������� )
	int                      HitFlag;

	// �L�����N�^�[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];

	// ���������I�u�W�F�N�g�̐�
	int                      HitNum;

	// ���������|���S���̃^�C�v�ʂ̐�
	int                      HitPolyNum[EStageCollType_Num];

	// ���������^�C�v�ʂ̊e�|���S���ւ̃|�C���^
	MV1_COLL_RESULT_POLY    *HitPoly[EStageCollType_Num][CHARA_MAX_HITCOLL];

	// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^
	MV1_COLL_RESULT_POLY    *Poly;

	// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	HITRESULT_LINE           LineRes;

	// �ړ��O�̍��W	
	VECTOR                   OldPos;

	// �ړ���̍��W
	VECTOR                   NextPos;

	VECTOR                   BottomPos1;
	VECTOR                   BottomPos2;
	VECTOR                   TopPos1;
	VECTOR                   TopPos2;
	VECTOR                   LineTopPos;
	VECTOR                   LineBottomPos;
	VECTOR                  *PolyNormal;
	float                    TotalMoveDistance;
	float                    CompMoveDistance;
	float                    OneMoveMaxDistance;
	float                    OneMoveDistance;
	float                    SearchDistance;
	VECTOR                   MoveDirection;
	VECTOR                   TempMoveVector;
	EStageCollType           CollType;
	int                      StageObjectNum;
	const SCharaBaseInfo    *CBInfo;
	VECTOR                   SlideVec;
	VECTOR                   TempYVector;
	VECTOR                   PolyXZNormal;
	int                      CollisionModelHandle;

	CBInfo = CInfo->BaseInfo;

	// �ړ����������擾
	TotalMoveDistance = VSize(MoveVector);

	// �ړ��������O�̏ꍇ�͉��ɂw�������Ɉړ��������Ƃɂ���
	if (TotalMoveDistance < 0.000001f)
	{
		MoveDirection = VGet(1.0f, 0.0f, 0.0f);
	}
	else
	{
		MoveDirection = VScale(MoveVector, 1.0f / TotalMoveDistance);
	}

	// �X�e�[�W�I�u�W�F�N�g�̐����擾����
	StageObjectNum = StageData_GetObjectNum();

	// ��x�Ɉړ����鋗�����Z�o
	OneMoveMaxDistance = CBInfo->StageHitTopInfo.Width * 2.0f;

	// ��x�Ɉړ����鋗�����ړ������������ꍇ�͕�����ɕ����Ĉړ�����
	CompMoveDistance = 0.0f;
	do
	{
		// �ړ����鋗��������
		if (CompMoveDistance + OneMoveMaxDistance > TotalMoveDistance)
		{
			OneMoveDistance = TotalMoveDistance - CompMoveDistance;
		}
		else
		{
			OneMoveDistance = OneMoveMaxDistance;
		}

		// �ړ��x�N�g���̎Z�o
		TempMoveVector = VScale(MoveDirection, OneMoveDistance);

		// �L�����N�^�[�̎��͂ɂ���|���S������������ۂ̌����������Z�o
		SearchDistance = CHARA_ENUM_DEFAULT_SIZE + OneMoveDistance;

		// �ړ����鋗���𑫂�
		CompMoveDistance += OneMoveDistance;

		// �ړ��O�̍��W��ۑ�
		OldPos = CInfo->Position;

		// �ړ���̍��W���Z�o
		NextPos = VAdd(CInfo->Position, TempMoveVector);

		// �����蔻��Ɏg�p������W���v�Z����
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
			&NextPos, &TopPos1, &TopPos2);

		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
			&NextPos, &BottomPos1, &BottomPos2);

		// �����蔻�茋�ʏ��̐���������
		HitNum = 0;

		// �L�����N�^�[�̎��͂ɂ���X�e�[�W�|���S�����擾����
		HitDim[HitNum] = MV1CollCheck_Sphere(
			StageData_GetCollisionModelHandle(), -1,
			CInfo->Position, SearchDistance);
		if (HitDim[HitNum].HitNum != 0)
		{
			HitNum++;
		}
		else
		{
			MV1CollResultPolyDimTerminate(HitDim[HitNum]);
		}

		// �L�����N�^�[�̎��͂ɂ���R���W�����I�u�W�F�N�g�̃|���S�����擾����
		for (i = 0; i < StageObjectNum; i++)
		{
			CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

			if (CollisionModelHandle != -1)
			{
				HitDim[HitNum] = MV1CollCheck_Sphere(CollisionModelHandle, -1,
					CInfo->Position, SearchDistance);
				if (HitDim[HitNum].HitNum != 0)
				{
					HitNum++;
				}
				else
				{
					MV1CollResultPolyDimTerminate(HitDim[HitNum]);
				}
			}
		}

		// x����y�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O�𗧂Ă�
		MoveFlag = fabs(TempMoveVector.x) > 0.01f || fabs(TempMoveVector.z) > 0.01f;

		// �ǃ|���S���Ə��|���S���ƓV��|���S���̐�������������
		for (i = 0; i < EStageCollType_Num; i++)
		{
			HitPolyNum[i] = 0;
		}

		// �����蔻�茋�ʏ��̐������J��Ԃ�
		for (j = 0; j < HitNum; j++)
		{
			// ���o���ꂽ�|���S���̐������J��Ԃ�
			for (i = 0; i < HitDim[j].HitNum; i++)
			{
				// �|���S���̖@���̃A�h���X���擾
				PolyNormal = &HitDim[j].Dim[i].Normal;

				// ���ȏ�̌X�΂͕ǃ|���S���Ƃ��ď�������
				if (PolyNormal->x * PolyNormal->x + PolyNormal->z * PolyNormal->z > 0.5f)
				{
					CollType = EStageCollType_Wall;
				}
				else
					// �@�������������ǂ����ŏ����𕪊�
					if (PolyNormal->y <= 0.0f)
					{
						// �W�����v�����㏸���̏ꍇ�͓V��|���S���Ƃ��ď������A
						// �����ł͂Ȃ��ꍇ�͕ǃ|���S���Ƃ��ď�������
						if (CInfo->JumpState && CInfo->Spd.y > 0.0f)
						{
							CollType = EStageCollType_Ceiling;
						}
						else
						{
							CollType = EStageCollType_Wall;
						}
					}
					else
					{
						// ����ȊO�̏ꍇ�͏��|���S���Ƃ���
						CollType = EStageCollType_Floor;
					}

				// �|���S���̐�������ɒB���Ă��Ȃ��ꍇ�̓^�C�v�ʂ̃|���S���z��ɓo�^����
				if (HitPolyNum[CollType] < CHARA_MAX_HITCOLL)
				{
					HitPoly[CollType][HitPolyNum[CollType]] = &HitDim[j].Dim[i];
					HitPolyNum[CollType] ++;
				}
			}
		}

		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (HitPolyNum[EStageCollType_Wall] != 0)
		{
			// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
			HitFlag = false;

			// �ړ��������ǂ����ŏ����𕪊�
			if (MoveFlag)
			{
				// �ړ����Ă����ꍇ�͏Փ˂����ǃ|���S���ɑ΂��ăX���C�h���鏈�����s��

				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					Poly = HitPoly[EStageCollType_Wall][i];

					// �|���S���ƃL�����N�^�[���������Ă��Ȃ������玟�̃��[�v��
					if (HitCheck_Capsule_Triangle(
						BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
						Poly->Position[0], Poly->Position[1],
						Poly->Position[2]) == FALSE &&
						HitCheck_Capsule_Triangle(
							TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == FALSE) continue;

					// �L�����N���[�^�C�v���G�e�������ꍇ�̓L�������폜���ďI��
					if (CBInfo->Type == ECharaType_EnemyShot)
					{
						Chara_Delete(CInfo);
						for (k = 0; k < HitNum; k++)
						{
							MV1CollResultPolyDimTerminate(HitDim[k]);
						}
						return true;
					}

					// �����ɂ�����|���S���ƃL�����N�^�[���������Ă���Ƃ������ƂȂ̂ŁA
					// �|���S���ɓ��������t���O�𗧂Ă�
					HitFlag = true;

					// �ǃ|���S���ɓ���������ǂɎՂ��Ȃ��ړ��������ړ�����

					// �@���̂w�y�l�����𒊏o
					PolyXZNormal = Poly->Normal;
					PolyXZNormal.y = 0.0f;

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					TempYVector = VCross(TempMoveVector, PolyXZNormal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(PolyXZNormal, TempYVector);

					// �x�������ɂ̓X���C�h���Ȃ�
					SlideVec.y = 0.0f;

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					NextPos = VAdd(OldPos, SlideVec);

					// �x���W�ɂ͌��̈ړ��l�����Z����
					NextPos.y += TempMoveVector.y;

					// �����蔻��Ɏg�p������W���X�V����
					CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
						&NextPos, &TopPos1, &TopPos2);

					CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
						&NextPos, &BottomPos1, &BottomPos2);

					// �V���Ȉړ�����W�ŕǃ|���S���Ɠ����邩�ǂ����𔻒肷��
					for (j = 0; j < HitPolyNum[EStageCollType_Wall]; j++)
					{
						// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
						Poly = HitPoly[EStageCollType_Wall][j];

						// �������Ă����烋�[�v���甲����
						if (HitCheck_Capsule_Triangle(
							BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == TRUE ||
							HitCheck_Capsule_Triangle(
								TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == TRUE) break;
					}

					// j �� HitPolyNum[ EStageCollType_Wall ] �������ꍇ��
					// �ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
					// �w�ǂɓ����������ǂ����x�̃t���O��|������Ń��[�v���甲����
					if (j == HitPolyNum[EStageCollType_Wall])
					{
						HitFlag = false;
						break;
					}
				}
			}
			else
			{
				// �ړ����Ă��Ȃ��ꍇ�͓������Ă���|���S�������邩�ǂ��������m�F

				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					Poly = HitPoly[EStageCollType_Wall][i];

					// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
					if (HitCheck_Capsule_Triangle(
						BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
						Poly->Position[0], Poly->Position[1],
						Poly->Position[2]) == TRUE ||
						HitCheck_Capsule_Triangle(
							TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == TRUE)
					{
						// �L�����N���[�^�C�v���G�e�������ꍇ�̓L�������폜���ďI��
						if (CBInfo->Type == ECharaType_EnemyShot)
						{
							Chara_Delete(CInfo);
							for (k = 0; k < HitNum; k++)
							{
								MV1CollResultPolyDimTerminate(HitDim[k]);
							}
							return true;
						}

						HitFlag = true;
						break;
					}
				}
			}

			// �ǂɓ������Ă�����ǂ��牟���o���������s��
			if (HitFlag)
			{
				// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
				for (k = 0; k < CHARA_HIT_TRYNUM; k++)
				{
					// �ǃ|���S���̐������J��Ԃ�
					for (i = 0; i < HitPolyNum[EStageCollType_Wall]; i++)
					{
						VECTOR PolyXZNormal;

						// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
						Poly = HitPoly[EStageCollType_Wall][i];

						// �L�����N�^�[�Ɠ������Ă��邩�𔻒�
						if (HitCheck_Capsule_Triangle(
							BottomPos1, BottomPos2, CBInfo->StageHitBottomInfo.Width,
							Poly->Position[0], Poly->Position[1],
							Poly->Position[2]) == FALSE &&
							HitCheck_Capsule_Triangle(
								TopPos1, TopPos2, CBInfo->StageHitTopInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == FALSE) continue;

						// �������Ă�����L�����N�^�[��ǂ̖@�������Ɉ�苗���ړ�������
						PolyXZNormal = Poly->Normal;
						PolyXZNormal.y = 0.0f;
						NextPos = VAdd(NextPos, VScale(PolyXZNormal,
							CHARA_HIT_SLIDE_DISTANCE));

						// �����蔻��Ɏg�p������W���X�V����
						CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitTopInfo,
							&NextPos, &TopPos1, &TopPos2);

						CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->StageHitBottomInfo,
							&NextPos, &BottomPos1, &BottomPos2);

						// �ړ�������ŕǃ|���S���Ɠ����邩�ǂ����𔻒�
						for (j = 0; j < HitPolyNum[EStageCollType_Wall]; j++)
						{
							// �������Ă����烋�[�v�𔲂���
							Poly = HitPoly[EStageCollType_Wall][j];
							if (HitCheck_Capsule_Triangle(
								BottomPos1, BottomPos2,
								CBInfo->StageHitBottomInfo.Width,
								Poly->Position[0], Poly->Position[1],
								Poly->Position[2]) == TRUE ||
								HitCheck_Capsule_Triangle(
									TopPos1, TopPos2,
									CBInfo->StageHitTopInfo.Width,
									Poly->Position[0], Poly->Position[1],
									Poly->Position[2]) == TRUE) break;
						}

						// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
						if (j == HitPolyNum[EStageCollType_Wall])
						{
							break;
						}
					}

					// i �� HitPolyNum[ EStageCollType_Wall ] �ł͂Ȃ��ꍇ��
					// �S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S����
					// �ڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
					if (i != HitPolyNum[EStageCollType_Wall])
					{
						break;
					}
				}
			}
		}

		// �V��|���S���Ƃ̓����蔻��
		if (HitPolyNum[EStageCollType_Ceiling] != 0)
		{
			float MinY;

			// �V��ɓ����Ԃ��鏈�����s��

			// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
			MinY = 0.0f;

			// �����������ǂ����̃t���O��|��
			HitFlag = false;

			// �����蔻��p�̐����̍��W���Z�o
			LineTopPos = VAdd(NextPos, CBInfo->StageHitTopInfo.CenterPosition);
			LineTopPos.y += CBInfo->StageHitTopInfo.Height / 2.0f
				+ CBInfo->StageHitTopInfo.Width;

			LineBottomPos = VAdd(NextPos, CBInfo->StageHitBottomInfo.CenterPosition);
			LineBottomPos.y -= CBInfo->StageHitBottomInfo.Height / 2.0f
				+ CBInfo->StageHitBottomInfo.Width;

			// �V��|���S���̐������J��Ԃ�
			for (i = 0; i < HitPolyNum[EStageCollType_Ceiling]; i++)
			{
				// i�Ԗڂ̓V��|���S���̃A�h���X��V��|���S���|�C���^�z�񂩂�擾
				Poly = HitPoly[EStageCollType_Ceiling][i];

				// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				LineRes = HitCheck_Line_Triangle(LineBottomPos, LineTopPos,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// �ڐG���Ă��Ȃ������牽�����Ȃ�
				if (LineRes.HitFlag == FALSE)
				{
					continue;
				}

				// �L�����N���[�^�C�v���G�e�������ꍇ�͍폜���ďI��
				if (CBInfo->Type == ECharaType_EnemyShot)
				{
					Chara_Delete(CInfo);
					for (k = 0; k < HitNum; k++)
					{
						MV1CollResultPolyDimTerminate(HitDim[k]);
					}
					return true;
				}

				// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o����
				// �V��|���S����荂���ꍇ�͉������Ȃ�
				if (HitFlag && MinY < LineRes.Position.y)
				{
					continue;
				}

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MinY = LineRes.Position.y;
			}

			// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
			if (HitFlag)
			{
				// �ڐG�����ꍇ�̓L�����N�^�[�̂x���W��ڐG���W�����ɍX�V
				NextPos.y = MinY - (LineTopPos.y - LineBottomPos.y);

				// �x�������̑��x�͔��]
				CInfo->Spd.y = -CInfo->Spd.y;
			}
		}

		// ���|���S���Ƃ̓����蔻��
		if (HitPolyNum[EStageCollType_Floor] != 0)
		{
			float MaxY;
			int   MaxIndex;

			// �|���S���ɓ����������ǂ����̃t���O��|���Ă���
			HitFlag = false;

			// ��ԍ������|���S���ɐڒn������ׂ̔���p�ϐ���������
			MaxY = 0.0f;
			MaxIndex = -1;

			// �����蔻��̐����̍��W���Z�b�g
			LineTopPos = VAdd(NextPos, CBInfo->StageHitTopInfo.CenterPosition);
			LineTopPos.y += CBInfo->StageHitTopInfo.Height / 2.0f
				+ CBInfo->StageHitTopInfo.Width;

			LineBottomPos = VAdd(NextPos, CBInfo->StageHitBottomInfo.CenterPosition);
			LineBottomPos.y -= CBInfo->StageHitBottomInfo.Height / 2.0f
				+ CBInfo->StageHitBottomInfo.Width;

			// ���[�̒l��������p�ɕ␳
			if (CInfo->JumpState)
			{
				LineBottomPos.y += CHARA_HIT_FLOOR_Y_ADJUST_JUMP;
			}
			else
			{
				LineBottomPos.y += CHARA_HIT_FLOOR_Y_ADJUST_WALK;
			}

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < HitPolyNum[EStageCollType_Floor]; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				Poly = HitPoly[EStageCollType_Floor][i];

				// �����ƃ|���S���̓����蔻��
				LineRes = HitCheck_Line_Triangle(LineTopPos, LineBottomPos,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// �������Ă��Ȃ������玟�̃��[�v��
				if (LineRes.HitFlag == FALSE)
				{
					continue;
				}

				// �L�����N���[�^�C�v���G�e�������ꍇ�͍폜���ďI��
				if (CBInfo->Type == ECharaType_EnemyShot)
				{
					Chara_Delete(CInfo);
					for (k = 0; k < HitNum; k++)
					{
						MV1CollResultPolyDimTerminate(HitDim[k]);
					}
					return true;
				}

				// ���ɓ��������|���S��������A
				// �����܂Ō��o�������|���S�����Ⴂ�ꍇ�͎��̃��[�v��
				if (HitFlag && MaxY > LineRes.Position.y)
				{
					continue;
				}

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MaxY = LineRes.Position.y;

				// �ڐG�����|���S���̃C���f�b�N�X��ۑ�����
				MaxIndex = i;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (HitFlag)
			{
				// ���������ꍇ

				// �ڐG�����|���S���ň�ԍ����x���W���L�����N�^�[�̂x���W�ɂ���
				NextPos.y = MaxY;

				// �x�������̈ړ����x�͂O��
				CInfo->Spd.y = 0.0f;

				// �ڐG�����|���S���̃}�e���A���^�C�v���擾����
				CInfo->OnMaterialType = StageData_GetCollisionModelMaterialType(
					HitPoly[EStageCollType_Floor][MaxIndex]->MaterialIndex);

				// �����W�����v���������ꍇ�͒��n��ԂɈڍs����
				if (CInfo->JumpState)
				{
					if (!Chara_Landed(CInfo))
					{
						return false;
					}
					CInfo->JumpState = false;
				}
			}
			else
			{
				// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
				// �W�����v��ԂɈڍs����
				if (!CInfo->JumpState)
				{
					CInfo->JumpState = true;
					if (!Chara_Fall(CInfo))
					{
						return false;
					}
				}
			}
		}

		// ���o�����L�����N�^�[�̎��͂̃|���S�������J������
		for (i = 0; i < HitNum; i++)
		{
			MV1CollResultPolyDimTerminate(HitDim[i]);
		}

		// �V�������W��ۑ�����
		CInfo->Position = NextPos;

		// �S�Ă̈ړ����������Ă��Ȃ��ꍇ�̓��[�v
	} while (CompMoveDistance + 0.001f < TotalMoveDistance);

	// �L�����N�^�[�̂R�c���f���̍��W���X�V����
	MV1SetPosition(CInfo->ModelHandle, CInfo->Position);

	// ����I��
	return true;
}

// �L�����̈ړ������𐄈ڂ�����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_MoveStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SCharaInfo *ChkCInfo;
	VECTOR      MoveVector;
	VECTOR      AnimMoveVector;
	int         i;

	// �W�����v���̏ꍇ�͏d�͏������s��
	if (CInfo->JumpState)
	{
		CInfo->Spd.y -= CHARA_GRAVITY * StepTime;
	}

	// �ړ��x�N�g���̎Z�o
	MoveVector = VScale(VAdd(CInfo->PrevSpd, CInfo->Spd), StepTime / 2.0f);

	// ���̈ړ����x��ۑ����Ă���
	CInfo->PrevSpd = CInfo->Spd;

	// ���[�g�t���[���̃A�j���[�V�����ɂ��ړ��l�̎Z�o
	AnimMoveVector = VScale(CInfo->AngleInfo.FrontDirection, -CInfo->AnimInfo.Move.z);
	AnimMoveVector = VAdd(AnimMoveVector,
		VScale(CInfo->AngleInfo.LeftDirection, -CInfo->AnimInfo.Move.x));

	// ���[�g�t���[���̃A�j���[�V�����ɂ��ړ����x��ۑ����Ă���
	if (StepTime < 0.0000001f)
	{
		CInfo->PrevAnimSpd = VGet(0.0f, 0.0f, 0.0f);
	}
	else
	{
		CInfo->PrevAnimSpd = VScale(AnimMoveVector, 1.0f / StepTime);
	}

	// �A�j���[�V�����ɂ��ړ��l���ړ��x�N�g���ɉ��Z
	MoveVector = VAdd(MoveVector, AnimMoveVector);

	// �v���O�����ɂ��ړ����s���ꍇ�̓v���O�����Őݒ肵���ړ����x�ɂ��ړ��l�����Z����	
	if (CInfo->ProgramMove)
	{
		MoveVector = VAdd(MoveVector, VScale(CInfo->AngleInfo.FrontDirection,
			CInfo->BaseInfo->ProgramSpd * StepTime));
	}

	// �ړ��x�N�g������ړ����x���Z�o
	if (StepTime < 0.0000001f)
	{
		CInfo->PrevFixSpd = VGet(0.0f, 0.0f, 0.0f);
	}
	else
	{
		CInfo->PrevFixSpd = VScale(MoveVector, 1.0f / StepTime);
	}

	// �G�̒e�ł͂Ȃ��ꍇ�̓L�����N�^�[���m�̓����蔻����s��
	if (CInfo->BaseInfo->Type != ECharaType_EnemyShot)
	{
		// �L�����N�^�[�̐������J��Ԃ�
		ChkCInfo = g_CharaInfo;
		for (i = 0; i < CHARA_MAXNUM; i++, ChkCInfo++)
		{
			// �g�p����Ă��Ȃ��\���̂̏ꍇ�͎��̃��[�v��
			if (ChkCInfo->UseFlag == FALSE)
			{
				continue;
			}

			// �`�F�b�N���悤�Ƃ��Ă���L������
			// �ړ����悤�Ƃ��Ă���L�����������玟�̃��[�v��
			if (ChkCInfo == CInfo)
			{
				continue;
			}

			// �`�F�b�N���悤�Ƃ��Ă���L�������G�̒e�������ꍇ��
			// �G�̒e�͂��蔲����̂Ŏ��̃��[�v��
			if (ChkCInfo->BaseInfo->Type == ECharaType_EnemyShot)
			{
				continue;
			}

			// �L�������m�̓����蔻��
			Chara_Collision(CInfo, &MoveVector, ChkCInfo);
		}
	}

	// �X�e�[�W�̂R�c���f���Ƃ̓����蔻��𔺂����ړ��������s��
	if (!Chara_Move(CInfo, MoveVector))
	{
		return false;
	}

	// �W�����v���ł͂Ȃ��ꍇ�͈ړ����x���O�ɂ���
	if (!CInfo->JumpState)
	{
		CInfo->Spd = VGet(0.0f, 0.0f, 0.0f);
	}

	// ����I��
	return true;
}

// �L�����̌�������������������
static void Chara_InitializeAngle(
	// ��������������������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �����̌���
	float Angle
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	CAngInfo->NowAngle = Angle;
	CAngInfo->TargetAngle = Angle;
	CAngInfo->AngleSpeed = CHARA_DEFAULT_ANGLE_SPEED;
	CAngInfo->AngleMoveEnd = false;
}

// �L�������w��̍��W�̕����Ɍ�������
void Chara_TargetAngleMove(
	// ������ύX����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �L�����Ɍ������������W
	VECTOR TargetPosition
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;
	VECTOR TargetVector;

	// �L�����̍��W����w��̍��W�ւ̃x�N�g�����Z�o
	TargetVector = VSub(TargetPosition, CInfo->Position);

	// atan2 ���g�p���ĕ����x�N�g������p�x�����߂�
	Chara_SetTargetAngle(CInfo, atan2(TargetVector.x, TargetVector.z), true);
}

// �L�����������ׂ�������ݒ肷��
void Chara_SetTargetDirection(
	// ������ݒ肷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �L�����Ɍ����������
	VECTOR Direction,

	// ���X�Ɍ�����ς��邩�ǂ���
	// ( true : ���X�Ɍ�����ς���  false : ��u�Ō�����ς��� )
	bool Interp,

	// Direction �̌����𔽓]���Ďg�p���邩�ǂ���
	// ( true : ���]���Ďg�p����  false : ���]���Ȃ� )
	bool Reverse
)
{
	// �����𔽓]���Ďg�p����ꍇ�͂����Ńx�N�g���𔽓]������
	if (Reverse)
	{
		Direction = VScale(Direction, -1.0f);
	}

	// atan2 ���g�p���Ċp�x�����߂�
	Chara_SetTargetAngle(CInfo, atan2(Direction.x, Direction.z), Interp);
}

// �L�����������ׂ�������ݒ肷��( �x���p�x�w��^�C�v )
void Chara_SetTargetAngle(
	// ������ݒ肷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �L�����Ɍ����������( �x���p�x )
	float Angle,

	// ���X�Ɍ�����ς��邩�ǂ���
	// ( true:���X�Ɍ�����ς���  false:��u�Ō�����ς��� )
	bool Interp
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	// �ڕW�Ƃ���p�x���Z�b�g
	CAngInfo->TargetAngle = Angle;

	// ��Ԃ��s�����ǂ����ŏ����𕪊�
	if (!Interp)
	{
		// ��Ԃ��s��Ȃ��ꍇ�͑����Ɍ����𔽉f����
		CAngInfo->NowAngle = Angle;
		CAngInfo->AngleMoveEnd = true;
		Chara_AngleParamRefresh(CInfo);
	}
	else
	{
		// ��Ԃ��s���ꍇ�͌����̕ύX���I�����Ă��邩�ǂ����̃t���O��|��
		CAngInfo->AngleMoveEnd = false;
	}
}

// �L�����̌��������̏�Ԑ��ڂ��s��
static void Chara_AngleStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;

	// �����̕ύX�������s��
	if (ParamChangeAngle(&CAngInfo->NowAngle, CAngInfo->TargetAngle,
		StepTime, CAngInfo->AngleSpeed))
	{
		// �ύX����������������̕ύX���I�����Ă��邩�ǂ����̃t���O�𗧂Ă�
		CAngInfo->AngleMoveEnd = true;

	}

	// �����ɕt������p�����[�^���X�V����
	Chara_AngleParamRefresh(CInfo);
}

// �L�����̌����Ă�������Ɋ�Â������X�V����
void Chara_AngleParamRefresh(
	// �����X�V����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SCharaAngleInfo *CAngInfo = &CInfo->AngleInfo;
	float            fSin;
	float            fCos;

	// �R�c���f���̌�����ݒ肷��
	MV1SetRotationXYZ(CInfo->ModelHandle, VGet(0.0f, CAngInfo->NowAngle + DX_PI_F, 0.0f));

	// �R�c���f���������Ă�������̃x�N�g�����Z�o
	fSin = (float)sin(CAngInfo->NowAngle);
	fCos = (float)cos(CAngInfo->NowAngle);
	CAngInfo->FrontDirection.x = fSin;
	CAngInfo->FrontDirection.y = 0.0f;
	CAngInfo->FrontDirection.z = fCos;

	// �R�c���f�����猩�č������̃x�N�g�����Z�o
	fSin = (float)sin(CAngInfo->NowAngle + DX_PI * 0.5f);
	fCos = (float)cos(CAngInfo->NowAngle + DX_PI * 0.5f);
	CAngInfo->LeftDirection.x = fSin;
	CAngInfo->LeftDirection.y = 0.0f;
	CAngInfo->LeftDirection.z = fCos;
}

// �L�����̃A�j���[�V��������������������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_InitializeAnim(
	// �A�j���[�V��������������������L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SCharaAnimInfo * CAInfo = &CInfo->AnimInfo;

	// ���̏�����
	CAInfo->NowAnim = ECharaAnim_Num;
	CAInfo->PrevAnim = ECharaAnim_Num;
	CAInfo->NowAttachIndex = -1;
	CAInfo->PrevAttachIndex = -1;
	CAInfo->ChangeSpeed = 1.0f;
	CAInfo->ChangeRate = 1.0f;
	CAInfo->NowTime = 0.0f;
	CAInfo->PrevTime = 0.0f;

	CAInfo->Cancel = false;
	CAInfo->End = false;
	CAInfo->Move = VGet(0.0f, 0.0f, 0.0f);

	// ������Ԃł̓j���[�g�����A�j���[�V�������Đ�����
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral, -1.0f))
	{
		return false;
	}

	// ����I��
	return true;
}

// �L�����̃A�j���[�V���������̏�Ԑ��ڂ��s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AnimStep(
	// ��Ԑ��ڂ��s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	SCharaAnimInfo       *CAInfo = &CInfo->AnimInfo;
	SAnimEventCheck       AECheck;
	const SAnimEventInfo *AEInfo;
	float                 AnimTotalTime;
	VECTOR                NowAnimMoveVector;
	VECTOR                PrevAnimMoveVector;
	VECTOR                PrevAnimPosition;
	VECTOR                NowAnimPosition;
	float                 StartTime;
	bool                  EndTimeFlag;
	int                   i;
	int                   RootFrameIndex;

	// �A�j���[�V�����ɂ��ړ��x�N�g����������
	NowAnimMoveVector = VGet(0.0f, 0.0f, 0.0f);
	PrevAnimMoveVector = VGet(0.0f, 0.0f, 0.0f);

	// ���[�g�t���[���̔ԍ����擾
	RootFrameIndex = CInfo->BaseInfo->RootFrameIndex;

	// �O��Đ����Ă����A�j���[�V����������ꍇ�͂��̏���������
	if (CAInfo->PrevAttachIndex >= 0)
	{
		// �O��Đ����Ă����A�j���[�V�����̃��[�g�t���[���̃��[�J�����W���擾
		PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
			CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

		// �O��Đ����Ă����A�j���[�V�����̑��Đ����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(
			CInfo->ModelHandle, CAInfo->PrevAttachIndex);

		// �O��Đ����Ă����A�j���[�V�����̍Đ����Ԃ�i�߂�
		CAInfo->PrevTime += StepTime * CHARA_ANIM_FPS;

		// �Đ����Ԃ����Đ����Ԃ��z�������ǂ����ŏ����𕪊�
		if (CAInfo->PrevTime >= AnimTotalTime)
		{
			// �������ꍇ�̓��[�v�Đ����邩�ǂ����ŏ����𕪊�
			if (CharaBase_GetAnimFileInfo(CAInfo->PrevAnim)->LoopFlag)
			{
				// ���[�v����ꍇ�͍Đ����Ԃ��瑍�Đ����Ԉ���
				CAInfo->PrevTime -= AnimTotalTime;

				// �A�j���[�V�����ɂ��ړ��x�N�g���Ƃ��āw�Đ����Ԃ�i�߂�O��
				// ���[�g�t���[������A�j���[�V�����̍Ō�̈ʒu�ł̃��[�g�t���[��
				// �̍��W�̍����x�Ɓw�A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[��
				// �̍��W����Đ����Ԃ�i�߂���̃��[�g�t���[���̍��W�̍����x���g�p����

				// �Đ����Ԃ��A�j���[�V�����̏I�[�ɕύX
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, AnimTotalTime);

				// �A�j���[�V�����I�[�ł̃��[�g�t���[���̃��[�J�����W���擾
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̈ʒu�ƁA�A�j���[�V�����I�[�ł�
				// ���[�g�t���[���̍��W�̍������Z�o
				PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);

				// �Đ����Ԃ��A�j���[�V�����̍ŏ��ɕύX
				MV1SetAttachAnimTime(CInfo->ModelHandle, CAInfo->PrevAttachIndex, 0);

				// �A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[���̃��[�J�����W���擾
				PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// �Đ����Ԃ�i�߂���̈ʒu�ɕύX
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

				// �Đ����Ԃ�i�߂���̃��[�g�t���[���̃��[�J�����W���擾
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				// �A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[���̃��[�J�����W��
				// �Đ����Ԃ�i�߂���̃��[�g�t���[���̃��[�J�����W�̍�����
				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̈ʒu�ƁA�A�j���[�V�����I�[�ł�
				// ���[�g�t���[���̍��W�̍����ɉ��Z
				PrevAnimMoveVector = VAdd(VSub(NowAnimPosition, PrevAnimPosition),
					PrevAnimMoveVector);
			}
			else
			{
				// ���[�v���Ȃ��ꍇ�͍Đ����Ԃ��I�[�̎��Ԃɂ���
				CAInfo->PrevTime = AnimTotalTime;

				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̃��[�J�����W��
				// �A�j���[�V�����̍Ō�̈ʒu�ł̃��[�g�t���[���̃��[�J�����W�̍������擾
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

				PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
			}
		}
		else
		{
			// ���Đ����Ԃ𒴂��Ă��Ȃ��ꍇ�͍Đ����Ԃ�i�߂���̃��[�g�t���[����
			// ���[�J�����W�ƍĐ����Ԃ�i�߂�O�̃��[�g�t���[���̃��[�J�����W�̍������擾
			MV1SetAttachAnimTime(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, CAInfo->PrevTime);

			NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, RootFrameIndex);

			PrevAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
		}
	}

	// ���ݍĐ����Ă���A�j���[�V����������ꍇ�͂��̏���������
	if (CAInfo->NowAttachIndex >= 0)
	{
		// �Đ����Ă���A�j���[�V�����̃��[�g�t���[���̃��[�J�����W���擾
		PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
			CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

		// �Đ����Ă���A�j���[�V�����̑��Đ����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(
			CInfo->ModelHandle, CAInfo->NowAttachIndex);

		// �i�߂�O�̍Đ����Ԃ�ێ����Ă���
		StartTime = CAInfo->NowTime;

		// �Đ����Ԃ��A�j���[�V�����̏I�[���ǂ����̃t���O��|���Ă���
		EndTimeFlag = false;

		// �Đ����Ԃ�i�߂�
		CAInfo->NowTime += StepTime * CHARA_ANIM_FPS;

		// �Đ����Ԃ����Đ����Ԃ��z�������ǂ����ŏ����𕪊�
		if (CAInfo->NowTime >= AnimTotalTime)
		{
			// �����Ă����ꍇ�̓��[�v�Đ��Ȃǂ̏������s��

			// �A�j���[�V�����̍Đ����I���������ǂ����̃t���O�𗧂Ă�
			CAInfo->End = true;

			// �A�j���[�V���������[�v���邩�ǂ����ŏ����𕪊�
			if (CharaBase_GetAnimFileInfo(CAInfo->NowAnim)->LoopFlag)
			{
				// ���[�v����ꍇ�͍Đ����Ԃ��瑍�Đ����Ԉ���
				CAInfo->NowTime -= AnimTotalTime;

				// �A�j���[�V�����ɂ��ړ��x�N�g���Ƃ��āw�Đ����Ԃ�i�߂�O��
				// ���[�g�t���[������A�j���[�V�����̍Ō�̈ʒu�ł̃��[�g�t���[��
				// �̍��W�̍����x�Ɓw�A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[��
				// �̍��W����Đ����Ԃ�i�߂���̃��[�g�t���[���̍��W�̍����x���g�p����

				// �Đ����Ԃ��A�j���[�V�����̏I�[�ɕύX
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, AnimTotalTime);

				// �A�j���[�V�����I�[�ł̃��[�g�t���[���̃��[�J�����W���擾
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̈ʒu�ƁA�A�j���[�V�����I�[�ł�
				// ���[�g�t���[���̍��W�̍������Z�o
				NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);

				// �Đ����Ԃ��A�j���[�V�����̍ŏ��ɕύX
				MV1SetAttachAnimTime(CInfo->ModelHandle, CAInfo->NowAttachIndex, 0);

				// �A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[���̃��[�J�����W���擾
				PrevAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// �Đ����Ԃ�i�߂���̈ʒu�ɕύX
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

				// �Đ����Ԃ�i�߂���̃��[�g�t���[���̃��[�J�����W���擾
				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				// �A�j���[�V�����̍ŏ��̈ʒu�ł̃��[�g�t���[���̃��[�J�����W��
				// �Đ����Ԃ�i�߂���̃��[�g�t���[���̃��[�J�����W�̍�����
				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̈ʒu�ƁA�A�j���[�V�����I�[�ł�
				// ���[�g�t���[���̍��W�̍����ɉ��Z
				NowAnimMoveVector = VAdd(VSub(NowAnimPosition, PrevAnimPosition),
					NowAnimMoveVector);
			}
			else
			{
				// ���[�v���Ȃ��ꍇ�͍Đ����Ԃ��I�[�̎��Ԃɂ���
				CAInfo->NowTime = AnimTotalTime;

				// �Đ����Ԃ��A�j���[�V�����̏I�[���ǂ����̃t���O�𗧂Ă�
				EndTimeFlag = true;

				// �Đ����Ԃ�i�߂�O�̃��[�g�t���[���̃��[�J�����W��
				// �A�j���[�V�����̍Ō�̈ʒu�ł̃��[�g�t���[���̃��[�J�����W�̍������擾
				MV1SetAttachAnimTime(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

				NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
					CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

				NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
			}
		}
		else
		{
			// ���Đ����Ԃ𒴂��Ă��Ȃ��ꍇ�͍Đ����Ԃ�i�߂���̃��[�g�t���[����
			// ���[�J�����W�ƍĐ����Ԃ�i�߂�O�̃��[�g�t���[���̃��[�J�����W�̍������擾
			MV1SetAttachAnimTime(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->NowTime);

			NowAnimPosition = MV1GetAttachAnimFrameLocalPosition(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, RootFrameIndex);

			NowAnimMoveVector = VSub(NowAnimPosition, PrevAnimPosition);
		}

		// �i�߂�O�̍Đ����ԂƁA�i�߂���̍Đ����Ԃ̊Ԃɑ��݂���
		// �A�j���[�V�����C�x���g���擾
		AnimInfoEventCheck(&CInfo->BaseInfo->AnimInfo[CAInfo->NowAnim].Info,
			&AECheck, StartTime, EndTimeFlag ? -1.0f : CAInfo->NowTime);

		// �A�j���[�V�����C�x���g�̐������J��Ԃ�
		for (i = 0; i < AECheck.EventNum; i++)
		{
			AEInfo = AECheck.Event[i];

			// �A�j���[�V�����C�x���g�^�C�v�ɂ���ď����𕪊�
			switch (AEInfo->Type)
			{
			case EAnimEventType_Sound:			// �����Đ�
												// �����Đ�����
				Sound_PlaySound3D(CInfo->Position, AEInfo->SoundHandle, DX_PLAYTYPE_BACK);
				break;

			case EAnimEventType_PhysicsSound:	// ���������Đ�
												// ���ݗ����Ă���ꏊ�̑f�ނɂ�����������������ꍇ�͍Đ�
				if (CInfo->BaseInfo->AnimPhysicsSound[AEInfo->PhysicsSound]
					[CInfo->OnMaterialType] != -1)
				{
					Sound_PlaySound3D(CInfo->Position,
						CInfo->BaseInfo->AnimPhysicsSound[AEInfo->PhysicsSound]
						[CInfo->OnMaterialType], DX_PLAYTYPE_BACK);
				}
				break;

			case EAnimEventType_AnimCancel:		// �A�j���[�V�����L�����Z��
												// �A�j���[�V�����L�����Z���C�x���g�������������ǂ����̃t���O�𗧂Ă�
				CAInfo->Cancel = true;
				break;

			case EAnimEventType_AttackStart:	// �U������J�n
												// �U������L���ɂ���
				CInfo->AttackInfo[AEInfo->AttackNo].Enable = true;
				CInfo->AttackInfo[AEInfo->AttackNo].AttackPosIndex =
					AEInfo->AttackPosIndex;
				break;

			case EAnimEventType_AttackEnd:		// �U������I��
												// �U�����𖳌��ɂ���
				CInfo->AttackInfo[AEInfo->AttackNo].Enable = false;
				break;

			case EAnimEventType_Other:			// ���̑�
												// ���̑��C�x���g�����������ۂɎ��s����֐������݂���ꍇ�͎��s����
				if (g_CharaFunctionTable[CInfo->Chara].AnimOtherEvent != NULL)
				{
					if (!g_CharaFunctionTable[CInfo->Chara].AnimOtherEvent(CInfo))
					{
						return false;
					}
				}
				break;
			}
		}
	}

	// �Đ�����A�j���[�V�����̕ύX���s���Ă��邩�ǂ����ŏ����𕪊�
	if (CAInfo->PrevAttachIndex >= 0)
	{
		// �A�j���[�V�����̕ύX���s���Ă���ꍇ

		// �ύX����i�߂�
		CAInfo->ChangeRate += CAInfo->ChangeSpeed * StepTime;

		// �ύX�������������ǂ����ŏ����𕪊�
		if (CAInfo->ChangeRate >= 1.0f)
		{
			CAInfo->ChangeRate = 1.0f;

			// �ύX�����������ꍇ�͑O��Đ����Ă����A�j���[�V�������f�^�b�`����
			MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
			CAInfo->PrevAttachIndex = -1;

			// ���ݍĐ����Ă���A�j���[�V�����̉e�������P�O�O���ɂ���
			MV1SetAttachAnimBlendRate(CInfo->ModelHandle, CAInfo->NowAttachIndex, 1.0f);

			// �A�j���[�V�����ɂ��ړ��x�N�g���Ƃ��Č��ݍĐ����Ă���A�j���[�V������
			// ���[�g�t���[���̈ړ��l��ݒ肷��
			CAInfo->Move = NowAnimMoveVector;
		}
		else
		{
			// �ύX���������Ă��Ȃ��ꍇ�͕ύX���ɉ�����
			// ��̃A�j���[�V�����̃u�����h���s��
			MV1SetAttachAnimBlendRate(
				CInfo->ModelHandle, CAInfo->NowAttachIndex, CAInfo->ChangeRate);
			MV1SetAttachAnimBlendRate(
				CInfo->ModelHandle, CAInfo->PrevAttachIndex, 1.0f - CAInfo->ChangeRate);

			// �A�j���[�V�����ɂ��ړ��x�N�g������̃A�j���[�V������
			// ���[�g�t���[���̈ړ��l���u�����h����
			CAInfo->Move = VScale(PrevAnimMoveVector, 1.0f - CAInfo->ChangeRate);
			CAInfo->Move = VAdd(CAInfo->Move, VScale(NowAnimMoveVector,
				CAInfo->ChangeRate));
		}
	}
	else
	{
		// �A�j���[�V�����̕ύX���s���Ă��Ȃ��ꍇ

		// �A�j���[�V�����ɂ��ړ��x�N�g���͌��ݍĐ����Ă���A�j���[�V������
		// ���[�g�t���[���̈ړ��l��ݒ肷��
		CAInfo->Move = NowAnimMoveVector;
	}

	// ����I��
	return true;
}

// �L�������Đ�����A�j���[�V������ύX����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Chara_ChangeAnim(
	// �A�j���[�V�������Đ�����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �V���ɍĐ�����A�j���[�V����
	ECharaAnim NewAnim,

	// ���ݍĐ����Ă���A�j������V���ɍĐ�����A�j���Ɉڍs���鑬�x( �ڍs��/�b )
	float ChangeSpeed
)
{
	SCharaAnimInfo *CAInfo = &CInfo->AnimInfo;
	int             i;

	// �w��̃A�j���[�V���������������ꍇ�͉��������ɏI��
	if (CInfo->BaseInfo->AnimInfo[NewAnim].Handle < 0)
	{
		return true;
	}

	// �O��Đ����Ă����A�j���[�V�����Ƃ̕ύX���������ꍇ�͋����I�ɕύX�������I��
	if (CAInfo->PrevAttachIndex >= 0)
	{
		MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
		CAInfo->PrevAnim = ECharaAnim_Num;
		CAInfo->PrevAttachIndex = -1;
	}

	// ���ݍĐ����Ă���A�j���[�V������O��Đ����Ă����A�j���[�V�����ɂ���
	CAInfo->PrevAnim = CAInfo->NowAnim;

	// ���ݍĐ����Ă���A�j���[�V������ύX
	CAInfo->NowAnim = NewAnim;

	// �O��Đ����Ă����A�j���[�V�����̍Đ����ԂƂ���
	// ���܂ōĐ����Ă����A�j���[�V�����̍Đ����Ԃ���
	CAInfo->PrevTime = CAInfo->NowTime;

	// ���ݍĐ����Ă���A�j���[�V�����̍Đ����Ԃ�������
	CAInfo->NowTime = 0.0f;

	// ���ݍĐ����Ă���A�j���[�V�����̃A�^�b�`�ԍ���
	// �O��Đ����Ă����A�j���[�V�����̃A�^�b�`�ԍ��ɂ���
	CAInfo->PrevAttachIndex = CAInfo->NowAttachIndex;

	// �V�����A�j���[�V�������R�c���f���ɃA�^�b�`����
	CAInfo->NowAttachIndex =
		MV1AttachAnim(CInfo->ModelHandle, 0, CInfo->BaseInfo->AnimInfo[NewAnim].Handle);

	// ChangeSpeed ���}�C�i�X�̏ꍇ�́w���X�ɃA�j���[�V������ύX����x�������s��Ȃ�
	if (ChangeSpeed < 0.0f)
	{
		// �A�j���[�V�����̕ύX�����P�O�O���ɂ���
		CAInfo->ChangeRate = 1.0f;

		// ���܂ōĐ����Ă����A�j���[�V�������f�^�b�`����
		CAInfo->PrevAnim = ECharaAnim_Num;
		if (CAInfo->PrevAttachIndex >= 0)
		{
			MV1DetachAnim(CInfo->ModelHandle, CAInfo->PrevAttachIndex);
			CAInfo->PrevAttachIndex = -1;
		}
	}
	else
	{
		// �A�j���[�V�����̕ύX����������
		CAInfo->ChangeRate = 0.0f;

		// �A�j���[�V�����̕ύX���x��ۑ�
		CAInfo->ChangeSpeed = ChangeSpeed;
	}

	// �A�j���[�V�����̍Đ��������������ǂ����̃t���O��|��
	CAInfo->End = false;

	// �A�j���[�V�����L�����Z���C�x���g�������������ǂ����̃t���O��|��
	CAInfo->Cancel = false;

	// �U���������Z�b�g
	for (i = 0; i < CHARA_ATTACK_MAX_NUM; i++)
	{
		CInfo->AttackInfo[i].Enable = false;
	}

	// �A�j���[�V�����ɕt����������X�V���邽�߂ɐ��ڎ��ԂO�ŏ�Ԑ��ڏ������s��
	if (!Chara_AnimStep(CInfo, 0.0f))
	{
		return false;
	}

	// ����I��
	return true;
}

// �L�����̐�����я������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Blow(
	// ������я������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	Chara_SetTargetDirection(CInfo, CInfo->DamageDirection, false, true);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_In, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Blow;
	CInfo->BlowState = ECharaBlowState_In;

	// ����I��
	return true;
}

// �L�����̓|�ꏈ�����s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Down(
	// �|�ꏈ�����s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	Chara_SetTargetDirection(CInfo, CInfo->DamageDirection, false, true);

	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Down, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Down;

	// ����I��
	return true;
}

// �L�����̃t�F�[�h�A�E�g�������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_FadeOut(
	// �t�F�[�h�A�E�g�������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	SEffectInfo *EInfo;

	// ���S�G�t�F�N�g���J�n
	EInfo = Effect_Create(EEffect_Dead);
	if (EInfo == NULL)
	{
		return false;
	}
	if (!Effect_Dead_Setup(EInfo, CInfo->ModelHandle, GetColorU8(255, 255, 255, 255)))
	{
		return false;
	}

	// ���ŉ��̍Đ�
	Sound_PlaySound3D(
		CInfo->Position, CharaBase_GetCommonSE(ECharaCommonSE_Nifram), DX_PLAYTYPE_BACK);

	// �t�F�[�h�A�E�g��ԂɈڍs
	CInfo->State = ECharaState_FadeOut;

	// ����I��
	return true;
}

// �L�����Ƀ_���[�W��^����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_Damage(
	// �_���[�W����炤�L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �󂯂�_���[�W
	int DamagePoint,

	// �U���������������W
	VECTOR HitPosition,

	// �U���̕���
	VECTOR AttackDirection,

	// �L�����Ƀ_���[�W��^����ꂽ���ǂ�����������ϐ��̃A�h���X
	bool *Result
)
{
	bool         DefaultProcess;
	bool         Defence;
	SEffectInfo *EInfo;

	// �_���[�W���󂯂��ۂɎ��s����֐������݂��Ȃ��ꍇ�̓_���[�W��^�����Ȃ�
	if (g_CharaFunctionTable[CInfo->Chara].Damage == NULL)
	{
		// �_���[�W��^�����Ȃ��������Ƃ����� false ��������
		*Result = false;

		// ����I��
		return true;
	}

	// �_���[�W���󂯂��ۂɎ��s����֐������s����
	if (!g_CharaFunctionTable[CInfo->Chara].Damage(
		CInfo, DamageType, DamagePoint,
		HitPosition, AttackDirection, &Defence, &DefaultProcess))
	{
		return false;
	}

	// �h�䂳�ꂸ�A���f�t�H���g�̏������s���t���O��
	// �����Ă���ꍇ�̂݃f�t�H���g�̏������s��
	if (!Defence && DefaultProcess)
	{
		// �a��U���������ꍇ�̓_���[�W�G�t�F�N�g�𔭐�������
		if (DamageType == ECharaAttack_DamageType_Cut)
		{
			EInfo = Effect_Create(EEffect_Damage);
			if (EInfo == NULL)
			{
				return false;
			}
			Effect_Damage_Setup(EInfo, HitPosition, GetColorU8(128, 16, 16, 255));
		}

		// �_���[�W���󂯂�������ۑ�
		CInfo->DamageDirection = AttackDirection;

		// �̗͂����炷
		CInfo->Health -= DamagePoint;

		// �̗͂��Ȃ��Ȃ������ǂ����ŏ����𕪊�
		if (CInfo->Health <= 0)
		{
			// �̗͂��Ȃ��Ȃ����Ƃ��̏���

			// �̗͂��O�ɂ���
			CInfo->Health = 0;

			// ������уA�j���[�V����������ꍇ�͐�����я�ԂɁA
			// �����ꍇ�̓_�E����Ԃɂ���
			if (CInfo->BaseInfo->AnimInfo[ECharaAnim_Blow_In].Handle > 0)
			{
				if (!Chara_Blow(CInfo))
				{
					return false;
				}
			}
			else
			{
				if (!Chara_Down(CInfo))
				{
					return false;
				}
			}

			// �̗̓Q�[�W���\���ɂ���
			CInfo->HealthGaugeHideDelay = 0.0f;
		}
		else
		{
			// �̗͂��c���Ă���ꍇ

			// �_���[�W�A�j���[�V����������ꍇ�̓_���[�W��Ԃɂ���
			if (CInfo->BaseInfo->AnimInfo[ECharaAnim_Damage].Handle > 0)
			{
				Chara_SetTargetDirection(CInfo, AttackDirection, false, true);

				if (!Chara_ChangeAnim(
					CInfo, ECharaAnim_Damage, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
				{
					return false;
				}
				CInfo->State = ECharaState_Damage;
			}

			// �_���[�W���󂯂���̗̓Q�[�W��\������
			CInfo->HealthGaugeHideDelay = CHARA_HPGAUGE_HIDE_DELAY;
		}
	}

	// �_���[�W���󂯂����ǂ�����������
	if (Result != NULL)
	{
		*Result = !Defence;
	}

	// ����I��
	return true;
}

// �L�����̗����������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Fall(
	// �����������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	// ���S�������J�n���Ă����牽�������ɏI��
	if (CInfo->State == ECharaState_Blow ||
		CInfo->State == ECharaState_Down ||
		CInfo->State == ECharaState_FadeOut)
	{
		return true;
	}

	// �W�����v��ԂɈڍs����
	if (!Chara_ChangeAnim(CInfo, ECharaAnim_Jump_Loop, CHARA_DEFAULT_CHANGE_ANIM_SPEED))
	{
		return false;
	}
	CInfo->State = ECharaState_Jump;

	// �O��̃t���[���̈ړ����x���Z�b�g����
	CInfo->Spd = VScale(CInfo->PrevFixSpd, 0.5f);

	// ������Ƃ����W�����v����
	CInfo->Spd.y = CHARA_FALL_UP_POWER;

	// ����I��
	return true;
}

// �L�����̒��n�������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_Landed(
	// ���n�������s���L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
)
{
	bool DefaultProcess;

	// ���n���Ɏ��s����֐������݂���ꍇ�͎��s����
	if (g_CharaFunctionTable[CInfo->Chara].Landed != NULL)
	{
		if (!g_CharaFunctionTable[CInfo->Chara].Landed(CInfo, &DefaultProcess))
		{
			return false;
		}
	}
	else
	{
		DefaultProcess = true;
	}

	// �f�t�H���g�̏������s�����ǂ����ŏ����𕪊�
	if (DefaultProcess)
	{
		// �L�����̏�Ԃ�������я�Ԃ��ǂ����ŏ����𕪊�
		if (CInfo->State == ECharaState_Blow)
		{
			// ������ђ��������ꍇ�͐�����яI����ԂɈڍs����
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Blow_Out,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}
			CInfo->BlowState = ECharaBlowState_Out;
		}
		else
		{
			// ������я�Ԃł͂Ȃ�������j���[�g�����A�j���[�V�������Đ�
			if (!Chara_ChangeAnim(CInfo, ECharaAnim_Neutral,
				CHARA_DEFAULT_CHANGE_ANIM_SPEED))
			{
				return false;
			}

			// �󒆂ɋ���ۂɕK�������W�����v���Ƃ͌���Ȃ��̂ŁA
			// ��Ԃ��W�����v���̏ꍇ�݈̂ړ���ԂɈڍs����
			if (CInfo->State == ECharaState_Jump)
			{
				CInfo->State = ECharaState_Move;
			}
		}
	}

	// ����I��
	return true;
}

// �u�U���ɓ��������L������X�e�[�W�̏��v�����Z�b�g����
static void Chara_AttackHitInfo_Reset(
	// ���Z�b�g����U���ɓ��������L�����̏��\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo
)
{
	AHInfo->HitCharaNum = 0;
	AHInfo->StageHit = false;
}

// �u�U���ɓ��������L������X�e�[�W�̏��v�ɐV���ɍU���ɓ��������L������ǉ�����
//     �߂�l : �ǉ����悤�Ƃ����L���������ɒǉ�����Ă������ǂ���
//              ( false : ���ɒǉ�����Ă���   true : �V���ɒǉ����� )
static bool Chara_AttackHitInfo_AddChara(
	// ����ǉ�����u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// ���ɒǉ�����L�����̏��\���̂̃A�h���X
	SCharaInfo *NewHitCInfo
)
{
	int i;

	// �ǉ����悤�Ƃ��Ă���L���������ɓo�^����Ă��邩�`�F�b�N
	for (i = 0; i < AHInfo->HitCharaNum; i++)
	{
		if (AHInfo->HitCharaInfo[i] == NewHitCInfo)
		{
			// �o�^����Ă����牽�������ɏI��
			return false;
		}
	}

	// �o�^�ł���L�����̐������ɍő吔�������ꍇ�͉��������ɏI��
	if (AHInfo->HitCharaNum >= CHARA_ATTACK_HIT_CHARA_MAX)
	{
		return false;
	}

	// �z��ɒǉ�
	AHInfo->HitCharaInfo[AHInfo->HitCharaNum] = NewHitCInfo;
	AHInfo->HitCharaNum++;

	// �����I��
	return true;
}

// �u�U���ɓ��������L������X�e�[�W�̏��v�̃X�e�[�W�Ɠ��������ꍇ�̏������s��
static void Chara_AttackHitInfo_ProcessStageHit(
	// �������s���u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// �U�������������ӏ��̃}�e���A���^�C�v
	EMaterialType MaterialType,

	// �U���������������W
	VECTOR HitPosition,

	// �U���Ɏg�p���ꂽ����
	ECharaWeapon Weapon
)
{
	int                         SoundIndex;
	const SCharaWeaponBaseInfo *WBInfo;

	// �X�e�[�W�ɓ����������ǂ����̃t���O�𗧂Ă�
	AHInfo->StageHit = true;

	// ���������ӏ��̃}�e���A���^�C�v��ۑ�
	AHInfo->StageHitMaterialType = MaterialType;

	// �U���Ɏg�p���ꂽ����̏����擾
	WBInfo = CharaBase_GetWeaponInfo(Weapon);

	// ���킪�����ɏՓ˂����ꍇ�ɕK���炷��������ꍇ�͍Đ�
	if (WBInfo->MaterialCommonSound >= 0)
	{
		Sound_PlaySound3D(HitPosition, WBInfo->MaterialCommonSound, DX_PLAYTYPE_BACK);
	}

	// ���������ӏ��̑f�ނɍ������������邩�ǂ����ŏ����𕪊�
	SoundIndex = -1;
	if (WBInfo->MaterialSound[AHInfo->StageHitMaterialType] >= 0)
	{
		SoundIndex = WBInfo->MaterialSound[AHInfo->StageHitMaterialType];
	}
	else
	{
		// �����ꍇ�͑S���틤�ʂŖ炷�����擾
		SoundIndex = CharaBase_GetWeaponCommonMaterialSound(AHInfo->StageHitMaterialType);
	}

	// �L���ȉ������݂����特��炷
	if (SoundIndex >= 0)
	{
		Sound_PlaySound3D(HitPosition, SoundIndex, DX_PLAYTYPE_BACK);
	}
}

// �u�U���ɓ��������L������X�e�[�W�̏��v�̃L�����Ɠ��������ꍇ�̏������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackHitInfo_ProcessCharaHit(
	// �������s���u�U���ɓ��������L������X�e�[�W�̏��v�\���̂̃A�h���X
	SCharaAttackHitInfo *AHInfo,

	// �U�������������L�����̏��\���̂̃A�h���X
	SCharaInfo *HitCInfo,

	// �U���������������W
	VECTOR HitPosition,

	// �U���̕���
	VECTOR AttackDirection,

	// �U���̃_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �U���Ɏg�p���ꂽ����
	ECharaWeapon Weapon,

	// �U���ɂ���Ď󂯂�_���[�W
	int DamagePoint
)
{
	bool DamageResult;

	// �L�����Ƀ_���[�W��^����
	if (!Chara_Damage(HitCInfo, DamageType, DamagePoint, HitPosition,
		AttackDirection, &DamageResult))
	{
		return false;
	}

	// �_���[�W��^���邱�Ƃɐ��������畐��ōU�����ꂽ�ۂ̉����Đ�����
	if (DamageResult)
	{
		if (Weapon != ECharaWeapon_Num &&
			HitCInfo->BaseInfo->WeaponAttackDamageSound[Weapon] != -1)
		{
			Sound_PlaySound3D(HitPosition,
				HitCInfo->BaseInfo->WeaponAttackDamageSound[Weapon], DX_PLAYTYPE_BACK);
		}
	}

	// ����I��
	return true;
}

// �L�����̍U���������Z�b�g����
static void Chara_AttackInfo_Reset(
	// ���Z�b�g����L�����̍U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo
)
{
	// ��񂪗L�����ǂ����̃t���O��|��
	AInfo->Enable = false;

	// ����̋N�_�ƂȂ�SCharaBaseInfo�\���̂̃����o�[�ϐ�AttackPosInfo�̗v�f�ԍ���������
	AInfo->AttackPosIndex = 0;

	// �u�U�������������L������X�e�[�W�̏��v�����Z�b�g
	Chara_AttackHitInfo_Reset(&AInfo->HitInfo);

	// �U������p�̍��W�̗L������������
	AInfo->ValidPositionNum = 0;

	// �U���̋O�ՃG�t�F�N�g���o���Ă����ꍇ�͏I���������s��
	if (AInfo->LocusEffect != NULL)
	{
		Effect_EndRequest(AInfo->LocusEffect);
		AInfo->LocusEffect = NULL;
	}
}

// �L�����̍U�����ɍU������p�̍��W��ǉ�����
static void Chara_AttackInfo_AddPosition(
	// ���W��ǉ�����U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo,

	// �V���ɒǉ�����N�_�ƂȂ�t���[���ɋ߂����_�̍��W
	VECTOR NewNearPos,

	// �V���ɒǉ�����N�_�ƂȂ�t���[�����牓�����_�̍��W
	VECTOR NewFarPos
)
{
	int i;

	// ���ɒǉ��ł���ő吔�ɒB���Ă��܂��Ă���ꍇ�͍ő吔������Ȃ����ɂ���
	if (AInfo->ValidPositionNum >= CHARA_ATTACK_POS_NUM)
	{
		AInfo->ValidPositionNum = CHARA_ATTACK_POS_NUM - 1;
	}

	// ���ɓo�^����Ă�����W������ړ�����
	for (i = AInfo->ValidPositionNum - 1; i >= 0; i--)
	{
		AInfo->NearPosition[i + 1] = AInfo->NearPosition[i];
		AInfo->FarPosition[i + 1] = AInfo->FarPosition[i];
	}

	// �z��̐擪�ɍ��W��ǉ�
	AInfo->NearPosition[0] = NewNearPos;
	AInfo->FarPosition[0] = NewFarPos;

	// �L���ȍ��W�̐�������₷
	AInfo->ValidPositionNum++;
}

// �L�����̍U�����ɂ��U��������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
static bool Chara_AttackInfo_Check(
	// ����Ɏg�p����L�����̍U�����\���̂̃A�h���X
	SCharaAttackInfo *AInfo,

	// �����蔻��̌`��
	ECharaAttackFormType FormType,

	// �U�����s���Ă���L�����̏��\���̂̃A�h���X
	SCharaInfo *AttackCInfo,

	// �U���Ώۂ̃L�����^�C�v
	ECharaType TargetType,

	// �U�������������ꍇ�̃_���[�W�^�C�v
	ECharaAttack_DamageType DamageType,

	// �U���Ɏg�p����镐��
	ECharaWeapon Weapon,

	// �U�������������ꍇ�̃_���[�W
	int DamagePoint,

	// �����蔻��̌`�� ECharaAttackFormType_Sphere �������ꍇ�̋��̑傫��
	float SphereSize
)
{
	int                      i;
	VECTOR                   CapsulePos1;
	VECTOR                   CapsulePos2;
	VECTOR                   HitPosition;
	VECTOR                   AttackDirection;
	int                      HitResult;
	SCharaInfo              *CInfo;
	MV1_COLL_RESULT_POLY_DIM HitDim;
	EMaterialType            HitMaterialType;
	int                      StageObjectNum;
	int                      CollisionModelHandle;

	// �U������p�̍��W�̗L�������Q�����̏ꍇ�͉��������I��
	if (AInfo->ValidPositionNum < 2)
	{
		return true;
	}

	// �U�����܂��X�e�[�W�ɓ������Ă��Ȃ��ꍇ�̓X�e�[�W�Ƃ̓����蔻����s��
	if (!AInfo->HitInfo.StageHit)
	{
		// �X�e�[�W�̃R���W�����p�R�c���f���n���h�����擾
		CollisionModelHandle = StageData_GetCollisionModelHandle();

		// �X�e�[�W�I�u�W�F�N�g�̐����擾����
		StageObjectNum = StageData_GetObjectNum();

		// �����蔻��̌`��ɂ���ď����𕪊�
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// �|���S��
											// �U������p���W���g�p�����|���S���ƃX�e�[�W�Ƃ̓����蔻����s��
			HitDim = MV1CollCheck_Triangle(
				CollisionModelHandle,
				-1,
				AInfo->FarPosition[0],
				AInfo->FarPosition[1],
				AInfo->NearPosition[0]
			);
			if (HitDim.HitNum == 0)
			{
				MV1CollResultPolyDimTerminate(HitDim);

				HitDim = MV1CollCheck_Triangle(
					CollisionModelHandle,
					-1,
					AInfo->NearPosition[0],
					AInfo->FarPosition[1],
					AInfo->NearPosition[1]
				);
			}

			// �X�e�[�W�Ɠ������Ă����瓖�������ӏ��̃}�e���A���^�C�v���擾����
			if (HitDim.HitNum != 0)
			{
				HitMaterialType =
					StageData_GetCollisionModelMaterialType(
						HitDim.Dim[0].MaterialIndex
					);
			}
			else
			{
				// �X�e�[�W�Ɠ������Ă��Ȃ�������I�u�W�F�N�g�Ƃ̓����蔻����s��
				MV1CollResultPolyDimTerminate(HitDim);

				for (i = 0; i < StageObjectNum; i++)
				{
					CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

					if (CollisionModelHandle == -1)
					{
						continue;
					}

					HitDim = MV1CollCheck_Triangle(
						CollisionModelHandle,
						-1,
						AInfo->FarPosition[0],
						AInfo->FarPosition[1],
						AInfo->NearPosition[0]
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);

					HitDim = MV1CollCheck_Triangle(
						CollisionModelHandle,
						-1,
						AInfo->NearPosition[0],
						AInfo->FarPosition[1],
						AInfo->NearPosition[1]
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);
				}

				// �I�u�W�F�N�g�Ɠ������Ă����瓖�������ӏ��̃}�e���A���^�C�v���擾����
				if (HitDim.HitNum != 0)
				{
					HitMaterialType =
						StageData_GetObjectCollisionModelMaterialType(
							i, HitDim.Dim[0].MaterialIndex
						);
				}
			}

			// �������Ă����瓖���������W���Z�o
			if (HitDim.HitNum != 0)
			{
				HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
				HitPosition = VAdd(HitPosition, AInfo->FarPosition[0]);
				HitPosition = VAdd(HitPosition, AInfo->FarPosition[1]);
				HitPosition = VScale(HitPosition, 1.0f / 4.0f);
			}
			break;

		case ECharaAttackFormType_Sphere:	// ��
											// �U������p���W���g�p�����J�v�Z���`��ƃX�e�[�W�Ƃ̓����蔻����s��
			HitDim = MV1CollCheck_Capsule(
				CollisionModelHandle,
				-1,
				AInfo->NearPosition[0],
				AInfo->NearPosition[1],
				SphereSize
			);

			// �X�e�[�W�Ɠ������Ă����瓖�������ӏ��̃}�e���A���^�C�v���擾����
			if (HitDim.HitNum != 0)
			{
				HitMaterialType =
					StageData_GetCollisionModelMaterialType(
						HitDim.Dim[0].MaterialIndex
					);
			}
			else
			{
				// �X�e�[�W�Ɠ������Ă��Ȃ�������I�u�W�F�N�g�Ƃ̓����蔻����s��
				MV1CollResultPolyDimTerminate(HitDim);

				for (i = 0; i < StageObjectNum; i++)
				{
					CollisionModelHandle = StageData_GetObjectCollisionModelHandle(i);

					if (CollisionModelHandle == -1)
					{
						continue;
					}

					HitDim = MV1CollCheck_Capsule(
						CollisionModelHandle,
						-1,
						AInfo->NearPosition[0],
						AInfo->NearPosition[1],
						SphereSize
					);

					if (HitDim.HitNum != 0)
					{
						break;
					}

					MV1CollResultPolyDimTerminate(HitDim);
				}

				// �I�u�W�F�N�g�Ɠ������Ă����瓖�������ӏ��̃}�e���A���^�C�v���擾����
				if (HitDim.HitNum != 0)
				{
					HitMaterialType =
						StageData_GetObjectCollisionModelMaterialType(
							i, HitDim.Dim[0].MaterialIndex
						);
				}
			}

			// �������Ă����瓖���������W���Z�o
			if (HitDim.HitNum != 0)
			{
				HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
				HitPosition = VScale(HitPosition, 1.0f / 2.0f);
			}
			break;
		}

		// �X�e�[�W�Ɠ������Ă�����A�X�e�[�W�Ɠ��������ꍇ�̏��������s����
		if (HitDim.HitNum != 0)
		{
			Chara_AttackHitInfo_ProcessStageHit(
				&AInfo->HitInfo, HitMaterialType, HitPosition, Weapon);
		}

		// �����蔻�茋�ʂ̌�n��
		MV1CollResultPolyDimTerminate(HitDim);
	}

	// ���ׂẴL�����Ƃ̓����蔻��
	CInfo = g_CharaInfo;
	for (i = 0; i < CHARA_MAXNUM; i++, CInfo++)
	{
		// �U�����Ă���L�������A�\���̂��g�p����Ă��Ȃ����A
		// ���Ɏ���ł���ꍇ�͎��̃��[�v��
		if (AttackCInfo == CInfo || !CInfo->UseFlag || CInfo->Health <= 0)
		{
			continue;
		}

		// �^�[�Q�b�g�̃L�����^�C�v�ł͂Ȃ��ꍇ�͎��̃��[�v��
		if (CInfo->BaseInfo->Type != TargetType)
		{
			continue;
		}

		// �L�����̓����蔻��Ɏg�p������W�̎Z�o
		CharaBase_GetHitCapsulePos(&CInfo->BaseInfo->DamageHitInfo,
			&CInfo->Position, &CapsulePos1, &CapsulePos2);

		// �����蔻��̌`��ɂ���ď����𕪊�
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// �|���S��
											// �U������p���W���g�p�����|���S���ƃL�����Ƃ̓����蔻����s��
			HitResult = HitCheck_Capsule_Triangle(
				CapsulePos1,
				CapsulePos2,
				CInfo->BaseInfo->DamageHitInfo.Width,
				AInfo->FarPosition[0],
				AInfo->FarPosition[1],
				AInfo->NearPosition[0]
			);

			if (HitResult == FALSE)
			{
				HitResult = HitCheck_Capsule_Triangle(
					CapsulePos1,
					CapsulePos2,
					CInfo->BaseInfo->DamageHitInfo.Width,
					AInfo->NearPosition[0],
					AInfo->FarPosition[1],
					AInfo->NearPosition[1]
				);
			}
			break;

		case ECharaAttackFormType_Sphere:	// ��
											// �U������p���W���g�p�������ƃL�����Ƃ̓����蔻����s��
			HitResult = HitCheck_Capsule_Capsule(
				CapsulePos1,
				CapsulePos2,
				CInfo->BaseInfo->DamageHitInfo.Width,
				AInfo->NearPosition[0],
				AInfo->NearPosition[1],
				SphereSize
			);
			break;
		}

		// �������Ă��Ȃ������玟�̃��[�v��
		if (HitResult == FALSE)
		{
			continue;
		}

		// ���ɓ������Ă����玟�̃��[�v��
		if (!Chara_AttackHitInfo_AddChara(&AInfo->HitInfo, CInfo))
		{
			continue;
		}

		// �U���̕������Z�o
		AttackDirection = VSub(CInfo->Position, AttackCInfo->Position);
		AttackDirection.y = 0.0f;
		AttackDirection = VNorm(AttackDirection);

		// �U���������������W���Z�o
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:		// �|���S��
			HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
			HitPosition = VAdd(HitPosition, AInfo->FarPosition[0]);
			HitPosition = VAdd(HitPosition, AInfo->FarPosition[1]);
			HitPosition = VScale(HitPosition, 1.0f / 4.0f);
			break;

		case ECharaAttackFormType_Sphere:	// ��
			HitPosition = VAdd(AInfo->NearPosition[0], AInfo->NearPosition[1]);
			HitPosition = VScale(HitPosition, 1.0f / 2.0f);
			break;
		}

		// �L�����ɍU�������������ꍇ�̏������s��
		if (!Chara_AttackHitInfo_ProcessCharaHit(&AInfo->HitInfo, CInfo, HitPosition,
			AttackDirection, DamageType, Weapon, DamagePoint))
		{
			return false;
		}
	}

	// �f�o�b�O����`�悷�邩�ǂ������`�F�b�N
	if (System_GetDispDebugInfo())
	{
		// �f�o�b�O���`��

		// �y�o�b�t�@���g�p�A�y�o�b�t�@�ւ̏������݂���A���C�e�B���O�����ɐݒ�
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
		SetUseLighting(FALSE);

		// �����蔻��̌`���`��
		switch (FormType)
		{
		case ECharaAttackFormType_Poly:
			DrawTriangle3D(AInfo->FarPosition[0], AInfo->FarPosition[1],
				AInfo->NearPosition[0], GetColor(0, 255, 0), TRUE);
			DrawTriangle3D(AInfo->NearPosition[0], AInfo->FarPosition[1],
				AInfo->NearPosition[1], GetColor(0, 255, 0), TRUE);
			break;

		case ECharaAttackFormType_Sphere:
			DrawCapsule3D(AInfo->NearPosition[0], AInfo->NearPosition[1],
				SphereSize, 12, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);
			break;
		}

		// �y�o�b�t�@���g�p���Ȃ��A�y�o�b�t�@�ւ̏������݂����Ȃ��A���C�e�B���O����ɐݒ�
		SetUseLighting(TRUE);
		SetWriteZBufferFlag(FALSE);
		SetUseZBufferFlag(FALSE);
	}

	// ����I��
	return true;
}
/*
//�퓬�f�[�^�̂����ȂǁB
#include"Chara.h"
#include"DxLib.h"
#include"Chara_Player.h"
#include"Key.h"
#include"Camera.h"
#include"System.h"


_CHARA_MGR CHARA_MGR[100];

int Model_Kind[model_Max];
int Model_Anim[model_Max][anim_Max];
int Attach_Model;
float Total_Anim_time;
float Play_Anim_time;
bool Anim_Flg[10];
int Root_flm;




float RATETION_TBL[] = {

	0.0f,		//direction_Down
	90.0f,		//direction_Left
	180.0f,		//direction_Up
	270.0f,		//direction_Right

};
*/
/*

	�A�j���[�V������ݒ肷��
	int modelHandle			:���f���n���h��
	int *attachModleHandle	:�A�^�b�`���f���n���h��
	int ChangeAnimHandle	:�؂�ւ���A�j���n���h��
	int animIdex = 0		:�A�j���[�V�����ԍ� �������͂��Ȃ����0���ݒ肳���
	return	�A�j���[�V�����̑�����
*/
/*
static int Change_AttachAnim(int modelHandle,int *attachModleHandle,int ChangeAnimHandle,int animIdex = 0) {

	MV1DetachAnim(modelHandle, *attachModleHandle);
	*attachModleHandle = MV1AttachAnim(modelHandle, animIdex, ChangeAnimHandle);
	return MV1GetAttachAnimTotalTime(modelHandle, *attachModleHandle);
}

//������
void Chara_Initialize(){

	memset(Anim_Flg,0,10);
	CHARA_MGR[model_Player].Move = true;
	//�v���C���[
	Model_Kind[model_Player] = MV1LoadModel("Data/Charactor/Player/PC.mv1");
	//�G�l�~�[
	Model_Kind[model_Goblin] = MV1LoadModel("Data/Charactor/Goblin/Goblin.mv1");
	//�v���C���[�A�j���[�V����
	Model_Anim[model_Player][anim_Neutral] = MV1LoadModel("Data/Charactor/Player/Anim_Neutral.mv1");
	Model_Anim[model_Player][anim_Run] = MV1LoadModel("Data/Charactor/Player/Anim_Run.mv1");
	Model_Anim[model_Player][anim_Attack] = MV1LoadModel("Data/Charactor/Player/Anim_Attack1.mv1");
	//�G�l�~�[�A�j���[�V����
	Model_Anim[model_Goblin][anim_Neutral] = MV1LoadModel("Data/Charactor/Goblin/Anim_Neutral.mv1");
	Model_Anim[model_Goblin][anim_Run] = MV1LoadModel("Data/Charactor/Goblin/Anim_Run.mv1");
	//�v���C���[�����A�j���[�V�����ݒ�
	Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);

}

void Chara_Update(){

	//�A�j���[�V�����i�s
	Play_Anim_time += 0.5f;
	if(Play_Anim_time > Total_Anim_time){
		Play_Anim_time = 0.0f;
	}
	MV1SetAttachAnimTime(Model_Kind[model_Player],Attach_Model,Play_Anim_time);

	//�v���C���[���f���ݒ�(����Aidol �؂�ւ�)
	if(CHARA_MGR[model_Player].Move == true){
		if(Anim_Flg[anim_Run] == true){
			Anim_Flg[anim_Run] = false;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Neutral]);
#if false
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}else{
		if(Anim_Flg[anim_Run] == false){
			Anim_Flg[anim_Run] = true;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Run]);

#if false	//�֐����쐬�����̂ŃR�����g�A�E�g By.Syara
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Run]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}
	
	

	//���f���̃t���[��������
	Root_flm = MV1SearchFrame(Model_Kind[model_Player],"root");
	MV1SetFrameUserLocalMatrix(Model_Kind[model_Player],Root_flm,MGetIdent());

}

void Chara_Draw(){
	
	float angle = RATETION_TBL[CHARA_MGR[model_Player].Direction] / 180.0f * DX_PI_F;
		angle += CHARA_MGR[model_Player].Angle;

	//�L�����̉�]
	MV1SetRotationXYZ(Model_Kind[model_Player],VGet(0.0f,angle/*CHARA_MGR[model_Player].Direction*90.0f+GetRad_Direction()*//*,0.0f));
	//�L�����̈ړ�
	MV1SetPosition(Model_Kind[model_Player],Get_Player_Pos());
	//�L�����̕`��
	MV1DrawModel(Model_Kind[model_Player]);

	MV1DrawModel(Model_Kind[model_Goblin]);

}


void Chara_Fainalize(){
	
	for(int i = 0; i > model_Max-1;i++){
		MV1DeleteModel(Model_Kind[i]);
	}

}

void Set_Chara_Direction(int charanum,int direction){
	CHARA_MGR[charanum].Direction = direction;
}

int Get_Chara_Direction(int charanum){
	return CHARA_MGR[charanum].Direction;
}

/*
	�p�x��ݒ肷��
*/
/*
void Set_Chara_Rotation(int charanum, float angle) {
	CHARA_MGR[charanum].Angle = angle;
}
*/
/*
	�p�x��ԋp����
*/
/*
float Get_Chara_Rotation(int charanum) {
	return CHARA_MGR[charanum].Angle;
}

void Set_Anim_Flg(int anim_Kind,bool flg){
	Anim_Flg[anim_Kind] = flg;
}

void Set_Move_flg(int model_Kind, bool flg){
	CHARA_MGR[model_Kind].Move = flg;
}

bool Get_Move_flg(int model_Kind){
	return	CHARA_MGR[model_Kind].Move;
}


void Debag_Chara(){

}
*/