#include "CharaBase.h"

#include "Chara_PlayerBase.h"
#include "Chara_EnemyBase.h"

// �S�L�����N�^�[�����ʂŎg�p������ʉ��̃t�@�C��������t�H���_
#define CHARAWEAPON_COMMONSOUND_DIR "SE\\Weapon\\Common\\"

// �S�L�����N�^�[�����ʂŎg�p������ʉ��̓o�^�ԍ�
int                  g_CharaCommonSE[ECharaCommonSE_Num];

// �f�ނɍ���������̏Փˉ��������ꍇ�Ɏg�p�����f�ޕʂ̏Փˉ��̓o�^�ԍ�
int                  g_CharaWeaponCommonMaterialSound[EMaterialType_Num];

// �L�����N�^�[�̕���̊�{���
SCharaWeaponBaseInfo g_CharaWeaponBaseInfo[ECharaWeapon_Num];

// �L�����N�^�[�̊�{���
SCharaBaseInfo       g_CharaBaseInfo[EChara_Num];

// �e�L�����N�^�[�ʂ̊�{���p�̃R�[���o�b�N�֐�
static SCharaBaseFunction g_CharaBaseFunctionTable[EChara_Num] =
{
	{ Chara_PlayerBase_Initialize },// EChara_Player
	{ Chara_EnemyBase_Initialize },	// EChara_Goblin
	{ Chara_EnemyBase_Initialize },	// EChara_Bee
	{ Chara_EnemyBase_Initialize },	// EChara_Golem
	{ Chara_EnemyBase_Initialize },	// EChara_RedGoblin
	{ NULL },						// EChara_Needle
};

// �L�����N�^�[�̊e�A�j���[�V�����̃t�@�C�����
static SCharaAnimFileInfo g_CharaAnimBaseInfoTable[ECharaAnim_Num] =
{
	true,	"Anim_Neutral",			// ECharaAnim_Neutral = 0,
	true,	"Anim_Walk",			// ECharaAnim_Walk,
	true,	"Anim_Run",				// ECharaAnim_Run,
	false,	"Anim_Jump_In",			// ECharaAnim_Jump_In,
	false,	"Anim_Jump_Loop",		// ECharaAnim_Jump_Loop,
	false,	"Anim_Jump_Out",		// ECharaAnim_Jump_Out,
	false,	"Anim_Landed",			// ECharaAnim_Landed,
	false,	"Anim_Atk1",			// ECharaAnim_Atk1,
	false,	"Anim_Atk2",			// ECharaAnim_Atk2,
	false,	"Anim_Atk3",			// ECharaAnim_Atk3,
	false,	"Anim_Guard_In",		// ECharaAnim_Guard_In,
	true,	"Anim_Guard_Loop",		// ECharaAnim_Guard_Loop,
	false,	"Anim_Guard_Impact",	// ECharaAnim_Guard_Impact,
	false,	"Anim_Guard_Out",		// ECharaAnim_Guard_Out,
	false,	"Anim_Damage",			// ECharaAnim_Damage,
	false,	"Anim_Blow_In",			// ECharaAnim_Blow_In,
	true,	"Anim_Blow_Loop",		// ECharaAnim_Blow_Loop,
	false,	"Anim_Blow_Out",		// ECharaAnim_Blow_Out,
	false,	"Anim_Down",			// ECharaAnim_Down,
	true,	"Anim_Down_Loop",		// ECharaAnim_Down_Loop,
};

// �f�ރ^�C�v��
static const char *g_MaterialTypeName[EMaterialType_Num] =
{
	"grass",			// EMaterialType_Grass
	"metal",			// EMaterialType_Metal
	"soil",				// EMaterialType_Soil
	"stone",			// EMaterialType_Stone
	"wood",				// EMaterialType_Wood
};

// �L�����N�^�[�^�C�v��
static const char *g_CharaTypeName[ECharaType_Num] =
{
	"Player",			// ECharaType_Player = 0,
	"Enemy",			// ECharaType_Enemy.
	"EnemyShot",		// ECharaType_EnemyShot.
};

// �L�����N�^�[�̍U������`��^�C�v�̖��O
static const char *g_CharaAtkFormTypeName[ECharaAtkFormType_Num] =
{
	"Poly",				// ECharaAtkFormType_Poly = 0,
	"Sphere",			// ECharaAtkFormType_Sphere.
};

// �S�L�����N�^�[�����ʂŎg�p������ʉ��̖��O
static const char *g_CharaCommonSEName[ECharaCommonSE_Num] =
{
	"enemy_Nifram",
};

// �L�����N�^�[�̕���̊�{��񏉊���
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
static bool CharaBase_WeaponInitialize(void)
{
	STextParamInfo        CommonTextParam;
	STextParamInfo        TextParam;
	SCharaWeaponBaseInfo *WBInfo;
	char                  String[256];
	const char           *DirectoryName;
	const char           *ParamString;
	int                   i;
	int                   j;

	// �e����̃t�@�C�����i�[����Ă���t�H���_�����L�q����Ă���
	// �e�L�X�g�t�@�C����ǂݍ���
	if (!LoadTextParam(&CommonTextParam, CHARADATA_DIR "Weapon\\Param.txt"))
	{
		return false;
	}

	// ����̐������J��Ԃ�
	WBInfo = g_CharaWeaponBaseInfo;
	for (i = 0; i < ECharaWeapon_Num; i++, WBInfo++)
	{
		// ����̃t�@�C�����i�[����Ă���t�H���_�����擾����
		DirectoryName = GetTextParamString(
			&CommonTextParam, "CharaWeapon%03d_DirectoryName", i);
		strcpy(WBInfo->DirectoryName, DirectoryName);

		// ����̃p�����[�^���L�q���ꂽ�e�L�X�g�t�@�C����ǂݍ���
		if (!LoadTextParam(&TextParam, CHARADATA_DIR "Weapon\\%s\\Param.txt",
			WBInfo->DirectoryName))
		{
			continue;
		}

		// ����̖��O���擾
		ParamString = GetTextParamString(&TextParam, "Name");
		strcpy(WBInfo->Name, ParamString);

		// �R�c���f���t�@�C�������擾
		ParamString = GetTextParamString(&TextParam, "ModelPath");
		if (ParamString == NULL)
		{
			WBInfo->ModelHandle = -1;
		}
		else
		{
			// �R�c���f���t�@�C����ǂݍ���
			sprintf(String, CHARADATA_DIR "Weapon\\%s\\%s",
				WBInfo->DirectoryName, ParamString);
			WBInfo->ModelHandle = MV1LoadModel(String);
			if (WBInfo->ModelHandle == -1)
			{
				return false;
			}
		}

		// ���킪�����ɏՓ˂����Ƃ��ɖ炷���ʂ̉��̓o�^
		sprintf(String, "SE\\Weapon\\%s\\MAT_impact_com", WBInfo->DirectoryName);
		WBInfo->MaterialCommonSound = Sound_AddSound(String, true);

		// ���킪�e�f�ނɏՓ˂����Ƃ��ɖ炷���̓o�^
		for (j = 0; j < EMaterialType_Num; j++)
		{
			sprintf(String, "SE\\%Weapon\\s\\MAT_impact_%s",
				WBInfo->DirectoryName, g_MaterialTypeName[i]);
			WBInfo->MaterialSound[j] = Sound_AddSound(String, true);
		}
	}

	// �����I��
	return true;
}

// �L�����N�^�[��{���̏�����
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������  false : ���s���� )
bool CharaBase_Initialize(void)
{
	int                   i;
	int                   j;
	int                   k;
	SCharaBaseInfo       *CBInfo;
	SCharaWeaponBaseInfo *WBInfo;
	STextParamInfo        CommonTextParam;
	STextParamInfo        TextParam;
	char                  String[256];
	const char           *DirectoryName;
	const char           *ParamString;

	// �e�L�����N�^�[�̃t�@�C�����i�[����Ă���t�H���_�����L�q����Ă���
	// �e�L�X�g�t�@�C���̓ǂݍ���
	if (!LoadTextParam(&CommonTextParam, CHARADATA_DIR "Param.txt"))
	{
		return false;
	}

	// �S�L�����N�^�[���ʂŎg�p������ʉ��̓ǂݍ���
	for (i = 0; i < ECharaCommonSE_Num; i++)
	{
		sprintf(String, "SE\\Common\\%s", g_CharaCommonSEName[i]);
		g_CharaCommonSE[i] = Sound_AddSound(String, true);
	}

	// �f�ނɍ���������̏Փˉ��������ꍇ�Ɏg�p�����f�ޕʂ̏Փˉ��̓ǂݍ���
	for (i = 0; i < EMaterialType_Num; i++)
	{
		sprintf(String, CHARAWEAPON_COMMONSOUND_DIR "MAT_impact_%s",
			g_MaterialTypeName[i]);
		g_CharaWeaponCommonMaterialSound[i] = Sound_AddSound(String, true);
	}

	// �L�����N�^�[�̕������������
	CharaBase_WeaponInitialize();

	// �L�����N�^�[�̐������J��Ԃ�
	CBInfo = g_CharaBaseInfo;
	for (i = 0; i < EChara_Num; i++, CBInfo++)
	{
		// �L�����N�^�[�̃t�@�C�����i�[����Ă���t�H���_���̎擾
		DirectoryName = GetTextParamString(
			&CommonTextParam, "Chara%03d_DirectoryName", i);
		strcpy(CBInfo->DirectoryName, DirectoryName);

		// �L�����N�^�[�̃p�����[�^���L�q���ꂽ�e�L�X�g�t�@�C���̓ǂݍ���
		if (!LoadTextParam(&TextParam, CHARADATA_DIR "%s\\Param.txt",
			CBInfo->DirectoryName))
		{
			continue;
		}

		// �L�����N�^�[���̎擾
		ParamString = GetTextParamString(&TextParam, "Name");
		strcpy(CBInfo->Name, ParamString);

		// �L�����N�^�[�̃^�C�v������
		ParamString = GetTextParamString(&TextParam, "Type");
		for (j = 0; j < ECharaType_Num; j++)
		{
			if (strcmp(ParamString, g_CharaTypeName[j]) == 0)
			{
				break;
			}
		}
		CBInfo->Type = (ECharaType)j;

		// �L�����N�^�[�̗̑͂��擾
		CBInfo->Hp = GetTextParamInt(&TextParam, "Hp");

		// �L�����N�^�[�̂R�c���f���t�@�C�������擾�{�t�@�C���̓ǂݍ���
		ParamString = GetTextParamString(&TextParam, "MainModelPath");
		sprintf(String, CHARADATA_DIR "%s\\%s", CBInfo->DirectoryName, ParamString);
		CBInfo->ModelHandle = MV1LoadModel(String);
		if (CBInfo->ModelHandle == -1)
		{
			return false;
		}

		// �e����ɂ��_���[�W���󂯂��ۂɖ炷���̓ǂݍ���
		WBInfo = g_CharaWeaponBaseInfo;
		for (j = 0; j < ECharaWeapon_Num; j++, WBInfo++)
		{
			sprintf(String, "SE\\%s\\dmg_by%s", DirectoryName, WBInfo->DirectoryName);
			CBInfo->WeaponAtkDamageSound[j] = Sound_AddSound(String, true);
		}

		// �f�ޕʂ̃A�j���[�V�����������̓ǂݍ���
		for (j = 0; j < EAnimPhysicsSound_Num; j++)
		{
			for (k = 0; k < EMaterialType_Num; k++)
			{
				sprintf(String, "SE\\%s\\MAT_%s_%s", DirectoryName,
					GetAnimPhysicsSoundName((EAnimPhysicsSound)j),
					g_MaterialTypeName[k]);
				CBInfo->AnimPhysicsSound[j][k] = Sound_AddSound(String, true);
			}
		}

		// ���[�g�t���[�����̎擾�ƁA���[�ƃt���[���ԍ����擾
		ParamString = GetTextParamString(&TextParam, "RootFrameName");
		CBInfo->RootFrameIndex = MV1SearchFrame(CBInfo->ModelHandle, ParamString);

		// �A�j���[�V�����̐������J��Ԃ�
		for (j = 0; j < ECharaAnim_Num; j++)
		{
			// �R�c���f���̃A�j���[�V�����t�@�C����ǂݍ���
			sprintf(String, CHARADATA_DIR "%s\\%s.mv1",
				CBInfo->DirectoryName, g_CharaAnimBaseInfoTable[j].FileName);

			// �t�@�C��������ꍇ�̂ݓǂݍ��ݏ������s��
			if (FileRead_size(String) > 0)
			{
				CBInfo->AnimInfo[j].Handle = MV1LoadModel(String);
			}

			// �A�j���[�V�������t�@�C���̓ǂݍ���
			sprintf(String, CHARADATA_DIR "%s\\%s",
				CBInfo->DirectoryName, g_CharaAnimBaseInfoTable[j].FileName);
			LoadAnimInfoFile(&CBInfo->AnimInfo[j].Info, String);

			// �A�j���[�V�������ɍĐ����鉹�̓ǂݍ���
			LoadAnimInfoSound(&CBInfo->AnimInfo[j].Info);
		}

		// �������햼���擾
		ParamString = GetTextParamString(&TextParam, "EquipWeapon");

		// �������킪���邩�ǂ����ŏ����𕪊�
		if (ParamString == NULL)
		{
			// �����ꍇ
			CBInfo->EquipWeapon = ECharaWeapon_Num;
			CBInfo->EquipWeaponAttachFrameIndex = -1;
		}
		else
		{
			// ����ꍇ�͕��햼����񋓌^�̒l�ɕϊ�
			WBInfo = g_CharaWeaponBaseInfo;
			for (j = 0; j < ECharaWeapon_Num; j++, WBInfo++)
			{
				if (strcmp(ParamString, WBInfo->DirectoryName) == 0)
				{
					break;
				}
			}
			CBInfo->EquipWeapon = (ECharaWeapon)j;

			// ����R�c���f�����A�^�b�`����{�̃��f���̃t���[�������擾
			ParamString = GetTextParamString(&TextParam, "EquipWeapon_AttachFrame");
			if (ParamString == NULL)
			{
				CBInfo->EquipWeaponAttachFrameIndex = -1;
			}
			else
			{
				// �t���[��������t���[���ԍ����擾
				CBInfo->EquipWeaponAttachFrameIndex =
					MV1SearchFrame(CBInfo->ModelHandle, ParamString);
			}
		}

		// �T�u�R�c���f���̓ǂݍ���
		for (j = 0; j < CHARA_MAX_SUBMODEL; j++)
		{
			// �T�u�R�c���f���̃t�@�C���p�X���擾
			ParamString = GetTextParamString(&TextParam, "SubModel%03d_Path", j);

			// ���������烋�[�v�𔲂���
			if (ParamString == NULL)
			{
				break;
			}

			// �T�u�R�c���f���̓ǂݍ���
			sprintf(String, CHARADATA_DIR "%s\\%s", CBInfo->DirectoryName, ParamString);
			CBInfo->SubModel[j].ModelHandle = MV1LoadModel(String);
			if (CBInfo->SubModel[j].ModelHandle == -1)
			{
				return false;
			}

			// �T�u�R�c���f�����A�^�b�`����{�̂R�c���f���̃t���[�����ƃt���[���ԍ����擾
			ParamString = GetTextParamString(&TextParam, "SubModel%03d_AttachFrame", j);
			CBInfo->SubModel[j].AttachFrameIndex =
				MV1SearchFrame(CBInfo->ModelHandle, ParamString);

			// �T�u�R�c���f���̐����C���N�������g
			CBInfo->SubModelNum++;
		}

		// �̗̓Q�[�W��`�悷�鑊�΍��W���擾
		CBInfo->HpGaugePosition =
			GetTextParamVector(&TextParam, "HpGaugePosition");

		// �U���ʒu���̎擾
		for (j = 0; j < CHARA_ATTACK_POS_MAX_NUM; j++)
		{
			// �N�_�ƂȂ�R�c���f���̃t���[�������擾
			ParamString = GetTextParamString(&TextParam,
				"AtkPosInfo%d_StartFrameName", j);
			if (ParamString != NULL)
			{
				// �t���[��������t���[���ԍ����擾
				CBInfo->AtkPosInfo[j].StartFrameIndex =
					MV1SearchFrame(CBInfo->ModelHandle, ParamString);

				// �U������̌`���Ɏg�p�����N�_����̑��΍��W�̎擾
				CBInfo->AtkPosInfo[j].EndLocalPosition =
					GetTextParamVector(&TextParam,
						"AtkPosInfo%d_EndLocalPosition", j);

				// �U������̋��̔��a���擾
				CBInfo->AtkPosInfo[j].SphereSize =
					GetTextParamFloat(&TextParam, "AtkPosInfo%d_SphereSize", j);

				// �U���̋O�ՃG�t�F�N�g�̐F���擾
				CBInfo->AtkPosInfo[j].EffectColor =
					GetTextParamColor(&TextParam, "AtkPosInfo%d_EffectColor", j);

				// �U������̌`����擾
				ParamString =
					GetTextParamString(&TextParam, "AtkPosInfo%d_FormType", j);
				for (k = 0; k < ECharaType_Num; k++)
				{
					if (strcmp(ParamString, g_CharaAtkFormTypeName[k]) == 0)
					{
						break;
					}
				}
				CBInfo->AtkPosInfo[j].FormType = (ECharaAtkFormType)k;
			}
			else
			{
				// �����ꍇ�̓f�t�H���g�l�ŏ�����
				CBInfo->AtkPosInfo[j].StartFrameIndex = -1;
				CBInfo->AtkPosInfo[j].EndLocalPosition = VGet(0.0f, 0.0f, 0.0f);
				CBInfo->AtkPosInfo[j].EffectColor = GetColorU8(0, 0, 0, 0);
				CBInfo->AtkPosInfo[j].FormType = ECharaAtkFormType_Poly;
				CBInfo->AtkPosInfo[j].SphereSize = 1.0f;
			}
		}

		// �v���O�����ňړ�����ꍇ�̑��x���擾
		CBInfo->ProgramSpd = GetTextParamFloat(&TextParam, "ProgramSpd");

		// �ۉe�̃T�C�Y���擾
		CBInfo->ShadowSize = GetTextParamFloat(&TextParam, "ShadowSize");

		// �_���[�W����̏����擾
		CBInfo->DamageHitInfo.Width =
			GetTextParamFloat(&TextParam, "DamageHitWidth");

		CBInfo->DamageHitInfo.Height =
			GetTextParamFloat(&TextParam, "DamageHitHeight");

		CBInfo->DamageHitInfo.CenterPosition =
			GetTextParamVector(&TextParam, "DamageHitCenterPosition");

		// �L�����N�^�[���m�̓����蔻��̏����擾
		CBInfo->CharaHitInfo.Width =
			GetTextParamFloat(&TextParam, "CharaHitWidth");

		CBInfo->CharaHitInfo.Height =
			GetTextParamFloat(&TextParam, "CharaHitHeight");

		CBInfo->CharaHitInfo.CenterPosition =
			GetTextParamVector(&TextParam, "CharaHitCenterPosition");

		// �X�e�[�W�Ƃ̓����蔻��̏����擾
		CBInfo->StageHitBottomInfo.Width =
			GetTextParamFloat(&TextParam, "StageHitBottomWidth");

		CBInfo->StageHitBottomInfo.Height =
			GetTextParamFloat(&TextParam, "StageHitBottomHeight");

		CBInfo->StageHitBottomInfo.CenterPosition =
			GetTextParamVector(&TextParam, "StageHitBottomCenterPosition");

		CBInfo->StageHitTopInfo.Width =
			GetTextParamFloat(&TextParam, "StageHitTopWidth");

		CBInfo->StageHitTopInfo.Height =
			GetTextParamFloat(&TextParam, "StageHitTopHeight");

		CBInfo->StageHitTopInfo.CenterPosition =
			GetTextParamVector(&TextParam, "StageHitTopCenterPosition");

		// �L�����N�^�[�ʂ̏������֐�������ꍇ�͌Ă�
		if (g_CharaBaseFunctionTable[i].Initialize != NULL)
		{
			if (!g_CharaBaseFunctionTable[i].Initialize(CBInfo, &TextParam))
			{
				return false;
			}
		}
	}

	// �����I��
	return true;
}

// �L�����N�^�[��{���̌�n��
void CharaBase_Terminate(void)
{
	int                   i;
	int                   j;
	SCharaBaseInfo       *CBInfo;
	SCharaWeaponBaseInfo *WBInfo;

	// ����̐������J��Ԃ�
	WBInfo = g_CharaWeaponBaseInfo;
	for (i = 0; i < ECharaWeapon_Num; i++, WBInfo++)
	{
		// ����̂R�c���f�����폜
		MV1DeleteModel(WBInfo->ModelHandle);
		WBInfo->ModelHandle = -1;
	}

	// �L�����N�^�[�̐������J��Ԃ�
	CBInfo = g_CharaBaseInfo;
	for (i = 0; i < EChara_Num; i++, CBInfo++)
	{
		// �{�̂̂R�c���f�����폜
		MV1DeleteModel(CBInfo->ModelHandle);
		CBInfo->ModelHandle = -1;

		// �S�ẴT�u���f�����폜
		for (j = 0; j < CBInfo->SubModelNum; j++)
		{
			MV1DeleteModel(CBInfo->SubModel[j].ModelHandle);
			CBInfo->SubModel[j].ModelHandle = -1;
		}

		// �L�����N�^�[�^�C�v�ʂ̏��p�Ƀ��������m�ۂ��Ă����ꍇ�͉��
		if (CBInfo->SubData != NULL)
		{
			free(CBInfo->SubData);
			CBInfo->SubData = NULL;
		}
	}
}

// �w��̃L�����N�^�[��{���̃A�h���X���擾����
//     �߂�l : �L�����N�^�[��{���\���̂̃A�h���X
SCharaBaseInfo * CharaBase_GetInfo(
	// �L�����N�^�[
	EChara Chara
)
{
	return &g_CharaBaseInfo[Chara];
}

// �w��̕����{���̃A�h���X���擾����
//     �߂�l : �����{���̃A�h���X
const SCharaWeaponBaseInfo * CharaBase_GetWeaponInfo(
	// ����
	ECharaWeapon Weapon
)
{
	return &g_CharaWeaponBaseInfo[Weapon];
}

// �w��̑f�ނ̋��ʃT�E���h�̓o�^�ԍ����擾����
//     �߂�l : �T�E���h�̓o�^�ԍ�
int CharaBase_GetWeaponCommonMaterialSound(
	// �f�ރ^�C�v
	EMaterialType MatType
)
{
	return g_CharaWeaponCommonMaterialSound[MatType];
}

// �w��̋��ʌ��ʉ��̓o�^�ԍ����擾����
//     �߂�l : �T�E���h�̓o�^�ԍ�
int CharaBase_GetCommonSE(
	// �L�����N�^�[���ʌ��ʉ�
	ECharaCommonSE CommonSE
)
{
	return g_CharaCommonSE[CommonSE];
}

// �w��̃L�����N�^�[�A�j���[�V�����̃t�@�C�������擾����
//     �߂�l : �A�j���[�V�����̃t�@�C�����\���̂̃A�h���X
const SCharaAnimFileInfo * CharaBase_GetAnimFileInfo(
	// �L�����N�^�[�A�j���[�V����
	ECharaAnim Anim
)
{
	return &g_CharaAnimBaseInfoTable[Anim];
}

// �w��̑f�ރ^�C�v�̖��O���擾����
//     �߂�l : ���O������̃A�h���X
const char * GetMaterialTypeName(
	// �f�ރ^�C�v
	EMaterialType MatType
)
{
	return g_MaterialTypeName[MatType];
}

// �����蔻��̏�񂩂�A�����蔻��Ɏg�p����J�v�Z���`����`������Q���W���擾����
void CharaBase_GetHitCapsulePos(
	// �����蔻����̃A�h���X
	const SCharaHitInfo *HitInfo,

	// �����蔻����s���L�����N�^�[�̍��W�̃A�h���X
	const VECTOR *Position,

	// �v�Z��̃J�v�Z���̍��W���i�[����A�h���X
	VECTOR *CapsulePosition1,
	VECTOR *CapsulePosition2
)
{
	*CapsulePosition1 =
		*CapsulePosition2 = VAdd(*Position, HitInfo->CenterPosition);

	CapsulePosition1->y -= HitInfo->Height / 2.0f;
	CapsulePosition2->y += HitInfo->Height / 2.0f;
}