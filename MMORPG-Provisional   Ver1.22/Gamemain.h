

#ifndef __GAME_MAIN_H__
#define __GAME_MAIN_H__


enum eFadeType {
	eFadeType_None = -1,	//状態なし
	eFadeType_In,			//フェードイン
	eFadeType_Out,			//フェードアウト
	eFadeType_CrossFade,	//クロスフェード
};

enum eGameState {
	eGameState_None = -1,	//状態なし
	eGameState_Initialize,	//初期化
	eGameState_Fade,		//フェード
	eGameState_Tutorial,	//チュートリアル
	eGameState_MainGame,	//メインゲーム
	eGameState_Pouse,		//ポーズ画面
	eGameState_Shop,		//ショップ
};


void GameMainInitialize();
void GameMainupdate();
void GameMainDraw();
void GameMainFainalize();


/*
ゲームメインのステートを切り替える
eGameState gameState				:次のゲームステートを設定
eFadeType fadeType = eFadeType_None	:フェードのタイプを設定　基本はフェードしない状態になっている

ゲームステート
eGameState_None,		//状態なし
eGameState_Initialize,	//初期化
eGameState_Fade,		//フェード		※これを次のステートに設定するとシーンが切り替わらないくなります
eGameState_Tutorial,	//チュートリアル
eGameState_MainGame,	//メインゲーム
eGameState_Pouse,		//ポーズ画面
eGameState_Shop,		//ショップ

フェードタイプ
eFadeType_None,			//状態なし
eFadeType_In,			//フェードイン
eFadeType_Out,			//フェードアウト
eFadeType_CrossFade,	//クロスフェード

*/
void GameMain_ChangeGameState(eGameState gameState, eFadeType fadeType = eFadeType_None);


#endif	//__GAME_MAIN_H__