#include "Task_AllStageClear.h"
#include "DxLib.h"
#include "Task.h"
#include "System.h"
#include <math.h>

// 全ステージクリア画面のデータがあるフォルダのパス
#define ALLSTAGECLEARDATA_DIR		"Data\\2D\\"

// 全ステージクリア画面をフェードインする前に待つ時間
#define FADE_IN_WAIT_TIME			(1.0f)

// 全ステージクリア画面のフェードインにかける時間
#define FADE_IN_TIME				(1.0f)

// 全ステージクリア画面をフェードアウトする前に待つ時間
#define FADE_OUT_WAIT				(8.1f)

// 全ステージクリア画面のフェードアウトにかける時間
#define FADE_OUT_TIME				(1.0f)

// 上下に揺れる文字表現で使用する sin の角度変化速度
#define ANGLE_SPEED					(1.5f)

// 上下に揺れる文字表現の sin に渡す角度の１文字辺りの変更角度
#define ONE_CHAR_ANGLE				(0.5f)

// 上下に揺れる文字表現の１文字毎の追加スペース
#define ONE_CHAR_SPACE				(16)

// 上下に揺れる文字表現の触れ幅
#define STRING_MOVE_Y_HEIGHT		(32.0f)


// 全ステージクリア画面処理の状態
typedef enum _ETask_AllStageClearState
{
	ETask_AllStageClearState_FadeInWait,	// フェードイン待ち中
	ETask_AllStageClearState_FadeIn,		// フェードイン中
	ETask_AllStageClearState_FadeOutWait,	// フェードアウト待ち中
	ETask_AllStageClearState_FadeOut,		// フェードアウト中
	ETask_AllStageClearState_End,			// 終了状態
} ETask_AllStageClearState;

// 全ステージクリア画面処理用のデータ
typedef struct _STask_AllStageClearData
{
	// タスクの情報
	STaskInfo                TaskInfo;

	// 処理の状態
	ETask_AllStageClearState State;

	// 汎用カウンタ
	float                    Counter;

	// 上下に揺れる文字表現で使用する sin に渡す角度
	float                    SinAngle;

	// 全ステージクリア表示の不透明度
	int                      Opacity;

	// 背景画像
	int                      BackGraphHandle;
} STask_AllStageClearData;

// 全ステージクリア画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_AllStageClear_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// 全ステージクリア画面の描画処理を行う
static void Task_AllStageClear_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// 全ステージクリア画面の後始末処理を行う
static void Task_AllStageClear_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// 全ステージクリア画面処理タスクの基本情報
static STaskBaseInfo g_Task_AllStageClearTaskBaseInfo =
{
	8,
	8,
	Task_AllStageClear_Step,
	Task_AllStageClear_Render,
	Task_AllStageClear_Terminate
};

// 全ステージクリア画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_AllStageClear_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;

	// 上下に揺れる文字表現で使用する sin に渡す角度の変更
	ASCData->SinAngle += ANGLE_SPEED * StepTime;
	if( ASCData->SinAngle > DX_TWO_PI_F )
	{
		ASCData->SinAngle -= DX_TWO_PI_F;
	}

	// 全ステージクリア表示処理の状態によって処理を分岐
	switch( ASCData->State )
	{
	case ETask_AllStageClearState_FadeInWait:	// フェードイン待ち中
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらフェードインを開始する
			ASCData->State   = ETask_AllStageClearState_FadeIn;
			ASCData->Counter = 0.0f;

			// 全ステージクリア画面ＢＧＭの再生を開始する
			Sound_PlayBGM( EBGM_AllStageClear );
		}
		break;

	case ETask_AllStageClearState_FadeIn:		// フェードイン中
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードインが完了したらフェードアウト待ちを開始する
			ASCData->Opacity = 255;

			ASCData->State   = ETask_AllStageClearState_FadeOutWait;
			ASCData->Counter = 0.0f;
		}
		else
		{
			// フェードイン中の全ステージクリア表示の不透明度を算出
			ASCData->Opacity = ( int )( ASCData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_AllStageClearState_FadeOutWait:	// フェードアウト待ち中
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_OUT_WAIT )
		{
			// フェードアウト待ち時間が経過したらフェードアウトを開始する
			System_FadeOut();

			ASCData->State   = ETask_AllStageClearState_FadeOut;
			ASCData->Counter = 0.0f;
		}
		break;

	case ETask_AllStageClearState_FadeOut:		// フェードアウト中
		ASCData->Counter += StepTime;
		if( ASCData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了した後の処理

			// 一応全ステージクリア表示の不透明度を０にする
			ASCData->Opacity = 0;

			// 状態を終了にする
			ASCData->State   = ETask_AllStageClearState_End;
			ASCData->Counter = 0.0f;

			// タイトル画面を開始する
			if( !System_StartTitle() )
			{
				return false;
			}

			// タスクを削除する
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &ASCData->TaskInfo );
		}
		else
		{
			// フェードアウト中の全ステージクリア表示の不透明度を算出
			ASCData->Opacity = 255 - ( int )( ASCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_AllStageClearState_End:			// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// 全ステージクリア画面の描画処理を行う
static void Task_AllStageClear_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;
	int                      DrawWidth;
	int                      DrawX;
	int                      DrawY;
	char                     DrawStr[ 2 ];
	int                      FontHandle;
	float                    Angle;
	int                      i;
	int                      CharNum;
	const char              *DispString = "ALL STAGE CLEAR!";
	int                      Bright;

	// 全ステージクリア表示の不透明度が０の場合は何もせず終了
	if( ASCData->Opacity == 0 )
	{
		return;
	}

	// 背景は表示の不透明度に応じて輝度を調節する
	Bright = ASCData->Opacity * 160 / 255;
	SetDrawBright( Bright, Bright, Bright );
	DrawGraph( 0, 0, ASCData->BackGraphHandle, FALSE );
	SetDrawBright( 255,255,255 );

	// 汎用フォントハンドルの取得
	FontHandle = System_GetFontHandle( EFontHandle_Big );

	// 文字の数を取得
	CharNum   = strlen( DispString );

	// 全ステージクリア表示の描画幅を算出
	DrawWidth = GetDrawStringWidthToHandle( DispString, CharNum, FontHandle ) 
												+ ( CharNum - 1 ) * ONE_CHAR_SPACE;

	// 描画開始X座標の算出
	DrawX     = ( GAME_SCREEN_WIDTH - DrawWidth ) / 2;

	// アルファブレンドモードで全ステージクリア表示の不透明度を設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, ASCData->Opacity );

	// 全ステージクリア表示の文字の数だけ繰り返し
	Angle = ASCData->SinAngle;
	for( i = 0; i < CharNum; i++ )
	{
		// １文字だけ取り出した文字列の作成
		DrawStr[ 0 ] = DispString[ i ];
		DrawStr[ 1 ] = '\0';

		// 文字を描画するY座標の算出
		DrawY = ( int )( ( GAME_SCREEN_HEIGHT - FONTHANDLE_BIG_SIZE ) / 2 
										+ sin( Angle ) * STRING_MOVE_Y_HEIGHT );

		// 文字の描画
		DrawStringToHandle( DrawX, DrawY, DrawStr, GetColor( 255,255,128 ), FontHandle );

		// １文字分描画X座標を移動する
		DrawX += GetDrawStringWidthToHandle( DrawStr, strlen( DrawStr ), FontHandle ) 
																		+ ONE_CHAR_SPACE;

		// １文字分上下に揺れるY座標の算出に使用する sin に渡す角度を変更する
		Angle += ONE_CHAR_ANGLE;
	}

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// 全ステージクリア画面の後始末処理を行う
static void Task_AllStageClear_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_AllStageClearData *ASCData = ( STask_AllStageClearData * )TInfo->Data;

	// 背景画像の削除
	DeleteGraph( ASCData->BackGraphHandle );
}

// 全ステージクリア画面の処理を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Task_AllStageClear_Start( void )
{
	STask_AllStageClearData *ASCData;

	// 全ステージクリア画面の処理で使用するデータを格納するメモリ領域の確保
	ASCData = ( STask_AllStageClearData * )calloc( 1, sizeof( STask_AllStageClearData ) );
	if( ASCData == NULL )
	{
		return false;
	}

	// 背景画像の読み込み
	ASCData->BackGraphHandle  = LoadGraph( ALLSTAGECLEARDATA_DIR "AllStageClearBack.png" );
	if( ASCData->BackGraphHandle == -1 )
	{
		return false;
	}

	// 全ステージクリア表示の不透明度を初期化
	ASCData->Opacity = 0;

	// 全ステージクリア画面のフェードイン待ちの状態にする
	ASCData->State    = ETask_AllStageClearState_FadeInWait;
	ASCData->Counter  = 0.0f;

	// 黒フェードの解除を開始
	System_FadeIn();

	// タスクを登録する
	ASCData->TaskInfo.Base = &g_Task_AllStageClearTaskBaseInfo;
	ASCData->TaskInfo.Data = ASCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &ASCData->TaskInfo );

	// 正常終了
	return true;
}


