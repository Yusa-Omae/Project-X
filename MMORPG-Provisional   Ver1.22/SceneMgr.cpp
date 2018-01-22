#include"DxLib.h"
#include"SceneMgr.h"
#include"Title.h"
#include"Gamemain.h"
#include"UI.h"
#include"System.h"

#ifdef __MY_DEBUG__
#include "Code/Test/TestMenu.h"
#include "Code//Test/TestString.h"
#endif	//__MY_DEBUG__


//�V�[���ύX�̒���

#ifdef __MY_DEBUG__
eScene NowScene = eScene_TestMemu;
#else
eScene NowScene = eScene_Title;
#endif	//__MY_DEBUG__
eScene NextScene = eScene_none;

//�V�[�����Ƃ̏������������ōs���Ă���
void SceneMgr_Initialize(int Scene)
{

	switch(Scene)
	{
	case eScene_Menu:

		break;
	case eScene_Gamemain:
		GameMainInitialize();
		UI_Initialize();
		break;
	case eScene_Title:
		TitleInitialize();
		break;
	case eScene_Result:
		break;

#ifdef __MY_DEBUG__
	case eScene_TestMemu:
		TestMenu_Initialize();
		break;
	case eScene_TestString:
		TestString_Initialize();
		break;
#endif	//__MY_DEBUG__

	}


}

//�V�[�����Ƃ̂��ݔ��������ōs���Ă���
void SceneMgr_Fainalize(int Scene)
{

	switch(Scene)
	{
	case eScene_Menu:

		break;
	case eScene_Gamemain:
		GameMainFainalize();
		UI_Fainalize();
		break;
	case eScene_Title:
		TitleFainalize();
		break;
	case eScene_Result:
		break;
#ifdef __MY_DEBUG__
	case eScene_TestMemu:
		TestMenu_Finalize();
		break;
	case eScene_TestString:
		TestString_Finalize();
		break;
#endif	//__MY_DEBUG__
	}



}
//�V�[����ύX����Ƃ�
void SceneMgr_ChangeScene(eScene nextScene)
{
	NextScene = nextScene;
}
//�V�[���̏�����
void SceneInitialize()
{
	SceneMgr_Initialize(NowScene);
}
//�V�[���̂��ݔ�
void SceneFainalize()
{
	SceneMgr_Fainalize(NowScene);
}
//�V�[�����Ƃ̍X�V��Ƃ������ōs���Ă���
//�V�[�����ڂ�����V�[���̓���ւ������łɍs���Ă���B
void SceneMgr_update()
{
	if(NextScene != eScene_none)
	{
		//�O�V�[���̃f�[�^�폜
		SceneMgr_Fainalize(NowScene);
		//�V�[���̓���ւ�
		NowScene = NextScene;
		//���̃V�[�����Ȃ��ɂ���
		NextScene = eScene_none;
		//�V�[���̏�����
		SceneMgr_Initialize(NowScene);
	}

	switch(NowScene)
	{
	case eScene_Menu:
		break;
	case eScene_Gamemain:
		GameMainupdate();
		UI_Update();
		break;
	case eScene_Title:
		Titleupdate();
		break;
	case eScene_Result:
		break;
#ifdef __MY_DEBUG__
	case eScene_TestMemu:
		TestMenu_Update();
		break;
	case eScene_TestString:
		TestString_Update();
		break;
#endif	//__MY_DEBUG__
	}


}


//�V�[���̑僁�C���A�V�[���̕`��������ōs���Ă���B
void SceneMgr_draw()
{
	switch(NowScene)
	{
	case eScene_Menu:
		break;
	case eScene_Gamemain:
		GameMainDraw();
		UI_Draw();
		break;
	case eScene_Title:
		TitleDraw();
		break;
#ifdef __MY_DEBUG__
	case eScene_TestMemu:
		TestMenu_Draw();
		break;
	case eScene_TestString:
		TestString_Draw();
		break;
#endif	//__MY_DEBUG__
	}

}