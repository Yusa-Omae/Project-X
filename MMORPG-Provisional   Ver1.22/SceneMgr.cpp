#include"DxLib.h"
#include"SceneMgr.h"
#include"Title.h"
#include"Gamemain.h"
#include"UI.h"
#include"System.h"


//シーン変更の中枢

eScene NowScene = eScene_Title;
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
	}

}