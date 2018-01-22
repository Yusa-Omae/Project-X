/*
				ファイル名		:TestMenu.h
				作成者			:Syara
				作成日時		:2018/01/22
				ソース説明		:
					
					テスト用メニューデバッグ実行時はここから各シーンへ移動するようにします
					デバッグ時のみ使用できます

				備考
				
				
				
!*/

#ifndef __TEST_MENU_H__
#define __TEST_MENU_H__

#ifdef __MY_DEBUG__

void TestMenu_Initialize();
void TestMenu_Finalize();
void TestMenu_Update();
void TestMenu_Draw();


#endif


#endif // __TEST_MENU_H__