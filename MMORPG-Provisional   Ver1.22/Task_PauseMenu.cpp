#include "Task_PauseMenu.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// ポーズメニューのフェードインにかける時間
#define FADE_IN_TIME			(0.5f)

// メニュー項目の明滅１周にかける時間
#define MENU_BRINK_TIME			(2.0f)

// ポーズメニューのフェードアウトにかける時間
#define FADE_OUT_TIME			(0.5f)

// ポーズメニューで使用するフォントのサイズ
#define FONT_SIZE				(40)

// メニュー項目の描画開始座標
#define MENU_X					(460)
#define MENU_Y					(250)

// メニュー項目一つあたりに空けるスペース
#define MENU_SPACE				(76)

// ポーズメニュー処理の状態
typedef enum _ETask_PauseMenuState
{
	ETask_PauseMenuState_FadeIn,		// フェードイン中
	ETask_PauseMenuState_Select,		// 項目選択中
	ETask_PauseMenuState_FadeOut,		// フェードアウト中
	ETask_PauseMenuState_End,			// 終了状態
} ETask_PauseMenuState;

// ポーズメニュー処理用のデータ
typedef struct _STask_PauseMenuData
{
	// タスクの情報
	STaskInfo	     TaskInfo;

	// 処理の状態
	ETask_PauseMenuState State;

	// 汎用カウンタ
	float            Counter;

	// 選択している項目
	int              SelectPosition;

	// メニューの不透明度
	int              MenuOpacity;

	// 選択項目の明滅処理用カウンタ
	float            MenuBrinkCounter;

	// メニュー項目用フォント
	int              MenuFontHandle;
} STask_PauseMenuData;

// ポーズメニューの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_PauseMenu_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ポーズメニューの描画処理を行う
static void Task_PauseMenu_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// ポーズメニューの後始末処理を行う
static void Task_PauseMenu_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// タイトル処理タスクの基本情報
static STaskBaseInfo g_Task_PauseMenuTaskBaseInfo =
{
	8,
	8,
	Task_PauseMenu_Step,
	Task_PauseMenu_Render,
	Task_PauseMenu_Terminate
};

// ポーズメニュー各項目の名前
static const char *g_PauseMenuTable[] =
{
	"ゲーム再開",
	"タイトル画面に戻る",
	"ゲームを終了する"
};

// ポーズメニューの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_PauseMenu_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;
	int                  EdgeInput;

	// メニュー項目明滅用カウンタの時間を進める
	PTData->MenuBrinkCounter += StepTime;
	if( PTData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		PTData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// ポーズメニュー処理の状態によって処理を分岐
	switch( PTData->State )
	{
	case ETask_PauseMenuState_FadeIn:			// フェードイン中
		PTData->Counter += StepTime;
		if( PTData->Counter > FADE_IN_TIME )
		{
			// フェードインが完了したら項目選択を開始する
			PTData->MenuOpacity = 255;
			PTData->State       = ETask_PauseMenuState_Select;
		}
		else
		{
			// フェードイン中のメニュー項目の不透明度を算出
			PTData->MenuOpacity = ( int )( PTData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_PauseMenuState_Select:			// 項目選択中
		// 押された瞬間のみの入力状態を取得
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 &&
			PTData->SelectPosition > 0 )
		{
			// 上方向入力がされていて、且つ一番上の項目を選択していない場合は
			// カーソル移動音を鳴らし、選択項目を一つ上に移動する
			System_PlayCommonSE( ECommonSE_Cursor );
			PTData->SelectPosition--;
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 &&
			PTData->SelectPosition < ETask_PauseMenu_Num - 1 )
		{
			// 下方向入力がされていて、且つ一番下の項目を選択していない場合は
			// カーソル移動音を鳴らし、選択項目を一つ下に移動する
			System_PlayCommonSE( ECommonSE_Cursor );
			PTData->SelectPosition++;
		}

		if( ( EdgeInput & ( 1 << EInputType_Atk ) ) != 0 ||
			( EdgeInput & ( 1 << EInputType_Pause  ) ) != 0 )
		{
			// 決定ボタンかポーズボタンが押されたら選択音を鳴らし、
			// フェードアウトを開始する
			if( PTData->SelectPosition == ETask_PauseMenu_Continue )
			{
				System_PlayCommonSE( ECommonSE_Cancel );
			}
			else
			{
				System_PlayCommonSE( ECommonSE_Enter );
			}

			// ゲーム終了かタイトルへ戻るが選択されたら画面のフェードアウトを開始する
			if( PTData->SelectPosition == ETask_PauseMenu_GameEnd ||
				PTData->SelectPosition == ETask_PauseMenu_BackToTitle )
			{
				System_FadeOut();
			}

			PTData->State   = ETask_PauseMenuState_FadeOut;
			PTData->Counter = 0;
		}
		break;

	case ETask_PauseMenuState_FadeOut:			// フェードアウト中
		// 選択された項目によって処理を分岐
		switch( PTData->SelectPosition )
		{
		case ETask_PauseMenu_Continue:
			PTData->Counter += StepTime;
			if( PTData->Counter > FADE_OUT_TIME )
			{
				// フェードアウトが完了した後の処理

				// 一応メニューとタイトルの不透明度を０にする
				PTData->MenuOpacity  = 0;
			
				// 状態を終了にする
				PTData->State = ETask_PauseMenuState_End;
			}
			else
			{
				// フェードアウト中のメニュー項目とタイトルの不透明度を算出
				PTData->MenuOpacity =
					255 - ( int )( PTData->Counter * 255 / FADE_OUT_TIME );
			}
			break;

		case ETask_PauseMenu_BackToTitle:
		case ETask_PauseMenu_GameEnd:
			if( !System_CheckFade() )
			{
				// 状態を終了にする
				PTData->State = ETask_PauseMenuState_End;
			}
			break;
		}
		break;

	case ETask_PauseMenuState_End:				// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// ポーズメニューの描画処理を行う
static void Task_PauseMenu_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;
	int                  SelectColor;
	int                  NormalColor;
	int                  DrawColor;
	int	                 DrawY;
	float                t;
	int                  i;

	// メニュー項目の不透明度が０ではない場合のみメニュー項目を描画する
	if( PTData->MenuOpacity > 0 )
	{
		// 画面全体を半透明の黒で覆う
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, PTData->MenuOpacity / 2 );
		DrawBox( 0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, GetColor( 0,0,0 ), TRUE );

		// 選択している項目の明滅する色の算出
		t = sin( ( float )PTData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
		if( t < 0.0f ) t = -t;
		SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

		// 通常の描画色の値を取得
		NormalColor = GetColor( 255,255,255 );

		// アルファブレンドモードでメニュー項目の不透明度を設定
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, PTData->MenuOpacity );

		// メニュー項目の数だけ繰り返し
		DrawY = MENU_Y;
		for( i = 0; i < ETask_PauseMenu_Num; i++ )
		{
			// 項目が選択されているかどうかで描画色を変える
			DrawColor = PTData->SelectPosition == i ? SelectColor : NormalColor;

			// 項目の描画
			DrawStringToHandle(
				MENU_X, DrawY, g_PauseMenuTable[ i ], DrawColor, PTData->MenuFontHandle);

			// 項目の描画Y座標を１項目分下に移動する
			DrawY += MENU_SPACE;
		}
	}

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// ポーズメニューの後始末処理を行う
static void Task_PauseMenu_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	// メニュー項目描画用のフォントを削除
	DeleteFontToHandle( PTData->MenuFontHandle );
}


// ポーズメニューが終了したかをチェックする
//     戻り値 : ポーズメニューが終了したいるかどうか
//              ( true:終了している  false:終了していない )
bool Task_PauseMenu_CheckEnd(
	// ポーズメニュータスク情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	// 処理の状態が終了状態の場合は、処理が終了しているので true を返す
	return PTData->State == ETask_PauseMenuState_End;
}

// ポーズメニューで選択された項目を取得する
//     戻り値 : 選択された項目の列挙型の値
ETask_PauseMenu Task_PauseMenu_GetChoiceIndex(
	// ポーズメニュータスク情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_PauseMenuData *PTData = ( STask_PauseMenuData * )TInfo->Data;

	return ( ETask_PauseMenu )PTData->SelectPosition;
}

// ポーズメニューの処理を開始する
//     戻り値 : ポーズメニュータスク情報構造体へのアドレス
STaskInfo * Task_PauseMenu_Start( void )
{
	STask_PauseMenuData *PTData;

	// ポーズメニューの処理で使用するデータを格納するメモリ領域の確保
	PTData = ( STask_PauseMenuData * )calloc( 1, sizeof( STask_PauseMenuData ) );
	if( PTData == NULL )
	{
		return NULL;
	}

	// ポーズメニューで使用するフォントの作成
	PTData->MenuFontHandle   = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( PTData->MenuFontHandle == -1 )
	{
		return NULL;
	}

	// ポーズメニューのフェードイン中にする
	PTData->State            = ETask_PauseMenuState_FadeIn;
	PTData->Counter          = 0.0f;

	// 選択項目の初期化
	PTData->SelectPosition   = 0;

	// メニュー項目の不透明度を初期化
	PTData->MenuOpacity      = 0;

	// 選択項目の明滅用カウンタの初期化
	PTData->MenuBrinkCounter = 0;

	// タスクを登録する
	PTData->TaskInfo.Base     = &g_Task_PauseMenuTaskBaseInfo;
	PTData->TaskInfo.Data     = PTData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &PTData->TaskInfo );

	// 正常終了
	return &PTData->TaskInfo;
}


