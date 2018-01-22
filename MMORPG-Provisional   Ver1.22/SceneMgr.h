#pragma once


typedef enum{
	eScene_Menu,
	eScene_Gamemain,
	eScene_Title,
	eScene_Result,
	eScene_none,
}eScene;

void SceneMgr_update();

void SceneMgr_draw();

void SceneMgr_ChangeScene(eScene nextScene);

void SceneInitialize();

void SceneFainalize();