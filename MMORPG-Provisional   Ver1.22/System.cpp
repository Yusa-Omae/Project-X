#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"
#include"math.h"

bool Debug_flg;


_SYSTEM_INFO SYSTEM_INFO;


void Set_Debug_Mode(){

	if(Debug_flg == 0){
		Debug_flg = 1;
	}else{
		Debug_flg = 0;
	}
}

bool Get_Debug_mode(){
	return Debug_flg;
}

void SystemInitialize(){

	// ウインドウモードで起動するか確認する
	//if( MessageBox( NULL, "ウインドウモードで起動しますか？", "画面モード確認", MB_YESNO ) == IDYES )
	//{
	//	// 「はい」が選択された場合はウインドウモードで起動
	//	ChangeWindowMode( TRUE );
	//}
	//if(MessageBox(NULL,"デバッグモードで起動しますか？","デバッグ確認",MB_YESNO) == IDYES){
	//	Set_Debug_Mode();
	//}else{
	//	Debug_flg = 0;
	//}

	//ゲーム終了フラグのオフ
	SYSTEM_INFO.Exit_Game = true;
	//ウィンドウハンドルの取得
	SYSTEM_INFO.WinHndl = GetMainWindowHandle();


}

void SystemMain(){





}



void Set_Exit_Game(){

	SYSTEM_INFO.Exit_Game = false;
}

bool Get_Exit_Game(){
	return SYSTEM_INFO.Exit_Game;
}

////フェードインアウト関数	true:FadeIN		false:FadeOut
//void Fade_io(int Color,int Fade_Flg){
//
//	SYSTEM_INFO.Fade_Out_Count = 0;
//	if(Fade_Flg == true){			
//		for(int i = 255;0 < i;i -= 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}else{
//		for(int i = 0;255 > i;i += 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}
//}

HWND GetConsoleHwnd(){








	return ;

}