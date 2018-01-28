#include "Sound.h"
#include "Stage.h"
#include "TextParam.h"
#include <string.h>
#include <stdlib.h>

// �R�c�T�E���h�̃f�t�H���g�̉�����������͈�
#define DEFAULT_RADIUS			(5000.0f)

// �T�E���h�̃f�t�H���g�����Đ��\��
#define DEFAULT_BUFFER_NUM		(6)

// �t�@�C���p�X�̍ő咷
#define MAX_PATH_LENGTH			(64)

// �P�T�E���h�ӂ�̍ő�o���G�[�V������
#define MAX_HANDLE_NUM			(8)

// �o�^�ł���T�E���h�̍ő吔
#define MAX_SOUNDINFO_NUM		(1024)

// �T�E���h�t�@�C�������݂���t�H���_�p�X
#define FILE_DIRECTORY_PATH		"Data\\Sound\\"

// �T�E���h�t�@�C���̊g���q
#define FILE_EXENAME			"wav"

// �T�E���h�̏��
typedef struct _SSoundInfo
{
	// �R�c�T�E���h���ǂ���( true:3D�T�E���h  false:2D�T�E���h )
	bool                 Is3DSound;

	// �t�@�C���p�X
	char                 FilePath[MAX_PATH_LENGTH];

	// �T�E���h�n���h���̐�
	int                  HandleNum;

	// �T�E���h�n���h��
	int                  Handle[MAX_HANDLE_NUM];
} SSoundInfo;

// �a�f�l�̏��
typedef struct _SBgmInfo
{
	// �T�E���h�n���h��
	int                  SoundHandle;

	// �a�f�l�̃��[�v���ɖ߂�ʒu
	int                  LoopStartPosition;

	// �a�f�l�̃��[�v������ʒu
	int                  LoopEndPosition;
} SBgmInfo;

// �T�E���h�V�X�e���̏��
typedef struct _SSoundSystemData
{
	// �T�E���h�̐�
	int                  SoundNum;

	// �T�E���h�̏��
	SSoundInfo           Sound[MAX_SOUNDINFO_NUM];

	// �a�f�l�̏��
	SBgmInfo             Bgm[EBGM_Num];

	// ���ݖ炵�Ă���a�f�l
	EBGM                 NowPlayBgm;
} SSoundSystemData;

static SSoundSystemData g_SndSysInfo;

// �T�E���h����������������
//     �߂�l : �������������������ǂ���( true:����  false:���s )
bool Sound_Initialize(void)
{
	int             i;
	SBgmInfo       *BInfo;
	STextParamInfo  TextParam;
	const char     *ParamString;
	char            String[256];

	// �T�E���h�̐���������
	g_SndSysInfo.SoundNum = 0;

	// �a�f�l�̏�񂪋L�q����Ă���e�L�X�g�t�@�C����ǂݍ���
	if (!LoadTextParam(&TextParam, FILE_DIRECTORY_PATH "BGM\\Param.txt"))
	{
		return false;
	}

	// �쐬����T�E���h�̃^�C�v���t�@�C���X�g���[�~���O�`���ɕύX����
	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);

	// �a�f�l�̐������J��Ԃ�
	BInfo = g_SndSysInfo.Bgm;
	for (i = 0; i < EBGM_Num; i++, BInfo++)
	{
		// �a�f�l�p�̃T�E���h�t�@�C���̃p�X���擾����
		ParamString = GetTextParamString(&TextParam, "BGM%d_FilePath", i);

		// �p�X��������Ȃ�������G���[
		if (ParamString == NULL)
		{
			return false;
		}

		// �a�f�l�p�̃T�E���h�t�@�C����ǂݍ���
		sprintf(String, FILE_DIRECTORY_PATH "BGM\\%s", ParamString);
		BInfo->SoundHandle = LoadSoundMem(String);

		// �ǂݍ��݂Ɏ��s������G���[
		if (BInfo->SoundHandle == -1)
		{
			return false;
		}

		// BGM�̃��[�v�J�n�ʒu�ƏI���ʒu���擾����
		BInfo->LoopStartPosition = GetTextParamInt(&TextParam, "BGM%d_Loop_Start", i);
		BInfo->LoopEndPosition = GetTextParamInt(&TextParam, "BGM%d_Loop_End", i);

		// �L���ȃ��[�v�ʒu������ꂽ�ꍇ�̓��[�v�ʒu���T�E���h�n���h���ɐݒ肷��
		if (BInfo->LoopStartPosition >= 0 &&
			BInfo->LoopEndPosition   >  0)
		{
			SetLoopSamplePosSoundMem(BInfo->LoopStartPosition, BInfo->SoundHandle);
			SetLoopStartSamplePosSoundMem(BInfo->LoopEndPosition, BInfo->SoundHandle);
		}
	}

	// �쐬����T�E���h�̃^�C�v��W���`���ɖ߂�
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

	// �w���ݍĐ�����BGM�x��������
	g_SndSysInfo.NowPlayBgm = EBGM_Num;

	// �����I��
	return true;
}

// �T�E���h�����̌�n��������
void Sound_Terminate(void)
{
	int         i;
	int         j;
	SSoundInfo *Info;
	SBgmInfo   *BInfo;

	// �S�ẴT�E���h�̃T�E���h�n���h�����폜����
	Info = g_SndSysInfo.Sound;
	for (i = 0; i < g_SndSysInfo.SoundNum; i++, Info++)
	{
		for (j = 0; j < Info->HandleNum; j++)
		{
			DeleteSoundMem(Info->Handle[j]);
		}
	}

	// �T�E���h�̐����O�ɂ���
	g_SndSysInfo.SoundNum = 0;

	// �S�Ă̂a�f�l�̃T�E���h�n���h�����폜����
	BInfo = g_SndSysInfo.Bgm;
	for (i = 0; i < EBGM_Num; i++, BInfo++)
	{
		DeleteSoundMem(BInfo->SoundHandle);

		// �T�E���h�n���h���Ƃ��Ė����Ȓl�������Ă���
		BInfo->SoundHandle = -1;
	}
}

// �T�E���h�t�@�C����ǉ�����
//     �߂�l : �T�E���h�t�@�C���̓o�^�ԍ�
int Sound_AddSound(
	// �ǉ�����T�E���h�t�@�C���̃p�X
	const char *FilePath,

	// �R�c�Ŗ炷�T�E���h���ǂ���( true:3D�T�E���h  false:2D�T�E���h )
	bool Is3DSound
)
{
	int             i;
	int             Index;
	SSoundInfo     *Info;
	char            FilePath2[MAX_PATH];
	STextParamInfo  TextParam;
	float           Radius;
	int             Volume;

	// ���ɓo�^���Ă���T�E���h�̒��ɁA�o�^���悤�Ƃ��Ă���T�E���h�����݂��邩���ׂ�
	Info = g_SndSysInfo.Sound;
	for (Index = 0; Index < g_SndSysInfo.SoundNum; Index++, Info++)
	{
		if (Info->Is3DSound == Is3DSound && strcmp(Info->FilePath, FilePath) == 0)
		{
			// ���݂����ꍇ�́A���ɓo�^�ς݂̃T�E���h�̓o�^�ԍ���Ԃ�
			return Index;
		}
	}

	// �o�^���Ă���T�E���h�̐����ő吔�ɒB���Ă�����G���[
	if (g_SndSysInfo.SoundNum == MAX_SOUNDINFO_NUM)
	{
		return -1;
	}

	// �R�c�T�E���h���ǂ�����ۑ�
	Info->Is3DSound = Is3DSound;

	// �쐬����T�E���h���R�c�T�E���h���ǂ�����ݒ肷��
	SetCreate3DSoundFlag(Is3DSound);

	// �R�c�T�E���h�̏ꍇ�͕������鋗���≹�ʂ��e�L�X�g�t�@�C������ǂݎ��
	if (Info->Is3DSound)
	{
		Radius = DEFAULT_RADIUS;
		Volume = 255;
		if (LoadTextParam(&TextParam, "%s%s_Param.txt", FILE_DIRECTORY_PATH, FilePath))
		{
			Radius = GetTextParamFloat(&TextParam, "Radius");
			Volume = (int)(GetTextParamFloat(&TextParam, "Volume") * 255.0f);
		}
	}

	// �T�E���h�t�@�C����ǂݍ���
	Info->HandleNum = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++)
	{
		// �T�E���h�t�@�C���p�X�̍쐬
		sprintf(FilePath2, "%s%s_%02d.%s",
			FILE_DIRECTORY_PATH, FilePath, i, FILE_EXENAME);

		// �T�E���h�t�@�C�������������烋�[�v�𔲂���
		if (FileRead_size(FilePath2) < 0)
		{
			break;
		}

		// �T�E���h�t�@�C���̓ǂݍ���
		Info->Handle[i] = LoadSoundMem(FilePath2, DEFAULT_BUFFER_NUM);
		if (Info->Handle[i] == -1)
		{
			return -1;
		}

		// �T�E���h�t�@�C���̏ꍇ�͕������鋗���Ɖ��ʂ��T�E���h�n���h���ɐݒ肷��
		if (Info->Is3DSound)
		{
			Set3DRadiusSoundMem(Radius, Info->Handle[i]);
			ChangeVolumeSoundMem(Volume, Info->Handle[i]);
		}
	}

	// �T�E���h�n���h���̐���ۑ�
	Info->HandleNum = i;

	// ����ǂݍ��߂Ȃ������ꍇ�̓G���[
	if (Info->HandleNum == 0)
	{
		return -1;
	}

	// �t�@�C���p�X��ۑ�
	strcpy(Info->FilePath, FilePath);

	// �T�E���h�̐��𑝂₷
	g_SndSysInfo.SoundNum++;

	// �T�E���h�̓o�^�ԍ���Ԃ�
	return Index;
}

// �T�E���h���Đ�����
void Sound_PlaySound(
	// �Đ�����T�E���h�̓o�^�ԍ�
	int SoundIndex,

	// �Đ��^�C�v( DX_PLAYTYPE_BACK �Ȃ� )
	int PlayType
)
{
	SSoundInfo *Info;

	// �w��̓o�^�ԍ����L�����`�F�b�N
	if (SoundIndex < 0 || SoundIndex >= g_SndSysInfo.SoundNum)
	{
		return;
	}
	Info = &g_SndSysInfo.Sound[SoundIndex];

	// �T�E���h�n���h������������ꍇ�̓����_���Ŗ炷�T�E���h�����肷��
	PlaySoundMem(Info->Handle[GetRand(Info->HandleNum - 1)], PlayType);
}

// �T�E���h���R�c�Đ�����
void Sound_PlaySound3D(
	// �T�E���h��炷���[���h���W
	VECTOR Position,

	// �Đ�����T�E���h�̓o�^�ԍ�
	int SoundIndex,

	// �Đ��^�C�v( DX_PLAYTYPE_BACK �Ȃ� )
	int PlayType
)
{
	SSoundInfo *Info;
	int         PlayIndex;

	// �w��̓o�^�ԍ����L�����`�F�b�N
	if (SoundIndex < 0 || SoundIndex >= g_SndSysInfo.SoundNum)
	{
		return;
	}
	Info = &g_SndSysInfo.Sound[SoundIndex];

	// �T�E���h�n���h������������ꍇ�͖炷�T�E���h�������_���Ō��肷��
	PlayIndex = GetRand(Info->HandleNum - 1);

	// �炷���[���h���W��ݒ肷��
	SetNextPlay3DPositionSoundMem(Position, Info->Handle[PlayIndex]);

	// �T�E���h���Đ�����
	PlaySoundMem(Info->Handle[PlayIndex], PlayType);
}

// �a�f�l���Đ�����
void Sound_PlayBGM(
	// �Đ�����a�f�l
	EBGM BGM
)
{
	SBgmInfo *BInfo = &g_SndSysInfo.Bgm[BGM];

	// ���ݍĐ����Ă���BGM�Ɠ����ꍇ�͉��������ɏI��
	if (g_SndSysInfo.NowPlayBgm == BGM)
	{
		return;
	}

	// ���ݑ���BGM���Đ����Ă������~����
	if (g_SndSysInfo.NowPlayBgm != EBGM_Num)
	{
		Sound_StopBGM();
	}

	// BGM�̍Đ����J�n����
	// �L���ȃ��[�v�|�C���g������ꍇ�̓��[�v�Đ�����
	PlaySoundMem(BInfo->SoundHandle,
		BInfo->LoopStartPosition >= 0 ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK
	);

	// �w���ݍĐ�����BGM�x���X�V����
	g_SndSysInfo.NowPlayBgm = BGM;
}

// �a�f�l�̍Đ����~�߂�
void Sound_StopBGM(void)
{
	// ���݉���BGM���Đ����Ă��Ȃ��ꍇ�͉��������I��
	if (g_SndSysInfo.NowPlayBgm == EBGM_Num)
	{
		return;
	}

	// BGM�̃T�E���h�n���h���̍Đ����~����
	StopSoundMem(g_SndSysInfo.Bgm[g_SndSysInfo.NowPlayBgm].SoundHandle);

	// �w���ݍĐ�����BGM�x�������Đ����Ă��Ȃ����Ƃ������l�ɂ���
	g_SndSysInfo.NowPlayBgm = EBGM_Num;
}
