#ifndef TOOLLIB_VISIBLE_H
#define TOOLLIB_VISIBLE_H

// 表示状態情報を作成する
//     戻り値 : 表示状態情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolVisible_Create( void );

// 表示状態情報を削除する
extern void ToolVisible_Delete(
	// 表示状態情報ハンドル
	int TVisibleHandle
);

// 全ての表示状態情報を削除する
extern void ToolVisible_Delete_All( void );

// 表示状態情報の表示状態を変更する
extern void ToolVisible_SetVisible(
	// 表示状態情報ハンドル
	int TVisibleHandle,
	
	// 新しい表示状態
	bool Visible
);

// 表示状態情報に親の表示状態情報を設定する
extern void ToolVisible_SetParent(
	// 表示状態情報ハンドル
	int TVisibleHandle,
	
	// 親となる表示状態情報ハンドル
	int ParentTVisibleHandle
);

// 表示状態情報の表示状態を取得する( 親の表示状態も考慮する )
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolVisible_GetVisible(
	// 表示状態情報ハンドル
	int TVisibleHandle
);


#endif
