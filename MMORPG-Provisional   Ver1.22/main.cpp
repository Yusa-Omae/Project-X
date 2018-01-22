#include"Dxlib.h"
#include"stdio.h"
#include"SceneMgr.h"
#include"Key.h"
#include"Define.h"
#include"System.h"

//メイン画面
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

    SetGraphMode( INIT_AREA_X2,INIT_AREA_Y2 , 32) ;//ウィンドウサイズ設定
    DxLib_Init(), SetDrawScreen( DX_SCREEN_BACK );//初期化と裏画面設定

	SystemInitialize();
	SceneInitialize();

#ifdef __MY_DEBUG__
	ChangeWindowMode(TRUE);			//ウィンドウモードにするとタイトル画面の描画がうまくいかないバグ。
#endif

	while( ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){

	    SceneMgr_update();  //更新
		gpUpdateKey();
		SceneMgr_draw();    //描画

		//ゲームの終了
		if(Get_Exit_Game() == false){
			break;
		}

	}
    
	SceneFainalize();

    // ＤＸライブラリの後始末
    DxLib_End() ;

    // ソフトの終了
    return 0 ;
}