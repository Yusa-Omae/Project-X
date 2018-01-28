#include "Task.h"
#include <string.h>
#include <malloc.h>

// タスクリストへの追加や削除を反映する
static void TaskSystem_RefreshList(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	STaskInfo *NextTInfo;
	STaskInfo *TempTInfo;

	// 『追加するタスクのリスト』に登録されているタスクを、タスクリストに追加する
	if( TSysInfo->AddTaskList != NULL )
	{
		for( TInfo = TSysInfo->AddTaskList; TInfo != NULL; TInfo = NextTInfo )
		{
			// 『追加するタスクのリスト』の次のタスクのアドレスを保持しておく
			NextTInfo = TInfo->AddOrDelNext;

			// 状態推移処理のタスクのリストに追加する
			TInfo->StepNext = TSysInfo->StepTask[ TInfo->Base->StepPriority ];
			TSysInfo->StepTask[ TInfo->Base->StepPriority ] = TInfo;

			// 描画処理のタスクのリストに追加する
			TInfo->RenderNext = TSysInfo->RenderTask[ TInfo->Base->RenderPriority ];
			TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] = TInfo;

			// 『追加するタスクのリスト』の次のタスクのアドレスを初期化する
			TInfo->AddOrDelNext = NULL;
		}

		// 『追加するタスクのリスト』の先頭タスクへのポインタを初期化する
		TSysInfo->AddTaskList = NULL;
	}

	// 『削除するタスクのリスト』に登録されているタスクを、タスクリストから外す
	if( TSysInfo->DelTaskList != NULL )
	{
		for( TInfo = TSysInfo->DelTaskList; TInfo != NULL; TInfo = NextTInfo )
		{
			// 『削除するタスクのリスト』の次のタスクのアドレスを保持しておく
			NextTInfo = TInfo->AddOrDelNext;

			// 外すタスクが状態推移処理のタスクのリストの先頭かをチェック
			if( TSysInfo->StepTask[ TInfo->Base->StepPriority ] == TInfo )
			{
				// 先頭だった場合は先頭を外すタスクの次のタスクにする
				TSysInfo->StepTask[ TInfo->Base->StepPriority ] = TInfo->StepNext;
			}
			else
			{
				// 状態推移処理のタスクのリストの先頭ではなかったら、
				// 状態推移処理のタスクのリストから外すタスクを検索
				TempTInfo = TSysInfo->StepTask[ TInfo->Base->StepPriority ];
				while( true )
				{
					// リストの終端に来てしまった場合はループを抜ける
					if( TempTInfo->StepNext == NULL )
					{
						break;
					}

					// 削除対象のタスクを発見した場合は、リストから外してループから抜ける
					if( TempTInfo->StepNext == TInfo )
					{
						TempTInfo->StepNext = TInfo->StepNext;
						break;
					}

					TempTInfo = TempTInfo->StepNext;
				}
			}

			// 外すタスクが描画処理のタスクのリストの先頭かをチェック
			if( TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] == TInfo )
			{
				// 先頭だった場合は先頭を外すタスクの次のタスクにする
				TSysInfo->RenderTask[ TInfo->Base->RenderPriority ] = TInfo->RenderNext;
			}
			else
			{
				// 描画処理のタスクのリストの先頭ではなかったら、
				// 描画処理のタスクのリストから外すタスクを検索
				TempTInfo = TSysInfo->RenderTask[ TInfo->Base->RenderPriority ];
				for(;;)
				{
					// リストの終端に来てしまった場合はループを抜ける
					if( TempTInfo->RenderNext == NULL )
					{
						break;
					}

					// 削除対象のタスクを発見した場合は、リストから外してループから抜ける
					if( TempTInfo->RenderNext == TInfo )
					{
						TempTInfo->RenderNext = TInfo->RenderNext;
						break;
					}

					TempTInfo = TempTInfo->RenderNext;
				}
			}

			// 後始末関数が設定されている場合は後始末関数を実行する
			if( TInfo->Base->Terminate != NULL )
			{
				TInfo->Base->Terminate( TInfo );
			}

			// 各リストの次のタスクへのポインタを初期化する
			TInfo->AddOrDelNext = NULL;
			TInfo->StepNext     = NULL;
			TInfo->RenderNext   = NULL;

			// データ格納用にメモリを確保していた場合は解放する
			if( TInfo->Data != NULL )
			{
				free( TInfo->Data );
			}
		}

		// 『削除するタスクのリスト』の先頭タスクへのポインタを初期化する
		TSysInfo->DelTaskList = NULL;
	}
}

// タスクシステムの初期化
void TaskSystem_Initialize(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
)
{
	// 構造体全体をゼロ初期化する
	memset( TSysInfo, 0, sizeof( STaskSystemInfo ) );
}

// タスクシステムの後始末
void TaskSystem_Terminate(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	int        i;

	// タスクの処理を実行しているかどうかのフラグを『実行中』にしておく
	TSysInfo->Run = true;

	// 実行優先順位の数だけ繰り返し
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// 実行優先順位 i に登録されているタスクを全て削除する
		for( TInfo = TSysInfo->StepTask[ i ]; TInfo != NULL; TInfo = TInfo->StepNext )
		{
			TaskSystem_DelTask( TSysInfo, TInfo );
		}
	}

	// タスクの処理を実行しているかどうかのフラグを『実行中ではない』にしておく
	TSysInfo->Run = false;

	// 削除を反映する
	TaskSystem_RefreshList( TSysInfo );
}

// タスクシステムの状態推移処理を実行する
//     戻り値 : 処理が正常に終了したかどうか(true:正常に終了した  false:エラーが発生した)
bool TaskSystem_Step(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 推移させる時間( 単位：秒 )
	float StepTime
)
{
	STaskInfo *TInfo;
	int        i;

	// タスクの処理を実行しているかどうかのフラグを『実行中』にしておく
	TSysInfo->Run = true;

	// 実行優先順位の数だけ繰り返し
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// 実行優先順位 i に登録されている全てのタスクの状態推移処理を実行する
		for( TInfo = TSysInfo->StepTask[ i ]; TInfo != NULL; TInfo = TInfo->StepNext )
		{
			if( !TInfo->Base->Step( TInfo, StepTime ) )
			{
				return false;
			}
		}
	}

	// タスクの処理を実行しているかどうかのフラグを『実行中ではない』にしておく
	TSysInfo->Run = false;

	// タスクの処理を実行している間に行われたタスクの登録や削除をリストへ反映する
	TaskSystem_RefreshList( TSysInfo );

	// 正常終了
	return true;
}

// タスクシステムの描画処理を実行する
void TaskSystem_Render(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo
)
{
	STaskInfo *TInfo;
	int        i;

	// タスクの処理を実行しているかどうかのフラグを『実行中』にしておく
	TSysInfo->Run = true;

	// 実行優先順位の数だけ繰り返し
	for( i = 0; i < TASK_PRIORITY_MAX; i++ )
	{
		// 実行優先順位 i に登録されている全てのタスクの描画処理を実行する
		for( TInfo = TSysInfo->RenderTask[ i ]; TInfo != NULL; TInfo = TInfo->RenderNext)
		{
			// 『削除するタスクのリスト』に登録されている場合は描画処理を行わない
			if( TInfo->AddOrDelNext != NULL )
			{
				continue;
			}

			TInfo->Base->Render( TInfo );
		}
	}

	// タスクの処理を実行しているかどうかのフラグを『実行中ではない』にしておく
	TSysInfo->Run = false;

	// タスクの処理を実行している間に行われたタスクの登録や削除をリストへ反映する
	TaskSystem_RefreshList( TSysInfo );
}

// タスクシステムにタスクを追加する
void TaskSystem_AddTask(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 追加するタスクの情報構造体のアドレス
	STaskInfo *TInfo
)
{
	// タスクシステム情報の『追加するタスクのリスト』にタスクを追加する
	TInfo->AddOrDelNext   = TSysInfo->AddTaskList;
	TSysInfo->AddTaskList = TInfo;

	// タスクの処理が実行中ではなかったら、直ぐにタスクリストに反映する
	if( !TSysInfo->Run )
	{
		TaskSystem_RefreshList( TSysInfo );
	}
}

// タスクシステムに登録したタスクを削除する
void TaskSystem_DelTask(
	// タスクシステム情報構造体のアドレス
	STaskSystemInfo *TSysInfo,

	// 削除するタスクの情報構造体のアドレス
	STaskInfo *TInfo
)
{
	STaskInfo *TempTInfo;

	// 『追加するタスクのリスト』に追加されている場合は、
	// 『追加するタスクのリスト』からタスクを外す
	if( TInfo->AddOrDelNext != NULL )
	{
		// 外すタスクが『追加するタスクのリスト』の先頭かをチェック
		if( TSysInfo->AddTaskList == TInfo )
		{
			// 先頭だった場合は先頭を外すタスクの次のタスクにする
			TSysInfo->AddTaskList = TInfo->AddOrDelNext;
		}
		else
		{
			// 『追加するタスクのリスト』の先頭ではなかったら、
			// 『追加するタスクのリスト』から外すタスクを検索
			TempTInfo = TSysInfo->AddTaskList;
			while( true )
			{
				// リストの終端に来てしまった場合は『追加するタスクのリスト』に
				// 追加されていたのではなく『削除するタスクのリスト』に追加されていた
				// ということなので、関数から抜ける
				if( TempTInfo->AddOrDelNext == NULL )
				{
					return;
				}

				// 削除対象のタスクを発見した場合は、リストから外してループから抜ける
				if( TempTInfo->AddOrDelNext == TInfo )
				{
					TempTInfo->AddOrDelNext = TInfo->AddOrDelNext;
					break;
				}

				TempTInfo = TempTInfo->AddOrDelNext;
			}
		}

		// 後始末関数が設定されている場合は後始末関数を実行する
		if( TInfo->Base->Terminate != NULL )
		{
			TInfo->Base->Terminate( TInfo );
		}

		// 『追加するタスクのリスト』の次のタスクへのポインタを初期化する
		TInfo->AddOrDelNext = NULL;

		// データ格納用にメモリを確保していた場合は解放する
		if( TInfo->Data != NULL )
		{
			free( TInfo->Data );
		}
	}
	else
	{
		// タスクシステムに既に登録されている場合は
		// 『削除するタスクのリスト』に追加する
		TInfo->AddOrDelNext   = TSysInfo->DelTaskList;
		TSysInfo->DelTaskList = TInfo;

		// タスクの処理が実行中ではなかったら、直ぐにタスクリストに反映する
		if( !TSysInfo->Run )
		{
			TaskSystem_RefreshList( TSysInfo );
		}
	}
}

