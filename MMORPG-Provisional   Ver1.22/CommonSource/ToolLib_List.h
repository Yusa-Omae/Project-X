#ifndef TOOLLIB_LIST_H
#define TOOLLIB_LIST_H

// リスト情報を作成する
//     戻り値 : リスト情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolList_Create( void );

// リスト情報を削除する
extern void ToolList_Delete(
	// リスト情報ハンドル
	int TListHandle
);

// 全てのリスト情報を削除する
extern void ToolList_Delete_All( void );

// リスト情報に関連付ける表示状態情報をセットする
extern void ToolList_SetVisibleHandle(
	// リスト情報ハンドル
	int TListHandle,

	// 表示状態情報ハンドル
	int TVisibleHandle
);

// リスト情報を初期化する
extern void ToolList_Initialize(
	// リスト情報ハンドル
	int TListHandle,
	
	// 関連付けるウインドウ情報ハンドル
	int TWindowHandle
);

// リスト情報の選択されている行の変更などが発生したかどうかを取得する
//     戻り値 : 変更が発生したかどうか( true:変更が発生した  false:変更は発生していない )
extern bool ToolList_GetChange(
	// リスト情報ハンドル
	int TListHandle,

	// 変更が発生したかどうかの情報をリセットするかどうか
	bool Reset
);

// リスト情報に行情報を追加する
//     戻り値 : 正常に処理が終了したかどうか( true:正常終了  false:エラー発生 )
extern bool ToolList_AddObj(
	// リスト情報ハンドル
	int TListHandle,

	// 追加する行に表示する文字列
	const char *String,
	
	// 可変個数引数
	...
);

// リスト情報の表示状態を変更する
extern void ToolList_SetVisible(
	// リスト情報ハンドル
	int TListHandle,
	
	// 新しい表示状態
	bool Visible
);

// リスト情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolList_GetVisible(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報の選択行を変更する
extern void ToolList_SetSelectIndex(
	// リスト情報ハンドル
	int TListHandle,
	
	// 行番号
	int Index
);

// リスト情報の選択している行を取得する
//     戻り値 : 選択している行の番号( どの行も選択されていない場合は -1 )
extern int ToolList_GetSelectIndex(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報に追加した行の数を取得する
extern int ToolList_GetObjNum(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報に追加した全ての行をあわせた縦幅を取得する
//     戻り値 : リストの全行の縦幅( ピクセル単位 )
extern int ToolList_GetHeight(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報に追加した全ての行をあわせた横幅を取得する
//     戻り値 : リストの全行の横幅( ピクセル単位 )
extern int ToolList_GetWidth(
	// リスト情報ハンドル
	int TListHandle
);

// 全てのリスト情報の状態推移処理を行う
extern void ToolList_Step_All( void );

// 全てのリスト情報の描画移処理を行う
extern void ToolList_Draw_All( void );

#endif

