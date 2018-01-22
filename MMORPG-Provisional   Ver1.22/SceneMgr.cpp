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


//シーン変更の中枢

#ifdef __MY_DEBUG__
eScene NowScene = eScene_TestMemu;
#else
eScene NowScene = eScene_Title;
#endif	//__MY_DEBUG__
eScene NextScene = eScene_none;

//シーンごとの初期化をここで行っている
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

//シーンごとのごみ箱をここで行っている
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
//シーンを変更するとき
void SceneMgr_ChangeScene(eScene nextScene)
{
	NextScene = nextScene;
}
//シーンの初期化
void SceneInitialize()
{
	SceneMgr_Initialize(NowScene);
}
//シーンのごみ箱
void SceneFainalize()
{
	SceneMgr_Fainalize(NowScene);
}
//シーンごとの更新作業をここで行っている
//シーンが移ったらシーンの入れ替えもついでに行っている。
void SceneMgr_update()
{
	if(NextScene != eScene_none)
	{
		//前シーンのデータ削除
		SceneMgr_Fainalize(NowScene);
		//シーンの入れ替え
		NowScene = NextScene;
		//次のシーンをなしにする
		NextScene = eScene_none;
		//シーンの初期化
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


//シーンの大メイン、シーンの描画をここで行っている。
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