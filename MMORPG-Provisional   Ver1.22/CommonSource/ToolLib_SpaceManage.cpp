#include "ToolLib_SpaceManage.h"
#include <malloc.h>

// 作成できるスペース管理情報の最大数
#define TOOL_SPACEMANAGE_MAX			(256)

// スペース管理情報に登録できるスペース情報の最大数
#define TOOL_SPACE_MAX					(256)

// スペース管理情報構造体
typedef struct _SToolSpaceManage
{
	// スペースの数
	int         SpaceNum;

	// 登録されているスペース情報へのアドレスを格納しているポインタ変数
	SToolSpace *Space[ TOOL_SPACE_MAX ];

	// 次のスペースに割り当てるスペース座標
	int         NextSpace;

	// 固定で確保されている総スペースサイズ
	int         TotalLockSpace;

	// 総スペースサイズ
	int         TotalSpace;

	// 初期家事の総スペースサイズ
	int         InitTotalSpace;
} SToolSpaceManage;

static int               g_TSpaceManageNum;
static SToolSpaceManage *g_TSpaceManage[ TOOL_SPACEMANAGE_MAX ];

// スペース管理情報を作成する
//     戻り値 : スペース管理情報ハンドル( 失敗した場合は -1 が返る )
int ToolSpaceManage_Create(
	// 総スペースサイズ
	int TotalSpace
)
{
	int               NewHandle;
	SToolSpaceManage *TSpaceManage;

	// 既にスペース管理情報が最大数に達している場合はエラー終了
	if( g_TSpaceManageNum == TOOL_SPACEMANAGE_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TSpaceManage[ NewHandle ]; NewHandle++ ){}

	// 新しいスペース管理情報を格納するためのメモリの確保
	TSpaceManage = ( SToolSpaceManage * )calloc( 1, sizeof( SToolSpaceManage ) );
	if( TSpaceManage == 0 )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TSpaceManage[ NewHandle ] = TSpaceManage;

	// スペース管理情報の数を増やす
	g_TSpaceManageNum++; 

	// 情報を初期化
	TSpaceManage->SpaceNum       = 0;
	TSpaceManage->NextSpace      = 0;
	TSpaceManage->TotalLockSpace = 0;
	TSpaceManage->TotalSpace     = TotalSpace;
	TSpaceManage->InitTotalSpace = TotalSpace;

	// ハンドルを返す
	return NewHandle;
}

// スペース管理情報を削除する
void ToolSpaceManage_Delete(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	// 確保していたメモリを解放する
	free( TSpaceManage );
	g_TSpaceManage[ TSpaceManageHandle ] = 0;

	// スペース管理情報の数を減らす
	g_TSpaceManageNum--;
}


// 全てのスペース管理情報を削除する
void ToolSpaceManage_Delete_All( void )
{
	int i;

	// 有効な全てのスペース管理を削除する
	for( i = 0; i < TOOL_SPACEMANAGE_MAX; i++ )
	{
		if( g_TSpaceManage[ i ] != 0 )
		{
			ToolSpaceManage_Delete( i );
		}
	}
}

// スペース管理情報にスペース情報を追加する
//     戻り値 : 情報の追加が成功したかどうか( true:成功  false:失敗 )
bool ToolSpaceManage_AddSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle,

	// スペース情報構造体のアドレス
	SToolSpace *Space,
	
	// 使用するスペースサイズ、-1を指定すると残りのスペース全てを割り当てる
	int UseSpace,
	
	// スペースが固定タイプかどうか( true:固定タイプ  false:通常タイプ )
	bool LockSpace
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	// 登録できるスペース情報の最大数に達していた場合はエラー
	if( TSpaceManage->SpaceNum == TOOL_SPACE_MAX )
	{
		return false;
	}

	// 使用するスペースサイズが -1 だった場合は残りの全てのスペースを割り当てる
	if( UseSpace == -1 )
	{
		UseSpace = TSpaceManage->TotalSpace - TSpaceManage->NextSpace;
	}

	// 情報を初期化
	Space->LockSpace    = LockSpace;
	Space->UseSpace     = UseSpace;
	Space->InitUseSpace = UseSpace;
	Space->SpaceStart   = TSpaceManage->NextSpace;

	// 次のスペースに割り当てるスペース開始位置を使用されるスペース分だけ移動する
	TSpaceManage->NextSpace += UseSpace;

	// 固定型スペースの場合は固定型スペースの総サイズに追加するスペースサイズを加算する
	if( LockSpace )
	{
		TSpaceManage->TotalLockSpace += UseSpace;
	}

	// スペース管理情報にスペース情報を追加する
	TSpaceManage->Space[ TSpaceManage->SpaceNum ] = Space;
	TSpaceManage->SpaceNum ++;

	// 正常終了
	return true;
}

// 総スペースサイズを変更する
void ToolSpaceManage_ChangeTotalSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle,
	
	// 総スペースサイズ
	int TotalSpace
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];
	int               FreeSpace;
	int               NextFreeSpace;
	int               i;
	int               SpaceStart;
	SToolSpace       *Space;

	// 今までの総スペースサイズでの固定型ではないスペースのサイズを算出
	FreeSpace     = TSpaceManage->InitTotalSpace - TSpaceManage->TotalLockSpace;

	// 新しい総スペースサイズでの固定型ではないスペースのサイズを算出
	NextFreeSpace = TotalSpace - TSpaceManage->TotalLockSpace;

	// スペースの数だけ繰り返し
	SpaceStart = 0;
	for( i = 0; i < TSpaceManage->SpaceNum; i ++ )
	{
		Space = TSpaceManage->Space[ i ];

		// スペース開始位置のセット
		Space->SpaceStart = SpaceStart;

		// 固定型スペースではない場合は総スペースサイズに対する割合から
		// 新しいスペースサイズを決定する
		if( !Space->LockSpace )
		{
			Space->UseSpace = Space->InitUseSpace * NextFreeSpace / FreeSpace;
		}

		// スペース開始位置を移動する
		SpaceStart += Space->UseSpace;
	}

	// 新しい総スペースサイズを保存
	TSpaceManage->TotalSpace = TotalSpace;
}

// 総スペースサイズを取得する
//     戻り値 : 総スペースサイズ
int ToolSpaceManage_GetTotalSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	return TSpaceManage->TotalSpace;
}

// 次のスペース情報に割り当てる開始位置を取得する
//     戻り値 : スペースの開始位置
int ToolSpaceManage_GetNextSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
)
{
	SToolSpaceManage *TSpaceManage = g_TSpaceManage[ TSpaceManageHandle ];

	return TSpaceManage->NextSpace;
}
