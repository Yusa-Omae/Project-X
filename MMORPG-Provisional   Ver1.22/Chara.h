#ifndef CHARA_H
#define CHARA_H

#include "CharaBase.h"
#include "CharaHealthGauge.h"
#include "Effect.h"
#include "Item.h"

// �L�����̃f�t�H���g��]���x
#define CHARA_DEFAULT_ANGLE_SPEED					(DX_PI_F * 6.0f)

// �L�����̃f�t�H���g�A�j���ύX���x
#define CHARA_DEFAULT_CHANGE_ANIM_SPEED				(15.0f)

// �L�����̃f�t�H���g�U���A�j���ύX���x
#define CHARA_DEFAULT_ATTACK_CHANGE_ANIM_SPEED		(30.0f)

// �U�������������L�������L�^�ł���ő吔
#define CHARA_ATTACK_HIT_CHARA_MAX					(16)

// �U������p�̍��W�̐�
#define CHARA_ATTACK_POS_NUM						(2)

// �L�����̗̑͂��\������Ă��������܂ł̎���
#define CHARA_HPGAUGE_HIDE_DELAY				(10.0f)

// �L�����̏��
typedef enum _ECharaState
{
	ECharaState_Move = 0,						// �ʏ�ړ���
	ECharaState_Attack,							// �U����
	ECharaState_Defence,						// �h�䒆
	ECharaState_Jump,							// �W�����v��
	ECharaState_Damage,							// �_���[�W��
	ECharaState_Blow,							// ������ђ�
	ECharaState_Down,							// �|�ꒆ
	ECharaState_FadeOut,						// �t�F�[�h�A�E�g��
} ECharaState;

// �_���[�W�^�C�v
typedef enum _ECharaAttack_DamageType
{
	ECharaAttack_DamageType_Cut,				// �a��U���ɂ��_���[�W
	ECharaAttack_DamageType_Earthquake,			// �n�k�ɂ��_���[�W
} ECharaAttack_DamageType;

// �L�����̐�����я��
typedef enum _ECharaBlowState
{
	ECharaBlowState_In,							// ������я�Ԉڍs�A�j���[�V������
	ECharaBlowState_WaitLanding,				// �ڒn�҂���
	ECharaBlowState_Out,						// �ڒn���Ă���̃A�j���[�V������
} ECharaBlowState;

// �L�����̕`�惂�[�h
typedef enum _ECharaRenderMode
{
	ECharaRenderMode_Always,					// ��ɕ`�悷��
	ECharaRenderMode_Distance_Less,				// �w��̋����ȓ��ł���Ε`�悷��
	ECharaRenderMode_Distance_Greater,			// �w��̋����ȏ�ł���Ε`�悷��
} ECharaRenderMode;

// �U���ɓ��������L������X�e�[�W�̏��
typedef struct _SCharaAttackHitInfo
{
	// �U�������������L�����̐�
	int                   HitCharaNum;

	// �U�������������L�����ւ̃|�C���^
	struct _SCharaInfo *  HitCharaInfo[ CHARA_ATTACK_HIT_CHARA_MAX ];

	// �X�e�[�W�ɑ΂��ē����������ǂ����̃t���O( true:��������  false:�������Ă��Ȃ� )
	bool                  StageHit;

	// �X�e�[�W�ɑ΂��ē��������ꍇ�ɁA���������ӏ��̑f�ރ^�C�v
	EMaterialType         StageHitMaterialType;
} SCharaAttackHitInfo;

// �L�����̍U�����
typedef struct _SCharaAttackInfo
{
	// ��񂪗L�����ǂ����̃t���O( true:�L��  false:���� )
	bool                  Enable;

	// �U������Ɏg�p���� SCharaBaseInfo �\���̂̃����o�[�ϐ� AttackPosInfo �̗v�f�ԍ�
	int                   AttackPosIndex;

	// �U���̋O�ՃG�t�F�N�g�ւ̃|�C���^
	SEffectInfo *         LocusEffect;

	// �U������p�̍��W�̗L����
	int                   ValidPositionNum;

	// �U������p�̃t���[���ɋ߂����̍��W
	VECTOR                NearPosition[ CHARA_ATTACK_POS_NUM ];

	// �U������p�̃t���[�����牓�����̍��W
	VECTOR                FarPosition[ CHARA_ATTACK_POS_NUM ];

	// �U���ɓ��������L������X�e�[�W�̏��
	SCharaAttackHitInfo   HitInfo;
} SCharaAttackInfo;

// �L�����̃A�j���[�V�������
typedef struct _SCharaAnimInfo
{
	// ���ݍĐ����̃A�j���[�V����
	ECharaAnim            NowAnim;

	// ��O�ɍĐ����Ă����A�j���[�V����
	ECharaAnim            PrevAnim;

	// ���ݍĐ����ɃA�j���[�V�����̃��f���ւ̃A�^�b�`�C���f�b�N�X
	int                   NowAttachIndex;

	// ��O�ɍĐ����Ă����A�j���[�V�����̃��f���ւ̃A�^�b�`�C���f�b�N�X
	int                   PrevAttachIndex;

	// �Đ��A�j���[�V�����̕ύX���x
	float                 ChangeSpeed;

	// �Đ��A�j���[�V�����̕ύX��
	float                 ChangeRate;

	// ���ݍĐ����̃A�j���[�V�����̍Đ�����
	float                 NowTime;

	// ��O�ɍĐ����Ă����A�j���[�V�����̍Đ�����
	float                 PrevTime;

	// �A�j���[�V�����̃L�����Z���C�x���g�������������ǂ����̃t���O
	bool                  Cancel;

	// �A�j���[�V�����̍Đ��������������ǂ����̃t���O
	bool                  End;

	// ���[�g�t���[���̈ړ��x�N�g��
	VECTOR                Move;
} SCharaAnimInfo;

// �L�����̌������
typedef struct _SCharaAngleInfo
{
	// ���݂̌���( �P�ʁF���W�A�� )
	float                 NowAngle;

	// �ڕW�̌���( �P�ʁF���W�A�� )
	float                 TargetAngle;

	// �ڕW�̌����ɓ��B�������ǂ���
	bool                  AngleMoveEnd;

	// �����̕ω����x
	float                 AngleSpeed;

	// ���݂̌����̐��ʕ����̃x�N�g��
	VECTOR                FrontDirection;

	// ���݂̌����̌������č������̃x�N�g��
	VECTOR                LeftDirection;
} SCharaAngleInfo;

// �L�����̏��
typedef struct _SCharaInfo
{
	// �\���̂��g�p���Ă��邩�ǂ����̃t���O
	bool                  UseFlag;

	// �R�c���f���n���h��
	int                   ModelHandle;

	// �L����
	EChara                Chara;

	// �L�������ʗp�ԍ�( ����L������|������X�e�[�W�N���A�A�Ƃ��������Ŏg�p )
	int                   CharaNo;

	// �L�����̊�{���ւ̃|�C���^
	const SCharaBaseInfo *BaseInfo;

	// �L�����̏��
	ECharaState           State;

	// �L�����̍��W
	VECTOR                Position;

	// �L�����̌������
	SCharaAngleInfo       AngleInfo;

	// �L�����̃A�j���[�V�������
	SCharaAnimInfo        AnimInfo;

	// ������я��
	ECharaBlowState       BlowState;

	// �L�����������Ă���ꏊ�̑f��
	EMaterialType         OnMaterialType;

	// �s�����x( 0.0 �` 1.0f )
	float                 OpacityRate;

	// �W�����v���Ă��邩( �󒆂ɂ��邩 )�ǂ����̃t���O
	bool                  JumpState;

	// �A�j���[�V�����̈ړ��l�ł͂Ȃ��ASpd �ɂ��ړ����s�����ǂ����̃t���O
	bool                  ProgramMove;

	//��b�̗�
	int                   Health;

	//�̗̓Q�[�W���
	SCharaHealthGauge     HealthGauge;

	//�̗̓Q�[�W��\�����邩�ǂ����̃t���O
	bool                  HealthGaugeVisible;

	//�̗̓Q�[�W���\���ɂ���܂ł̎���
	float                 HealthGaugeHideDelay;	

	//�_���[�W���󂯂�����
	VECTOR                DamageDirection;

	//��b�U����
	int                   Atk;

	//�U�����
	SCharaAttackInfo	  AttackInfo[ CHARA_ATTACK_MAX_NUM ];

	//��b�h���
	int					  Def;

	//��b�N���e�B�J����
	float				  Crt;

	//�ړ����x
	VECTOR                Spd;

	//��O�̃t���[���ł� Spd �̒l
	VECTOR                PrevSpd;

	//��O�̃t���[���ł̃��[�g�t���[���̃A�j���[�V�����ɂ��ړ����x
	VECTOR                PrevAnimSpd;

	//��O�̃t���[���ł̍ŏI�I�Ȉړ����x
	// ( Spd �ɃA�j���[�V�����̈ړ��l�Ȃǂ����Z�������� )
	VECTOR                PrevFixSpd;

	//������(Player:������ Enemy:�h���b�v��)�|���ꂽ����z�h���b�v
	int					  Gold;

	//�����A�C�e��
	int					  ItemHav[10];

	//�����A�C�e���̑ϋv�l
	int					  Item_Db[10];

	//�A�C�e���ł̒ǉ��ő�HP���v�l
	int					  Add_Health;

	//�A�C�e���ł̒ǉ��U���͍��v�l
	int					  Add_Atk;

	//�A�C�e���ł̒ǉ��h��͍��v�l
	int					  Add_Def;

	//�A�C�e���ł̒ǉ��N���e�B�J�������v�l
	float				  Add_Crt;

	//�A�C�e���ł̒ǉ��ړ����x���v�l
	float				  Add_Spd;

	//�A�C�e���ł̍U����^�����ۂ�HP�z�������v�l
	float				  Abp_Heal;

	//�A�C�e���ł̕b�ԉ񕜑��x���v�l
	int					  Auto_Heal;

	//�A�C�e���ł̂����h���b�v�ʑ������v�l(Player�F�G��|�����ۂɊl���ł���ʑ����� Enemy:�|���ꂽ�ۂ̃S�[���h���Ƃ��ʂ̑�����)
	float				  Gold_Per;

	// �L�����ʂ̏��ւ̃|�C���^
	void *                SubData;
} SCharaInfo;


// �L�����̏����̏�����������
//     �߂�l : �������������������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Chara_Initialize( void );

// �L�����̏����̌�n��������
extern void Chara_Terminate( void );

// �S�ẴL�����̏�Ԑ��ڏ��������s����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_StepAll(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
);

// �S�ẴL�����̕`�揈�����s��
extern void Chara_RenderAll(
	// �`�惂�[�h
	ECharaRenderMode RenderMode,

	// �`�惂�[�h�� ECharaRenderMode_Always �ȊO�̏ꍇ�Ɏg�p������r�p����
	float CompareDistance
);

// �S�ẴL�����̊ۉe�̕`�揈�����s��
extern void Chara_ShadowRenderAll(
	// �`�惂�[�h
	ECharaRenderMode RenderMode,

	// �`�惂�[�h�� ECharaRenderMode_Always �ȊO�̏ꍇ�Ɏg�p������r�p����
	float CompareDistance
);

// �S�ẴL�����̂Q�c�`�揈�����s��
extern void Chara_2D_RenderAll( void );

// �w��̃^�C�v�̃L������|��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_TagetTypeKill(
	// �|�������L�����^�C�v
	ECharaType Type
);

// �V�����L�������쐬����
//     �߂�l : �L�����̍쐬�ɐ����������ǂ���
//              ( true : ��������   false : ���s���� )
extern bool Chara_Create(
	// �o��������L����
	EChara Chara,

	// �o��������L�����̏����ʒu
	VECTOR Position,

	// �o��������L�����̏�������
	float Angle,

	// �o��������L�����̎��ʗp�ԍ�
	int CharaNo
);

// �w��̃L�������폜����
extern void Chara_Delete(
	// �폜����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �S�ẴL�������폜����
extern void Chara_AllDelete( void );

// �w��L�����̎��͂ɋ���w��^�C�v�̃L������T��
//     �߂�l : �����Ō��������L�����̏��\���̂̃A�h���X
extern SCharaInfo * Chara_SearchTarget(
	// ������ƂȂ�L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �T���L�����̃^�C�v
	ECharaType SearchCharaType,

	// �T������( CInfo �̋���ʒu����ǂꂾ���̋����͈̔͂��������邩 )
	float SearchDistance,

	// �T���p�x�͈�( CInfo �̌����Ă����������ǂ͈̔͂��������邩 )
	float SearchAngle
);

// �w����W�̎��͂ɋ���L�����N�^�[�Ƀ��b�Z�[�W�𑗂�
extern void Chara_SendMessage(
	// ���b�Z�[�W�𑗂�L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// ���b�Z�[�W�𑗂��ƂȂ���W
	VECTOR BasePosition,

	// ���b�Z�[�W�𑗂�͈�
	float SendDistance,

	// ���b�Z�[�W�̎��ʔԍ�
	int Message
);

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
);

// �L�������w��̍��W�̕����Ɍ�������
extern void Chara_TargetAngleMove(
	// ������ύX����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �L�����Ɍ������������W
	VECTOR TargetPosition
);

// �L�����������ׂ�������ݒ肷��
extern void Chara_SetTargetDirection(
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
);

// �L�����������ׂ�������ݒ肷��( �x���p�x�w��^�C�v )
extern void Chara_SetTargetAngle(
	// ������ݒ肷��L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �L�����Ɍ����������( �x���p�x )
	float Angle,

	// ���X�Ɍ�����ς��邩�ǂ���
	// ( true:���X�Ɍ�����ς���  false:��u�Ō�����ς��� )
	bool Interp
);

// �L�����̌����Ă�������Ɋ�Â������X�V����
extern void Chara_AngleParamRefresh(
	// �����X�V����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo
);

// �L�������Đ�����A�j���[�V������ύX����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool Chara_ChangeAnim(
	// �A�j���[�V�������Đ�����L�����̏��\���̂̃A�h���X
	SCharaInfo *CInfo,

	// �V���ɍĐ�����A�j���[�V����
	ECharaAnim NewAnim,

	// ���ݍĐ����Ă���A�j������V���ɍĐ�����A�j���Ɉڍs���鑬�x( �ڍs��/�b )
	float ChangeSpeed
);

#endif