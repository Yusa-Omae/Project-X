#include "Task_KeyConfig.h"
#include "Task.h"
#include "Input.h"
#include "DxLib.h"
#include "System.h"
#include <math.h>

// キーコンフィグ画面のデータがあるフォルダのパス
#define KEYCONFIGDATA_DIR		"Data\\2D\\"

// キーコンフィグ画面をフェードインする前に待つ時間
#define FADE_IN_WAIT_TIME		(0.5f)

// キーコンフィグ画面のフェードインにかける時間
#define FADE_IN_TIME			(0.5f)

// メニュー項目の明滅１周にかける時間
#define MENU_BRINK_TIME			(2.0f)

// キーコンフィグ画面のフェードアウトにかける時間
#define FADE_OUT_TIME			(0.5f)

// キーコンフィグ画面で使用するフォントのサイズ
#define FONT_SIZE				(32)

// キーコンフィグ画面のタイトルを描画する座標
#define TITLE_X					(550)
#define TITLE_Y					(32)

// 入力の種類を描画するＹ座標
#define TYPE_Y					(100)

// メニュー項目の描画開始座標
#define MENU_X					(200)
#define MENU_Y					(150)

// メニュー項目一つあたりに空けるスペース
#define MENU_SPACE				(50)

// メニュー項目のパッド入力を描画するＸ座標
#define MENU_PAD_X				(550)

// メニュー項目のキー入力を描画するＸ座標
#define MENU_KEY_X				(950)

// キーコンフィグ処理の状態
typedef enum _ETask_KeyConfigState
{
	ETask_KeyConfigState_FadeInWait,	// フェードイン待ち中
	ETask_KeyConfigState_FadeIn,		// フェードイン中
	ETask_KeyConfigState_Select,		// 項目選択中
	ETask_KeyConfigState_FadeOut,		// フェードアウト中
	ETask_KeyConfigState_End,			// 終了状態
} ETask_KeyConfigState;

// キーコンフィグの項目
typedef enum _ETask_KeyConfigMenu
{
	ETask_KeyConfigMenu_Attack,			// 攻撃ボタン
	ETask_KeyConfigMenu_Defence,		// 防御ボタン
	ETask_KeyConfigMenu_Jump,			// ジャンプボタン
	ETask_KeyConfigMenu_Pause,			// ポーズボタン
	ETask_KeyConfigMenu_CameraLeft,		// カメラ左旋回
	ETask_KeyConfigMenu_CameraRight,	// カメラ右旋回
	ETask_KeyConfigMenu_CameraUp,		// カメラ上旋回
	ETask_KeyConfigMenu_CameraDown,		// カメラ下旋回

	ETask_KeyConfigMenu_Default,		// 設定をデフォルトに戻す
	ETask_KeyConfigMenu_Back,			// タイトル画面に戻る

	ETask_KeyConfigMenu_Num				// 項目の数
} ETask_KeyConfigMenu;

// キーコンフィグ処理用のデータ
typedef struct _STask_KeyConfigData
{
	// タスクの情報
	STaskInfo            TaskInfo;

	// 処理の状態
	ETask_KeyConfigState State;

	// 汎用カウンタ
	float                Counter;

	// 選択している項目
	int                  SelectPosition;

	// メニューの不透明度
	int                  MenuOpacity;

	// 選択項目の明滅処理用カウンタ
	float                MenuBrinkCounter;

	// キーコンフィグ画面で使用するフォントのハンドル
	int                  FontHandle;

	// キーコンフィグの背景画像
	int                  BackGraphHandle;
} STask_KeyConfigData;

// キーコンフィグの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_KeyConfig_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// キーコンフィグの描画処理を行う
static void Task_KeyConfig_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// キーコンフィグの後始末処理を行う
static void Task_KeyConfig_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// キーコンフィグ処理タスクの基本情報
static STaskBaseInfo g_Task_KeyConfigTaskBaseInfo =
{
	8,
	8,
	Task_KeyConfig_Step,
	Task_KeyConfig_Render,
	Task_KeyConfig_Terminate
};

// キーコンフィグ各項目の名前
static const char *g_KeyConfigMenuTable[] =
{
	"攻撃",
	"防御",
	"ジャンプ",
	"一時停止",
	"カメラ左旋回",
	"カメラ右旋回",
	"カメラ上旋回",
	"カメラ下旋回",
	"デフォルト設定に戻す",
	"タイトル画面に戻る",
};

// 各メニュー項目と入力情報との対応テーブル
static EInputType g_InputTypeTable[] =
{
	EInputType_Attack,
	EInputType_Defence,
	EInputType_Jump,
	EInputType_Pause,

	EInputType_Camera_Left,
	EInputType_Camera_Right,
	EInputType_Camera_Up,
	EInputType_Camera_Down,
};

// キーコンフィグの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_KeyConfig_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;
	int                  EdgeInput;

	// メニュー項目明滅用カウンタの時間を進める
	KCData->MenuBrinkCounter += StepTime;
	if( KCData->MenuBrinkCounter > MENU_BRINK_TIME )
	{
		KCData->MenuBrinkCounter -= MENU_BRINK_TIME;
	}

	// キーコンフィグ画面処理の状態によって処理を分岐
	switch( KCData->State )
	{
	case ETask_KeyConfigState_FadeInWait:	// フェードイン待ち中
		KCData->Counter += StepTime;
		if( KCData->Counter > FADE_IN_WAIT_TIME )
		{
			// フェードイン待ち時間が経過したらフェードインを開始する
			KCData->State   = ETask_KeyConfigState_FadeIn;
			KCData->Counter = 0;
		}
		break;

	case ETask_KeyConfigState_FadeIn:		// フェードイン中
		KCData->Counter += StepTime;
		if( KCData->Counter >= FADE_IN_TIME )
		{
			// フェードインが完了したら項目選択を開始する
			KCData->MenuOpacity = 255;
			KCData->State       = ETask_KeyConfigState_Select;
		}
		else
		{
			// フェードイン中のメニュー項目の不透明度を算出
			KCData->MenuOpacity = ( int )( KCData->Counter * 255 / FADE_IN_TIME );
		}
		break;

	case ETask_KeyConfigState_Select:		// 項目選択中
		// 押された瞬間のみの入力状態を取得
		EdgeInput = GetEdgeInput();

		if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 )
		{
			// 上方向入力がされたらカーソル移動音を鳴らし、選択項目を一つ上に移動する
			System_PlayCommonSE( ECommonSE_Cursor );

			if( KCData->SelectPosition == 0 )
			{
				KCData->SelectPosition = ETask_KeyConfigMenu_Num - 1;
			}
			else
			{
				KCData->SelectPosition--;
			}
		}

		if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 )
		{
			// 下方向入力がされたらカーソル移動音を鳴らし、選択項目を一つ上に移動する
			System_PlayCommonSE( ECommonSE_Cursor );

			if( KCData->SelectPosition == ETask_KeyConfigMenu_Num - 1 )
			{
				KCData->SelectPosition = 0;
			}
			else
			{
				KCData->SelectPosition++;
			}
		}

		// 選択項目が「設定をデフォルトに戻す」より上にあるかどうかで処理を分岐
		if( KCData->SelectPosition < ETask_KeyConfigMenu_Default )
		{
			// 選択項目が「設定をデフォルトに戻す」より上にある場合はキーやパッドの
			// 押下状態をチェックして、押下されていたらキーコンフィグ設定に反映させる
			if( UpdateInputTypeInfo( g_InputTypeTable[ KCData->SelectPosition ] ) )
			{
				// 押下があったら音を鳴らす
				System_PlayCommonSE( ECommonSE_Enter );
			}
		}
		else
		{
			// 選択項目が「設定をデフォルトに戻す」かそれより下にある場合

			if( ( EdgeInput & ( 1 << EInputType_Attack ) ) != 0 )
			{
				// 決定ボタンが押されたらカーソルの位置に応じて処理を分岐
				switch( KCData->SelectPosition )
				{
				case ETask_KeyConfigMenu_Default:
					// 「設定をデフォルトに戻す」が選択されていた場合は
					// キーコンフィグ設定をデフォルトに戻す
					System_PlayCommonSE( ECommonSE_Enter );

					SetDefaultSetting();
					break;

				case ETask_KeyConfigMenu_Back:
					// 「タイトルに戻る」が選択されていた場合はフェードアウトを開始する
					System_PlayCommonSE( ECommonSE_Cancel );

					System_FadeOut();

					KCData->State = ETask_KeyConfigState_FadeOut;
					KCData->Counter = 0;
					break;
				}
			}
		}
		break;

	case ETask_KeyConfigState_FadeOut:		// フェードアウト中
		KCData->Counter += StepTime;
		if( KCData->Counter > FADE_OUT_TIME )
		{
			// フェードアウトが完了した後の処理

			// 一応メニューの不透明度を０にする
			KCData->MenuOpacity = 0;

			// 状態を終了にする
			KCData->State = ETask_KeyConfigState_End;

			// タスクを削除する
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &KCData->TaskInfo );

			// キーコンフィグの設定を保存する
			SaveInputSetting();

			// タイトル画面を開始する
			if( !System_StartTitle() )
			{
				return false;
			}
		}
		else
		{
			// フェードアウト中のメニュー項目の不透明度を算出
			KCData->MenuOpacity = 255 - ( int )( KCData->Counter * 255 / FADE_OUT_TIME );
		}
		break;

	case ETask_KeyConfigState_End:			// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// キーコンフィグの描画処理を行う
static void Task_KeyConfig_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;
	int                  SelectColor;
	int                  NormalColor;
	int                  DrawColor;
	int                  DrawY;
	float                t;
	int                  i;
	char                 PadString[ 256 ];
	char                 KeyString[ 256 ];
	int                  Bright;

	// メニュー項目の不透明度が０の場合は何もせずに終了
	if( KCData->MenuOpacity == 0 )
	{
		return;
	}

	// 選択している項目の明滅する色の算出
	t = sin( ( float )KCData->MenuBrinkCounter * DX_TWO_PI_F / MENU_BRINK_TIME );
	if( t < 0.0f ) t = -t;
	SelectColor = GetColor( ( int )( t * 128 ) + 127, 0, 0 );

	// 通常の描画色の値を取得
	NormalColor = GetColor( 255,255,255 );

	// キーコンフィグの背景はメニュー項目の不透明度に応じて輝度を調節する
	Bright = KCData->MenuOpacity * 80 / 255;
	SetDrawBright( Bright, Bright, Bright );
	DrawGraph( 0, 0, KCData->BackGraphHandle, FALSE );
	SetDrawBright( 255,255,255 );

	// アルファブレンドモードでメニュー項目の不透明度を設定
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, KCData->MenuOpacity );

	// キーコンフィグ画面のタイトルを描画
	DrawStringToHandle(
		TITLE_X, TITLE_Y, "KEY CONFIG", NormalColor, KCData->FontHandle );

	// 入力の種類のゲームパッドを示す「PAD」の描画
	DrawStringToHandle(
		MENU_PAD_X, TYPE_Y, "PAD", NormalColor, KCData->FontHandle );

	// 入力の種類のキーボードキーを示す「KEY」の描画
	DrawStringToHandle(
		MENU_KEY_X, TYPE_Y, "KEY", NormalColor, KCData->FontHandle );

	// メニュー項目の数だけ繰り返し
	DrawY = MENU_Y;
	for( i = 0; i < ETask_KeyConfigMenu_Num; i++ )
	{
		// 項目が選択されているかどうかで描画色を変える
		DrawColor = KCData->SelectPosition == i ? SelectColor : NormalColor;

		// 項目名の描画
		DrawStringToHandle(
			MENU_X, DrawY, g_KeyConfigMenuTable[ i ], DrawColor, KCData->FontHandle );

		// 「デフォルトの設定に戻す」より上の項目の場合は、
		// ゲームの入力に割り当てられているゲームパッドの入力とキーの入力を描画する
		if( i < ETask_KeyConfigMenu_Default )
		{
			// ゲームの入力に割り当てられているパッドの入力とキーの入力の名前を取得する
			GetInputTypeString( g_InputTypeTable[ i ], PadString, KeyString );

			// 割り当てられているパッドの入力名を描画する
			DrawStringToHandle(
				MENU_PAD_X, DrawY, PadString, DrawColor, KCData->FontHandle );

			// 割り当てられているキーの入力名を描画する
			DrawStringToHandle(
				MENU_KEY_X, DrawY, KeyString, DrawColor, KCData->FontHandle );
		}

		// 項目の描画Y座標を１項目分下に移動する
		DrawY += MENU_SPACE;
	}

	// ブレンドモードを標準の状態に戻す
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );
}

// キーコンフィグの後始末処理を行う
static void Task_KeyConfig_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_KeyConfigData *KCData = ( STask_KeyConfigData * )TInfo->Data;

	// キーコンフィグ画面用に作成したフォントハンドルを削除する
	DeleteFontToHandle( KCData->FontHandle );

	// キーコンフィグの背景画像の削除
	DeleteGraph( KCData->BackGraphHandle );
}

// キーコンフィグ画面を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Task_KeyConfig_Start( void )
{
	STask_KeyConfigData *KCData;

	// キーコンフィグ画面の処理で使用するデータを格納するメモリ領域の確保
	KCData = ( STask_KeyConfigData * )calloc( 1, sizeof( STask_KeyConfigData ) );
	if( KCData == NULL )
	{
		return false;
	}

	// キーコンフィグ画面で使用するフォントの作成
	KCData->FontHandle = CreateFontToHandle( NULL, FONT_SIZE, 8, DX_FONTTYPE_ANTIALIASING_8X8 );
	if( KCData->FontHandle == -1 )
	{
		return false;
	}

	// キーコンフィグ画面の背景画像の読み込み
	KCData->BackGraphHandle  = LoadGraph( KEYCONFIGDATA_DIR "KeyConfigBack.png" );
	if( KCData->BackGraphHandle == -1 )
	{
		return false;
	}

	// メニュー項目のフェードイン待ちの状態にする
	KCData->State            = ETask_KeyConfigState_FadeInWait;
	KCData->Counter          = 0.0f;

	// 選択項目の初期化
	KCData->SelectPosition   = 0;

	// メニュー項目の不透明度を初期化
	KCData->MenuOpacity      = 0;

	// 選択項目の明滅用カウンタの初期化
	KCData->MenuBrinkCounter = 0;

	// 黒フェードの解除を開始
	System_FadeIn();

	// タスクを登録する
	KCData->TaskInfo.Base = &g_Task_KeyConfigTaskBaseInfo;
	KCData->TaskInfo.Data = KCData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &KCData->TaskInfo );

	// 正常終了
	return true;
}


