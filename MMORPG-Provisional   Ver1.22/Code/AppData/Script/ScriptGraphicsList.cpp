/*
				ファイル名		:ScriptGraphicsList.cpp
				作成者			:Syara
				作成日時		:2018/02/06
				ソース説明		:
				
				
				備考
				
				
				
!*/
#include "DxLib.h"
#include "ScriptGraphicsList.h"
#include <list>

static std::list<SCRIPT_GRAPHICS_t> s_GraphicsList;
static int s_Counter = 0;

/*
	画像を追加する
	const char* fileName	:画像パス
	...						:
*/
void ScriptGraphicsList_AddGraphic(const char* fileName,...) {

	va_list vlist;
	char path[256];

	va_start(vlist, fileName);
	vsprintfDx(path, fileName, vlist);
	va_end(vlist);
	

	SCRIPT_GRAPHICS_t add;
	add.id = s_Counter++;
	add.image = LoadGraph(path);
	s_GraphicsList.push_back(add);

}

/*
	指定のIDの画像を取得する
*/
const SCRIPT_GRAPHICS_t* ScriptGraphicsList_GetGraphics(int id) {

	int count = s_GraphicsList.size();

	if (id >= 0 && id < count) {
		for each (SCRIPT_GRAPHICS_t graph in s_GraphicsList)
		{
			if (graph.id == id) {
				return (const SCRIPT_GRAPHICS_t*)&graph;
			}
		}
	}
	return NULL;
}

/*
	登録した画像をすべて削除する
	IDも0からスタートになる
*/
void ScriptGraphicsList_AllDelete() {
	for each (SCRIPT_GRAPHICS_t graph in s_GraphicsList)
	{
		
		DeleteGraph(graph.image);
		
	}
	s_GraphicsList.clear();
	s_Counter = 0;
}