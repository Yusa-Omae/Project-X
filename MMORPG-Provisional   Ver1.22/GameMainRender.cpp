#include "GameMainRender.h"
#include "Stage.h"
#include "StageData.h"
#include "Chara.h"
#include "Effect.h"
#include "Camera.h"
#include "System.h"
#include "DxLib.h"

// �V���h�E�}�b�v�̉𑜓x
#define SHADOWMAP_RESOLUTION		(2048)

// �Q�[�����C���̕`�揈���p�̐����l�p�����[�^
typedef enum _EGameMainRender_ParamInt
{
	EGMRParInt_NearShadowDistance,	// �ߋ����p�V���h�E�}�b�v�̋���
	EGMRParInt_NearShadowHeight,	// �ߋ����p�V���h�E�}�b�v�̍���

	EGMRParInt_Num,					// �����p�����[�^�̐�
} EGameMainRender_ParamInt;

// �Q�[�����C���̕`�揈���̏��
typedef struct _SGameMainRenderData
{
	// �v���C���[�̍��W
	VECTOR PlayerPosition;

	// �X�e�[�W�R�c���f���p�̃V���h�E�}�b�v�n���h��
	int    StageModelShadowMapHandle;

	// �������p�̃V���h�E�}�b�v�n���h��
	int    FarShadowMapHandle;

	// �ߋ����p�̃V���h�E�}�b�v�n���h��
	int    NearShadowMapHandle;

	// �A���`�G�C���A�X�����_�����O�p�̕`��Ώۂɂł���O���t�B�b�N�n���h��
	int    AntialiasScreenHandle;
} SGameMainRenderData;

// �Q�[�����C���̕`�揈���p�̐����p�����[�^�e�[�u��
static SParamInt g_GMRParam[EGMRParInt_Num] =
{
	{ 7,  "NearShadowDistance" },
	{ 10, "NearShadowHeight" },
};

// �Q�[�����C���̕`�揈���p�̏��
static SGameMainRenderData g_GMRData;

// �Q�[�����C���̕`�揈��������������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
bool GameMainRender_Initialize(void)
{
	// ���I�ҏW�p�ɐ����p�����[�^��o�^
	// System_SetDebugParamInt( g_GMRParam, EGMRParInt_Num );

	// �Ꮘ�����׃��[�h�ł͖����ꍇ�̓V���h�E�}�b�v��A���`�G�C���A�X��ʂ��쐬����
	if (!System_GetLowSpecMode())
	{
		// �V���h�E�}�b�v�n���h�����쐬����
		g_GMRData.StageModelShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.StageModelShadowMapHandle == -1)
		{
			return false;
		}


		g_GMRData.FarShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.FarShadowMapHandle == -1)
		{
			return false;
		}

		g_GMRData.NearShadowMapHandle =
			MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (g_GMRData.NearShadowMapHandle == -1)
		{
			return false;
		}

		// �A���`�G�C���A�X�����_�����O�p�̕`��Ώۂɂł���O���t�B�b�N�n���h�����쐬����
		SetCreateDrawValidGraphMultiSample(4, 4);
		g_GMRData.AntialiasScreenHandle =
			MakeScreen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, FALSE);
		SetCreateDrawValidGraphMultiSample(0, 0);
	}

	// ����I��
	return true;
}

// �Q�[�����C���̕`�揈���̌�n��������
void GameMainRender_Terminate(void)
{
	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v��A���`�G�C���A�X��ʂ��폜����
	if (!System_GetLowSpecMode())
	{
		// �V���h�E�}�b�v�n���h�����폜����
		DeleteShadowMap(g_GMRData.StageModelShadowMapHandle);
		g_GMRData.StageModelShadowMapHandle = -1;

		DeleteShadowMap(g_GMRData.FarShadowMapHandle);
		g_GMRData.FarShadowMapHandle = -1;

		DeleteShadowMap(g_GMRData.NearShadowMapHandle);
		g_GMRData.NearShadowMapHandle = -1;

		// �A���`�G�C���A�X�����_�����O�p�̕`��Ώۂɂł���O���t�B�b�N�n���h�����폜����
		DeleteGraph(g_GMRData.AntialiasScreenHandle);
	}
}

// �X�e�[�W�J�n���̕`�揀�����s��
void GameMainRender_StageSetup(void)
{
	VECTOR              LightDirection;
	MV1_REF_POLYGONLIST PolyList;
	bool                UseStageShadowMap;

	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�͉������p�V���h�E�}�b�v�ɃX�e�[�W��`�悷��
	if (!System_GetLowSpecMode())
	{
		// �����_�����O���[�h�ɂ���ď����𕪊�
		switch (StageData_GetRenderType())
		{
		case EStageRenderType_Outdoors:		// ���O
											// ���O�̏ꍇ�̓f�B���N�V���i�����C�g�̕������Z�b�g
			LightDirection = Stage_GetLightDirection();

			// �X�e�[�W���f���p�V���h�E�}�b�v���g�p���邩�ǂ����̃t���O�𗧂Ă�
			UseStageShadowMap = true;
			break;

		case EStageRenderType_Cave:			// ���A
											// ���A�̏ꍇ�͐^���������Z�b�g
			LightDirection = VGet(0.0f, -1.0f, 0.0f);

			// �X�e�[�W���f���p�V���h�E�}�b�v���g�p���邩�ǂ����̃t���O��|��
			UseStageShadowMap = false;
			break;
		}

		// �V���h�E�}�b�v�ւ̃����_�����O�Ŏg�p���郉�C�g�̕������Z�b�g
		SetShadowMapLightDirection(g_GMRData.StageModelShadowMapHandle, LightDirection);
		SetShadowMapLightDirection(g_GMRData.FarShadowMapHandle, LightDirection);
		SetShadowMapLightDirection(g_GMRData.NearShadowMapHandle, LightDirection);

		// �X�e�[�W���f���p�Ɖ������p�̃V���h�E�}�b�v�ɂ̓X�e�[�W�S�̂����߂�
		PolyList = MV1GetReferenceMesh(StageData_GetModelHandle(), -1, TRUE);
		SetShadowMapDrawArea(g_GMRData.StageModelShadowMapHandle,
			PolyList.MinPosition, PolyList.MaxPosition);
		SetShadowMapDrawArea(g_GMRData.FarShadowMapHandle,
			PolyList.MinPosition, PolyList.MaxPosition);

		// �X�e�[�W���f���p�̃V���h�E�}�b�v�ɃX�e�[�W�������_�����O
		if (UseStageShadowMap)
		{
			ShadowMap_DrawSetup(g_GMRData.StageModelShadowMapHandle);
			Stage_Render(true);
			ShadowMap_DrawEnd();
		}
	}
}

// �Q�[�����C���̕`�揈�����s��
void GameMainRender_Render(void)
{
	float       NearDistance;
	float       Height;
	VECTOR      MinPos;
	VECTOR      MaxPos;
	SCharaInfo *PCInfo;
	bool        SkyModelRender;
	bool        UseStageShadowMap;

	// �v���C���[�̍��W���X�V
	PCInfo = System_GetPlayerCharaInfo();
	if (PCInfo != NULL)
	{
		g_GMRData.PlayerPosition = PCInfo->Position;
	}

	// �ߋ����p�̃V���h�E�}�b�v�̕`��͈͂��Z�o
	NearDistance = g_GMRParam[EGMRParInt_NearShadowDistance].Param * 100.0f;
	Height = g_GMRParam[EGMRParInt_NearShadowHeight].Param * 100.0f;
	MinPos = VAdd(g_GMRData.PlayerPosition, VGet(-NearDistance, 0.0f, -NearDistance));
	MaxPos = VAdd(g_GMRData.PlayerPosition, VGet(NearDistance, 0.0f, NearDistance));
	MinPos.y = -1.0f;
	MaxPos.y = g_GMRData.PlayerPosition.y + Height;

	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�͕`�����A���`�G�C���A�X�p��ʂɁA
	// �Ꮘ�����׃��[�h�̏ꍇ�͕`���𗠉�ʂɂ���
	if (!System_GetLowSpecMode())
	{
		SetDrawScreen(g_GMRData.AntialiasScreenHandle);

		// �A���`�G�C���A�X�p��ʂ�������
		ClearDrawScreen();
	}
	else
	{
		SetDrawScreen(DX_SCREEN_BACK);
	}

	// �J�����̐ݒ���s��
	Camera_Setup();

	// ���C�g�̃Z�b�g�A�b�v
	Stage_LightSetup();

	// �V���h�E�}�b�v�ւ̃����_�����O�Ŏg�p���郉�C�g�̕������Z�b�g
	switch (StageData_GetRenderType())
	{
	case EStageRenderType_Outdoors:		// ���O
										// ��p�R�c���f����`�悷�邩�ǂ����̃t���O�𗧂Ă�
		SkyModelRender = true;

		// �X�e�[�W���f���p�V���h�E�}�b�v���g�p���邩�ǂ����̃t���O�𗧂Ă�
		UseStageShadowMap = true;
		break;

	case EStageRenderType_Cave:			// ���A
										// ��p�R�c���f����`�悷�邩�ǂ����̃t���O��|��
		SkyModelRender = false;

		// �X�e�[�W���f���p�V���h�E�}�b�v���g�p���邩�ǂ����̃t���O��|��
		UseStageShadowMap = false;
		break;
	}

	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v�ւ̕`����s��
	if (!System_GetLowSpecMode())
	{
		// �������p�̃V���h�E�}�b�v�ɃL�����N�^�[�������_�����O
		ShadowMap_DrawSetup(g_GMRData.FarShadowMapHandle);
		Chara_RenderAll(ECharaRenderMode_Distance_Greater, NearDistance);
		ShadowMap_DrawEnd();

		// �ߋ����p�̃V���h�E�}�b�v�̕`��͈͂��Z�b�g
		SetShadowMapDrawArea(g_GMRData.NearShadowMapHandle, MinPos, MaxPos);

		// �ߋ����p�̃V���h�E�}�b�v�ɃL�����N�^�[�������_�����O
		ShadowMap_DrawSetup(g_GMRData.NearShadowMapHandle);
		Chara_RenderAll(ECharaRenderMode_Distance_Less, NearDistance);
		ShadowMap_DrawEnd();
	}

	// �`��^�C�v�ɂ���ď����𕪊�
	switch (StageData_GetRenderType())
	{
	case EStageRenderType_Outdoors:		// ���O
										// ��p�R�c���f����`��
		Stage_SkyRender();

		// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v���g�p����
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(0, g_GMRData.StageModelShadowMapHandle);
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);
		}

		// �������v�f�̖���������`��
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
		Stage_Render(false);
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		// �������v�f�̂��镔����`��
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
		Stage_Render(false);
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);

		// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v�̎g�p������
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		break;

	case EStageRenderType_Cave:			// ���A
										// �������v�f�̖���������`��
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);

		// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓X�e�[�W�̃����_�����O�ɃV���h�E�}�b�v���g�p����
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);

			Stage_Render(false);

			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		else
		{
			Stage_Render(false);
		}

		// �L�����N�^�[�̕`��ɂ̓V���h�E�}�b�v���g�p���Ȃ�
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		// �������v�f�̂��镔����`��
		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);

		// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓X�e�[�W�̃����_�����O�ɃV���h�E�}�b�v���g�p����
		if (!System_GetLowSpecMode())
		{
			SetUseShadowMap(1, g_GMRData.FarShadowMapHandle);
			SetUseShadowMap(2, g_GMRData.NearShadowMapHandle);

			Stage_Render(false);

			SetUseShadowMap(1, -1);
			SetUseShadowMap(2, -1);
		}
		else
		{
			Stage_Render(false);
		}

		// �L�����N�^�[�̕`��ɂ̓V���h�E�}�b�v���g�p���Ȃ�
		Chara_RenderAll(ECharaRenderMode_Always, 0.0f);

		MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
		break;
	}

	// �Ꮘ�����׃��[�h�̏ꍇ�͊ۉe��`�悷��
	if (System_GetLowSpecMode())
	{
		Chara_ShadowRenderAll(ECharaRenderMode_Always, 0.0f);
	}

	// �G�t�F�N�g��`��
	Effect_RenderAll();

	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�͗���ʂɃA���`�G�C���A�X�p��ʂ�`�悷��
	if (!System_GetLowSpecMode())
	{
		// �`���𗠉�ʂɐݒ�
		SetDrawScreen(DX_SCREEN_BACK);

		// �A���`�G�C���A�X��ʂ𗠉�ʂɕ`��
		DrawGraph(0, 0, g_GMRData.AntialiasScreenHandle, FALSE);

		// SetDrawScreen �����s����ƃJ�����̐ݒ肪���Z�b�g����Ă��܂��̂ŁA
		// �J�����̐ݒ���ēx���s
		Camera_Setup();
	}

	// �Q�c�֌W�̕`�揈��
	Chara_2D_RenderAll();
}
