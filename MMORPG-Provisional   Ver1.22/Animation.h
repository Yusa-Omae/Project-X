#ifndef ANIMATION_H
#define ANIMATION_H

#include "Sound.h"

// �A�j���[�V�����C�x���g�̃T�E���h�t�@�C���p�X�Ŏg�p�ł���ő啶����
#define ANIM_EVENT_SOUNDPATH_MAX_LENGTH			(64)

// �A�j���[�V�����C�x���g�̍ő吔
#define ANIM_EVENT_MAX_NUM						(64)

// �A�j���[�V�����̃^�C���𐮐��^�ɕϊ�����ۂ̃X�P�[��
#define ANIM_TIME_INT_SCALE						(10000.0f)

// �A�j���[�V�����ōĐ�������ʉ��̃t�@�C�����i�[����Ă���t�H���_
#define ANIM_SE_FILE_DIRECTORY_PATH				"SE\\"

// �A�j���[�V�����C�x���g�̃^�C�v
typedef enum _EAnimEventType
{
	EAnimEventType_Sound,			// �����Đ�
	EAnimEventType_PhysicsSound,	// ���������Đ�
	EAnimEventType_AnimCancel,		// �A�j���[�V�����L�����Z��
	EAnimEventType_AtkStart,		// �U������J�n
	EAnimEventType_AtkEnd,		// �U������I��
	EAnimEventType_Other,			// ���̑�

	EAnimEventType_Num				// �A�j���[�V�����C�x���g�̃^�C�v�̐�
} EAnimEventType;

// �A�j���[�V�����������̃^�C�v
typedef enum _EAnimPhysicsSound
{
	EAnimPhysicsSound_Footstep,		// ����
	EAnimPhysicsSound_Down,			// �|���

	EAnimPhysicsSound_Num			// �A�j���[�V�����������̃^�C�v�̐�
} EAnimPhysicsSound;

// �A�j���[�V�����C�x���g���
typedef struct _SAnimEventInfo
{
	// �A�j���[�V�����C�x���g�^�C�v
	EAnimEventType         Type;

	// �C�x���g�^�C���̕��������_�^�l
	float                  TimeF;

	// �C�x���g�^�C���̐����^�l
	int                    TimeI;

	// �U���ԍ�
	// ( Type �� EAnimEventType_AtkStart ���� EAnimEventType_AtkEnd �̏ꍇ�Ɏg�p )
	int                    AtkNo;

	// �U���ʒu�ԍ�
	// ( Type �� EAnimEventType_AtkStart �̏ꍇ�Ɏg�p )
	int                    AtkPosIndex;

	// �A�j���[�V�����������^�C�v
	// ( Type �� EAnimEventType_PhysicsSound �̏ꍇ�Ɏg�p )
	EAnimPhysicsSound      PhysicsSound;

	// �T�E���h�t�@�C���̃p�X
	// ( Type �� EAnimEventType_Sound �̏ꍇ�Ɏg�p )
	char                   SoundPath[ANIM_EVENT_SOUNDPATH_MAX_LENGTH];

	// �T�E���h�n���h��
	// ( Type �� EAnimEventType_Sound �̏ꍇ�Ɏg�p )
	int                    SoundHandle;
} SAnimEventInfo;

// �A�j���[�V�������
typedef struct _SAnimInfo
{
	// �C�x���g���̐�
	int                    EventNum;

	// �C�x���g���̔z��
	SAnimEventInfo         Event[ANIM_EVENT_MAX_NUM];

	// ���e���ύX���ꂽ���ǂ����̃t���O( �c�[���Ŏg�p )
	bool                   Change;
} SAnimInfo;

// �w�莞�Ԕ͈͂̃A�j���[�V�����C�x���g�擾�p���
typedef struct _SAnimEventCheck
{
	// �w�莞�Ԕ͈͂̃A�j���[�V�����C�x���g�̐�
	int                    EventNum;

	// �w�莞�Ԕ͈͂̃A�j���[�V�����C�x���g���ւ̃|�C���^
	const SAnimEventInfo * Event[ANIM_EVENT_MAX_NUM];
} SAnimEventCheck;


// �A�j���[�V���������t�@�C������ǂݍ���
//     �߂�l : �ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool LoadAnimInfoFile(
	// �A�j���[�V�������\���̂̃A�h���X
	SAnimInfo *AInfo,

	// �A�j���[�V�������t�@�C���p�X
	const char *FilePath
);

// �A�j���[�V���������t�@�C���ɕۑ�����
//     �߂�l : �ۑ��ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool SaveAnimInfoFile(
	// �A�j���[�V�������\���̂̃A�h���X
	const SAnimInfo *AInfo,

	// �A�j���[�V�������t�@�C���p�X
	const char *FilePath
);

// �A�j���[�V�������Ŏg�p����T�E���h�t�@�C����ǂݍ���
//     �߂�l : �ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
extern bool LoadAnimInfoSound(
	// �A�j���[�V�������\���̂̃A�h���X
	SAnimInfo *AInfo
);

// �w��̎��Ԕ͈͂ɂ���A�j���[�V�����C�x���g���擾����
extern void AnimInfoEventCheck(
	// �A�j���[�V�������\���̂̃A�h���X
	const SAnimInfo *AInfo,

	// �������ʂ̏����i�[����\���̂ւ̃A�h���X
	SAnimEventCheck *AECheck,

	// �����J�n����
	float StartTime,

	// �����I������
	float EndTime
);

// �A�j���[�V�����C�x���g�^�C�v�̖��O���擾����
//     �߂�l : �A�j���[�V�����C�x���g�^�C�v��������̃A�h���X
extern const char *GetAnimEventTypeName(
	// �A�j���[�V�����C�x���g�^�C�v��
	EAnimEventType AnimEventType
);

// �A�j���[�V�����������^�C�v�̖��O���擾����
//     �߂�l : �A�j���[�V�����������^�C�v��������̃A�h���X
extern const char *GetAnimPhysicsSoundName(
	// �A�j���[�V�����������^�C�v
	EAnimPhysicsSound AnimPhysicsSound
);

#endif
