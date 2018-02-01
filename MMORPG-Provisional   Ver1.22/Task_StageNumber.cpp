#include "Task_StageNumber.h"
#include "DxLib.h"
#include "System.h"
#include "Stage.h"
#include "StageData.h"
#include <math.h>

// ステージ番号表示をフェードインする前に待つ時間
#define FADE_IN_WAIT_TIME			(1.0f)

// ステージ番号表示のフェードインにかける時間
#define FADE_IN_TIME				(1.0f)

// ステージ番号フェードイン時の移動開始Y座標
#define MOVE_START_Y				(128)

// ステージ番号表示をフェードアウトする前に待つ時間
#define FADE_OUT_WAIT				(1.0f)

// フェードアウト時にステージ数表示が画面下に移動する速度
#define OUT_MOVE_SPEED				(256.0f)

// ステージ番号表示のフェードアウトにかける時間
#define FADE_OUT_TIME				(1.0f)

// ステージ番号表示処理の状態
typedef enum _ETask_StageNumberState
{
	ETask_StageNumberState_FadeInWait,		// フェードイン待ち中
	ETask_StageNumberState_FadeIn,			// フェードイン中
	ETask_StageNumberState_FadeOutWait,		// フェードアウト待ち中
	ETask_StageNumberState_FadeOut,			// フェードアウト中
	ETask_StageNumberState_End,				// 終了状態
} ETask_StageNumberState;

// ステージ番号表示処理用のデータ
typedef struct _STask_StageNumberData
{
	// タスクの情報
	STaskInfo              TaskInfo;

	// 処理の状態
	ETask_StageNumberState State;

	// 汎用カウンタ
	float                  Counter;

	// ステージ番号を表示するY座標
	int                    DrawY;

	// ステージ番号表示の不透明度
	int                    Opacity;
} STask_StageNumberData;

// ステージ番号表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_StageNumber_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ステージ番号表示の描画処理を行う
static void Task_StageNumber_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// ステージ番号表示処理タスクの基本情報
static STaskBaseInfo g_Task_StageNumberTaskBaseInfo =
{
	8,
	8,
	Task_StageNumber_Step,
	Task_StageNumber_Render,
	NULL
};

// ステージ番号表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_StageNumber_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;
	float                  FirstSpeed;
	float                  SubSpeed;
	float                  Distance;

	// ステージ番号表示処理の状態によって処理を分岐
	switch( SSData->State )
	{
	case ETask_StageNumberState_FadeInWait:		// フェードイン待ち中
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらフェードインを開始する
			SSData->State   = ETask_StageNumberState_FadeIn;
			SSData->Counter = 0.0f;
		}
		break;

	case ETask_StageNumberState_FadeIn:			// フェードイン中
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_IN_TIME )
		{
			// フェードインが完了したらフェードアウト待ちを開始する
			SSData->DrawY   = 0;
			SSData->Opacity = 255;

			SSData->State   = ETask_StageNumberState_FadeOutWait;
			SSData->Counter = 0.0f;
		}
		else
		{
			// フェードイン中のステージ番号表示のY座標と不透明度を算出
			FirstSpeed = MOVE_START_Y * 2.0f / FADE_IN_TIME;
			SubSpeed   = FirstSpeed / FADE_IN_TIME;
			Distance   = ( FirstSpeed + FirstSpeed - SubSpeed * SSData->Counter ) 
				* SSData->Counter / 2.0f;

			SSData->DrawY   = ( int )( MOVE_START_Y - Distance );
			SSData->Opacity = ( int )( SSData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_StageNumberState_FadeOutWait:		// フェードアウト待ち中
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_OUT_WAIT )
		{
			// フェードアウト待ち時間が経過したらフェードアウトを開始する
			SSData->State   = ETask_StageNumberState_FadeOut;
			SSData->Counter = 0.0f;
		}
		break;

	case ETask_StageNumberState_FadeOut:			// フェードアウト中
		SSData->Counter += StepTime;
		if( SSData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了しら終了状態にする
			SSData->Opacity = 0;

			SSData->State    = ETask_StageNumberState_End;
			SSData->Counter  = 0.0f;
		}
		else
		{
			// フェードアウト中のステージ番号表示のY座標と不透明度を算出
			SSData->DrawY   =
				-( int )( OUT_MOVE_SPEED * SSData->Counter * SSData->Counter / 2.0f );

			SSData->Opacity = 255 - ( int )( SSData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_StageNumberState_End:				// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// ステージ番号表示の描画処理を行う
static void Task_StageNumber_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;
	int                    DrawWidth1;
	int                    DrawWidth2;
	int                    DrawX1;
	int                    DrawX2;
	int                    DrawY;
	char                   NumberStr[ 3 ];
	int                    FontHandle;

	// ステージ番号表示の不透明度が０だったら何もせず終了
	if( SSData->Opacity == 0 )
	{
		return;
	}

	// 汎用フォントハンドルの取得
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// ステージ番号の文字列を作成
	if (StageData_GetLoadStageNo() >= 9) {
		
		int statgeNumber = StageData_GetLoadStageNo() + 1;

		NumberStr[0] = '0' + statgeNumber / 10;
		NumberStr[1] = '0' + statgeNumber % 10;
		NumberStr[2] = '\0';

		// ステージ番号表示全体の横幅と、『STAGE 』だけを描画した場合の横幅を取得
		DrawWidth1 = GetDrawStringWidthToHandle("WAVE 00", strlen("WAVE 00"), FontHandle);
		DrawWidth2 = GetDrawStringWidthToHandle("WAVE ", strlen("WAVE "), FontHandle);

	}
	else {
		NumberStr[0] = '1' + StageData_GetLoadStageNo();
		NumberStr[1] = '\0';
		NumberStr[2] = '\0';

		// ステージ番号表示全体の横幅と、『STAGE 』だけを描画した場合の横幅を取得
		DrawWidth1 = GetDrawStringWidthToHandle("WAVE 0", strlen("WAVE 0"), FontHandle);
		DrawWidth2 = GetDrawStringWidthToHandle("WAVE ", strlen("WAVE "), FontHandle);

	}

	// ステージ番号を描画するY座標の算出
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2 - SSData->DrawY;

	
	// 『STAGE』を描画するX座標とステージ番号を描画するX座標の算出
	DrawX1     = GAME_SCREEN_WIDTH / 2 - DrawWidth1 / 2;
	DrawX2     = DrawX1 + DrawWidth2;

	// アルファブレンドモードでステージ番号表示の不透明度を設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, SSData->Opacity );

	// 『STAGE』とステージ番号の描画
	DrawStringToHandle( DrawX1, DrawY, "WAVE",   GetColor( 255,255,255 ), FontHandle );
	DrawStringToHandle( DrawX2, DrawY, NumberStr, GetColor( 255,  0,  0 ), FontHandle );

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// ステージ番号表示が終了したかをチェックする
//     戻り値 : ステージ番号表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
bool        Task_StageNumber_CheckEnd(
	// ステージ番号表示タスク情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_StageNumberData *SSData = ( STask_StageNumberData * )TInfo->Data;

	// 処理の状態が終了状態の場合は、処理が終了しているので true を返す
	return SSData->State == ETask_StageNumberState_End;
}

// ステージ番号表示を開始する
//     戻り値 : ステージ番号表示タスク情報構造体へのアドレス
STaskInfo * Task_StageNumber_Start( void )
{
	STask_StageNumberData *SSData;

	// ステージ番号表示の処理で使用するデータを格納するメモリ領域の確保
	SSData = ( STask_StageNumberData * )calloc( 1, sizeof( STask_StageNumberData ) );
	if( SSData == NULL )
	{
		return NULL;
	}

	// フェードイン待ちの状態にする
	SSData->State   = ETask_StageNumberState_FadeInWait;
	SSData->Counter = 0.0f;

	// タスクを登録する
	SSData->TaskInfo.Base = &g_Task_StageNumberTaskBaseInfo;
	SSData->TaskInfo.Data = SSData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &SSData->TaskInfo );

	// タスク情報構造体のアドレスを返す
	return &SSData->TaskInfo;
}


