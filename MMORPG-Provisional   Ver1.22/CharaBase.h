#ifndef CHARABASE_H
#define CHARABASE_H

#include "DxLib.h"
#include "TextParam.h"
#include "Animation.h"

// �L�����N�^�[�̃f�[�^�t�@�C�����i�[����Ă���t�H���_
#define CHARADATA_DIR					"Data\\Character\\"

// �L�����N�^�[�A�j���[�V������FPS
#define CHARA_ANIM_FPS					(30)

// �L�����N�^�[�ɕt������T�u�R�c���f���̍ő吔
#define CHARA_MAX_SUBMODEL				(4)

// �L�����N�^�[�̕�����f�[�^�Ŏg�p�ł���ő啶����
#define CHARA_MAX_STRINGLENGTH			(64)

// �L�����N�^�[�̂P�A�j���[�V�����ň�����U��������̐�
#define CHARA_ATTACK_MAX_NUM			(4)

// �L�����N�^�[�̍U������p�ʒu���̍ő吔
#define CHARA_ATTACK_POS_MAX_NUM		(4)

// �L�����N�^�[�^�C�v
typedef enum _ECharaType
{
	ECharaType_Player = 0,			// �v���C���[
	ECharaType_Enemy,				// �G
	ECharaType_EnemyShot,			// �G�̒e

	ECharaType_Num					// �L�����N�^�[�^�C�v�̐�
} ECharaType;

// �L�����N�^�[
typedef enum _EChara
{
	EChara_Player = 0,				// �v���C���[
	EChara_Goblin,					// �S�u����
	EChara_Bee,						// �I
	EChara_Golem,					// �S�[����
	EChara_RedGoblin,				// �ԃS�u����
	EChara_Needle,					// �j

	EChara_Num						// �L�����N�^�[�̐�
} EChara;

// �L�����N�^�[�̃A�j���[�V����
typedef enum _ECharaAnim
{
	ECharaAnim_Neutral = 0,			// �j���[�g����
	ECharaAnim_Walk,				// ����
	ECharaAnim_Run,					// ����
	ECharaAnim_Jump_In,				// �W�����v�J�n
	ECharaAnim_Jump_Loop,			// �W�����v���[�v
	ECharaAnim_Jump_Out,			// �W�����v�I��
	ECharaAnim_Landed,				// ���n
	ECharaAnim_Attack1,				// �U���A�j���[�V�����P
	ECharaAnim_Attack2,				// �U���A�j���[�V�����Q
	ECharaAnim_Attack3,				// �U���A�j���[�V�����R
	ECharaAnim_Guard_In,			// �K�[�h�J�n
	ECharaAnim_Guard_Loop,			// �K�[�h���[�v
	ECharaAnim_Guard_Impact,		// �U�����K�[�h
	ECharaAnim_Guard_Out,			// �K�[�h�I��
	ECharaAnim_Damage,				// �_���[�W
	ECharaAnim_Blow_In,				// ������ъJ�n
	ECharaAnim_Blow_Loop,			// ������у��[�v
	ECharaAnim_Blow_Out,			// ������яI��
	ECharaAnim_Down,				// �_�E��
	ECharaAnim_Down_Loop,			// �_�E�����[�v

	ECharaAnim_Num					// �A�j���[�V�����̐�
} ECharaAnim;

// �ړ��^�C�v
typedef enum _ECharaMoveType
{
	ECharaMoveType_RootAnim,		// Root�t���[���A�j���[�V�����ɂ��ړ�
	ECharaMoveType_Program,			// �v���O�����ɂ��ړ�

	ECharaMoveType_Num				// �ړ��^�C�v�̐�
} ECharaMoveType;

// �L�����N�^�[�̕���
typedef enum _ECharaWeapon
{
	ECharaWeapon_Sabel,				// �T�[�x��
	ECharaWeapon_Sword,				// ��
	ECharaWeapon_Axe,				// ��
	ECharaWeapon_Needle,			// �j
	ECharaWeapon_RockKnuckle,		// �≣��

	ECharaWeapon_Num				// �L�����N�^�[�̕���̐�
} ECharaWeapon;

// �L�����N�^�[�̍U������̌`��
typedef enum _ECharaAttackFormType
{
	ECharaAttackFormType_Poly,		// �|���S��
	ECharaAttackFormType_Sphere,	// ��

	ECharaAttackFormType_Num		// �U������̌`��̐�
} ECharaAttackFormType;

// �L�����N�^�[���ʌ��ʉ�
typedef enum _ECharaCommonSE
{
	ECharaCommonSE_Nifram,			// �L�����N�^�[���ŉ�

	ECharaCommonSE_Num				// �L�����N�^�[���ʌ��ʉ��̐�
} ECharaCommonSE;

// �L�����N�^�[�A�j���[�V�����t�@�C�����
typedef struct _SCharaAnimFileInfo
{
	// ���[�v�A�j���[�V�������ǂ���
	bool                  LoopFlag;

	// �t�@�C����
	const char *          FileName;
} SCharaAnimFileInfo;

// �L�����N�^�[�T�u�R�c���f�����
typedef struct _SCharaSubModelInfo
{
	// �R�c���f���n���h��
	int                   ModelHandle;

	// �R�c���f�����A�^�b�`����t���[���ԍ�
	int                   AttachFrameIndex;
} SCharaSubModelInfo;

// �L�����N�^�[�A�j���[�V������{���
typedef struct _SCharaAnimBaseInfo
{
	// �A�j���[�V�������
	SAnimInfo             Info;

	// �A�j���[�V�����̂R�c���f���n���h��
	int                   Handle;
} SCharaAnimBaseInfo;

// �L�����N�^�[�̍U���ʒu���
typedef struct _SCharaAttackPositionInfo
{
	// �N�_�ƂȂ�R�c���f���t���[���̔ԍ�
	int                   StartFrameIndex;

	// �U������̌`��
	ECharaAttackFormType  FormType;

	// �U������̌`���Ɏg�p�����N�_����̑��΍��W
	// ( FormType �� ECharaAttackFormType_Poly �̏ꍇ�Ɏg�p )
	VECTOR                EndLocalPosition;

	// ���̔��a
	// ( FormType �� ECharaAttackFormType_Sphere �̏ꍇ�Ɏg�p )
	float                 SphereSize;

	// �U���̋O�ՃG�t�F�N�g�̐F
	COLOR_U8              EffectColor;
} SCharaAttackPositionInfo;

// �L�����N�^�[�̕���̊�{���
typedef struct _SCharaWeaponBaseInfo
{
	// �f�[�^�t�@�C�������݂���t�H���_��
	char                  DirectoryName[CHARA_MAX_STRINGLENGTH];

	// ����̖��O
	char                  Name[CHARA_MAX_STRINGLENGTH];

	// ����̂R�c���f���n���h��
	int                   ModelHandle;

	// ���킪�w�i�̉����ɓ������Ă��炷��
	int                   MaterialCommonSound;

	// ���킪�w�i�ɓ��������ۂɖ炷�f�ޕʂ̉�
	int                   MaterialSound[EMaterialType_Num];
} SCharaWeaponBaseInfo;

// �L�����N�^�[�̓����蔻��̏��
typedef struct _SCharaHitInfo
{
	// �����蔻��̕�
	float                 Width;

	// �����蔻��̍���
	float                 Height;

	// �����蔻��̒��S���W
	VECTOR                CenterPosition;
} SCharaHitInfo;

// �L�����N�^�[�̊�{���
typedef struct _SCharaBaseInfo
{
	// �L�����N�^�[�f�[�^�t�@�C�������݂���t�H���_��
	char                  DirectoryName[CHARA_MAX_STRINGLENGTH];

	// �L�����N�^�[�̖��O
	char                  Name[CHARA_MAX_STRINGLENGTH];

	// �L�����N�^�[�̃^�C�v
	ECharaType            Type;

	// �̗�
	int                   Health;
	/*

	Health=��{�̗͒l
	Attack=��{�U���l
	Def=��{�h��l
	Crt=��{�N���e�B�J����
	Speed=��{�ړ��X�s�[�h
	Gold=������
	Fame=����
	
	ItemHav[10]=�A�C�e���͉��������Ă��邩�̊m�F
	Item_Db[10]=�A�C�e�����̑ϋv�l
	
	Add_Health=�����A�C�e���̍ő�HP�A�b�v���v�l
	Add_Atk=��������A�C�e���̍U����
	Add_Def=�����A�C�e���̖h��͍��v�l
	Add_Crt=�����A�C�e���̃N���e�B�J�������v�l
	Add_Spd=�����A�C�e���̈ړ��X�s�[�h�l���v
	Abp_Health=�U����^�����ۂ�Health�z����(�_���[�W*�z����(%))
	Auto_Heal=���W�F�l(1�b�Ԃɉ񕜂����(����))
	Gold_Per=�G��|�����ۂɂ��炦�邨���̗ʂ�������B

	*/

	// ����ɂ��U�����󂯂��Ƃ��ɖ炷���̓o�^�ԍ�
	int                   WeaponAttackDamageSound[ECharaWeapon_Num];

	// �f�ޕʂ̃A�j���[�V�����������̓o�^�ԍ�
	int                   AnimPhysicsSound[EAnimPhysicsSound_Num][EMaterialType_Num];

	// �L�����N�^�[�̂R�c���f���n���h��
	int                   ModelHandle;

	// ��������
	ECharaWeapon          EquipWeapon;

	// ������A�^�b�`���Ă���R�c���f���t���[���ԍ�
	int                   EquipWeaponAttachFrameIndex;

	// �T�u�R�c���f���̐�
	int                   SubModelNum;

	// �T�u�R�c���f���̏��
	SCharaSubModelInfo    SubModel[CHARA_MAX_SUBMODEL];

	// ���[�g�R�c���f���t���[���ԍ�
	int                   RootFrameIndex;

	// �A�j���[�V�������
	SCharaAnimBaseInfo    AnimInfo[ECharaAnim_Num];

	// �_���[�W������
	SCharaHitInfo         DamageHitInfo;

	// �̗̓Q�[�W��\��������W
	VECTOR                HealthGaugePosition;

	// �U������ʒu���
	SCharaAttackPositionInfo AttackPosInfo[CHARA_ATTACK_POS_MAX_NUM];

	// �ړ��^�C�v�� ECharaMoveType_Program �̏ꍇ�̈ړ����x
	float                 ProgramSpd;

	// �ۉe�̑傫��
	float                 ShadowSize;

	// �L�����N�^�[���m�̓����蔻����
	SCharaHitInfo         CharaHitInfo;

	// �����X�e�[�W�Ƃ̓����蔻��̉����̕�
	SCharaHitInfo         StageHitBottomInfo;

	// �����X�e�[�W�Ƃ̓����蔻��̏㑤�̕�
	SCharaHitInfo         StageHitTopInfo;

	// �L�����N�^�[�^�C�v�ʂ̏��ւ̃|�C���^
	void *                SubData;
} SCharaBaseInfo;

// �L�����N�^�[�ʂ̊�{���p�̃R�[���o�b�N�֐�
typedef struct _SCharaBaseFunction
{
	// �L�����N�^�[�̊�{��񏉊����p�֐��̃|�C���^
	bool(*Initialize)(SCharaBaseInfo *CBInfo, STextParamInfo *TextParam);
} SCharaBaseFunction;

// �L�����N�^�[��{���̏�����
//     �߂�l : �������ɐ����������ǂ���
//              ( true : ��������  false : ���s���� )
extern bool CharaBase_Initialize(void);

// �L�����N�^�[��{���̌�n��
extern void CharaBase_Terminate(void);

// �w��̃L�����N�^�[��{���̃A�h���X���擾����
//     �߂�l : �L�����N�^�[��{���\���̂̃A�h���X
extern SCharaBaseInfo * CharaBase_GetInfo(
	// �L�����N�^�[
	EChara Chara
);

// �w��̕����{���̃A�h���X���擾����
//     �߂�l : �����{���̃A�h���X
extern const SCharaWeaponBaseInfo * CharaBase_GetWeaponInfo(
	// ����
	ECharaWeapon Weapon
);

// �w��̑f�ނ̋��ʃT�E���h�̓o�^�ԍ����擾����
//     �߂�l : �T�E���h�̓o�^�ԍ�
extern int CharaBase_GetWeaponCommonMaterialSound(
	// �f�ރ^�C�v
	EMaterialType MatType
);

// �w��̋��ʌ��ʉ��̓o�^�ԍ����擾����
//     �߂�l : �T�E���h�̓o�^�ԍ�
extern int CharaBase_GetCommonSE(
	// �L�����N�^�[���ʌ��ʉ�
	ECharaCommonSE CommonSE
);

// �w��̃L�����N�^�[�A�j���[�V�����̃t�@�C�������擾����
//     �߂�l : �A�j���[�V�����̃t�@�C�����\���̂̃A�h���X
extern const SCharaAnimFileInfo * CharaBase_GetAnimFileInfo(
	// �L�����N�^�[�A�j���[�V����
	ECharaAnim Anim
);

// �w��̑f�ރ^�C�v�̖��O���擾����
//     �߂�l : ���O������̃A�h���X
extern const char * GetMaterialTypeName(
	// �f�ރ^�C�v
	EMaterialType MatType
);

// �����蔻��̏�񂩂�A�����蔻��Ɏg�p����J�v�Z���`����`������Q���W���擾����
extern void CharaBase_GetHitCapsulePos(
	// �����蔻����̃A�h���X
	const SCharaHitInfo *HitInfo,

	// �����蔻����s���L�����N�^�[�̍��W�̃A�h���X
	const VECTOR *Position,

	// �v�Z��̃J�v�Z���̍��W���i�[����A�h���X
	VECTOR *CapsulePosition1,
	VECTOR *CapsulePosition2
);

#endif
