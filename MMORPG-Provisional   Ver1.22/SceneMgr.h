#pragma once


typedef enum {
	eScene_none = -1,
	eScene_Menu,
	eScene_Gamemain,
	eScene_Title,
	eScene_Result,

//デバッグ用シーン

#ifdef __MY_DEBUG__

	eScene_TestMemu,

#endif

//

	eScene_Num,

}eScene;

void SceneMgr_update();

void SceneMgr_draw();

void SceneMgr_ChangeScene(eScene nextScene);

void SceneInitialize();

void SceneFainalize();