#include "Task_GameMain.h"
#include "DxLib.h"
#include "System.h"
#include "Input.h"
#include "Stage.h"
#include "StageData.h"
#include "Camera.h"
#include "Chara.h"
#include "GameMainRender.h"
#include "Chara_Player.h"
#include "Task_StageNumber.h"
#include "Task_StageClear.h"
#include "Task_GameOver.h"
#include "Task_PauseMenu.h"
#include "Code/Scene/Adventure/Adventure.h"
#include "Code/Scene/Shop/Shop.h"
#include <math.h>

// ステージ開始をする前に待つ時間
#define STAGE_START_WAIT		(0.1f)

// ステージクリア処理を開始する前に待つ時間
#define STAGE_CLEAR_WAIT		(1.5f)

// 全ステージクリア処理を開始する前に待つ時間
#define STAGE_ALLCLEAR_WAIT		(1.0f)

// ゲームオーバー処理を開始する前に待つ時間
#define GAMEOVER_WAIT			(0.1f)

// プレイヤーの体力ゲージを表示する座標
#define PC_HEALTH_X				(140)
#define PC_HEALTH_Y				(32)

// プレイヤーの体力ゲージの幅と高さ
#define PC_HEALTH_SIZE_X		(400)
#define PC_HEALTH_SIZE_Y		(16)

// プレイヤーの体力ゲージの『PLAYER』と表示する座標
#define PC_NAME_X				(32)
#define PC_NAME_Y				(28)
#define PC_NAME_STRING			"PLAYER"

// 倒す敵の数を表示する座標
#define ENEMY_NUM_X				(32)
#define ENEMY_NUM_Y				(GAME_SCREEN_HEIGHT - 56)

// ゲームメイン処理の状態
typedef enum _ETask_GameMainState
{
	
	ETask_GameMainState_Adventure_EndWait,	//会話パート終了待ち
	ETask_GameMainState_StageNumber,		// ステージ番号表示中

	ETask_GameMainState_StageStart_Wait,	// ステージ開始待ち中
	ETask_GameMainState_Stage_FadeIn,		// ステージフェードイン中

	ETask_GameMainState_InGame,				// ゲーム中
	ETask_GameMainState_PauseMenu,			// ポーズメニュー中
	ETask_GameMainState_StageClear_Wait,	// ステージクリア処理開始待ち中
	ETask_GameMainState_StageClear_String,	// ステージクリア文字表示中
	ETask_GameMainState_StageClear_FadeOut,	// ステージクリア後フェードアウト中

	ETask_GameMainState_Shop_Start,			//ショップを開く
	ETask_GameMainState_Shop_End,			//ショップ終了待ち
	ETask_GameMainState_Shop_FadeOut,		//ショップ終了後フェードアウト中

	ETask_GameMainState_StageAllClear_Wait,	// 全ステージクリア処理開始待ち中
	ETask_GameMainState_GameOver_Wait,		// ゲームオーバー処理開始待ち中
	ETask_GameMainState_GameOver,			// ゲームオーバー表示中
	ETask_GameMainState_GameOver_FadeOut,	// ゲームオーバー後フェードアウト中
	ETask_GameMainState_End,				// 終了状態
} ETask_GameMainState;

// ゲームメイン処理用のデータ
typedef struct _STask_GameMainData
{
	// タスクの情報
	STaskInfo           TaskInfo;

	// 処理の状態
	ETask_GameMainState State;

	// 汎用カウンタ
	float               Counter;

	// ゲームメインの状態推移処理を行うかどうかのフラグ
	bool                EnableGameMainStepProcess;

	// ゲームメインの描画処理を行うかどうかのフラグ
	bool                EnableGameMainRenderProcess;

	// 倒した敵の数
	int                 KillEnemyNum;

	// 倒すべき敵を倒したかどうかのフラグ( true:倒した  false:倒していない )
	bool                KillTargetCharaKill;

	// プレイヤーの体力表示などを行うかどうかのフラグ( true:表示する  false:表示しない )
	bool                DrawHUD;

	// ステージ番号表示タスク情報構造体へのポインタ
	STaskInfo *         StageNumberTaskInfo;

	// ステージクリア表示タスク情報構造体へのポインタ
	STaskInfo *         StageClearTaskInfo;

	// ゲームオーバー表示タスク情報構造体へのポインタ
	STaskInfo *         GameOverTaskInfo;

	// ポーズメニュータスク情報構造体へのポインタ
	STaskInfo *         PauseMenuTaskInfo;

	// 会話パートタスク情報構造体へのポインタ
	STaskInfo *         AdventureTaskInfo;

	//ショップタスク情報構造体へのポインタ
	STaskInfo *			ShopTaskInfo;

} STask_GameMainData;

// ゲームメインの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_GameMain_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
);

// ゲームメインの描画処理を行う
static void Task_GameMain_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
);

// ゲームメイン処理タスクの基本情報
static STaskBaseInfo g_Task_GameMainTaskBaseInfo =
{
	7,
	7,
	Task_GameMain_Step,
	Task_GameMain_Render,
	NULL
};

// ゲームメインの状態推移処理を行う
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
static bool Task_GameMain_Step(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo,
	
	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;

	// 各種ゲームメイン用の状態推移処理を行う
	if( GMData->EnableGameMainStepProcess )
	{
		if( !Stage_Step(         StepTime ) )
		{
			return false;
		}

		if( !Chara_StepAll(      StepTime ) )
		{
			return false;
		}

		Effect_StepAll(     StepTime );
		Camera_Step(        StepTime );
	}

	// ゲームメイン処理の状態によって処理を分岐
	switch( GMData->State )
	{

	case ETask_GameMainState_Adventure_EndWait:
		if (!System_CheckFade())
		{
			if (Task_Adventure_IsDelete() == true) {
				TaskSystem_DelTask(

					//会話タスク削除
					System_GetTaskSystemInfo(), GMData->AdventureTaskInfo);


				// 最初のステージのセットアップを行う
				if (!Stage_Setup(System_GetStartStage()))
				{
					return false;
				}

				System_FadeOut();

				GMData->State = ETask_GameMainState_StageStart_Wait;
			}
		}
		break;
	case ETask_GameMainState_StageStart_Wait:	// ステージ開始待ち中
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_START_WAIT )
		{
			// フェードイン待ち時間が経過したらフェードインを開始する
			System_FadeIn();

			// フェードインが完了したらステージ番号表示を開始する
			GMData->StageNumberTaskInfo = Task_StageNumber_Start();
			if (GMData->StageNumberTaskInfo == NULL)
			{
				return false;
			}

			//プレイヤーの体力などを表示する
			GMData->DrawHUD = true;

			GMData->State   = ETask_GameMainState_Stage_FadeIn;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_Stage_FadeIn:	// ステージフェードイン中
		if( !System_CheckFade() )
		{
			
			GMData->State   = ETask_GameMainState_StageNumber;
			GMData->Counter = 0.0f;
		}
		break;
	case ETask_GameMainState_StageNumber:	// ステージ番号表示中
		if( Task_StageNumber_CheckEnd( GMData->StageNumberTaskInfo ) )
		{
			// ステージ番号表示処理が完了したらゲーム中処理を開始する

			// ステージ番号表示タスクの削除
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->StageNumberTaskInfo );
			GMData->StageNumberTaskInfo = NULL;

			GMData->State   = ETask_GameMainState_InGame;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_InGame:				// ゲーム中
		if( System_GetPlayerCharaInfo() == NULL )
		{
			// プレイヤーが居なくなったら倒されたということなので
			// ゲームオーバー処理を開始する
			GMData->State   = ETask_GameMainState_GameOver_Wait;
			GMData->Counter = 0.0f;
		}
		else
		{
			// ステージのクリア条件によって処理を分岐
			switch( StageData_GetStageClearCondition() )
			{
			case EStageClearCondition_CharaKillNum:
				// 指定数の敵を倒したらクリアの場合
				if( GMData->KillEnemyNum >= StageData_GetKillCharaNum() )
				{
					// 指定数の敵を倒したらクリア処理開始待ちを開始する
					GMData->State   = ETask_GameMainState_StageClear_Wait;
					GMData->Counter = 0.0f;
				}
				break;

			case EStageClearCondition_TargetCharaKill:
				// 指定の敵を倒したらクリアの場合
				if( GMData->KillTargetCharaKill )
				{
					// 指定の敵を倒したらそれ以外の敵も倒したことにする
					if( !Chara_TagetTypeKill( ECharaType_Enemy ) ||
					    !Chara_TagetTypeKill( ECharaType_EnemyShot ) )
					{
						return false;
					}

					// ステージクリア開始待ちを開始する
					GMData->State   = ETask_GameMainState_StageClear_Wait;
					GMData->Counter = 0.0f;
				}
				break;
			}

			// 状態が変更されていなくて、且つポーズボタンが押されたら
			// ポーズメニューを表示する
			if( GMData->State == ETask_GameMainState_InGame &&
				( GetEdgeInput() & ( 1 << EInputType_Pause ) ) != 0 )
			{
				// ゲームの状態推移処理を止める
				GMData->EnableGameMainStepProcess = false;

				// 音を鳴らす
				System_PlayCommonSE( ECommonSE_Enter );

				// ポーズメニュー処理を開始する
				GMData->PauseMenuTaskInfo = Task_PauseMenu_Start();
				if( GMData->PauseMenuTaskInfo == NULL )
				{
					return false;
				}

				GMData->State   = ETask_GameMainState_PauseMenu;
				GMData->Counter = 0.0f;
			}
		}
		break;

	case ETask_GameMainState_PauseMenu:			// ポーズメニュー中
		if( Task_PauseMenu_CheckEnd( GMData->PauseMenuTaskInfo ) )
		{
			// ポーズメニューが完了したら選択された項目によって処理を分岐する
			switch( Task_PauseMenu_GetChoiceIndex( GMData->PauseMenuTaskInfo ) )
			{
			case ETask_PauseMenu_Continue:		// ゲーム再開
				// ゲームの状態推移処理を再開する
				GMData->EnableGameMainStepProcess = true;

				// ゲーム中に戻る
				GMData->State   = ETask_GameMainState_InGame;
				GMData->Counter = 0.0f;
				break;

			case ETask_PauseMenu_BackToTitle:	// タイトル画面に戻る
				// タイトル画面処理を開始する
				if( !System_StartTitle() )
				{
					return false;
				}

				// 全てのキャラを削除する
				Chara_AllDelete();

				// 終了状態にする
				GMData->State   = ETask_GameMainState_End;
				GMData->Counter = 0.0f;

				// タスクを削除する
				TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
				break;

			case ETask_PauseMenu_GameEnd:		// ゲーム終了
				// ゲーム終了が選択されていたらゲームを終了する
				System_ExitGame();

				// 全てのキャラを削除する
				Chara_AllDelete();

				// 終了状態にする
				GMData->State   = ETask_GameMainState_End;
				GMData->Counter = 0.0f;

				// タスクを削除する
				TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
				break;
			}

			// ポーズメニュー処理タスクを削除
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->PauseMenuTaskInfo );
			GMData->PauseMenuTaskInfo = NULL;
		}
		break;

	case ETask_GameMainState_StageClear_Wait:	// ステージクリア処理開始待ち中
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_CLEAR_WAIT )
		{
			// ステージクリア開始待ち時間が経過したらステージクリア表示を開始する
			Sound_PlayBGM( EBGM_StageClear );
			GMData->StageClearTaskInfo = Task_StageClear_Start();
			if( GMData->StageClearTaskInfo == NULL )
			{
				return false;
			}

			GMData->State   = ETask_GameMainState_StageClear_String;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_StageClear_String:	// ステージクリア文字表示中
		if( Task_StageClear_CheckEnd( GMData->StageClearTaskInfo ) )
		{
			// ステージクリア表示処理が完了したらフェードアウトを開始する

			// ステージクリア表示処理タスクを削除する
			TaskSystem_DelTask( 
				System_GetTaskSystemInfo(), GMData->StageClearTaskInfo );
			GMData->StageClearTaskInfo = NULL;

			System_FadeOut();

			GMData->State   = ETask_GameMainState_StageClear_FadeOut;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_StageClear_FadeOut:	// ステージクリア後フェードアウト中
		if( !System_CheckFade() )
		{

			/*
			
				ここで、ショップを挟むをタスクで実行させる。

				ケースを増やし、ショップでの購入が終了したかを判断するケース分を追加する

				
			
			
			*/

			// ステージクリア後フェードアウトが完了したら、一旦キャラを全て削除する
			Chara_AllDelete();

			// 全ステージクリアしたかどうかで処理を分岐
			//if( StageData_GetTotalStageNum() == StageData_GetLoadStageNo() + 1 )
			if (STAGE_MAX_NUM == StageData_GetLoadStageNo() + 1)
			{
				// 全ステージクリアした場合は全ステージクリア表示開始待ちを開始する
				GMData->EnableGameMainStepProcess = false;
				GMData->EnableGameMainRenderProcess = false;
				System_FadeIn();
				GMData->DrawHUD = false;

				GMData->State   = ETask_GameMainState_StageAllClear_Wait;
				GMData->Counter = 0.0f;
			}
			else
			{
			


				GMData->KillEnemyNum        = 0;
				GMData->KillTargetCharaKill = false;

				GMData->State   = ETask_GameMainState_Shop_Start;
				GMData->Counter = 0.0f;
			}

			

		}
		break;
	case ETask_GameMainState_Shop_Start:
		
		/*
			ここでショップタスク生成
		
		*/

		GMData->ShopTaskInfo = Task_Shop_Start();
		if (GMData->ShopTaskInfo == NULL) {
			return false;
		}


		System_FadeIn();
		GMData->State = ETask_GameMainState_Shop_End;
		GMData->Counter = 0.0f;
		
		break;
	case ETask_GameMainState_Shop_End:
		if (Task_Shop_IsExit()) {
					
			
			System_FadeOut();
						

			GMData->State = ETask_GameMainState_Shop_FadeOut;
			GMData->Counter = 0.0f;

		}
		break;
	case ETask_GameMainState_Shop_FadeOut:

		if (!System_CheckFade()) {

			//ショップタスクの削除をする
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->ShopTaskInfo);
			GMData->ShopTaskInfo = NULL;

			// 次のステージがある場合は、次のステージのセットアップを行い、
			// その後ステージ開始待ちを開始する
			if (!Stage_Setup(StageData_GetLoadStageNo() + 1))
			{
				return false;
			}

			GMData->State = ETask_GameMainState_StageStart_Wait;
			GMData->Counter = 0.0f;

		}

		break;
	case ETask_GameMainState_StageAllClear_Wait:	// 全ステージクリア処理開始待ち中
		GMData->Counter += StepTime;
		if( GMData->Counter > STAGE_ALLCLEAR_WAIT )
		{
			// 全ステージクリア表示開始待ち時間が経過したら全ステージクリア表示を開始する
			if( !System_StartAllStageClear() )
			{
				return false;
			}

			// 終了状態にする
			GMData->State   = ETask_GameMainState_End;
			GMData->Counter = 0.0f;

			// タスクを削除する
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
		}
		break;

	case ETask_GameMainState_GameOver_Wait:		// ゲームオーバー処理開始待ち中
		GMData->Counter += StepTime;
		if( GMData->Counter > GAMEOVER_WAIT )
		{
			// ゲームオーバー処理開始待ち時間が経過したらゲームオーバー表示を開始する
			GMData->GameOverTaskInfo = Task_GameOver_Start();
			if( GMData->GameOverTaskInfo == NULL )
			{
				return false;
			}

			GMData->State   = ETask_GameMainState_GameOver;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_GameOver:		// ゲームオーバー表示中
		if( Task_GameOver_CheckEnd( GMData->GameOverTaskInfo ) )
		{
			// ゲームオーバー表示が完了したらフェードアウトを開始する

			// ゲームオーバー表示タスクを削除
			TaskSystem_DelTask(
				System_GetTaskSystemInfo(), GMData->GameOverTaskInfo );
			GMData->GameOverTaskInfo = NULL;

			System_FadeOut();

			GMData->State   = ETask_GameMainState_GameOver_FadeOut;
			GMData->Counter = 0.0f;
		}
		break;

	case ETask_GameMainState_GameOver_FadeOut:	// ゲームオーバー後フェードアウト中
		if( !System_CheckFade() )
		{
			// フェードアウトが完了したらタイトル画面処理を開始する
			if( !System_StartTitle() )
			{
				return false;
			}

			// 全てのキャラを削除する
			Chara_AllDelete();

			// 終了状態にする
			GMData->State   = ETask_GameMainState_End;
			GMData->Counter = 0.0f;

			// タスクを削除する
			TaskSystem_DelTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );
		}
		break;

	case ETask_GameMainState_End:				// 終了状態
		break;
	}

	// 正常終了
	return true;
}

// ゲームメインの描画処理を行う
static void Task_GameMain_Render(
	// タスクシステム情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;
	SCharaInfo         *PCInfo;
	int                 SmallFontHandle;
	int                 MidiumFontHandle;

	// ゲームメイン用の描画処理を行う
	if( GMData->EnableGameMainRenderProcess )
	{
		GameMainRender_Render();
	}

	// Ｓキーが押されていたらゲーム画面のスクリーンショットを保存する
//	if( CheckHitKey( KEY_INPUT_S ) != 0 )
//	{
//		SaveDrawScreen( 0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,
//													"ScreenShot.bmp" );
//	}

	// 体力表示などを行うかどうかのフラグが倒れている場合は何もせず終了
	if( !GMData->DrawHUD )
	{
		return;
	}

	// 描画ブレンドモードを標準設定にする
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );

	// 汎用フォントハンドルを取得する
	SmallFontHandle  = System_GetFontHandle( EFontHandle_Small );
	MidiumFontHandle = System_GetFontHandle( EFontHandle_Midium );

	// クリア条件が『敵を指定数倒す』だった場合は、倒すべき敵の残り数を描画する
	if( StageData_GetStageClearCondition() == EStageClearCondition_CharaKillNum )
	{
		DrawFormatStringToHandle(
			ENEMY_NUM_X, ENEMY_NUM_Y, GetColor( 255,64,64 ), MidiumFontHandle,
			"ENEMY × %d", StageData_GetKillCharaNum() - GMData->KillEnemyNum
		);
	}

	// 『PLAYER』を描画する
	DrawStringToHandle( PC_NAME_X, PC_NAME_Y, PC_NAME_STRING, GetColor( 255,255,255 ), SmallFontHandle );

	// プレイヤーの体力表示の枠部分を描画する
	DrawBox(
		PC_HEALTH_X - 4,                    PC_HEALTH_Y - 4,
		PC_HEALTH_X + PC_HEALTH_SIZE_X + 4, PC_HEALTH_Y + PC_HEALTH_SIZE_Y + 4,
		GetColor( 200,200,200 ),
		TRUE
	);

	// プレイヤーの情報を取得する
	PCInfo = System_GetPlayerCharaInfo();

	// プレイヤーが居る場合はプレイヤーの体力を描画する
	// プレイヤーが居ない場合はプレイヤーの体力は０として描画する
	CharaHealthGaugeDraw(
		PCInfo != NULL ? &PCInfo->HealthGauge : NULL,
		PC_HEALTH_X,      PC_HEALTH_Y,
		PC_HEALTH_SIZE_X, PC_HEALTH_SIZE_Y
	);

	// デバッグ情報を表示するかどうかのフラグが立っていたらデバッグ表示を行う
	if( System_GetDispDebugInfo() )
	{
		SCharaInfo *CInfo;

		CInfo = System_GetPlayerCharaInfo();

		Stage_CollisionRender();

		if( CInfo != NULL )
		{
			DrawFormatString( 0, GAME_SCREEN_HEIGHT - 32, GetColor( 255,255,255 ),
				"Position:%.2f,%.2f,%.2f TargetAngle:%.2f NowAngle:%.2f",
				CInfo->Position.x, CInfo->Position.y, CInfo->Position.z,
				CInfo->AngleInfo.TargetAngle, CInfo->AngleInfo.NowAngle );
		}
	}
}

// ゲームメインの処理に敵を倒したことを伝える
void Task_GameMain_AddKillEnemy(
	// ゲームメインタスク情報構造体のアドレス
	STaskInfo *TInfo,

	// 倒した敵の識別番号
	int CharaNo
)
{
	STask_GameMainData *GMData = ( STask_GameMainData * )TInfo->Data;

	// 倒した敵の数を増やす
	GMData->KillEnemyNum++;

	// 倒した敵が、クリアするために倒さなければならない敵だった場合は、
	// 倒すべき敵を倒したかどうかのフラグを立てる
	if( StageData_GetKillTargetChara() == CharaNo )
	{
		GMData->KillTargetCharaKill = true;
	}
}

// ゲームメインの処理を開始する
//     戻り値 : ゲームメインタスク情報構造体へのアドレス
STaskInfo * Task_GameMain_Start( void )
{
	STask_GameMainData *GMData;

	// ゲームメインの処理で使用するデータを格納するメモリ領域の確保
	GMData = ( STask_GameMainData * )calloc( 1, sizeof( STask_GameMainData ) );
	if( GMData == NULL )
	{
		return NULL;
	}

	//会話タスク生成
	GMData->AdventureTaskInfo = Task_Adventure_Start();
	if (GMData->AdventureTaskInfo == NULL) {
		return false;
	}

	// ゲームメインの処理を行うかどうかのフラグを立てる
	GMData->EnableGameMainStepProcess   = true;
	GMData->EnableGameMainRenderProcess = true;

	// 倒した敵の数と、倒すべき敵を倒したかどうかのフラグを初期化する
	GMData->KillEnemyNum        = 0;
	GMData->KillTargetCharaKill = false;

	// プレイヤーの体力表示などを行うかどうかのフラグを立てる
	GMData->DrawHUD             = false;

	// ステージ開始待ち状態にする
	//GMData->State               = ETask_GameMainState_StageStart_Wait;
	GMData->State = ETask_GameMainState_Adventure_EndWait;
	GMData->Counter             = 0.0f;

	// タスクを登録する
	GMData->TaskInfo.Base = &g_Task_GameMainTaskBaseInfo;
	GMData->TaskInfo.Data = GMData;
	TaskSystem_AddTask( System_GetTaskSystemInfo(), &GMData->TaskInfo );

	System_FadeIn();

	// タスク情報構造体のアドレスを返す
	return &GMData->TaskInfo;
}




