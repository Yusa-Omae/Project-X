#include "Task_StageClear.h"
#include "DxLib.h"
#include "System.h"
#include "Sound.h"
#include "Stage.h"
#include "StageData.h"
#include <math.h>

// ステージクリア表示の出現前に待つ時間
#define SCREEN_IN_WAIT_TIME			(1.0f)

// ステージ番号出現時の移動開始Y座標
#define MOVE_START_Y				(500)

// ステージクリア表示の出現時の初期Y速度
#define SCREEN_IN_Y_SPEED			(-3000.0f)

// ステージクリア表示の出現時のY加速度
#define SCREEN_IN_Y_ACCEL			(6000.0f)

// ステージクリア表示をフェードアウトする前に待つ時間
#define FADE_OUT_WAIT				(2.0f)

// ステージクリア表示のフェードアウトにかける時間
#define FADE_OUT_TIME				(1.0f)

// ステージクリア表示処理の状態
typedef enum _ETask_StageClearState
{
	ETask_StageClearState_ScreenInWait,		// 出現待ち中
	ETask_StageClearState_ScreenIn,			// 出現中
	ETask_StageClearState_FadeOutWait,		// フェードアウト待ち中
	ETask_StageClearState_FadeOut,			// フェードアウト中
	ETask_StageClearState_End,				// 終了状態
} ETask_StageClearState;

// ステージクリア表示処理用のデータ
typedef struct _STask_StageClearData
{
	// タスクの情報
	STaskInfo             TaskInfo;

	// 処理の状態
	ETask_StageClearState State;

	// 汎用カウンタ
	float                 Counter;

	// ステージクリアの文字列を描画するY座標
	float                 StringY;

	// ステージクリアの文字列を描画するY座標の速度
	float                 StringYSpeed;

	// ステージクリア表示の不透明度
	int                   StringOpacity;
} STask_StageClearData;

// ステージクリア表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_StageClear_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ステージクリア表示の描画処理を行う
static void Task_StageClear_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// ステージクリア表示処理タスクの基本情報
static STaskBaseInfo g_Task_StageClearTaskBaseInfo =
{
	8,
	8,
	Task_StageClear_Step,
	Task_StageClear_Render,
	NULL
};

// ステージクリア表示の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_StageClear_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;
	float                 PrevStringYSpeed;

	// ステージクリア表示処理の状態によって処理を分岐
	switch( SCData->State )
	{
	case ETask_StageClearState_ScreenInWait:		// 出現待ち中
		SCData->Counter += StepTime;
		if( SCData->Counter > SCREEN_IN_WAIT_TIME )
		{
			// 出現待ち時間が経過したら出現を開始する
			SCData->State   = ETask_StageClearState_ScreenIn;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_ScreenIn:			// 出現中
		// Y座標の速度に従って表示Y座標を移動する
		PrevStringYSpeed = SCData->StringYSpeed;
		SCData->StringYSpeed += StepTime * SCREEN_IN_Y_ACCEL;
		SCData->StringY      += 
			( PrevStringYSpeed + SCData->StringYSpeed ) * StepTime / 2.0f;

		// 落下中且つ停止位置に到達していたらフェードアウト待ちを開始する
		if( SCData->StringY > 0.0f && SCData->StringYSpeed > 0.0f )
		{
			SCData->StringY = 0.0f;

			SCData->State   = ETask_StageClearState_FadeOutWait;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_FadeOutWait:		// フェードアウト待ち中
		SCData->Counter += StepTime;
		if( SCData->Counter > FADE_OUT_WAIT )
		{
			// フェードアウト待ち時間が経過したらフェードアウトを開始する
			SCData->State   = ETask_StageClearState_FadeOut;
			SCData->Counter = 0.0f;
		}
		break;

	case ETask_StageClearState_FadeOut:			// フェードアウト中
		SCData->Counter += StepTime;
		if( SCData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了したら終了状態にする
			SCData->StringOpacity = 0;

			SCData->State         = ETask_StageClearState_End;
			SCData->Counter  = 0.0f;
		}
		else
		{
			// フェードアウト中のステージクリア表示の不透明度を算出
			SCData->StringOpacity =
				255 - ( int )( SCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_StageClearState_End:				// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// ステージクリア表示の描画処理を行う
static void Task_StageClear_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;
	int                   DrawWidth1;
	int                   DrawWidth2;
	int                   DrawWidth3;
	int                   DrawX1;
	int                   DrawX2;
	int                   DrawX3;
	int                   DrawY;
	char                  NumberStr[ 2 ];
	int                   FontHandle;

	// ステージクリア表示の不透明度が０だったら何もせず終了
	if( SCData->StringOpacity == 0 )
	{
		return;
	}

	// 汎用フォントハンドルの取得
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// ステージ番号の文字列を作成
	NumberStr[ 0 ] = '1' + StageData_GetLoadStageNo();
	NumberStr[ 1 ] = '\0';

	// ステージクリア表示全体の横幅と、『STAGE 』だけを描画した場合の横幅と、
	// 『 CLEAR!』だけを描画した場合の横幅を取得
	DrawWidth1 = GetDrawStringWidthToHandle( "WAVE ",  strlen( "WAVE " ), FontHandle );
	DrawWidth2 = GetDrawStringWidthToHandle( NumberStr, strlen( NumberStr ),FontHandle );
	DrawWidth3 = GetDrawStringWidthToHandle( " CLEAR!", strlen( " CLEAR!" ),FontHandle );

	// 『STAGE』とステージ番号と『CLEAR!』の描画X座標を算出
	DrawX1 = GAME_SCREEN_WIDTH  / 2 - ( DrawWidth1 + DrawWidth2 + DrawWidth3 ) / 2;
	DrawX2 = DrawX1 + DrawWidth1;
	DrawX3 = DrawX2 + DrawWidth2;

	// ステージクリア表示の描画Y座標の算出
	DrawY = GAME_SCREEN_HEIGHT / 2 - FONTHANDLE_BIG_SIZE / 2 + ( int )SCData->StringY;

	// アルファブレンドモードでステージクリア表示の不透明度を設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, SCData->StringOpacity );

	// 『STAGE』とステージ番号と『CLEAR!』の描画
	DrawStringToHandle( DrawX1, DrawY, "WAVE",   GetColor( 255,255,255 ), FontHandle );
	DrawStringToHandle( DrawX2, DrawY, NumberStr, GetColor( 255,0  ,0   ), FontHandle );
	DrawStringToHandle( DrawX3, DrawY, " CLEAR!", GetColor( 255,255,255 ), FontHandle );

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// ステージクリア表示が終了したかをチェックする
//     戻り値 : ステージクリア表示が終了したいるかどうか
//              ( true:終了している  false:終了していない )
bool Task_StageClear_CheckEnd(
	// ステージクリア表示タスク情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_StageClearData *SCData = ( STask_StageClearData * )TInfo->Data;

	// 処理の状態が終了状態の場合は、処理が終了しているので true を返す
	return SCData->State == ETask_StageClearState_End;
}

// ステージクリア表示を開始する
//     戻り値 : ステージクリア表示タスク情報構造体へのアドレス
STaskInfo * Task_StageClear_Start( void )
{
	STask_StageClearData *SCData;

	// ステージクリア表示の処理で使用するデータを格納するメモリ領域の確保
	SCData = ( STask_StageClearData * )calloc( 1, sizeof( STask_StageClearData ) );
	if( SCData == NULL )
	{
		return NULL;
	}

	// ステージクリア表示の初期Y座標と初期Y速度を設定
	SCData->StringY       = MOVE_START_Y;
	SCData->StringYSpeed  = SCREEN_IN_Y_SPEED;

	// ステージクリア表示の不透明度を設定
	SCData->StringOpacity = 255;

	// ステージクリア表示出現待ち状態にする
	SCData->State         = ETask_StageClearState_ScreenInWait;
	SCData->Counter       = 0.0f;

	// タスクを登録する
	SCData->TaskInfo.Base = &g_Task_StageClearTaskBaseInfo;
	SCData->TaskInfo.Data = SCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &SCData->TaskInfo );

	// タスク情報構造体のアドレスを返す
	return &SCData->TaskInfo;
}


