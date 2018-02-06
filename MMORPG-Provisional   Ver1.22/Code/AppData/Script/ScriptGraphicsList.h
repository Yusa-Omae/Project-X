/*
				ファイル名		:ScriptGraphicsList.h
				作成者			:Syara
				作成日時		:2018/02/06
				ソース説明		:
				
					スクリプト用画像リスト設定
				
				備考
				
				
				
!*/

#ifndef __SCRIPT_GRAPHICS_LIST_H__
#define __SCRIPT_GRAPHICS_LIST_H__

typedef struct {
	int id;
	int image;
}SCRIPT_GRAPHICS_t;

/*
画像を追加する
const char* fileName	:画像パス
...						:
*/
void ScriptGraphicsList_AddGraphic(const char* fileName, ...);

/*
指定のIDの画像を取得する
*/
const SCRIPT_GRAPHICS_t* ScriptGraphicsList_GetGraphics(int id);

/*
登録した画像をすべて削除する
*/
void ScriptGraphicsList_AllDelete();

#endif // __SCRIPT_GRAPHICS_LIST_H__