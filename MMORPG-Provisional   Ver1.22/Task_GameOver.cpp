#include "Task_GameOver.h"
#include "DxLib.h"
#include "System.h"
#include "Sound.h"
#include <math.h>

// 『GAME OVER』の文字列
#define GAMEOVER_STRING					"GAME OVER"

// ゲームオーバー表示をフェードインする前に待つ時間
#define FADE_IN_WAIT_TIME				(1.0f)

// ゲームオーバー表示のフェードインにかける時間
#define FADE_IN_TIME					(1.0f)

// ゲームオーバー表示をフェードアウトする前に待つ時間
#define FADE_OUT_WAIT					(5.0f)

// ゲームオーバー表示のフェードアウトにかける時間
#define FADE_OUT_TIME					(1.0f)

// ゲームオーバー表示処理の状態
typedef enum _ETask_GameOverState
{
	ETask_GameOverState_FadeInWait,		// フェードイン待ち中
	ETask_GameOverState_FadeIn,			// フェードイン中
	ETask_GameOverState_FadeOutWait,	// フェードアウト待ち中
	ETask_GameOverState_FadeOut,		// フェードアウト中
	ETask_GameOverState_End,			// 終了状態
} ETask_GameOverState;

// ゲームオーバー表示処理用のデータ
typedef struct _STask_GameOverData
{
	// タスクの情報
	STaskInfo           TaskInfo;

	// 処理の状態
	ETask_GameOverState State;

	// 汎用カウンタ
	float               Counter;

	// ゲームオーバー表示の不透明度
	int                 Opacity;
} STask_GameOverData;

// ゲームオーバー表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_GameOver_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ゲームオーバー表示の描画処理を行う
static void Task_GameOver_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// ゲームオーバー表示処理タスクの基本情報
static STaskBaseInfo g_Task_GameOverTaskBaseInfo =
{
	8,
	8,
	Task_GameOver_Step,
	Task_GameOver_Render,
	NULL
};

// ゲームオーバー表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_GameOver_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;

	// ゲームオーバー表示処理の状態によって処理を分岐
	switch( GOData->State )
	{
	case ETask_GameOverState_FadeInWait:	// フェードイン待ち中
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらゲームオーバーBGMの再生と
			// フェードインを開始する
			Sound_PlayBGM( EBGM_GameOver );

			GOData->State   = ETask_GameOverState_FadeIn;
			GOData->Counter = 0.0f;
		}
		break;

	case ETask_GameOverState_FadeIn:		// フェードイン中
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードインが完了したらフェードアウト待ちを開始する
			GOData->Opacity = 255;

			GOData->State   = ETask_GameOverState_FadeOutWait;
			GOData->Counter = 0.0f;
		}
		else
		{
			// フェードイン中のゲームオーバー表示の不透明度を算出
			GOData->Opacity = ( int )( GOData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_GameOverState_FadeOutWait:	// フェードアウト待ち中
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_OUT_WAIT )
		{
			// フェードアウト待ち時間が経過したらフェードアウトを開始する
			GOData->State   = ETask_GameOverState_FadeOut;
			GOData->Counter = 0.0f;
		}
		break;

	case ETask_GameOverState_FadeOut:		// フェードアウト中
		GOData->Counter += StepTime;
		if( GOData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了したら終了状態にする
			GOData->Opacity = 0;

			GOData->State   = ETask_GameOverState_End;
			GOData->Counter = 0.0f;
		}
		else
		{
			// フェードアウト中のゲームオーバー表示の不透明度を算出する
			GOData->Opacity = 255 - ( int )( GOData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_GameOverState_End:			// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// ゲームオーバー表示の描画処理を行う
static void Task_GameOver_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;
	int                 DrawWidth;
	int                 DrawX;
	int                 DrawY;
	int                 FontHandle;

	// ゲームオーバー表示の不透明度が０の場合は何もせず終了
	if( GOData->Opacity == 0 )
	{
		return;
	}

	// 汎用フォントハンドルの取得
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// 『GAME OVER』表示の幅を取得する
	DrawWidth = GetDrawStringWidthToHandle(
					GAMEOVER_STRING, strlen( GAMEOVER_STRING ), FontHandle );

	// 『GAME OVER』を画面中心に描画するための座標を算出
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2;
	DrawX = GAME_SCREEN_WIDTH  / 2 - DrawWidth           / 2;

	// アルファブレンドモードでゲームオーバー表示の不透明度を設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, GOData->Opacity );

	// 『GAME OVER』の描画
	DrawStringToHandle( DrawX, DrawY, GAMEOVER_STRING, GetColor( 255,0,0 ), FontHandle );

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// ゲームオーバー表示が終了したかをチェックする
//     戻り値 : ゲームオーバー表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
bool Task_GameOver_CheckEnd(
	// ゲームオーバー表示タスク情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_GameOverData *GOData = ( STask_GameOverData * )TInfo->Data;

	// 処理の状態が終了状態の場合は、処理が終了しているので true を返す
	return GOData->State == ETask_GameOverState_End;
}

// ゲームオーバー表示を開始する
//     戻り値 : ゲームオーバー表示タスク情報構造体へのアドレス
STaskInfo * Task_GameOver_Start( void )
{
	STask_GameOverData *GOData;

	// ゲームオーバー表示の処理で使用するデータを格納するメモリ領域の確保
	GOData = ( STask_GameOverData * )calloc( 1, sizeof( STask_GameOverData ) );
	if( GOData == NULL )
	{
		return NULL;
	}

	// 不透明度を初期化する
	GOData->Opacity = 0;
	
	// フェードイン待ちの状態にする
	GOData->State = ETask_GameOverState_FadeInWait;
	GOData->Counter = 0.0f;

	// タスクを登録する
	GOData->TaskInfo.Base = &g_Task_GameOverTaskBaseInfo;
	GOData->TaskInfo.Data = GOData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &GOData->TaskInfo );

	// タスク情報構造体のアドレスを返す
	return &GOData->TaskInfo;
}


