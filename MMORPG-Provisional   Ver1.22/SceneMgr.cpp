#include"DxLib.h"
#include"SceneMgr.h"
#include"Title.h"
#include"Gamemain.h"
#include"UI.h"
#include"System.h"


//�V�[���ύX�̒���

eScene NowScene = eScene_Title;
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
	}

}