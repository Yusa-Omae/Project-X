#include "Animation.h"
#include "BinaryFile.h"
#include "TextParam.h"
#include "DxLib.h"
#include <stdio.h>
#include <string.h>

// �A�j���[�V�����C�x���g�^�C�v���e�[�u��
static const char *g_AnimEventTypeName[EAnimEventType_Num] =
{
	"Sound",			// EAnimEventType_Sound
	"PhysicsSound",		// EAnimEventType_PhysicsSound
	"AnimCancel",		// EAnimEventType_AnimCancel
	"AttackStart",		// EAnimEventType_AttackStart,
	"AttackEnd",		// EAnimEventType_AttackEnd,
	"Other",			// EAnimEventType_Other,
};

// �A�j���[�V�����������^�C�v���e�[�u��
static const char *g_AnimPhysicsSoundName[EAnimPhysicsSound_Num] =
{
	"footstep",			// EAnimPhysicsSound_Run
	"down",				// EAnimPhysicsSound_Down
};

// �A�j���[�V���������t�@�C������ǂݍ���
//     �߂�l : �ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
bool LoadAnimInfoFile(
	// �A�j���[�V�������\���̂̃A�h���X
	SAnimInfo *AInfo,

	// �A�j���[�V�������t�@�C���p�X
	const char *FilePath
)
{
	const char     *ParamString;
	SAnimEventInfo *Event;
	int             i;
	int             j;
	int             ParamInt;
	STextParamInfo  TextParam;
	SBinaryFileData BinFileData;
	bool            LoadSuccess;

	// �A�j���[�V������񂪕ύX���ꂽ���ǂ����̃t���O��������
	AInfo->Change = false;

	// �A�j���[�V�����C�x���g�̐���������
	AInfo->EventNum = 0;

	// �ǂݍ��݂������������ǂ����̃t���O��|���Ă���
	LoadSuccess = false;

	// �A�j���[�V�������̃o�C�i���t�@�C����ǂݍ���
	if (ReadBinFile_Open(&BinFileData, "%s.dat", FilePath))
	{
		// �A�j���[�V�����C�x���g�̐���ǂݍ���
		ReadBinFile_Int(&BinFileData, &AInfo->EventNum);

		// �A�j���[�V�����C�x���g���̐������J��Ԃ�
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// �A�j���[�V�����C�x���g�^�C�v��ǂݍ���
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Event->Type = (EAnimEventType)ParamInt;

			// �A�j���[�V�����C�x���g�^�C����ǂݍ���
			ReadBinFile_Float(&BinFileData, &Event->TimeF);
			Event->TimeI = (int)(Event->TimeF * ANIM_TIME_INT_SCALE);

			// ���̑��̏��̏�����
			Event->SoundHandle = -1;
			Event->AttackPosIndex = 0;
			Event->AttackNo = 0;
			Event->PhysicsSound = (EAnimPhysicsSound)0;
			memset(Event->SoundPath, 0, sizeof(Event->SoundPath));

			// �A�j���[�V�����C�x���g�^�C�v�ɂ���ď����𕪊�
			switch (Event->Type)
			{
			case EAnimEventType_Sound:
				// ���Đ��C�x���g�̏ꍇ�̓T�E���h�t�@�C���̃p�X��ǂݍ���
				ReadBinFile_String(&BinFileData, Event->SoundPath);
				break;

			case EAnimEventType_PhysicsSound:
				// �������C�x���g�̏ꍇ�͕������^�C�v��ǂݍ���
				ReadBinFile_Int(&BinFileData, &ParamInt);
				Event->PhysicsSound = (EAnimPhysicsSound)ParamInt;
				break;

			case EAnimEventType_AttackStart:
				// �U������J�n�C�x���g�̏ꍇ�͍U���ԍ��ƍU���ʒu�ԍ���ǂݍ���
				ReadBinFile_Int(&BinFileData, &Event->AttackNo);
				ReadBinFile_Int(&BinFileData, &Event->AttackPosIndex);
				break;

			case EAnimEventType_AttackEnd:
				// �U������I���C�x���g�̏ꍇ�͍U���ԍ���ǂݍ���
				ReadBinFile_Int(&BinFileData, &Event->AttackNo);
				break;
			}
		}

		// �o�C�i���t�@�C�������
		ReadBinFile_Close(&BinFileData);

		// �ǂݍ��݂����������t���O�𗧂Ă�
		LoadSuccess = true;
	}

	// �o�C�i���t�@�C�����ǂݍ��߂Ȃ������ꍇ�̓A�j���[�V��������
	// �e�L�X�g�t�@�C����ǂݍ���
	if (!LoadSuccess &&
		LoadTextParam(&TextParam, "%s.txt", FilePath))
	{
		// �A�j���[�V�����C�x���g�̐���ǂݍ���
		AInfo->EventNum = GetTextParamInt(&TextParam, "EventNum");

		// �A�j���[�V�����C�x���g���̐������J��Ԃ�
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// �A�j���[�V�����C�x���g�^�C�v��ǂݍ���
			ParamString = GetTextParamString(&TextParam, "Event%d_TypeName", i);
			for (j = 0; j < EAnimEventType_Num; j++)
			{
				if (strcmp(g_AnimEventTypeName[j], ParamString) == 0)
				{
					break;
				}
			}
			if (j == EAnimEventType_Num)
			{
				return false;
			}
			Event->Type = (EAnimEventType)j;

			// �A�j���[�V�����C�x���g�^�C����ǂݍ���
			Event->TimeF = GetTextParamFloat(&TextParam, "Event%d_Time", i);
			Event->TimeI = (int)(Event->TimeF * ANIM_TIME_INT_SCALE);

			// ���̑��̏��̏�����
			Event->SoundHandle = -1;
			Event->AttackPosIndex = 0;
			Event->AttackNo = 0;
			Event->PhysicsSound = (EAnimPhysicsSound)0;
			memset(Event->SoundPath, 0, sizeof(Event->SoundPath));

			// �A�j���[�V�����C�x���g�^�C�v�ɂ���ď����𕪊�
			switch (Event->Type)
			{
			case EAnimEventType_Sound:
				// ���Đ��C�x���g�̏ꍇ�̓T�E���h�t�@�C���̃p�X��ǂݍ���
				ParamString = GetTextParamString(&TextParam, "Event%d_SoundPath", i);
				strcpy(Event->SoundPath, ParamString);
				break;

			case EAnimEventType_PhysicsSound:
				// �������C�x���g�̏ꍇ�͕������^�C�v��ǂݍ���
				ParamString = GetTextParamString(&TextParam, "Event%d_PhysicsSoundType", i);
				for (j = 0; j < EAnimPhysicsSound_Num; j++)
				{
					if (strcmp(g_AnimPhysicsSoundName[j], ParamString) == 0)
					{
						break;
					}
				}
				if (j == EAnimPhysicsSound_Num)
				{
					return false;
				}
				Event->PhysicsSound = (EAnimPhysicsSound)j;
				break;

			case EAnimEventType_AttackStart:
				// �U������J�n�C�x���g�̏ꍇ�͍U���ԍ��ƍU���ʒu�ԍ���ǂݍ���
				Event->AttackNo = GetTextParamInt(&TextParam, "Event%d_AttackNo", i);
				Event->AttackPosIndex = GetTextParamInt(&TextParam, "Event%d_AttackPosIndex", i);
				break;

			case EAnimEventType_AttackEnd:
				// �U������I���C�x���g�̏ꍇ�͍U���ԍ���ǂݍ���
				Event->AttackNo = GetTextParamInt(&TextParam, "Event%d_AttackNo", i);
				break;
			}
		}

		// �ǂݍ��݂����������t���O�𗧂Ă�
		LoadSuccess = true;
	}

	// �ǂݍ��݂������������ǂ����̃t���O��Ԃ�
	return LoadSuccess;
}

// �A�j���[�V���������t�@�C���ɕۑ�����
//     �߂�l : �ۑ��ɐ����������ǂ���
//              ( true:��������  false:���s���� )
bool SaveAnimInfoFile(
	// �A�j���[�V�������\���̂̃A�h���X
	const SAnimInfo *AInfo,

	// �A�j���[�V�������t�@�C���p�X
	const char *FilePath
)
{
	SBinaryFileData       BinFileData;
	FILE                 *fp;
	const SAnimEventInfo *Event;
	char                  FilePathS[MAX_PATH];
	int                   i;

	// �o�C�i���t�@�C���̏o��

	// �o�͗p�̃o�C�i���t�@�C�����J��
	sprintf(FilePathS, "%s.dat", FilePath);
	if (!WriteBinFile_Open(&BinFileData, FilePathS))
	{
		char MessageString[1024];
		char FullPath[1024];

		ConvertFullPath(FilePathS, FullPath);
		sprintf(MessageString, "�Z�[�u�Ɏ��s���܂���", FullPath);
		MessageBox(NULL, MessageString, "�Z�[�u���s", MB_OK);
		return false;
	}

	// �A�j���[�V�����C�x���g�̐��������o��
	WriteBinFile_Int(&BinFileData, AInfo->EventNum);

	// �A�j���[�V�����C�x���g�̐������J��Ԃ�
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// �A�j���[�V�����C�x���g�^�C�v�������o��
		WriteBinFile_Int(&BinFileData, Event->Type);

		// �A�j���[�V�����C�x���g�^�C���������o��
		WriteBinFile_Float(&BinFileData, Event->TimeF);

		// �A�j���[�V�����C�x���g�^�C�v�ɂ���ď����𕪊�
		switch (Event->Type)
		{
		case EAnimEventType_Sound:
			// ���Đ��C�x���g�̏ꍇ�̓T�E���h�t�@�C���̃p�X�������o��
			WriteBinFile_String(&BinFileData, Event->SoundPath);
			break;

		case EAnimEventType_PhysicsSound:
			// �������C�x���g�̏ꍇ�͕������^�C�v�������o��
			WriteBinFile_Int(&BinFileData, Event->PhysicsSound);
			break;

		case EAnimEventType_AttackStart:
			// �U������J�n�C�x���g�̏ꍇ�͍U���ԍ��ƍU���ʒu�ԍ��������o��
			WriteBinFile_Int(&BinFileData, Event->AttackNo);
			WriteBinFile_Int(&BinFileData, Event->AttackPosIndex);
			break;

		case EAnimEventType_AttackEnd:
			// �U������I���C�x���g�̏ꍇ�͍U���ԍ��������o��
			WriteBinFile_Int(&BinFileData, Event->AttackNo);
			break;
		}
	}

	// �o�C�i���t�@�C�������
	WriteBinFile_Close(&BinFileData);


	// �e�L�X�g�t�@�C���̏o��

	// �o�͗p�̃e�L�X�g�t�@�C�����J��
	sprintf(FilePathS, "%s.txt", FilePath);
	fp = fopen(FilePathS, "wt");
	if (fp == NULL)
	{
		char MessageString[1024];
		char FullPath[1024];

		ConvertFullPath(FilePathS, FullPath);
		sprintf(MessageString, "�Z�[�u�Ɏ��s���܂���", FullPath);
		MessageBox(NULL, MessageString, "�Z�[�u���s", MB_OK);
		return false;
	}

	// �A�j���[�V�����C�x���g�̐��������o��
	fprintf(fp, "int EventNum = %d\n", AInfo->EventNum);

	// �A�j���[�V�����C�x���g�̐������J��Ԃ�
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// �A�j���[�V�����C�x���g�^�C�v�������o��
		fprintf(fp, "string Event%d_TypeName = %s\n", i,
			g_AnimEventTypeName[Event->Type]);

		// �A�j���[�V�����C�x���g�^�C���������o��
		fprintf(fp, "float Event%d_Time = %.3f\n", i, Event->TimeF);

		// �A�j���[�V�����C�x���g�^�C�v�ɂ���ď����𕪊�
		switch (Event->Type)
		{
		case EAnimEventType_Sound:
			// ���Đ��C�x���g�̏ꍇ�̓T�E���h�t�@�C���̃p�X�������o��
			fprintf(fp, "string Event%d_SoundPath = %s\n", i, Event->SoundPath);
			break;

		case EAnimEventType_PhysicsSound:
			// �������C�x���g�̏ꍇ�͕������^�C�v�������o��
			fprintf(fp, "string Event%d_PhysicsSoundType = %s\n", i,
				g_AnimPhysicsSoundName[Event->PhysicsSound]);
			break;

		case EAnimEventType_AttackStart:
			// �U������J�n�C�x���g�̏ꍇ�͍U���ԍ��ƍU���ʒu�ԍ��������o��
			fprintf(fp, "int Event%d_AttackNo = %d\n", i, Event->AttackNo);
			fprintf(fp, "int Event%d_AttackPosIndex = %d\n", i, Event->AttackPosIndex);
			break;

		case EAnimEventType_AttackEnd:
			// �U������I���C�x���g�̏ꍇ�͍U���ԍ��������o��
			fprintf(fp, "int Event%d_AttackNo = %d\n", i, Event->AttackNo);
			break;
		}

		// ��̃A�j���[�V�����C�x���g���ɉ��s������
		fprintf(fp, "\n");
	}

	// �e�L�X�g�t�@�C�������
	fclose(fp);

	// �����I��
	return true;
}

// �A�j���[�V�������Ŏg�p����T�E���h�t�@�C����ǂݍ���
//     �߂�l : �ǂݍ��݂ɐ����������ǂ���
//              ( true:��������  false:���s���� )
bool LoadAnimInfoSound(
	// �A�j���[�V�������\���̂̃A�h���X
	SAnimInfo *AInfo
)
{
	int             i;
	SAnimEventInfo *Event;
	char            FilePath[256];

	// �A�j���[�V�����C�x���g�̐������J��Ԃ�
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// �����Đ�����C�x���g�ȊO�̏ꍇ�͂Ȃɂ����Ȃ�
		if (Event->Type != EAnimEventType_Sound)
		{
			continue;
		}

		// �ǂݍ��ރT�E���h�t�@�C���̃p�X���쐬����
		strcpy(FilePath, ANIM_SE_FILE_DIRECTORY_PATH);
		strcat(FilePath, Event->SoundPath);

		// �T�E���h�t�@�C���̓ǂݍ���
		Event->SoundHandle = Sound_AddSound(FilePath, true);
		if (Event->SoundHandle < 0)
		{
			return false;
		}
	}

	// �����I��
	return true;
}

// �w��̎��Ԕ͈͂ɂ���A�j���[�V�����C�x���g���擾����
void AnimInfoEventCheck(
	// �A�j���[�V�������\���̂̃A�h���X
	const SAnimInfo *AInfo,

	// �������ʂ̏����i�[����\���̂ւ̃A�h���X
	SAnimEventCheck *AECheck,

	// �����J�n����
	float StartTime,

	// �����I������
	float EndTime
)
{
	int                   i;
	const SAnimEventInfo *Event;
	int                   StartTimeI;
	int                   EndTimeI;

	// �����J�n���ԂƏI�����Ԃ𐮐��l�ɕϊ�
	StartTimeI = (int)(StartTime * ANIM_TIME_INT_SCALE);
	EndTimeI = (int)(EndTime   * ANIM_TIME_INT_SCALE);

	// �����Ɉ������������C�x���g�̐�������������
	AECheck->EventNum = 0;

	// �J�n���Ԃ��I�����Ԃ��傫�Ȓl�̏ꍇ�̓A�j���[�V���������[�v�����ꍇ�̔���Ȃ̂�
	// �����𕪊򂷂�
	if (EndTimeI >= 0 && StartTimeI > EndTimeI)
	{
		// �C�x���g�̐������J��Ԃ�
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// �����͈͊O���ǂ����𔻒�
			if (StartTimeI > Event->TimeI && EndTimeI <= Event->TimeI)
			{
				continue;
			}

			// �����͈͓��������ꍇ�͌��������A�j���[�V�����C�x���g�Ƃ��Ēǉ�����
			AECheck->Event[AECheck->EventNum] = Event;
			AECheck->EventNum++;
		}
	}
	else
	{
		// �C�x���g�̐������J��Ԃ�
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// �����͈͊O���ǂ����𔻒�
			if (StartTimeI > Event->TimeI || (EndTimeI >= 0 && EndTimeI <= Event->TimeI))
			{
				continue;
			}

			// �����͈͓��������ꍇ�͌��������A�j���[�V�����C�x���g�Ƃ��Ēǉ�����
			AECheck->Event[AECheck->EventNum] = Event;
			AECheck->EventNum++;
		}
	}
}

// �A�j���[�V�����C�x���g�^�C�v�̖��O���擾����
//     �߂�l : �A�j���[�V�����C�x���g�^�C�v��������̃A�h���X
const char *GetAnimEventTypeName(
	// �A�j���[�V�����C�x���g�^�C�v��
	EAnimEventType AnimEventType
)
{
	return g_AnimEventTypeName[AnimEventType];
}

// �A�j���[�V�����������^�C�v�̖��O���擾����
//     �߂�l : �A�j���[�V�����������^�C�v��������̃A�h���X
const char *GetAnimPhysicsSoundName(
	// �A�j���[�V�����������^�C�v
	EAnimPhysicsSound AnimPhysicsSound
)
{
	return g_AnimPhysicsSoundName[AnimPhysicsSound];
}
