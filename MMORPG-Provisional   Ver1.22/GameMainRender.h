#ifndef GAMEMAINRENDER_H
#define GAMEMAINRENDER_H

// ゲームメインの描画処理を初期化する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool GameMainRender_Initialize(void);

// ゲームメインの描画処理の後始末をする
extern void GameMainRender_Terminate(void);

// ステージ開始時の描画準備を行う
extern void GameMainRender_StageSetup(void);

// ゲームメインの描画処理を行う
extern void GameMainRender_Render(void);

#endif