#include "Stage.h"
#include "StageData.h"
#include "Camera.h"
#include "Chara.h"
#include "GameMainRender.h"
#include "TextParam.h"
#include "System.h"
#include <stdlib.h>
#include <malloc.h>

s_Stage STAGE;
SStageInfo g_StageInfo;
//�f�[�^�ǂݍ��ݗp
char path[256];

// �X�e�[�W�����p�̐����p�����[�^�e�[�u��
static SParamInt g_Stage_ParamInt[EStgParInt_Num] =
{
	{ 0, "DirLight_Diff_R" },
	{ 0, "DirLight_Diff_G" },
	{ 0, "DirLight_Diff_B" },
	{ 0, "DirLight_Amb_R" },
	{ 0, "DirLight_Amb_G" },
	{ 0, "DirLight_Amb_B" },
	{ 0, "PointLight_Atten0" },
	{ 0, "PointLight_Atten1" },
	{ 0, "PointLight_Atten2" },
	{ 0, "PointLight_Diff_R" },
	{ 0, "PointLight_Diff_G" },
	{ 0, "PointLight_Diff_B" },
	{ 0, "PointLight_Amb_R" },
	{ 0, "PointLight_Amb_G" },
	{ 0, "PointLight_Amb_B" },
	{ 0, "PointLight_Pos_X" },
	{ 0, "PointLight_Pos_Y" },
	{ 0, "PointLight_Pos_Z" },
};

// �X�e�[�W�����̏�����������
//     �߂�l : �������ɐ����������ǂ���
//              ( true:����   false:���s )
bool Stage_Initialize(void)
{
	int i;

	// �X�e�[�W�f�[�^�����̏��������s��
	if (!StageData_Initialize(false))
	{
		return false;
	}

	// ���C�g�Ɋւ���p�����[�^���e�L�X�g�t�@�C������ǂݍ���
	for (i = 0; i < EStgParInt_Num; i++)
	{
		g_Stage_ParamInt[i].Param =
			GetTextParamInt(&g_StageData.TextParam, g_Stage_ParamInt[i].Name);
	}

	// �����I��
	return true;
}

// �X�e�[�W�����̌�n��������
void Stage_Terminate(void)
{
	// �X�e�[�W�f�[�^�����̌�n�����s��
	StageData_Terminate();
}

// �X�e�[�W���Z�b�g�A�b�v����
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Stage_Setup(
	// �Z�b�g�A�b�v����X�e�[�W�̔ԍ�
	int StageNumber
)
{
	// �C�x���g�����p�̃t���O������������
	memset(g_StageInfo.EventRunFlag, 0, sizeof(g_StageInfo.EventRunFlag));
	memset(g_StageInfo.EventHitEdgeFlag, 0, sizeof(g_StageInfo.EventHitEdgeFlag));
	memset(g_StageInfo.EventHitFlag, 0, sizeof(g_StageInfo.EventHitFlag));

	// �L������S���폜����
	Chara_AllDelete();

	// �X�e�[�W�f�[�^��ǂݍ���
	if (!StageData_Load(StageNumber))
	{
		return false;
	}

	// �v���C���[�L�������X�e�[�W�J�n�ʒu�ɍ쐬����
	if (!Chara_Create(
		EChara_Player,
		g_StageData.Header.PlayerPosition,
		g_StageData.Header.PlayerAngle,
		-1
	))
	{
		return false;
	}

	// �`��̏������s��
	GameMainRender_StageSetup();

	// ����I��
	return true;
}

// �X�e�[�W�̏�Ԑ��ڏ������s��
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool Stage_Step(
	// ���ڂ����鎞��( �P�ʁF�b )
	float StepTime
)
{
	int                   i;
	int                   j;
	SStageCharaInfo      *AInfo;
	SStageEventInfo      *EInfo;
	SCharaInfo           *PCInfo;
	VECTOR                Distance;
	bool                  EventHitFlagOld;

	// �v���C���[�̏����擾����
	PCInfo = System_GetPlayerCharaInfo();

	// �v���C���[�����݂���ꍇ�̓C�x���g�̏������s��
	if (PCInfo != NULL)
	{
		// �C�x���g�̐������J��Ԃ�
		EInfo = g_StageData.EventInfo;
		for (i = 0; i < g_StageData.Header.EventInfoNum; i++, EInfo++)
		{
			EventHitFlagOld = g_StageInfo.EventHitFlag[i];
			g_StageInfo.EventHitFlag[i] = false;

			// �v���C���[���C�x���g�̃{�����[���ɐG��Ă��邩�ǂ������`�F�b�N����
			Distance = VSub(PCInfo->Position, EInfo->Position);
			if (Distance.x < 0.0f) Distance.x = -Distance.x;
			if (Distance.y < 0.0f) Distance.y = -Distance.y;
			if (Distance.z < 0.0f) Distance.z = -Distance.z;

			if (Distance.x <= EInfo->AreaSize.x / 2.0f &&
				Distance.y <= EInfo->AreaSize.y / 2.0f &&
				Distance.z <= EInfo->AreaSize.z / 2.0f)
			{
				g_StageInfo.EventHitFlag[i] = true;
				g_StageInfo.EventHitEdgeFlag[i] = !EventHitFlagOld &&
					g_StageInfo.EventHitFlag[i];

				// �G��Ă����ꍇ�́A�C�x���g�̃^�C�v�ɉ����ď������s��
				switch (EInfo->EventType)
				{
				case EStageEventType_CharaAppear:			// �L�������o��������

															// ��x�ł��C�x���g�����s���Ă���ꍇ�͉��������ɏI��
					if (g_StageInfo.EventRunFlag[i])
					{
						continue;
					}

					// �C�x���g�Ɋ֘A�t�����Ă���L�������o��������
					for (j = 0; j < EInfo->ObjectNum; j++)
					{
						AInfo = &g_StageData.CharaInfo[EInfo->Object[j]];

						if (!Chara_Create(AInfo->Chara, AInfo->Position,
							AInfo->Angle, EInfo->Object[j]))
						{
							return false;
						}
					}
					break;

				case EStageEventType_BGM:		// �a�f�l��ύX����
												// �a�f�l�ύX����x�ł����s���Ă���ꍇ�͉��������ɏI��
					if (g_StageInfo.EventRunFlag[i])
					{
						continue;
					}

					// �C�x���g�ɐݒ肳��Ă���a�f�l�̍Đ����J�n����
					Sound_PlayBGM(EInfo->Bgm);
					break;
				}

				// �C�x���g�����s�������ǂ����̃t���O�𗧂Ă�
				g_StageInfo.EventRunFlag[i] = true;
			}
			else
			{
				// �C�x���g�ɐG��Ă��Ȃ������ꍇ�́A�G�ꂽ�u�Ԃ��ǂ����̃t���O��|��
				g_StageInfo.EventHitEdgeFlag[i] = false;
			}
		}
	}

	// ����I��
	return true;
}

// �X�e�[�W�̃��C�g�̃Z�b�g�A�b�v���s��
void Stage_LightSetup(void)
{
	SCharaInfo *PCInfo;
	COLOR_F     DifColor;
	COLOR_F     AmbColor;
	VECTOR      Position;

	System_SetDebugParamInt(g_Stage_ParamInt, EStgParInt_Num);

	// �`��^�C�v�ɂ���ď����𕪊�
	switch (g_StageData.Header.RenderType)
	{
	case EStageRenderType_Outdoors:	// ���O
									// �f�B���N�V���i�����C�g�̐ݒ���s��
		ChangeLightTypeDir(Stage_GetLightDirection());
		DifColor = GetColorF(
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_R].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_G].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Diff_B].Param * 0.01f,
			1.0f
		);
		AmbColor = GetColorF(
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_R].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_G].Param * 0.01f,
			g_Stage_ParamInt[EStgParInt_DirLight_Amb_B].Param * 0.01f,
			1.0f
		);
		SetLightDifColor(DifColor);
		SetLightAmbColor(AmbColor);
		break;

	case EStageRenderType_Cave:		// ���A
		PCInfo = System_GetPlayerCharaInfo();
		if (PCInfo != NULL)
		{
			// �v���C���[�̋���ʒu�Ƀ|�C���g���C�g��z�u����
			Position =
				VScale(
					PCInfo->AngleInfo.LeftDirection,
					g_Stage_ParamInt[EStgParInt_PointLight_Pos_X].Param * 10.0f
				);

			Position.y =
				g_Stage_ParamInt[EStgParInt_PointLight_Pos_Y].Param * 10.0f;

			Position =
				VAdd(Position, VScale(
					PCInfo->AngleInfo.FrontDirection,
					g_Stage_ParamInt[EStgParInt_PointLight_Pos_Z].Param * 10.0f
				));
			ChangeLightTypePoint(
				VAdd(PCInfo->Position, Position),
				POINTLIGHT_RANGE,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten0].Param * 0.1f,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten1].Param * 0.0001f,
				g_Stage_ParamInt[EStgParInt_PointLight_Atten2].Param * 0.0000001f
			);
			DifColor = GetColorF(
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_R].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_G].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Diff_B].Param * 0.01f,
				1.0f
			);
			AmbColor = GetColorF(
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_R].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_G].Param * 0.01f,
				g_Stage_ParamInt[EStgParInt_PointLight_Amb_B].Param * 0.01f,
				1.0f
			);

			SetLightDifColor(DifColor);
			SetLightAmbColor(AmbColor);
		}
		break;
	}
}

// �X�e�[�W��`�悷��
void Stage_Render(
	// �V���h�E�}�b�v�ւ̕`�悩�ǂ���
	bool ShadowMapDraw
)
{
	int               i;
	SStageObjectInfo *OInfo;
	bool              LowSpecMode;

	// �X�e�[�W�R�c���f����`��
	MV1DrawModel(g_StageData.ModelHandle);

	// �Ꮘ�����׃��[�h���ǂ������擾���Ă���
	LowSpecMode = System_GetLowSpecMode();

	// �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̂R�c���f����`��
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		// �Ꮘ�����׃��[�h�̏ꍇ�̓R���W�����p�R�c���f���̖����I�u�W�F�N�g�͕`�悵�Ȃ�
		if (LowSpecMode && OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		// �R�c���f����`��
		MV1DrawModel(OInfo->ModelHandle);
	}
}

// �X�e�[�W�̃R���W�����p�R�c���f����`�悷��
void Stage_CollisionRender(void)
{
	int               i;
	SStageObjectInfo *OInfo;

	// �X�e�[�W�̃R���W�����p�R�c���f�������C���[�t���[���`��
	MV1DrawModelDebug(
		g_StageData.CollisionModelHandle,
		GetColor(0, 255, 0), FALSE, 1.0f, TRUE, FALSE);

	// �X�e�[�W�ɔz�u����Ă���I�u�W�F�N�g�̃R���W�����p�R�c���f�������C���[�t���[���`��
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		if (OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		MV1DrawModelDebug(
			OInfo->CollisionModelHandle,
			GetColor(0, 255, 0), FALSE, 1.0f, TRUE, FALSE);
	}
}

// �X�e�[�W�̋�p�R�c���f����`�悷��
void Stage_SkyRender(void)
{
	SCharaInfo *PCInfo;

	// �v���C���[������ꍇ�̓v���C���[�̍��W�ɋ�p�R�c���f���̈ʒu�����킹��
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo != NULL)
	{
		MV1SetPosition(g_StageData.SkyModelHandle, PCInfo->Position);
	}

	// ��p�R�c���f���`��p�̃J�����ݒ���s��
	SetCameraNearFar(SKYMODEL_CAMERA_NEAR_Z, SKYMODEL_CAMERA_FAR_Z);

	// ��p�R�c���f���̕`��ɂy�o�b�t�@�͎g�p���Ȃ�
	MV1SetUseZBuffer(g_StageData.SkyModelHandle, FALSE);

	// ��p�R�c���f����`�悷��
	MV1DrawModel(g_StageData.SkyModelHandle);

	// �ύX�����J�����ݒ�����ɖ߂�
	SetCameraNearFar(CAMERA_NEAR_Z, CAMERA_FAR_Z);
}

// �X�e�[�W�̃f�B���N�V���i�����C�g�̕������擾����
//     �߂�l : �f�B���N�V���i�����C�g�̕���
VECTOR Stage_GetLightDirection(void)
{
	return VGet(DIRLIGHT_DIRECTION_X, DIRLIGHT_DIRECTION_Y, DIRLIGHT_DIRECTION_Z);
}

// �w��̃J�v�Z���`�󂪃X�e�[�W�̃R���W�����p�R�c���f���̃|���S����
// �X�e�[�W�I�u�W�F�N�g�̃R���W�����p�R�c���f���̃|���S���ɓ����邩�ǂ������`�F�b�N����
//     �߂�l : �R���W�����p�|���S���ɓ����邩�ǂ���
//              ( true : ������   false : ������Ȃ� )
bool Stage_HitCheck(
	// �J�v�Z�����`��������W�P
	VECTOR Pos1,

	// �J�v�Z�����`��������W�Q
	VECTOR Pos2,

	// �J�v�Z���̕�
	float r
)
{
	MV1_COLL_RESULT_POLY_DIM  HitDim;
	int                       HitNum;
	int                       i;
	SStageObjectInfo         *OInfo;

	// �X�e�[�W�p�R���W�����R�c���f���ɓ����邩���`�F�b�N
	HitDim = MV1CollCheck_Capsule(g_StageData.CollisionModelHandle, -1, Pos1, Pos2, r);
	HitNum = HitDim.HitNum;
	MV1CollResultPolyDimTerminate(HitDim);

	// �������Ă����炱���ŏI��
	if (HitNum > 0)
	{
		return true;
	}

	// �X�e�[�W�̃R���W�����p�|���S���ɓ������Ă��Ȃ�������
	// �X�e�[�W�I�u�W�F�N�g�̃R���W�����p�|���S���ɂ��������Ă��Ȃ����`�F�b�N
	OInfo = g_StageData.ObjectInfo;
	for (i = 0; i < g_StageData.Header.ObjectInfoNum; i++, OInfo++)
	{
		if (OInfo->CollisionModelHandle == -1)
		{
			continue;
		}

		HitDim = MV1CollCheck_Capsule(OInfo->CollisionModelHandle, -1, Pos1, Pos2, r);
		HitNum = HitDim.HitNum;
		MV1CollResultPolyDimTerminate(HitDim);

		// �������Ă������_�ŏI��
		if (HitNum > 0)
		{
			return true;
		}
	}

	// �����ɗ����瓖�����Ă��Ȃ��Ƃ�������
	return false;
}
//�X�e�[�W�N���X�̏���������
void StageInitialize(){
	
	
	/*�K�v
	���X�e�[�W�̏ڍדǂݍ���

	�E�C�x���g�����p�̃t���O
	�E�L�����N�^�[�̑S�폜�֐�
	�E�v���C���[�L�����̃X�e�[�W���ǂ��ɐݒ肷�邩�̏��
	�E
	
	*/




	//�X�e�[�W�Ŏg�����f���̓ǂݍ���
	STAGE.handle = MV1LoadModel("Data\\Stage\\Stage00.mv1");
	STAGE.c_handle = MV1LoadModel("Data\\Stage\\Stage00_c.mv1");
	STAGE.sky_handle = MV1LoadModel("Data\\Stage\\Stage00_sky.mv1");
	
	for(int i = 0;i < OBJECT_NUM;i++){

		sprintf(path,"Stage\\Stage_Obj%03d.mv1",i);
		STAGE.obj[i] = MV1LoadModel(path);

		sprintf(path,"Stage\\Stage_Obj%03d_c.mv1",i);
		STAGE.c_obj[i] = MV1LoadModel(path);
	}

	//�V���h�E�}�b�v�̍쐬
	STAGE.smap = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(STAGE.smap, VGet(1,-1,0));				//�J�����̃��C�g�ƍ��킹��Ƃ��Ƀ��C�e�B���O�̃x�N�g���������ɂ�������OK
	SetShadowMapDrawArea(STAGE.smap, SHADOWMAP_INIT1, SHADOWMAP_INIT2);

	//�ʒu�ݒ�
	MV1SetPosition(STAGE.handle, STAGE_INIT);



}

//�X�e�[�W�N���X�̍X�V����
void StageUpdata(){

	



}

//�X�e�[�W�N���X�̕`�揈��
void StageDraw(){

	//�V���h�E�}�b�v�ɕ`������
	ShadowMap_DrawSetup(STAGE.smap);
	MV1DrawModel(STAGE.handle);

	for(int i = 0;i < 100;i++){
	MV1DrawModel(STAGE.d_obj[i]);
	}

	ShadowMap_DrawEnd();
	SetUseShadowMap(0, STAGE.smap);

	//���f���`��
	MV1DrawModel(STAGE.handle);

	for (int i = 0; i < 100; i++) {
		MV1DrawModel(STAGE.d_obj[i]);
	}

	SetUseShadowMap(0,-1);

}

//�X�e�[�W�N���X�㏈��
void StageFainalize(){

}