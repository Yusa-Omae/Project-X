/*
				ファイル名		:Shop.h
				作成者			:Syara
				作成日時		:2018/01/23
				ソース説明		:
				
					ショップシーン
				
				備考
				
				
				
!*/

#ifndef __SHOP_H__
#define __SHOP_H__

STaskInfo* Task_Shop_Start();
/*
ショップが終了するか判断する
return	true	:終了
false	:終了しない
*/
bool Task_Shop_IsExit();

#if false 
void Shop_Intialize();
void Shop_Finalize();
void Shop_Update();
void Shop_Draw();
#endif

#endif // __SHOP_H__