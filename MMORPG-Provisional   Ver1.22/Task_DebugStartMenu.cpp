#include "Task_DebugStartMenu.h"
#include "Task.h"
#include "System.h"
#include "DxLib.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"

// カーソルの描画X座標
#define CURSOR_X		(24)

// メニュー項目の描画開始座標
#define MENU_X			(64)
#define MENU_Y			(54)

// メニュー項目一つあたりに空けるスペース
#define MENU_SPACE		(36)

// デバッグ起動メニューの項目
typedef enum _ETask_DebugStartMenu
{
	ETask_DebugStartMenu_StageSelect,	// ステージ選択
	ETask_DebugStartMenu_Title,			// タイトル画面
	ETask_DebugStartMenu_KeyConfig,		// キーコンフィグ画面
	ETask_DebugStartMenu_AllStageClear,	// 全ステージクリア画面
	ETask_DebugStartMenu_Num,			// 項目の数
} ETask_DebugStartMenu;

// デバッグ起動画面処理用のデータ
typedef struct _STask_DebugStartMenuInfo
{
	// タスクの情報
	STaskInfo TaskInfo;

	// 選択している項目
	int       SelectPosition;

	// 選択しているステージ番号
	int       SelectStageNo;
} STask_DebugStartMenuInfo;

// デバッグ起動画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_DebugStartMenu_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
);

// デバッグ起動画面の描画処理を行う
static void Task_DebugStartMenu_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// デバッグ起動画面処理タスクの基本情報
static STaskBaseInfo g_Task_DebugStartMenuTaskBaseInfo =
{
	8,
	8,
	Task_DebugStartMenu_Step,
	Task_DebugStartMenu_Render,
	NULL
};

// デバッグ起動画面の状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_DebugStartMenu_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_DebugStartMenuInfo *DSMData = ( STask_DebugStartMenuInfo * )TInfo->Data;
	int                       EdgeInput;
	int                       TotalStageNum;

	// 押された瞬間のみの入力状態を取得
	EdgeInput = GetEdgeInput();

	// 全ステージ数を取得
	TotalStageNum = StageData_GetTotalStageNum();

	// ステージ選択項目が選択されている場合は処理を分岐
	if( DSMData->SelectPosition == ETask_DebugStartMenu_StageSelect )
	{
		// 左方向入力がされていた場合は選択ステージを１マイナスする
		if( ( EdgeInput & ( 1 << EInputType_Left ) ) != 0 )
		{
			System_PlayCommonSE( ECommonSE_Cursor );

			DSMData->SelectStageNo--;
			if( DSMData->SelectStageNo < 0 )
			{
				DSMData->SelectStageNo = TotalStageNum - 1;
			}
		}

		// 右方向入力がされていた場合は選択ステージを１プラスする
		if( ( EdgeInput & ( 1 << EInputType_Right ) ) != 0 )
		{
			System_PlayCommonSE( ECommonSE_Cursor );

			DSMData->SelectStageNo++;
			if( DSMData->SelectStageNo == TotalStageNum )
			{
				DSMData->SelectStageNo = 0;
			}
		}
	}

	// 上方向入力がされていた場合は選択項目を一つ上に移動する
	if( ( EdgeInput & ( 1 << EInputType_Up ) ) != 0 )
	{
		System_PlayCommonSE( ECommonSE_Cursor );

		DSMData->SelectPosition--;
		if( DSMData->SelectPosition < 0 )
		{
			DSMData->SelectPosition = ETask_DebugStartMenu_Num - 1;
		}
	}

	// 上方向入力がされていた場合は選択項目を一つ下に移動する
	if( ( EdgeInput & ( 1 << EInputType_Down ) ) != 0 )
	{
		System_PlayCommonSE( ECommonSE_Cursor );

		DSMData->SelectPosition++;
		if( DSMData->SelectPosition == ETask_DebugStartMenu_Num )
		{
			DSMData->SelectPosition = 0;
		}
	}

	// 決定ボタンが押されたら選択項目に応じた画面に移動する
	if( ( EdgeInput & ( 1 << EInputType_Attack ) ) != 0 )
	{
		// 決定音を鳴らす
		System_PlayCommonSE( ECommonSE_Enter );

		// フェードアウトを開始する
		System_FadeOut();

		// 選択している項目に応じて処理を分岐
		switch( DSMData->SelectPosition )
		{
		case ETask_DebugStartMenu_StageSelect:
			// ステージ選択が選択されていたら、選択されたステージからゲーム本編を開始する
			System_SetStartStage( DSMData->SelectStageNo );
			if( !System_StartGameMain() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_Title:
			// タイトル画面を開始する
			if( !System_StartTitle() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_KeyConfig:
			// キーコンフィグ画面を開始する
			if( !System_StartKeyConfig() )
			{
				return false;
			}
			break;

		case ETask_DebugStartMenu_AllStageClear:
			// 全ステージクリア画面を開始する
			if( !System_StartAllStageClear() )
			{
				return false;
			}
			break;
		}

		// タスクを削除する
		TaskSystem_DelTask( System_GetTaskSystemInfo(), &DSMData->TaskInfo );
	}

	// 正常終了
	return true;
}

// デバッグ起動画面の描画処理を行う
static void Task_DebugStartMenu_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_DebugStartMenuInfo *DSMData = ( STask_DebugStartMenuInfo * )TInfo->Data;
	int                       Color;
	int                       DrawY;
	int                       FontHandle;

	// フォントハンドルの取得
	FontHandle = System_GetFontHandle( EFontHandle_Small );

	// 文字の描画に使用する色を取得
	Color = GetColor( 255,255,255 );

	// デバッグ起動メニューの文字列を描画
	DrawStringToHandle( 0, 0, "Debug Start Menu", Color, FontHandle );

	// 各メニュー項目の描画
	DrawY = MENU_Y;

	DrawFormatStringToHandle( MENU_X, DrawY, Color, FontHandle,
										"Stage Select : %d",DSMData->SelectStageNo );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "Title Start",           Color, FontHandle );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "KeyConfig Start",       Color, FontHandle );
	DrawY += MENU_SPACE;

	DrawStringToHandle( MENU_X, DrawY, "All Stage Clear Start", Color, FontHandle );
	DrawY += MENU_SPACE;

	// 選択している項目にカーソルを描画
	DrawStringToHandle( CURSOR_X, MENU_Y + MENU_SPACE * DSMData->SelectPosition,
										"⇒", GetColor( 255,255,255 ), FontHandle );
}

// デバッグ起動メニューの処理を開始する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool Task_DebugStartMenu_Start( void )
{
	STask_DebugStartMenuInfo *DSMData;

	// デバッグ起動画面の処理で使用するデータを格納するメモリ領域の確保
	DSMData = ( STask_DebugStartMenuInfo * )calloc( 1, sizeof( STask_DebugStartMenuInfo ) );
	if( DSMData == NULL )
	{
		return false;
	}

	// 黒フェードの解除を開始
	System_FadeIn();

	// タスクを登録する
	DSMData->TaskInfo.Base = &g_Task_DebugStartMenuTaskBaseInfo;
	DSMData->TaskInfo.Data = DSMData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &DSMData->TaskInfo );

	// 正常終了
	return true;
}

