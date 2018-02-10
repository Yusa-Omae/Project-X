#include"StageData.h"
#include"Define.h"
#include"BinaryFile.h"
#include"DxLib.h"
#include<malloc.h>
#include"Sound.h"

SStageData g_StageData;

// �R���W�����p�R�c���f���̎w��̔ԍ��̃}�e���A���^�C�v���擾����
static EMaterialType StageData_GetCollisionModelMaterialType(
	// �R���W�����p�R�c���f���n���h��
	int CollisionModelHandle,

	// ���ׂ�}�e���A���̔ԍ�
	int MaterialIndex
);

// �I�u�W�F�N�g�̂R�c���f����ǂݍ���
//     �߂�l : �ǂݍ��݂������������ǂ���( true:��������  false:���s���� )
static bool StageData_ObjectModel_Load(
	// �������I�u�W�F�N�g�̂R�c���f���̔ԍ�
	int Index
);

// �I�u�W�F�N�g�̂R�c���f�����������
static void StageData_ObjectModel_Release(
	// �������I�u�W�F�N�g�̂R�c���f���̔ԍ�
	int Index
);

// �X�e�[�W�f�[�^����������������
//     �߂�l : �������������������ǂ���( true:��������  false:���s���� )
bool StageData_Initialize(
	// �I�u�W�F�N�g���g�p����R�c���f������ɂ��ׂēǂݍ��ނ��ǂ���
	// ( true:�S�ēǂݍ���  false:�K�v�Ȃ��̂����ǂݍ��� )
	bool ObjectModelAlwaysLoad
)
{
	int i;

	// �X�e�[�W�̏�񂪋L�q����Ă���e�L�X�g�t�@�C����ǂݍ���
	if (!LoadTextParam(&g_StageData.TextParam, "Data\\Stage\\Param.txt"))
	{
		return false;
	}

	// �S�X�e�[�W�����擾
	g_StageData.TotalStageNum = GetTextParamInt(&g_StageData.TextParam, "StageNum");

	// �w���ݓǂݍ��܂�Ă���X�e�[�W�ԍ��x���w�����ǂݍ��܂�Ă��Ȃ��x�ɃZ�b�g
	g_StageData.LoadStageNumber = -1;

	// �I�u�W�F�N�g�̂R�c���f������ɑS�ēǂݍ��ނ��ǂ����̃t���O��ۑ�
	g_StageData.ObjectModelAlwaysLoad = ObjectModelAlwaysLoad;

	// �I�u�W�F�N�g�̂R�c���f������ɑS�ēǂݍ��ޏꍇ�͂����őS�ēǂݍ���
	if (g_StageData.ObjectModelAlwaysLoad)
	{
		for (i = 0; i < OBJECT_DRAW_MAX_NUM; i++)
		{
			StageData_ObjectModel_Load(i);
		}
	}

	// �����I��
	return true;
}

// �X�e�[�W�f�[�^�����̌�n��������
void StageData_Terminate(void)
{
	int i;

	// �X�e�[�W�f�[�^�̉�����s��
	StageData_Unload();

	// �I�u�W�F�N�g�p�̂R�c���f����S�ĉ������
	for (i = 0; i < STAGE_OBJECT_MODEL_MAX_NUM; i++)
	{
		StageData_ObjectModel_Release(i);
	}
}

// �R���W�����p�R�c���f���̎w��̔ԍ��̃}�e���A���^�C�v���擾����
static EMaterialType StageData_GetCollisionModelMaterialType(
	// �R���W�����p�R�c���f���n���h��
	int CollisionModelHandle,

	// ���ׂ�}�e���A���̔ԍ�
	int MaterialIndex
)
{
	const char   *MaterialName;
	EMaterialType MaterialType;

	// �}�e���A�������擾
	MaterialName = MV1GetMaterialName(CollisionModelHandle, MaterialIndex);

	// �P�����ڂ��wP�x�̏ꍇ�͂Q�����ڂƂR�����ڂ��}�e���A���^�C�v�Ƃ��ĕϊ�����
	if (MaterialName[0] == 'P')
	{
		MaterialType =
			(EMaterialType)((MaterialName[1] - '0') * 10
				+ (MaterialName[2] - '0'));
	}
	else
	{
		// ����ȊO�̏ꍇ�̓}�e���A���^�C�v��΂ɂ��Ă���
		MaterialType = EMaterialType_Stone;
	}

	return MaterialType;
}

// �I�u�W�F�N�g�̂R�c���f����ǂݍ���
//     �߂�l : �ǂݍ��݂������������ǂ���( true:��������  false:���s���� )
static bool StageData_ObjectModel_Load(
	// �������I�u�W�F�N�g�̂R�c���f���̔ԍ�
	int Index
)
{
	SStageObjectModelInfo *OMInfo;
	int                    MaterialNum;
	int                    i;
	char                   FilePath[MAX_PATH];

	OMInfo = &g_StageData.ObjectModelInfo[Index];

	// �ǂݍ��܂�Ă��邩�ǂ����̃t���O�������Ă����牽�������ɏI��
	if (OMInfo->Load)
	{
		return true;
	}

	// �R�c���f���̃t�@�C���p�X���쐬
	sprintf(FilePath, "Data\\Stage\\Stage_Obj%03d.mv1", Index);

	// �R�c���f�������݂��Ȃ������玸�s
	if (FileRead_size(FilePath) <= 0)
	{
		return false;
	}

	// �R�c���f���̓ǂݍ���
	OMInfo->ModelHandle = MV1LoadModel(FilePath);

	// �t�@�C�������������玸�s�I��
	if (OMInfo->ModelHandle < 0)
	{
		return false;
	}

	// �R���W�����p�̂R�c���f���̓ǂݍ���
	sprintf(FilePath, "Data\\Stage\\Stage_Obj%03d_c.mv1", Index);
	if (FileRead_size(FilePath) > 0)
	{
		OMInfo->CollisionModelHandle = MV1LoadModel(FilePath);

		// �R���W�����p�R�c���f���̃}�e���A���^�C�v�𒲂ׂ�
		if (OMInfo->CollisionModelHandle != -1)
		{
			MaterialNum = MV1GetMaterialNum(OMInfo->CollisionModelHandle);
			if (MaterialNum >= STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM)
			{
				MaterialNum = STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM;
			}
			for (i = 0; i < MaterialNum; i++)
			{
				// �}�e���A���^�C�v���擾
				OMInfo->ColModelMatType[i] =
					StageData_GetCollisionModelMaterialType(
						OMInfo->CollisionModelHandle, i);
			}
		}
	}
	else
	{
		// �R���W�����p�̂R�c���f���������ꍇ�� -1 �������Ă���
		OMInfo->CollisionModelHandle = -1;
	}

	// �ǂݍ��܂�Ă��邩�ǂ����̃t���O�𗧂Ă�
	OMInfo->Load = true;

	// ����I��
	return true;
}

// �I�u�W�F�N�g�̂R�c���f�����������
static void StageData_ObjectModel_Release(
	// �������I�u�W�F�N�g�̂R�c���f���̔ԍ�
	int Index
)
{
	SStageObjectModelInfo *OMInfo;

	OMInfo = &g_StageData.ObjectModelInfo[Index];

	// �ǂݍ��܂�Ă��邩�ǂ����̃t���O���|��Ă����牽�������ɏI��
	if (!OMInfo->Load)
	{
		return;
	}

	// �R�c���f���n���h���̍폜
	MV1DeleteModel(OMInfo->ModelHandle);
	OMInfo->ModelHandle = -1;

	// �R���W�����p�R�c���f��������ꍇ�͂�����폜
	if (OMInfo->CollisionModelHandle != -1)
	{
		MV1DeleteModel(OMInfo->CollisionModelHandle);
		OMInfo->CollisionModelHandle = -1;
	}

	// �ǂݍ��܂�Ă��邩�ǂ����̃t���O��|��
	OMInfo->Load = false;
}

// �X�e�[�W�f�[�^��ǂݍ���
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool StageData_Load(
	// �ǂݍ��ރX�e�[�W�̔ԍ�
	int StageNumber
)
{
	char                    FilePath[MAX_PATH];
	SStageCharaInfo        *CInfo;
	SStageObjectInfo       *OInfo;
	SStageObjectModelInfo  *OMInfo;
	SStageEventInfo        *EInfo;
	int                     i;
	int                     j;
	int                     ParamInt;
	SBinaryFileData         BinFileData;
	SStageInfoFileHeader   *Header;
	int                     MaterialNum;
	bool                    LoadSuccess;
	bool                    UseObject[STAGE_OBJECT_MODEL_MAX_NUM];

	// ���ɃX�e�[�W���ǂݍ��܂�Ă����ꍇ�̂��߂ɉ���������s��
	StageData_Unload();

	// �ǂݍ��݂����������t���O��|���Ă���
	LoadSuccess = false;

	// �w�b�_�̃A�h���X����
	Header = &g_StageData.Header;

	//20��Ɉ�x�X�e�[�W�f�[�^2�A����ȊO��1��ǂݍ���
	int stageNumberBuffer = (StageNumber % 19 == 0 && StageNumber != 0 ? 1 : 0);

	// �X�e�[�W�f�[�^�̃o�C�i���t�@�C�����J��
	if (ReadBinFile_Open(&BinFileData, "Data\\Stage\\Stage%02d.dat", stageNumberBuffer))
	{
		// �ŏ��̂S������ �wMAPD�x ���ǂ������`�F�b�N
		ReadBinFile_Data(&BinFileData, &Header->MagicID, 4);
		if (strncmp(Header->MagicID, STAGEFILE_ID, 4) == 0)
		{
			// �X�e�[�W�f�[�^�t�@�C���̃w�b�_��ǂݍ���
			ReadBinFile_Int(&BinFileData, &Header->CharaInfoNum);
			ReadBinFile_Int(&BinFileData, &Header->ObjectInfoNum);
			ReadBinFile_Int(&BinFileData, &Header->EventInfoNum);
			ReadBinFile_Vector(&BinFileData, &Header->PlayerPosition);
			ReadBinFile_Float(&BinFileData, &Header->PlayerAngle);
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Header->RenderType = (EStageRenderType)ParamInt;
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Header->StageClearCondition = (EStageClearCondition)ParamInt;
			ReadBinFile_Int(&BinFileData, &Header->KillCharaNum);
			ReadBinFile_Int(&BinFileData, &Header->KillTargetChara);

#if true
#ifdef __MY_DEBUG__

			Header->KillCharaNum = 0;

#endif
#endif

			// �L�����̔z�u����ǂݍ���
			CInfo = g_StageData.CharaInfo;
			for (i = 0; i < Header->CharaInfoNum; i++, CInfo++)
			{
				ReadBinFile_Int(&BinFileData, &ParamInt);
				CInfo->Chara = (EChara)ParamInt;
				ReadBinFile_Vector(&BinFileData, &CInfo->Position);
				ReadBinFile_Float(&BinFileData, &CInfo->Angle);
			}

			// �I�u�W�F�N�g�̔z�u����ǂݍ���
			OInfo = g_StageData.ObjectInfo;
			for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
			{
				ReadBinFile_Int(&BinFileData, &OInfo->ModelID);
				ReadBinFile_Vector(&BinFileData, &OInfo->Position);
				ReadBinFile_Vector(&BinFileData, &OInfo->Angle);
				ReadBinFile_Vector(&BinFileData, &OInfo->Scale);
			}

			// �C�x���g�̏���ǂݍ���
			EInfo = g_StageData.EventInfo;
			for (i = 0; i < Header->EventInfoNum; i++, EInfo++)
			{
				ReadBinFile_Int(&BinFileData, &ParamInt);
				EInfo->EventType = (EStageEventType)ParamInt;
				ReadBinFile_Vector(&BinFileData, &EInfo->Position);
				ReadBinFile_Vector(&BinFileData, &EInfo->AreaSize);
				ReadBinFile_Int(&BinFileData, &ParamInt);
				EInfo->Bgm = (EBGM)ParamInt;

				ReadBinFile_Int(&BinFileData, &EInfo->ObjectNum);
				for (j = 0; j < EInfo->ObjectNum; j++)
				{
					ReadBinFile_Int(&BinFileData, &EInfo->Object[j]);
				}
			}

			// �ǂݍ��݂����������t���O�𗧂Ă�
			LoadSuccess = true;
		}

		// �o�C�i���t�@�C�������
		ReadBinFile_Close(&BinFileData);
	}

	// ���[�h�Ɏ��s������f�t�H���g�����Z�b�g
	if (!LoadSuccess)
	{
		Header->CharaInfoNum = 0;
		Header->ObjectInfoNum = 0;
		Header->EventInfoNum = 0;
		Header->PlayerPosition = VGet(0.0f, 0.0f, 0.0f);
		Header->PlayerAngle = 0.0f;
		Header->RenderType = (EStageRenderType)0;
		Header->StageClearCondition = EStageClearCondition_CharaKillNum;
		Header->KillCharaNum = 0;
		Header->KillTargetChara = 0;
	}

	// �I�u�W�F�N�g�̂R�c���f������ɑS�ēǂݍ��ގw��ɂȂ��Ă��Ȃ��ꍇ��
	// �����ŕK�v�ȃI�u�W�F�N�g�̂R�c���f����ǂݍ���
	if (!g_StageData.ObjectModelAlwaysLoad)
	{
		// �g�p����Ă���I�u�W�F�N�g�̂R�c���f����񋓂���
		memset(UseObject, 0, sizeof(UseObject));
		OInfo = g_StageData.ObjectInfo;
		for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
		{
			UseObject[OInfo->ModelID] = true;
		}

		// �g�p����Ă��Ȃ��I�u�W�F�N�g�̂R�c���f����������āA
		// �g�p����Ă���I�u�W�F�N�g�̂R�c���f����ǂݍ���
		for (i = 0; i < STAGE_OBJECT_MODEL_MAX_NUM; i++)
		{
			// �g�p����Ă��邩�ǂ����ŏ����𕪊�
			if (UseObject[i])
			{
				// �g�p����Ă���ꍇ�͓ǂݍ���
				if (!StageData_ObjectModel_Load(i))
				{
					return false;
				}
			}
			else
			{
				// �g�p����Ă��Ȃ��ꍇ�͉��
				StageData_ObjectModel_Release(i);
			}
		}
	}

	// �I�u�W�F�N�g�z�u�p�ɃI�u�W�F�N�g�p�R�c���f��������s�p�̔h���R�c���f�����쐬����
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
	{
		OMInfo = &g_StageData.ObjectModelInfo[OInfo->ModelID];

		// ���s���̕`��p�ɔh���R�c���f���n���h�����쐬����
		OInfo->ModelHandle = MV1DuplicateModel(OMInfo->ModelHandle);
		if (OInfo->ModelHandle == -1)
		{
			return false;
		}

		// �R���W�����p�R�c���f��������ꍇ�͔h���n���h�����쐬����
		if (OMInfo->CollisionModelHandle != -1)
		{
			OInfo->CollisionModelHandle =
				MV1DuplicateModel(OMInfo->CollisionModelHandle);
			if (OInfo->CollisionModelHandle == -1)
			{
				return false;
			}
		}
		else
		{
			OInfo->CollisionModelHandle = -1;
		}

		// �ʒu�ƌ����ƃX�P�[����ݒ�
		MV1SetPosition(OInfo->ModelHandle, OInfo->Position);
		MV1SetRotationXYZ(OInfo->ModelHandle, OInfo->Angle);
		MV1SetScale(OInfo->ModelHandle, OInfo->Scale);
		if (OInfo->CollisionModelHandle != -1)
		{
			MV1SetPosition(OInfo->CollisionModelHandle, OInfo->Position);
			MV1SetRotationXYZ(OInfo->CollisionModelHandle, OInfo->Angle);
			MV1SetScale(OInfo->CollisionModelHandle, OInfo->Scale);
		}

		// �����x���T�O���ȏ�̕����͕`�悵�Ȃ��ݒ�ɂ���
		MV1SetMaterialDrawAlphaTestAll(OInfo->ModelHandle, TRUE,
			DX_CMP_GREATER, 128);
	}

	// �X�e�[�W�̂R�c���f����ǂݍ���
	sprintf(FilePath, "Data\\Stage\\Stage%02d.mv1", stageNumberBuffer);
	g_StageData.ModelHandle = MV1LoadModel(FilePath);
	if (g_StageData.ModelHandle == -1)
	{
		return false;
	}

	// �X�e�[�W�̃R���W�����p�R�c���f����ǂݍ���
	sprintf(FilePath, "Data\\Stage\\Stage%02d_c.mv1", stageNumberBuffer);
	g_StageData.CollisionModelHandle = MV1LoadModel(FilePath);
	if (g_StageData.CollisionModelHandle == -1)
	{
		return false;
	}

	// �X�e�[�W�̋�\���p�R�c���f����ǂݍ���
	sprintf(FilePath, "Data\\Stage\\Stage%02d_sky.mv1", stageNumberBuffer);
	g_StageData.SkyModelHandle = MV1LoadModel(FilePath);

	// �R���W�����p�R�c���f���̃}�e���A���^�C�v�𒲂ׂ�
	MaterialNum = MV1GetMaterialNum(g_StageData.CollisionModelHandle);
	if (MaterialNum >= STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM)
	{
		MaterialNum = STAGE_COLLISION_MODEL_MATERIAL_MAX_NUM;
	}
	for (i = 0; i < MaterialNum; i++)
	{
		g_StageData.ColModelMatType[i] =
			StageData_GetCollisionModelMaterialType(
				g_StageData.CollisionModelHandle, i);
	}

	// �ǂݍ��ނ񂾃X�e�[�W�ԍ���ۑ�
	g_StageData.LoadStageNumber = StageNumber;

	// ����I��
	return true;
}

// �X�e�[�W�f�[�^���������
void StageData_Unload(void)
{
	SStageObjectInfo *OInfo;
	int               i;

	// �X�e�[�W���ǂݍ��܂�Ă��Ȃ��ꍇ�͉��������ɏI��
	if (g_StageData.LoadStageNumber < 0)
	{
		return;
	}

	// �X�e�[�W�̂R�c���f���n���h�����폜
	MV1DeleteModel(g_StageData.ModelHandle);
	g_StageData.ModelHandle = -1;

	// �X�e�[�W�̃R���W�����p�R�c���f���n���h�����폜
	MV1DeleteModel(g_StageData.CollisionModelHandle);
	g_StageData.CollisionModelHandle = -1;

	// �X�e�[�W�̋�p�R�c���f���n���h�����폜
	MV1DeleteModel(g_StageData.SkyModelHandle);
	g_StageData.SkyModelHandle = -1;

	// �I�u�W�F�N�g�p�̂R�c���f���n���h������쐬�������s���p�R�c���f���n���h�����폜
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		MV1DeleteModel(OInfo->ModelHandle);
		OInfo->ModelHandle = -1;

		if (OInfo->CollisionModelHandle != -1)
		{
			MV1DeleteModel(OInfo->CollisionModelHandle);
			OInfo->CollisionModelHandle = -1;
		}
	}

	// �w���ݓǂݍ��܂�Ă���X�e�[�W�ԍ��x���w�����ǂݍ��܂�Ă��Ȃ��x�ɃZ�b�g
	g_StageData.LoadStageNumber = -1;
}

// �X�e�[�W�f�[�^��ۑ�����
//     �߂�l : �X�e�[�W�f�[�^�̕ۑ��ɐ����������ǂ���
//              ( true:��������  false:���s���� )
bool StageData_Save(void)
{
	SBinaryFileData        BinFileData;
	SStageCharaInfo       *CInfo;
	SStageObjectInfo      *OInfo;
	SStageEventInfo       *EInfo;
	char                   FilePath[MAX_PATH];
	int                    i;
	int                    j;
	SStageInfoFileHeader  *Header;

	// �X�e�[�W���ǂݍ��܂�Ă��Ȃ��ꍇ�͉��������ɏI��
	if (g_StageData.LoadStageNumber < 0)
	{
		return false;
	}

	// �w�b�_�̃A�h���X����
	Header = &g_StageData.Header;

	// �X�e�[�W�f�[�^�̃o�C�i���t�@�C�����o��

	// �o�͗p�̃o�C�i���t�@�C�����J��
	sprintf(FilePath, "Data\\Stage\\Stage%02d.dat", g_StageData.LoadStageNumber);
	if (!WriteBinFile_Open(&BinFileData, FilePath))
	{
		MessageBox(NULL, "�Z�[�u�Ɏ��s���܂����B", "�Z�[�u���s", MB_OK);
		return false;
	}

	// �X�e�[�W�f�[�^�̃w�b�_�������o��
	strncpy(g_StageData.Header.MagicID, STAGEFILE_ID, 4);
	WriteBinFile_Data(&BinFileData, Header->MagicID, 4);

	WriteBinFile_Int(&BinFileData, Header->CharaInfoNum);
	WriteBinFile_Int(&BinFileData, Header->ObjectInfoNum);
	WriteBinFile_Int(&BinFileData, Header->EventInfoNum);
	WriteBinFile_Vector(&BinFileData, Header->PlayerPosition);
	WriteBinFile_Float(&BinFileData, Header->PlayerAngle);
	WriteBinFile_Int(&BinFileData, Header->RenderType);
	WriteBinFile_Int(&BinFileData, Header->StageClearCondition);
	WriteBinFile_Int(&BinFileData, Header->KillCharaNum);
	WriteBinFile_Int(&BinFileData, Header->KillTargetChara);

	// �L�����̔z�u���������o��
	CInfo = g_StageData.CharaInfo;
	for (i = 0; i < Header->CharaInfoNum; i++, CInfo++)
	{
		WriteBinFile_Int(&BinFileData, CInfo->Chara);
		WriteBinFile_Vector(&BinFileData, CInfo->Position);
		WriteBinFile_Float(&BinFileData, CInfo->Angle);
	}

	// �I�u�W�F�N�g�̔z�u���������o��
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < Header->ObjectInfoNum; i++, OInfo++)
	{
		WriteBinFile_Int(&BinFileData, OInfo->ModelID);
		WriteBinFile_Vector(&BinFileData, OInfo->Position);
		WriteBinFile_Vector(&BinFileData, OInfo->Angle);
		WriteBinFile_Vector(&BinFileData, OInfo->Scale);
	}

	// �C�x���g�̏��������o��
	EInfo = g_StageData.EventInfo;
	for (i = 0; i < Header->EventInfoNum; i++, EInfo++)
	{
		WriteBinFile_Int(&BinFileData, EInfo->EventType);
		WriteBinFile_Vector(&BinFileData, EInfo->Position);
		WriteBinFile_Vector(&BinFileData, EInfo->AreaSize);
		WriteBinFile_Int(&BinFileData, EInfo->Bgm);

		WriteBinFile_Int(&BinFileData, EInfo->ObjectNum);
		for (j = 0; j < EInfo->ObjectNum; j++)
		{
			WriteBinFile_Int(&BinFileData, EInfo->Object[j]);
		}
	}

	// �t�@�C�������
	WriteBinFile_Close(&BinFileData);

	// �����I��
	return true;
}

// �X�e�[�W�̂R�c���f���n���h�����擾����
//     �߂�l : �X�e�[�W�̂R�c���f���n���h��
int StageData_GetModelHandle(void)
{
	return g_StageData.ModelHandle;
}

// �X�e�[�W�̃R���W�����p�R�c���f���n���h�����擾����
//     �߂�l : �X�e�[�W�̃R���W�����p�R�c���f���n���h��
int StageData_GetCollisionModelHandle(void)
{
	return g_StageData.CollisionModelHandle;
}

// �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̐����擾����
//     �߂�l : �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̐�
int StageData_GetObjectNum(void)
{
	return g_StageData.Header.ObjectInfoNum;
}

// �I�u�W�F�N�g���g�p����R���W�����p�R�c���f���n���h�����擾����
//     �߂�l : �R���W�����p�R�c���f���n���h��
int StageData_GetObjectCollisionModelHandle(
	// �I�u�W�F�N�g���g�p����R�c���f���̔ԍ�
	int Index
)
{
	return g_StageData.ObjectInfo[Index].CollisionModelHandle;
}


// �I�u�W�F�N�g�̃R���W�����p�R�c���f���̎w��ԍ��̃}�e���A���^�C�v���擾����
EMaterialType StageData_GetObjectCollisionModelMaterialType(
	// �I�u�W�F�N�g���g�p����R�c���f���̔ԍ�
	int Index,

	// �R���W�����p�R�c���f���̃}�e���A���ԍ�
	int MaterialIndex
)
{
	SStageObjectInfo      *OInfo;
	SStageObjectModelInfo *OMInfo;

	OInfo = &g_StageData.ObjectInfo[Index];
	OMInfo = &g_StageData.ObjectModelInfo[OInfo->ModelID];

	return OMInfo->ColModelMatType[MaterialIndex];
}

// �S�X�e�[�W�����擾����
//     �߂�l : �S�X�e�[�W��
int StageData_GetTotalStageNum(void)
{
	return g_StageData.TotalStageNum;
}

// �ǂݍ���ł���X�e�[�W�̔ԍ����擾����
//     �߂�l : �X�e�[�W�ԍ�
int StageData_GetLoadStageNo(void)
{
	return g_StageData.LoadStageNumber;
}

// �X�e�[�W�̕`��^�C�v���擾����
//     �߂�l : �`��^�C�v
EStageRenderType StageData_GetRenderType(void)
{
	return g_StageData.Header.RenderType;
}

// �X�e�[�W�̃N���A�������擾����
//     �߂�l : �X�e�[�W�̃N���A����
EStageClearCondition StageData_GetStageClearCondition(void)
{
	return g_StageData.Header.StageClearCondition;
}

// �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̐����擾����
//     �߂�l :  �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̐�
int StageData_GetKillCharaNum(void)
{
	return g_StageData.Header.KillCharaNum;
}

// �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̔ԍ����擾����
//     �߂�l : �X�e�[�W���N���A���邽�߂ɓ|���Ȃ���΂Ȃ�Ȃ��G�̔ԍ�
int StageData_GetKillTargetChara(void)
{
	return g_StageData.Header.KillTargetChara;
}

// �X�e�[�W�̃R���W�����p�R�c���f���̎w��ԍ��̃}�e���A���^�C�v���擾����
//     �߂�l : �}�e���A���^�C�v
extern EMaterialType StageData_GetCollisionModelMaterialType(
	// �R���W�����p�R�c���f���̃}�e���A���ԍ�
	int MaterialIndex
)
{
	return g_StageData.ColModelMatType[MaterialIndex];
}
