#include "System.h"
#include "DxLib.h"
#include "Chara.h"
#include "Effect.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"
#include "GameMainRender.h"
#include "Camera.h"
#include "Task_GameMain.h"
#include "Task_Title.h"
#include "Task_KeyConfig.h"
#include "Task_AllStageClear.h"
#include "Task_DebugStartMenu.h"
#include "Mathematics.h"
#include "Code/Common/Mouse/Mouse.h"

// フェードアウト、フェードインの速度
#define FADE_SPEED			(1.0f)

// １フレームの最大時間
#define MAX_DELTA_TIME		(1.0f / 120.0f)

// 一度に処理する最大フレーム数
#define MAX_FRAME_NUM		(8)

// 編集できる整数パラメータの最大数
#define DEBUG_INT_MAXNUM	(20)

// システムの情報
typedef struct _SSystemInfo
{
	// 一つ前のフレームの時間
	LONGLONG        PrevTime;

	// 状態推移処理で推移させる時間
	float           StepTime;

	// １フレームで状態推移処理を行う回数
	int             StepNum;

	// ソフトを終了するかどうかのフラグ
	bool            ExitGame;

	// プレイヤーキャラのキャラ情報構造体のアドレスを格納するポインタ変数
	SCharaInfo *    PlayerCharaInfo;

	// ゲーム開始時のステージ
	int             StartStage;

	// 動的編集をする整数パラメータの数
	int             DebugParamIntNum;

	// 動的編集をする整数パラメータ配列の先頭アドレスを格納するポインタ変数
	SParamInt *     DebugParamInt;

	// デバッグ情報を表示するかどうかのフラグ
	bool            DispDebugInfo;

	// タスクシステム情報
	STaskSystemInfo TaskSystemInfo;

	// ソフト全体で使用するフォントハンドル
	int             FontHandle[EFontHandle_Num];

	// 共有効果音のハンドル
	int				CommonSEHandle[ECommonSE_Num];

	// フェード処理を行っているかどうかのフラグ
	bool            Fade;

	// フェードアウト状態かどうかのフラグ
	bool            FadeOut;

	// フェードアウト率
	float           FadeOutRate;

	// 前回フレームレート表示用にフレームカウントをリセットした時間
	LONGLONG        BackDispFrameTime;

	// フレームレート表示用のフレームカウンタ
	int             FrameCounter;

	// フレームレート表示で表示するフレームレート
	int             DispFrameCount;

	// ゲームメインタスクのタスク情報構造体のアドレスを格納するポインタ変数
	STaskInfo *     GameMainTaskInfo;

	// 低処理負荷モードかどうかのフラグ
	bool			LowSpecMode;
} SSystemInfo;

// 共有効果音のファイル名
static const char *g_CommonSEFileName[ECommonSE_Num] =
{
	"system_alert.wav",
	"system_cancel.wav",
	"system_counter.wav",
	"system_cursor.wav",
	"system_enter.wav",
};

// システムの情報
static SSystemInfo g_SystemInfo;

// 状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool System_Step(
	// 推移させる時間( 単位 : 秒 )
	float StepTime
);

// 描画処理を行う
static void System_Render(void);

// 初期化処理を行う
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した   false : 失敗した )
static bool System_Initialize(void);

// メインループを実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool System_MainLoop(void);

// 後始末を行う
static void System_Terminate(void);

// フェード処理の状態推移処理を行う
static void System_FadeStep(
	// 推移させる時間( 単位 : 秒 )
	float StepTime
);

// フェード処理の描画を行う
static void System_FadeRender(void);

// デバッグ関係の状態推移処理を行う
static void System_DebugStep(void);

// デバッグ関係の描画処理を行う
static void System_DebugDraw(void);

// フェード処理の状態推移処理を行う
static void System_FadeStep(
	// 推移させる時間( 単位 : 秒 )
	float StepTime
)
{
	// フェード処理を行っているかどうかのフラグが倒れている場合は何もせず終了
	if (!g_SystemInfo.Fade)
	{
		return;
	}

	// フェードアウト率の変更
	if (ParamChangeFloat(&g_SystemInfo.FadeOutRate,
		g_SystemInfo.FadeOut ? 1.0f : 0.0f, StepTime, FADE_SPEED))
	{
		// 値の変更が完了したらフェードアウトを行っているかどうかのフラグを倒す
		g_SystemInfo.Fade = false;
	}
}

// フェード処理の描画を行う
static void System_FadeRender(void)
{
	int Alpha;

	// フェードアウト率から不透明度を計算
	Alpha = (int)(g_SystemInfo.FadeOutRate * 255.0f);

	// 不透明度が０以上なら描画
	if (Alpha > 0)
	{
		// 画面全体にフェードアウト用の黒い四角を描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawBox(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, GetColor(0, 0, 0), TRUE);
	}
}

// デバッグ関係の状態推移処理を行う
static void System_DebugStep(void)
{
	int EditParamInt;
	int EditParamIntAdd;

	// デバッグ情報を表示するかどうかのフラグが倒れているかどうかで処理を分岐
	if (!g_SystemInfo.DispDebugInfo)
	{
		// \キーが押されたらデバッグ情報表示をＯＮにする
		if (GetInputChar(TRUE) == '\\')
		{
			g_SystemInfo.DispDebugInfo = true;
		}
	}
	else
	{
		EditParamInt = -1;

		// 押されたキーによって処理を分岐
		switch (GetInputChar(TRUE))
		{
		case '\\':	// デバッグ表示をＯＦＦにする
			g_SystemInfo.DispDebugInfo = false;
			break;

			// 整数パラメータの動的編集処理
		case '1': EditParamInt = 0; EditParamIntAdd = 1; break;
		case 'q': EditParamInt = 0; EditParamIntAdd = -1; break;
		case '2': EditParamInt = 1; EditParamIntAdd = 1; break;
		case 'w': EditParamInt = 1; EditParamIntAdd = -1; break;
		case '3': EditParamInt = 2; EditParamIntAdd = 1; break;
		case 'e': EditParamInt = 2; EditParamIntAdd = -1; break;
		case '4': EditParamInt = 3; EditParamIntAdd = 1; break;
		case 'r': EditParamInt = 3; EditParamIntAdd = -1; break;
		case '5': EditParamInt = 4; EditParamIntAdd = 1; break;
		case 't': EditParamInt = 4; EditParamIntAdd = -1; break;
		case '6': EditParamInt = 5; EditParamIntAdd = 1; break;
		case 'y': EditParamInt = 5; EditParamIntAdd = -1; break;
		case '7': EditParamInt = 6; EditParamIntAdd = 1; break;
		case 'u': EditParamInt = 6; EditParamIntAdd = -1; break;
		case '8': EditParamInt = 7; EditParamIntAdd = 1; break;
		case 'i': EditParamInt = 7; EditParamIntAdd = -1; break;
		case '9': EditParamInt = 8; EditParamIntAdd = 1; break;
		case 'o': EditParamInt = 8; EditParamIntAdd = -1; break;
		case '0': EditParamInt = 9; EditParamIntAdd = 1; break;
		case 'p': EditParamInt = 9; EditParamIntAdd = -1; break;

		case 'a': EditParamInt = 10; EditParamIntAdd = 1; break;
		case 'z': EditParamInt = 10; EditParamIntAdd = -1; break;
		case 's': EditParamInt = 11; EditParamIntAdd = 1; break;
		case 'x': EditParamInt = 11; EditParamIntAdd = -1; break;
		case 'd': EditParamInt = 12; EditParamIntAdd = 1; break;
		case 'c': EditParamInt = 12; EditParamIntAdd = -1; break;
		case 'f': EditParamInt = 13; EditParamIntAdd = 1; break;
		case 'v': EditParamInt = 13; EditParamIntAdd = -1; break;
		case 'g': EditParamInt = 14; EditParamIntAdd = 1; break;
		case 'b': EditParamInt = 14; EditParamIntAdd = -1; break;
		case 'h': EditParamInt = 15; EditParamIntAdd = 1; break;
		case 'n': EditParamInt = 15; EditParamIntAdd = -1; break;
		case 'j': EditParamInt = 16; EditParamIntAdd = 1; break;
		case 'm': EditParamInt = 16; EditParamIntAdd = -1; break;
		case 'k': EditParamInt = 17; EditParamIntAdd = 1; break;
		case ',': EditParamInt = 17; EditParamIntAdd = -1; break;
		case 'l': EditParamInt = 18; EditParamIntAdd = 1; break;
		case '.': EditParamInt = 18; EditParamIntAdd = -1; break;
		case ';': EditParamInt = 19; EditParamIntAdd = 1; break;
		case '/': EditParamInt = 19; EditParamIntAdd = -1; break;
		}

		// 動的編集処理用に整数パラメータが設定されている場合は編集処理を行う
		if (g_SystemInfo.DebugParamInt != NULL &&
			EditParamInt >= 0 &&
			g_SystemInfo.DebugParamIntNum > EditParamInt)
		{
			g_SystemInfo.DebugParamInt[EditParamInt].Param += EditParamIntAdd;
		}
	}
}

// デバッグ関係の描画処理を行う
static void System_DebugDraw(void)
{
	int                i;
	static const char *KeyName[] =
	{
		"1/q", "2/w", "3/e", "4/r", "5/t", "6/y", "7/u", "8/i", "9/o", "0/p",
		"a/z", "s/x", "d/c", "f/v", "g/b", "h/n", "j/m", "k/,", "l/.", ";//"
	};

	// デバッグ表示をするかどうかのフラグが倒れている場合は何もせずに終了
	if (!g_SystemInfo.DispDebugInfo)
	{
		return;
	}

	// 状態推移処理１回当たりの時間を描画
	DrawFormatString(0, GAME_SCREEN_HEIGHT - 16, GetColor(255, 255, 255),
		"StepTime:%fsec", g_SystemInfo.StepTime);

	// 動的編集の対象になっている整数パラメータの描画
	for (i = 0; i < g_SystemInfo.DebugParamIntNum; i++)
	{
		DrawFormatString(
			0, 64 + i * 16,
			GetColor(255, 255, 255),
			"%s %20s:%d",
			KeyName[i],
			g_SystemInfo.DebugParamInt[i].Name,
			g_SystemInfo.DebugParamInt[i].Param
		);
	}
}

// 初期化処理を行う
//     戻り値 : 初期化に成功したかどうか
//              ( true : 成功した   false : 失敗した )
static bool System_Initialize(void)
{
	int  i;
	char FilePath[MAX_PATH];

	// ウインドウモードで起動するか確認する
	if (MessageBox(NULL, "ウインドウモードで起動しますか？", "画面モード確認", MB_YESNO) == IDYES)
	{
		// 「はい」が選択された場合はウインドウモードで起動
		ChangeWindowMode(TRUE);
	}

	// 低処理負荷モードで起動するか確認する
	if (MessageBox(NULL, "低処理負荷モードで起動しますか？", "処理負荷モード確認", MB_YESNO) == IDYES)
	{
		// 「はい」が選択された場合は低処理負荷モードフラグを立てる
		g_SystemInfo.LowSpecMode = true;
	}

	// ゲーム画面の解像度を設定
	SetGraphMode(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, 32);

	// ウインドウのタイトルを設定
	SetMainWindowText("Sword Bout");

	// 低処理負荷モードかどうかでXAudioを使用するかどうかの設定を変更する
	SetEnableXAudioFlag(g_SystemInfo.LowSpecMode ? FALSE : TRUE);

	// ３Ｄサウンドの１メートル当たりの値をセット
	Set3DSoundOneMetre(100.0f);

	// ＤＸライブラリを初期化
	if (DxLib_Init() == -1)
	{
		return false;
	}

	// フォントハンドルの作成
	g_SystemInfo.FontHandle[EFontHandle_Small] = CreateFontToHandle(
		NULL, FONTHANDLE_SMALL_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	g_SystemInfo.FontHandle[EFontHandle_Midium] = CreateFontToHandle(
		NULL, FONTHANDLE_MIDIUM_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	g_SystemInfo.FontHandle[EFontHandle_Big] = CreateFontToHandle(
		NULL, FONTHANDLE_BIG_SIZE, 8, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 2);

	for (i = 0; i < EFontHandle_Num; i++)
	{
		if (g_SystemInfo.FontHandle[i] == -1)
		{
			return false;
		}
	}

	// 共有効果音の読み込み
	for (i = 0; i < ECommonSE_Num; i++)
	{
		sprintf(FilePath, "Data\\Sound\\SE\\Common\\%s", g_CommonSEFileName[i]);
		g_SystemInfo.CommonSEHandle[i] = LoadSoundMem(FilePath);
		if (g_SystemInfo.CommonSEHandle[i] == -1)
		{
			return false;
		}
	}

	// プレイヤーキャラのキャラ情報構造体のアドレスを格納するポインタ変数の初期化
	g_SystemInfo.PlayerCharaInfo = NULL;

	// フェードアウト関係の変数を初期化
	g_SystemInfo.FadeOut = true;
	g_SystemInfo.Fade = false;
	g_SystemInfo.FadeOutRate = 1.0f;

	// タスクシステムの初期化
	TaskSystem_Initialize(&g_SystemInfo.TaskSystemInfo);

	// サウンド処理の初期化
	if (!Sound_Initialize())
	{
		return false;
	}

	// ゲームメインの描画処理の初期化
	if (!GameMainRender_Initialize())
	{
		return false;
	}

	// 入力処理の初期化
	InputInitialize();

	// キャラクターの処理の初期化
	if (!Chara_Initialize())
	{
		return false;
	}

	// エフェクトの処理の初期化
	if (!Effect_Initialize())
	{
		return false;
	}

	// ステージの処理の初期化
	if (!Stage_Initialize())
	{
		return false;
	}

	// カメラの処理の初期化
	Camera_Initialize();

#ifdef DEBUG_START_MODE
	// デバッグスタートモードの場合はデバッグメニューから開始する
	if (!System_StartDebugStartMenu())
	{
		return false;
	}
#else
	// 通常モードの場合はタイトルメニューから開始する
	if (!System_StartTitle())
	{
		return false;
	}
#endif

	// フレームレート計測用の変数の初期化
	g_SystemInfo.BackDispFrameTime = GetNowHiPerformanceCount();
	g_SystemInfo.FrameCounter = 0;
	g_SystemInfo.DispFrameCount = 0;

	// 成功終了
	return true;
}

// メインループを実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool System_MainLoop(void)
{
	LONGLONG NowTime;
	int      i;

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// 最初のフレームの状態推移時間などをセット
	g_SystemInfo.StepTime = MAX_DELTA_TIME;
	g_SystemInfo.PrevTime = GetNowHiPerformanceCount();
	g_SystemInfo.StepNum = 1;

	// メインループ
	while (ProcessMessage() == 0)
	{
		// 状態推移処理を行う回数分ループ
		for (i = 0; i < g_SystemInfo.StepNum; i++)
		{
			Mouse_Update();

			// エスケープキーが押されるか、
			// ソフトを終了するかどうかのフラグが立っていたらループを抜ける
			if (CheckHitKey(KEY_INPUT_ESCAPE) == 1 || g_SystemInfo.ExitGame)
			{
				g_SystemInfo.ExitGame = true;
				break;
			}

#ifdef _DEBUG
			// デバッグ関係の状態推移処理を行う
			System_DebugStep();
#endif

			// 状態推移処理を行う
			if (!System_Step(g_SystemInfo.StepTime))
			{
				return false;
			}
		}

		// ソフトを終了するかどうかのフラグが立っていたらループを抜ける
		if (g_SystemInfo.ExitGame)
		{
			break;
		}

		// 描画処理を行う
		System_Render();

		// Ｓキーが押されていたらゲーム画面のスクリーンショットを保存する
		if (CheckHitKey(KEY_INPUT_S) != 0)
		{
			SaveDrawScreen(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,
				"ScreenShot.bmp");
		}

		// 裏画面の内容を表画面に反映させる
		ScreenFlip();

		// 現在の時間を取得
		NowTime = GetNowHiPerformanceCount();

		// 前回取得した時間からの経過時間を算出
		g_SystemInfo.StepTime = (NowTime - g_SystemInfo.PrevTime) / 1000000.0f;

		// 状態推移処理を行う回数を算出
		g_SystemInfo.StepNum = (int)(g_SystemInfo.StepTime / MAX_DELTA_TIME);

		// 状態推移処理を行う回数が０回の場合は１回にする
		if (g_SystemInfo.StepNum == 0)
		{
			g_SystemInfo.StepNum = 1;
		}
		else
		{
			// ０回ではない場合は状態推移で進める時間を状態推移を行う回数で割る
			g_SystemInfo.StepTime /= g_SystemInfo.StepNum;

			// もし状態推移を行う回数が最大値を超えていたら最大値に抑える
			if (g_SystemInfo.StepNum > MAX_FRAME_NUM)
			{
				g_SystemInfo.StepNum = MAX_FRAME_NUM;
			}
		}

		// 今回の時間を保存する
		g_SystemInfo.PrevTime = NowTime;

		// フレームレート計測用のカウンタを１足す
		g_SystemInfo.FrameCounter++;

		// 前回フレームレートを更新してから１秒経過したら表示用フレームカウントを更新する
		if (NowTime - g_SystemInfo.BackDispFrameTime > 1000000)
		{
			// 表示用フレームカウントを更新
			g_SystemInfo.DispFrameCount = g_SystemInfo.FrameCounter;

			// 計測用フレームカウンタを初期化
			g_SystemInfo.FrameCounter = 0;

			// 表示用フレームカウントを更新した時間を更新
			g_SystemInfo.BackDispFrameTime = NowTime;
		}

		// 画面を初期化する
		ClearDrawScreen();
	}

	// 正常終了
	return true;
}

// 状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool System_Step(
	// 推移させる時間( 単位 : 秒 )
	float StepTime
)
{
	// 入力処理を行う
	ProcessInput();

	// フェード処理の状態推移を行う
	System_FadeStep(StepTime);

	// タスクシステムの状態推移処理を行う
	if (!TaskSystem_Step(&g_SystemInfo.TaskSystemInfo, StepTime))
	{
		return false;
	}

	// 正常終了
	return true;
}

// 描画処理を行う
static void System_Render(void)
{
	// タスクシステムの描画処理を行う
	TaskSystem_Render(&g_SystemInfo.TaskSystemInfo);

	// Ｚバッファは使用しない設定にする
	SetUseZBufferFlag(FALSE);

	// フェード処理の描画を行う
	System_FadeRender();

	// フレームレートを画面右下に描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawFormatString(GAME_SCREEN_WIDTH - 84, GAME_SCREEN_HEIGHT - 20,
		GetColor(255, 255, 255), "FPS:%d", (int)g_SystemInfo.DispFrameCount);

	// デバッグ関係の描画処理を行う
	System_DebugDraw();
}

// 後始末を行う
static void System_Terminate(void)
{
	int i;

	// サウンド処理の後始末
	Sound_Terminate();

	// キャラクターの処理の後始末
	Chara_Terminate();

	// エフェクトの処理の後始末
	Effect_Terminate();

	// ゲームメイン描画処理の後始末
	GameMainRender_Terminate();

	// フォントハンドルの削除
	for (i = 0; i < EFontHandle_Num; i++)
	{
		DeleteFontToHandle(g_SystemInfo.FontHandle[i]);
		g_SystemInfo.FontHandle[i] = -1;
	}

	// 共有効果音の削除
	for (i = 0; i < ECommonSE_Num; i++)
	{
		DeleteSoundMem(g_SystemInfo.CommonSEHandle[i]);
		g_SystemInfo.CommonSEHandle[i] = -1;
	}

	// ＤＸライブラリの終了処理
	DxLib_End();
}

// ソフトの処理を実行する
//     戻り値 : 正常に処理が完了したかどうか
//              ( true : 正常に処理を完了した   false : エラーが発生した )
bool System_Main(void)
{
	// 初期化処理を行う
	if (!System_Initialize())
	{
		return false;
	}

	// メインループ処理を行う
	if (!System_MainLoop())
	{
		return false;
	}

	// 後始末を行う
	System_Terminate();

	// 正常終了
	return true;
}

// デバッグ起動メニューを開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool System_StartDebugStartMenu(void)
{
	if (!Task_DebugStartMenu_Start())
	{
		return false;
	}

	// 正常終了
	return true;
}

// ゲームメインを開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool System_StartGameMain(void)
{
	g_SystemInfo.GameMainTaskInfo = Task_GameMain_Start();
	if (g_SystemInfo.GameMainTaskInfo == NULL)
	{
		return false;
	}

	// 正常終了
	return true;
}

// タイトル画面を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool System_StartTitle(void)
{
	if (!Task_Title_Start())
	{
		return false;
	}

	// 正常終了
	return true;
}

// キーコンフィグ画面を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool System_StartKeyConfig(void)
{
	if (!Task_KeyConfig_Start())
	{
		return false;
	}

	// 正常終了
	return true;
}

// 全ステージクリア演出を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool System_StartAllStageClear(void)
{
	if (!Task_AllStageClear_Start())
	{
		return false;
	}

	// 正常終了
	return true;
}

// ゲームを終了する
void System_ExitGame(void)
{
	g_SystemInfo.ExitGame = true;
}

// 画面をフェードアウトさせる
void System_FadeOut(void)
{
	g_SystemInfo.FadeOut = true;
	g_SystemInfo.Fade = true;
}

// 画面をフェードインする
void System_FadeIn(void)
{
	g_SystemInfo.FadeOut = false;
	g_SystemInfo.Fade = true;
}

// 画面のフェード処理中かどうかを取得する
//     戻り値 : フェード処理中かどうか
//              ( true : フェード処理中   false : フェード処理は行われていない )
bool System_CheckFade(void)
{
	return g_SystemInfo.Fade;
}

// 共有効果音を再生する
void System_PlayCommonSE(
	// 効果音
	ECommonSE CommonSE
)
{
	PlaySoundMem(g_SystemInfo.CommonSEHandle[CommonSE], DX_PLAYTYPE_BACK);
}

// プレイヤーキャラのキャラ情報構造体のアドレスを取得する
//     戻り値 : プレイヤーキャラのキャラ情報構造体のアドレス
SCharaInfo *System_GetPlayerCharaInfo(void)
{
	return g_SystemInfo.PlayerCharaInfo;
}

// プレイヤーキャラのキャラ情報構造体のアドレスを登録する
void System_SetPlayerCharaInfo(
	// プレイヤーキャラのキャラ情報構造体のアドレス
	SCharaInfo *CInfo
)
{
	g_SystemInfo.PlayerCharaInfo = CInfo;
}

// 指定サイズのフォントハンドルを取得する
//     戻り値 : フォントハンドル
int System_GetFontHandle(
	// フォントハンドルタイプ
	EFontHandle FontType
)
{
	return g_SystemInfo.FontHandle[FontType];
}

// ゲーム開始時のステージを設定する
void System_SetStartStage(
	// ステージ番号
	int StageNo
)
{
	g_SystemInfo.StartStage = StageNo;
}

// ゲーム開始時のステージを取得する
//     戻り値 : ゲーム開始時のステージ番号
int System_GetStartStage(void)
{
	return g_SystemInfo.StartStage;
}

// タスクシステム情報構造体のアドレスを取得する
//     戻り値 : タスクシステム情報構造体のアドレス
STaskSystemInfo *System_GetTaskSystemInfo(void)
{
	return &g_SystemInfo.TaskSystemInfo;
}

// ゲームメインタスクのタスク情報構造体のアドレスを取得する
//     戻り値 : ゲームメインタスクのタスク情報構造体のアドレス
STaskInfo * System_GetGameMainTaskInfo(void)
{
	return g_SystemInfo.GameMainTaskInfo;
}

// デバッグ情報を表示するかどうかのフラグを取得する
//     戻り値 : デバッグ情報を表示するかどうか
//              ( true : デバッグ情報を表示する  false : デバッグ情報を表示しない )
bool System_GetDispDebugInfo(void)
{
	return g_SystemInfo.DispDebugInfo;
}

// 低処理負荷モードかどうかを取得する
bool System_GetLowSpecMode(void)
{
	return g_SystemInfo.LowSpecMode;
}

// 動的編集をする整数パラメータを登録する
void System_SetDebugParamInt(
	// 整数パラメータ配列の先頭アドレス
	SParamInt *ParamInt,

	// 整数パラメータの数
	int ParamNum
)
{
	g_SystemInfo.DebugParamInt = ParamInt;
	g_SystemInfo.DebugParamIntNum = ParamNum;

	// 動的に編集できる最大数を超えていたら値を補正する
	if (g_SystemInfo.DebugParamIntNum > DEBUG_INT_MAXNUM)
	{
		g_SystemInfo.DebugParamIntNum = DEBUG_INT_MAXNUM;
	}
}




/*
#include<math.h>
#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"


bool Debug_flg;

//SYSTEM_INFOは既存のライブラリ内で宣言されていたので名前を変えました by.Syara
static _SYSTEM_INFO_t s_SYSTEM_INFO;


void Set_Debug_Mode(){

	if(Debug_flg == 0){
		Debug_flg = 1;
	}else{
		Debug_flg = 0;
	}
}

bool Get_Debug_mode(){
	return Debug_flg;
}

void SystemInitialize(){

	// ウインドウモードで起動するか確認する
	//if( MessageBox( NULL, "ウインドウモードで起動しますか？", "画面モード確認", MB_YESNO ) == IDYES )
	//{
	//	// 「はい」が選択された場合はウインドウモードで起動
	//	ChangeWindowMode( TRUE );
	//}
	//if(MessageBox(NULL,"デバッグモードで起動しますか？","デバッグ確認",MB_YESNO) == IDYES){
	//	Set_Debug_Mode();
	//}else{
	//	Debug_flg = 0;
	//}

	//ゲーム終了フラグのオフ
	s_SYSTEM_INFO.Exit_Game = true;
	//ウィンドウハンドルの取得
	s_SYSTEM_INFO.WinHndl = GetMainWindowHandle();


}

void SystemMain(){

}

void Set_Exit_Game(){

	s_SYSTEM_INFO.Exit_Game = false;
}

bool Get_Exit_Game(){
	return s_SYSTEM_INFO.Exit_Game;
}

////フェードインアウト関数	true:FadeIN		false:FadeOut
//void Fade_io(int Color,int Fade_Flg){
//
//	SYSTEM_INFO.Fade_Out_Count = 0;
//	if(Fade_Flg == true){			
//		for(int i = 255;0 < i;i -= 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}else{
//		for(int i = 0;255 > i;i += 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}
//}

HWND GetConsoleHwnd(){

	return NULL;

}
*/