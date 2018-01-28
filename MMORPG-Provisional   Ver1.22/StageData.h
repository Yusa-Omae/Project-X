#ifndef STAGEDATA_H
#define STAGEDATA_H

#include "Sound.h"
#include "CharaBase.h"
#include "TextParam.h"

// �X�e�[�W�f�[�^�t�@�C���̐擪�ɏ������ޕ�����
#define STAGEFILE_ID								"MAPD"

// �X�e�[�W�f�[�^�Ɋ܂߂���ő�L������
#define STAGE_CHARA_MAX_NUM							(256)

// �X�e�[�W�f�[�^�Ɋ܂߂���ő�I�u�W�F�N�g��
#define STAGE_OBJECT_MAX_NUM						(512)

// �X�e�[�W�f�[�^�Ɋ܂߂���ő�I�u�W�F�N�g�p���f����
#define STAGE_OBJECT_MODEL_MAX_NUM					(256)

// �X�e�[�W�f�[�^�Ɋ܂߂���ő�C�x���g��
#define STAGE_EVENT_MAX_NUM							(256)

// ��̃C�x���g�ӂ�Ɏw��ł���I�u�W�F�N�g�̍ő吔
#define STAGE_EVENT_OBJECT_MAX_NUM					(16)

// �R���W�������f���̍ő�}�e���A����
#define STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM		(32)

// �X�e�[�W�̕`��^�C�v
typedef enum _EStageRenderType
{
	EStageRenderType_Outdoors,				// ���O
	EStageRenderType_Cave,					// ���A
} EStageRenderType;

// �X�e�[�W�N���A�����^�C�v
typedef enum _EStageClearCondition
{
	EStageClearCondition_CharaKillNum,		// �G���w�萔�|��
	EStageClearCondition_TargetCharaKill,	// �w��̓G��|��
} EStageClearCondition;

// �X�e�[�W�C�x���g�̃^�C�v
typedef enum _EStageEventType
{
	EStageEventType_CharaAppear,			// �L�������o��������
	EStageEventType_BGM,					// �a�f�l��ύX����

	EStageEventType_Num						// �X�e�[�W�C�x���g�̃^�C�v��
} EStageEventType;

// �X�e�[�W�ɔz�u����L�����̏��
typedef struct _SStageCharaInfo
{
	// �L�����N�^�[
	EChara                Chara;

	// �z�u������W
	VECTOR                Position;

	// �z�u���̌���
	float                 Angle;
} SStageCharaInfo;

// �I�u�W�F�N�g�Ŏg�p����R�c���f���̏��
typedef struct _SStageObjectModelInfo
{
	// �ǂݍ��܂�Ă��邩�ǂ����̃t���O
	bool                  Load;

	// �R�c���f���n���h��
	int                   ModelHandle;

	// �R���W�����p�R�c���f���n���h��
	int                   CollisionModelHandle;

	// �R���W�����R�c���f���̃}�e���A���^�C�v
	EMaterialType         ColModelMatType[STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM];
} SStageObjectModelInfo;

// �X�e�[�W�ɔz�u����I�u�W�F�N�g�̏��
typedef struct _SStageObjectInfo
{
	// �g�p����R�c���f��
	int                   ModelID;

	// �z�u������W
	VECTOR                Position;

	// �z�u���̌���
	VECTOR                Angle;

	// �z�u���̃X�P�[��
	VECTOR                Scale;

	// ���s���p�̂R�c���f���n���h��
	int                   ModelHandle;

	// ���s���p�̃R���W�����p�R�c���f���n���h��
	int                   CollisionModelHandle;
} SStageObjectInfo;

// �X�e�[�W���̃C�x���g�̏��
typedef struct _SStageEventInfo
{
	// �C�x���g�^�C�v
	EStageEventType       EventType;

	// �z�u���W
	VECTOR                Position;

	// �C�x���g��������͈̔�
	VECTOR                AreaSize;

	// BGM�ύX�C�x���g�p��BGM���
	EBGM                  Bgm;

	// ���炩�̃I�u�W�F�N�g�ɑ΂���C�x���g�p�́A�Ώۂ̃I�u�W�F�N�g�̐�
	int                   ObjectNum;

	// ���炩�̃I�u�W�F�N�g�ɑ΂���C�x���g�p�̃I�u�W�F�N�g�ԍ��̔z��
	int                   Object[STAGE_EVENT_OBJECT_MAX_NUM];
} SStageEventInfo;

// �X�e�[�W�f�[�^�t�@�C���̃w�b�_�[
typedef struct _SStageInfoFileHeader
{
	// �X�e�[�W�f�[�^�t�@�C���m�F�p������
	char                  MagicID[4];

	// �L�����z�u���̐�
	int                   CharaInfoNum;

	// �I�u�W�F�N�g�z�u���̐�
	int                   ObjectInfoNum;

	// �C�x���g���̐�
	int                   EventInfoNum;

	// �X�e�[�W�J�n���̃v���C���[�̍��W
	VECTOR                PlayerPosition;

	// �X�e�[�W�J�n���̃v���C���[�̌���
	float                 PlayerAngle;

	// �X�e�[�W�Ŏg�p����`��^�C�v
	EStageRenderType      RenderType;

	// �X�e�[�W�N���A����
	EStageClearCondition  StageClearCondition;

	// �X�e�[�W�N���A�ɕK�v�ȓ|���G�̐�
	// ( StageClearCondition �� EStageClearCondition_CharaKillNum �̏ꍇ�Ɏg�p )
	int                   KillCharaNum;

	// �X�e�[�W�N���A�ɕK�v�ȓ|���ׂ��G�̔ԍ�
	// ( StageClearCondition �� EStageClearCondition_TargetCharaKill �̏ꍇ�Ɏg�p )
	int                   KillTargetChara;
} SStageInfoFileHeader;

// �X�e�[�W�f�[�^�����̏��
typedef struct _SStageData
{
	// �S�X�e�[�W��
	int                   TotalStageNum;

	// ���ݓǂݍ���ł���X�e�[�W�̔ԍ�
	int                   LoadStageNumber;

	// �X�e�[�W�f�[�^�t�@�C���̃w�b�_�[
	SStageInfoFileHeader  Header;

	// �X�e�[�W�̂R�c���f���n���h��
	int                   ModelHandle;

	// �X�e�[�W�̃R���W�����p�R�c���f���n���h��
	int                   CollisionModelHandle;

	// �X�e�[�W�̋�\���p�R�c���f���n���h��
	int                   SkyModelHandle;

	// �R���W�����R�c���f���̃}�e���A���^�C�v
	EMaterialType         ColModelMatType[STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM];

	// �X�e�[�W�̃L�����z�u���
	SStageCharaInfo       CharaInfo[STAGE_CHARA_MAX_NUM];

	// �X�e�[�W�̃I�u�W�F�N�g�z�u���
	SStageObjectInfo      ObjectInfo[STAGE_OBJECT_MAX_NUM];

	// �X�e�[�W�̃C�x���g���
	SStageEventInfo       EventInfo[STAGE_EVENT_MAX_NUM];

	// �I�u�W�F�N�g���g�p����R�c���f������ɑS�ēǂݍ��ނ��ǂ����̃t���O
	bool                  ObjectModelAlwaysLoad;

	// �I�u�W�F�N�g���g�p����R�c���f���n���h��
	SStageObjectModelInfo ObjectModelInfo[STAGE_OBJECT_MODEL_MAX_NUM];

	// �X�e�[�W�p�e�L�X�g�p�����[�^
	STextParamInfo        TextParam;
} SStageData;

extern SStageData g_StageData;

// �X�e�[�W�f�[�^����������������
//     �߂�l : �������������������ǂ���( true:��������  false:���s���� )
extern bool StageData_Initialize(
	// �I�u�W�F�N�g���g�p����R�c���f������ɂ��ׂēǂݍ��ނ��ǂ���
	// ( true:�S�ēǂݍ���  false:�K�v�Ȃ��̂����ǂݍ��� )
	bool ObjectModelAlwaysLoad
);

// �X�e�[�W�f�[�^�����̌�n��������
extern void StageData_Terminate(void);

// �X�e�[�W�f�[�^��ǂݍ���
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool StageData_Load(
	// �ǂݍ��ރX�e�[�W�̔ԍ�
	int StageNumber
);

// �X�e�[�W�f�[�^���������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern void StageData_Unload(void);

// �X�e�[�W�f�[�^��ۑ�����
//     �߂�l : �X�e�[�W�f�[�^�̕ۑ��ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool StageData_Save(void);

// �X�e�[�W�̂R�c���f���n���h�����擾����
//     �߂�l : �X�e�[�W�̂R�c���f���n���h��
extern int StageData_GetModelHandle(void);

// �X�e�[�W�̃R���W�����p�R�c���f���n���h�����擾����
//     �߂�l : �X�e�[�W�̃R���W�����p�R�c���f���n���h��
extern int StageData_GetCollisionModelHandle(void);

// �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̐����擾����
//     �߂�l : �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̐�
extern int StageData_GetObjectNum(void);

// �I�u�W�F�N�g���g�p����R���W�����p�R�c���f���n���h�����擾����
//     �߂�l : �R���W�����p�R�c���f���n���h��
extern int StageData_GetObjectCollisionModelHandle(
	// �I�u�W�F�N�g���g�p����R�c���f���̔ԍ�
	int Index
);

// �I�u�W�F�N�g�̃R���W�����p�R�c���f���̎w��ԍ��̃}�e���A���^�C�v���擾����
extern EMaterialType StageData_GetObjectCollisionModelMaterialType(
	// �I�u�W�F�N�g���g�p����R�c���f���̔ԍ�
	int Index,

	// �R���W�����p�R�c���f���̃}�e���A���ԍ�
	int MaterialIndex
);

// �S�X�e�[�W�����擾����
//     �߂�l : �S�X�e�[�W��
extern int StageData_GetTotalStageNum(void);

// �ǂݍ���ł���X�e�[�W�̔ԍ����擾����
//     �߂�l : �X�e�[�W�ԍ�
extern int StageData_GetLoadStageNo(void);

// �X�e�[�W�̕`��^�C�v���擾����
//     �߂�l : �`��^�C�v
extern EStageRenderType StageData_GetRenderType(void);

// �X�e�[�W�̃N���A�������擾����
//     �߂�l : �X�e�[�W�̃N���A����
extern EStageClearCondition StageData_GetStageClearCondition(void);

// �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̐����擾����
//     �߂�l :  �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̐�
extern int StageData_GetKillCharaNum(void);

// �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̔ԍ����擾����
//     �߂�l : �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̔ԍ�
extern int StageData_GetKillTargetChara(void);

// �X�e�[�W�̃R���W�����p�R�c���f���̎w��ԍ��̃}�e���A���^�C�v���擾����
//     �߂�l : �}�e���A���^�C�v
extern EMaterialType StageData_GetCollisionModelMaterialType(
	// �R���W�����p�R�c���f���̃}�e���A���ԍ�
	int MaterialIndex
);

#endif