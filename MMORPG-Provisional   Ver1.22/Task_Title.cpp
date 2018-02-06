#include "Task_Title.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// タイトル画面のデータがあるフォルダのパス
#define TITLEDATA_DIR			"Data\\2D\\"

// タイトル画面をフェードインする前に待つ時間
#define FADE_IN_WAIT_TIME		(0.5f)

// タイトル画面のフェードインにかける時間
#define FADE_IN_TIME			(1.0f)

// メニュー項目をフェードインする前に待つ時間
#define MENU_IN_WAIT_TIME		(0.2f)

// メニュー項目のフェードインにかける時間
#define MENU_IN_TIME			(0.25f)

// メニュー項目の明滅１周にかける時間
#define MENU_BRINK_TIME			(2.0f)

// タイトル画面のフェードアウトにかける時間
#define FADE_OUT_TIME			(1.0f)

// タイトル画面で使用するフォントのサイズ
#define FONT_SIZE				(40)

// メニュー項目の描画開始座標
#define MENU_X					(530)
#define MENU_Y					(450)

// メニュー項目一つあたりに空けるスペース
#define MENU_SPACE				(64)

// タイトル画面処理の状態
typedef enum _ETask_TitleState
{
	ETask_TitleState_FadeInWait,	// フェードイン待ち中
	ETask_TitleState_FadeIn,		// フェードイン中
	ETask_TitleState_MenuInWait,	// メニュー項目フェードイン待ち中
	ETask_TitleState_MenuIn,		// メニュー項目フェードイン中
	ETask_TitleState_Select,		// 項目選択中
	ETask_TitleState_FadeOut,		// フェードアウト中
	ETask_TitleState_End,			// 終了状態
} ETask_TitleState;

// タイトル画面のメニュー項目
typedef enum _ETask_TitleMenu
{
	ETask_TitleMenu_GameStart,		// ゲームスタート
	ETask_TitleMenu_KeyConfig,		// キーコンフィグ
	ETask_TitleMenu_Exit,			// ゲーム終了

	ETask_TitleMenu_Num,			// 項目の数
} ETask_TitleMenu;

// タイトル画面処理用のデータ
typedef struct _STask_TitleData
{
	// タスクの情報
	STaskInfo	     TaskInfo;

	// 処理の状態
	ETask_TitleState State;

	// 汎用カウンタ
	float            Counter;

	// 選択している項目
	int              SelectPosition;

	// タイトルの不透明度
	int              TitleOpacity;

	// メニューの不透明度
	int              MenuOpacity;

	// 選択項目の明滅処理用カウンタ
	float            MenuBrinkCounter;

	// タイトル画像
	int              TitleGraphHandle;

	// タイトルの背景画像
	int              BackGraphHandle;

	// メニュー項目用フォント
	int              MenuFontHandle;
} STask_TitleData;

// タイトル画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_Title_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// タイトル画面の描画処理を行う
static void Task_Title_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// タイトル画面の後始末処理を行う
static void Task_Title_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// タイトル処理タスクの基本情報
static STaskBaseInfo g_Task_TitleTaskBaseInfo =
{
	8,
	8,
	Task_Title_Step,
	Task_Title_Render,
	Task_Title_Terminate
};

// タイトル画面各項目の名前
static const char *g_TitleMenuTable[] =
{
	"GAME START",
	"KEY CONFIG",
	"EXIT"
};

// タイトル画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_Title_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;
	int              EdgeInput;

	// メニュー項目明滅用カウンタの時間を進める
	TTData->MenuBrinkCounter += StepTime;
	if( TTData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		TTData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// タイトル画面処理の状態によって処理を分岐
	switch( TTData->State )
	{
	case ETask_TitleState_FadeInWait:		// フェードイン待ち中
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらフェードインを開始する
			TTData->State   = ETask_TitleState_FadeIn;
			TTData->Counter = 0;

			// タイトル画面ＢＧＭの再生を開始する
			Sound_PlayBGM( EBGM_Title );
		}
		break;

	case ETask_TitleState_FadeIn:			// フェードイン中
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_IN_TIME )
		{
			// フェードインが完了したらメニュー項目フェードイン待ちを開始する
			TTData->TitleOpacity = 255;
			TTData->State        = ETask_TitleState_MenuInWait;
			TTData->Counter      = 0.0f;
		}
		else
		{
			// フェードイン中のタイトルの不透明度を算出
			TTData->TitleOpacity = ( int )( TTData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_TitleState_MenuInWait:		// メニュー項目フェードイン待ち中
		TTData->Counter += StepTime;
		if( TTData->Counter > MENU_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらメニュー項目のフェードインを開始する
			TTData->State   = ETask_TitleState_MenuIn;
			TTData->Counter = 0.0f;
		}
		break;

	case ETask_TitleState_MenuIn:			// メニュー項目フェードイン中
		TTData->Counter += StepTime;
		if( TTData->Counter > MENU_IN_TIME )
		{
			// フェードインが完了したら項目選択を開始する
			TTData->MenuOpacity = 255;
			TTData->State       = ETask_TitleState_Select;
		}
		else
		{
			// フェードイン中のメニュー項目の不透明度を算出
			TTData->MenuOpacity = ( int )( TTData->Counter * 255 / MENU_IN_TIME );
		}
		break;

	case ETask_TitleState_Select:			// 項目選択中
		// 押された瞬間のみの入力状態を取得
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 &&
			TTData->SelectPosition > 0 )
		{
			// 上方向入力がされていて、且つ一番上の項目を選択していない場合は
			// カーソル移動音を鳴らし、選択項目を一つ上に移動する
			System_PlayCommonSE( ECommonSE_Cursor );
			TTData->SelectPosition--;
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 &&
			TTData->SelectPosition < ETask_TitleMenu_Num - 1 )
		{
			// 下方向入力がされていて、且つ一番下の項目を選択していない場合は
			// カーソル移動音を鳴らし、選択項目を一つ下に移動する
			System_PlayCommonSE( ECommonSE_Cursor );
			TTData->SelectPosition++;
		}

		if( ( EdgeInput & ( 1 << EInputType_Atk ) ) != 0 )
		{
			// 決定ボタンが押されたら選択音を鳴らし、フェードアウトを開始する
			if( TTData->SelectPosition == ETask_TitleMenu_Exit )
			{
				System_PlayCommonSE( ECommonSE_Cancel );
			}
			else
			{
				System_PlayCommonSE( ECommonSE_Enter );
			}

			System_FadeOut();

			TTData->State   = ETask_TitleState_FadeOut;
			TTData->Counter = 0;
		}
		break;

	case ETask_TitleState_FadeOut:			// フェードアウト中
		TTData->Counter += StepTime;
		if( TTData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了した後の処理

			// 一応メニューとタイトルの不透明度を０にする
			TTData->MenuOpacity  = 0;
			TTData->TitleOpacity = 0;
			
			// 状態を終了にする
			TTData->State = ETask_TitleState_End;

			// 選択した項目に応じて処理を分岐
			switch( TTData->SelectPosition )
			{
			case ETask_TitleMenu_GameStart:
				// ゲームスタートが選択されていたらゲーム本編を開始する
				if( !System_StartGameMain() )
				{
					return false;
				}
				break;

			case ETask_TitleMenu_KeyConfig:
				// キーコンフィグが選択されていたらキーコンフィグ画面を開始する
				if( !System_StartKeyConfig() )
				{
					return false;
				}
				break;

			case ETask_TitleMenu_Exit:
				// ゲーム終了が選択されていたらゲームを終了する
				System_ExitGame();
				break;
			}

			// タスクを削除する
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &TTData->TaskInfo );
		}
		else
		{
			// フェードアウト中のメニュー項目とタイトルの不透明度を算出
			TTData->TitleOpacity =
				255 - ( int )( TTData->Counter * 255 / FADE_OUT_TIME );

			TTData->MenuOpacity  = TTData->TitleOpacity;
		}
		break;

	case ETask_TitleState_End:				// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// タイトル画面の描画処理を行う
static void Task_Title_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;
	int              SelectColor;
	int              NormalColor;
	int              DrawColor;
	int	             DrawY;
	float            t;
	int              i;
	int              Bright;

	// タイトルの不透明度が０ではない場合のみタイトルを描画する
	if( TTData->TitleOpacity > 0 )
	{
		// タイトルの背景はタイトルの不透明度に応じて輝度を調節する
		Bright = TTData->TitleOpacity * 160 / 255;
		SetDrawBright( Bright/ 4, Bright, Bright / 2 );
		DrawGraph( 0, 0, TTData->BackGraphHandle, FALSE );
		SetDrawBright( 255,255,255 );

		// アルファブレンドモードでタイトルの不透明度を設定し、タイトル画像を描画する
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, TTData->TitleOpacity );
		DrawGraph( 0, 0, TTData->TitleGraphHandle, TRUE );
	}

	// メニュー項目の不透明度が０ではない場合のみメニュー項目を描画する
	if( TTData->MenuOpacity > 0 )
	{
		// 選択している項目の明滅する色の算出
		t = sin( ( float )TTData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
		if( t < 0.0f ) t = -t;
		SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

		// 通常の描画色の値を取得
		NormalColor = GetColor( 255,255,255 );

		// アルファブレンドモードでメニュー項目の不透明度を設定
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, TTData->MenuOpacity );

		// メニュー項目の数だけ繰り返し
		DrawY = MENU_Y;
		for( i = 0; i < ETask_TitleMenu_Num; i++ )
		{
			// 項目が選択されているかどうかで描画色を変える
			DrawColor = TTData->SelectPosition == i ? SelectColor : NormalColor;

			// 項目の描画
			DrawStringToHandle(
				MENU_X, DrawY, g_TitleMenuTable[ i ], DrawColor, TTData->MenuFontHandle);

			// 項目の描画Y座標を１項目分下に移動する
			DrawY += MENU_SPACE;
		}
	}

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// タイトル画面の後始末処理を行う
static void Task_Title_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_TitleData *TTData = ( STask_TitleData * )TInfo->Data;

	// タイトル画像の削除
	DeleteGraph( TTData->TitleGraphHandle );

	// タイトルの背景画像の削除
	DeleteGraph( TTData->BackGraphHandle );

	// メニュー項目描画用のフォントを削除
	DeleteFontToHandle( TTData->MenuFontHandle );
}

// タイトル画面の処理を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Task_Title_Start( void )
{
	STask_TitleData *TTData;

	// タイトル画面の処理で使用するデータを格納するメモリ領域の確保
	TTData = ( STask_TitleData * )calloc( 1, sizeof( STask_TitleData ) );
	if( TTData == NULL )
	{
		return false;
	}

	// タイトル画像の読み込み
	TTData->TitleGraphHandle  = LoadGraph( TITLEDATA_DIR "Swordbout_logo.png" );
	if( TTData->TitleGraphHandle == -1 )
	{
		return false;
	}

	// タイトルの背景画像の読み込み
	TTData->BackGraphHandle  = LoadGraph( TITLEDATA_DIR "TitleBack.png" );
	if( TTData->BackGraphHandle == -1 )
	{
		return false;
	}

	// タイトル画面で使用するフォントの作成
	TTData->MenuFontHandle    = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( TTData->MenuFontHandle == -1 )
	{
		return false;
	}

	// タイトル画面のフェードイン待ちの状態にする
	TTData->State            = ETask_TitleState_FadeInWait;
	TTData->Counter          = 0.0f;

	// 選択項目の初期化
	TTData->SelectPosition   = 0;

	// タイトルとメニュー項目の不透明度を初期化
	TTData->TitleOpacity     = 0;
	TTData->MenuOpacity      = 0;

	// 選択項目の明滅用カウンタの初期化
	TTData->MenuBrinkCounter = 0;

	// 黒フェードの解除を開始
	System_FadeIn();

	// タスクを登録する
	TTData->TaskInfo.Base     = &g_Task_TitleTaskBaseInfo;
	TTData->TaskInfo.Data     = TTData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &TTData->TaskInfo );

	// 正常終了
	return true;
}


