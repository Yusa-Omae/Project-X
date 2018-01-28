#include "ToolLib_Visible.h"
#include <string.h>
#include <malloc.h>

// 作成できる表示状態情報の最大数
#define TOOL_VISIBLE_MAX			(256)

// 表示状態情報構造体
typedef struct _SToolVisible
{
	// 親の表示状態情報のハンドル( 親が居ない場合は -1 )
	int  ParentTVisibleHandle;

	// 表示状態
	bool Visible;
} SToolVisible;

static int           g_TVisibleNum;
static SToolVisible *g_TVisible[ TOOL_VISIBLE_MAX ];

// 表示状態情報を作成する
//     戻り値 : 表示状態情報ハンドル( 失敗した場合は -1 が返る )
int ToolVisible_Create( void )
{
	int           NewHandle;
	SToolVisible *TVisible;

	// 既に表示状態情報が最大数に達している場合はエラー終了
	if( g_TVisibleNum == TOOL_VISIBLE_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TVisible[ NewHandle ]; NewHandle++ ){}

	// 新しい表示状態情報を格納するためのメモリの確保
	TVisible = ( SToolVisible * )calloc( 1, sizeof( SToolVisible ) );
	if( TVisible == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TVisible[ NewHandle ] = TVisible;

	// 表示状態情報の数を増やす
	g_TVisibleNum++; 

	// 情報を初期化
	TVisible->ParentTVisibleHandle = -1;
	TVisible->Visible              = false;

	// ハンドルを返す
	return NewHandle;
}

// 表示状態情報を削除する
void ToolVisible_Delete(
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// 確保していたメモリを解放する
	free( TVisible );
	g_TVisible[ TVisibleHandle ] = NULL;

	// 表示状態情報の数を減らす
	g_TVisibleNum--;
}

// 全ての表示状態情報を削除する
void ToolVisible_Delete_All( void )
{
	int i;

	// 有効な全ての表示状態を削除する
	for( i = 0; i < TOOL_VISIBLE_MAX; i++ )
	{
		if( g_TVisible[ i ] != NULL )
		{
			ToolVisible_Delete( i );
		}
	}
}

// 表示状態情報の表示状態を変更する
void ToolVisible_SetVisible(
	// 表示状態情報ハンドル
	int TVisibleHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// 新しい表示状態を保存
	TVisible->Visible = Visible;
}

// 表示状態情報に親の表示状態情報を設定する
void ToolVisible_SetParent(
	// 表示状態情報ハンドル
	int TVisibleHandle,
	
	// 親となる表示状態情報ハンドル
	int ParentTVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];

	// 親となる表示状態情報のハンドルを保存
	TVisible->ParentTVisibleHandle = ParentTVisibleHandle;
}

// 表示状態情報の表示状態を取得する( 親の表示状態も考慮する )
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolVisible_GetVisible(
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolVisible *TVisible = g_TVisible[ TVisibleHandle ];
	SToolVisible *ParentTVisible;

	// 表示フラグが倒れていたら false を返す
	if( !TVisible->Visible )
	{
		return false;
	}

	// 親の表示状態情報が有効な場合は親の表示状態も考慮する
	if( TVisible->ParentTVisibleHandle >= 0 )
	{
		ParentTVisible = g_TVisible[ TVisible->ParentTVisibleHandle ];

		// 親の表示フラグが倒れていたら false を返す
		if( !ParentTVisible->Visible )
		{
			return false;
		}

		// 親にも親が居たら、親が居ない親の表示状態まで考慮する
		while( ParentTVisible->ParentTVisibleHandle >= 0 )
		{
			ParentTVisible = g_TVisible[ ParentTVisible->ParentTVisibleHandle ];

			// 親の表示フラグが倒れていたら false を返す
			if( !ParentTVisible->Visible )
			{
				return false;
			}
		}
	}

	// ここにきたら非表示ではないということなので true を返す
	return true;
}


