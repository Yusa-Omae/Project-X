#ifndef TOOLLIB_SPACEMANAGE_H
#define TOOLLIB_SPACEMANAGE_H

// スペース情報構造体
typedef struct _SToolSpace
{
	// スペースが固定タイプかどうか( true:固定タイプ  false:通常タイプ )
	bool LockSpace;

	// 使用するスペースサイズ
	int  UseSpace;

	// 使用するスペースの開始位置
	int  SpaceStart;

	// 初期化時のスペースサイズ
	int  InitUseSpace;
} SToolSpace;

// スペース管理情報を作成する
//     戻り値 : スペース管理情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolSpaceManage_Create(
	// 総スペースサイズ
	int TotalSpace
);

// スペース管理情報を削除する
extern void ToolSpaceManage_Delete(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
);

// 全てのスペース管理情報を削除する
extern void ToolSpaceManage_Delete_All( void );

// スペース管理情報にスペース情報を追加する
//     戻り値 : 情報の追加が成功したかどうか( true:成功  false:失敗 )
extern bool ToolSpaceManage_AddSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle,

	// スペース情報構造体のアドレス
	SToolSpace *Space,
	
	// 使用するスペースサイズ、-1を指定すると残りのスペース全てを割り当てる
	int UseSpace,
	
	// スペースが固定タイプかどうか( true:固定タイプ  false:通常タイプ )
	bool LockSpace
);

// 総スペースサイズを変更する
extern void ToolSpaceManage_ChangeTotalSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle,
	
	// 総スペースサイズ
	int TotalSpace
);

// 総スペースサイズを取得する
//     戻り値 : 総スペースサイズ
extern int ToolSpaceManage_GetTotalSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
);

// 次のスペース情報に割り当てる開始位置を取得する
//     戻り値 : スペースの開始位置
extern int ToolSpaceManage_GetNextSpace(
	// スペース管理情報ハンドル
	int TSpaceManageHandle
);

#endif
