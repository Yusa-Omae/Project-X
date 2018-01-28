#include "System.h"

#include <Windows.h>

// WinMain�֐�
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// �Q�[���V�X�e���̃��C���֐����Ă�
	System_Main();

	return 0;
}

#if false
#include"Dxlib.h"
#include"stdio.h"
#include"SceneMgr.h"
#include"Key.h"
#include"Code/Common/Mouse/Mouse.h"
#include"Define.h"
#include"System.h"

//���C�����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	SetGraphMode(INIT_AREA_X2, INIT_AREA_Y2, 32);//�E�B���h�E�T�C�Y�ݒ�
#ifdef __MY_DEBUG__
	ChangeWindowMode(TRUE);			//�E�B���h�E���[�h�ɂ���ƃ^�C�g����ʂ̕`�悪���܂������Ȃ��o�O�B
#endif
	DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);//�������Ɨ���ʐݒ�

	SystemInitialize();
	SceneInitialize();



	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {

		gpUpdateKey();
		Mouse_Update();

		SceneMgr_update();  //�X�V
		SceneMgr_draw();    //�`��

							//�Q�[���̏I��
		if (Get_Exit_Game() == false) {
			break;
		}

	}

	SceneFainalize();

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}
#endif