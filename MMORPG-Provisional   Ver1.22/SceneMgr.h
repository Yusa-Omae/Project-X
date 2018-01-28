#if false
#pragma once


typedef enum {
	eScene_none = -1,
	eScene_Menu,
	eScene_Gamemain,
	eScene_Title,
	eScene_Result,

//�f�o�b�O�p�V�[��

#ifdef __MY_DEBUG__

	eScene_TestMemu,
	eScene_TestString,

#endif

// --------------------

}eScene;

void SceneMgr_update();

void SceneMgr_draw();

void SceneMgr_ChangeScene(eScene nextScene);

void SceneInitialize();

void SceneFainalize();
#endif