#ifndef SYSTEM_H
#define SYSTEM_H

#include "DxLib.h"
#include "Task.h"
#include "Chara.h"

// ゲーム画面の幅
#define GAME_SCREEN_WIDTH			(1280)

// ゲーム画面の高さ
#define GAME_SCREEN_HEIGHT			(720)

// 小フォントのサイズ
#define FONTHANDLE_SMALL_SIZE		(24)

// 中フォントのサイズ
#define FONTHANDLE_MIDIUM_SIZE		(32)

// 大フォントのサイズ
#define FONTHANDLE_BIG_SIZE			(80)

// フォントハンドル
typedef enum _EFontHandle
{
	EFontHandle_Small,		// 小フォントハンドル
	EFontHandle_Midium,		// 中フォントハンドル
	EFontHandle_Big,		// 大フォントハンドル

	EFontHandle_Num			// フォントハンドルの数
} EFontHandle;

// 共有効果音
typedef enum _ECommonSE
{
	ECommonSE_Alert,		// 警告
	ECommonSE_Cancel,		// キャンセル
	ECommonSE_Counter,		// カウント
	ECommonSE_Cursor,		// カーソル
	ECommonSE_Enter,		// 決定

	ECommonSE_Num,			// 共有効果音の数
} ECommonSE;

// 整数パラメータ情報
typedef struct _SParamInt
{
	// 整数パラメータ
	int         Param;

	// パラメータ名
	const char *Name;
} SParamInt;

// ソフトの処理を実行する
//     戻り値 : 正常に処理が完了したかどうか
//              ( true : 正常に処理を完了した   false : エラーが発生した )
extern bool System_Main(void);

// デバッグ起動メニューを開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool System_StartDebugStartMenu(void);

// ゲームメインを開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool System_StartGameMain(void);

// タイトル画面を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool System_StartTitle(void);

// キーコンフィグ画面を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool System_StartKeyConfig(void);

// 全ステージクリア演出を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
extern bool System_StartAllStageClear(void);

// ゲームを終了する
extern void System_ExitGame(void);

// 画面をフェードアウトさせる
extern void System_FadeOut(void);

// 画面をフェードインする
extern void System_FadeIn(void);

// 画面のフェード処理中かどうかを取得する
//     戻り値 : フェード処理中かどうか
//              ( true : フェード処理中   false : フェード処理は行われていない )
extern bool System_CheckFade(void);

// 共有効果音を再生する
extern void System_PlayCommonSE(
	// 効果音
	ECommonSE CommonSE
);

// プレイヤーキャラのキャラ情報構造体のアドレスを取得する
//     戻り値 : プレイヤーキャラのキャラ情報構造体のアドレス
extern SCharaInfo *System_GetPlayerCharaInfo(void);

// プレイヤーキャラのキャラ情報構造体のアドレスを登録する
extern void System_SetPlayerCharaInfo(
	// プレイヤーキャラのキャラ情報構造体のアドレス
	SCharaInfo *CInfo
);

// 指定サイズのフォントハンドルを取得する
//     戻り値 : フォントハンドル
extern int System_GetFontHandle(
	// フォントハンドルタイプ
	EFontHandle FontType
);

// ゲーム開始時のステージを設定する
extern void System_SetStartStage(
	// ステージ番号
	int StageNo
);

// ゲーム開始時のステージを取得する
//     戻り値 : ゲーム開始時のステージ番号
extern int System_GetStartStage(void);

// タスクシステム情報構造体のアドレスを取得する
//     戻り値 : タスクシステム情報構造体のアドレス
extern STaskSystemInfo *System_GetTaskSystemInfo(void);

// ゲームメインタスクのタスク情報構造体のアドレスを取得する
//     戻り値 : ゲームメインタスクのタスク情報構造体のアドレス
extern STaskInfo *System_GetGameMainTaskInfo(void);

// デバッグ情報を表示するかどうかのフラグを取得する
//     戻り値 : デバッグ情報を表示するかどうか
//              ( true : デバッグ情報を表示する  false : デバッグ情報を表示しない )
extern bool System_GetDispDebugInfo(void);

// 低処理負荷モードかどうかを取得する
extern bool System_GetLowSpecMode(void);

// 動的編集をする整数パラメータを登録する
extern void System_SetDebugParamInt(
	// 整数パラメータ配列の先頭アドレス
	SParamInt *ParamInt,

	// 整数パラメータの数
	int ParamNum
);




void Set_Exit_Game();
bool Get_Exit_Game();
void SystemInitialize();
void SystemMain();
//void Fade_io(int Color,int Fade_Flg);


enum eCol{
	eCol_White,
	eCol_EmeraldGreen,
	eCol_Black,
	eCol_Max,
};

//カラーテーブル0:白1:エメラルドグリーン2:黒
const int Color_ValTbl[eCol_Max] = {
	GetColor(255,255,255),
	GetColor(0,255,127),
	GetColor(0,0,0)
};

//システム系統の構造体
struct _SYSTEM_INFO_t{	//_SYSTEM_INFOは既存のライブラリで使用されていたので名前を変更しました by.Syara

	//ゲームを終了するかどうか
	bool Exit_Game;
	//全体のフェードアウトで使用するカウント変数
	int Fade_Out_Count;
	//Windowのハンドル
	HWND WinHndl;
	//Windowの座標値
	LPRECT WinAxis;
	//Windowのタイトル
	char WinTitle;

};

#endif