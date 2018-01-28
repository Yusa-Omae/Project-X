#ifndef TOOLLIB_FILESELECT_H
#define TOOLLIB_FILESELECT_H

// ファイル選択処理情報を作成する
//     戻り値 : ファイル選択処理情報ハンドル( 失敗した場合は -1 が返る )
extern int  ToolFileSelect_Create(
	// 名前の文字列が格納されたメモリアドレス
	const char *Name,
	
	// ファイル選択処理の表示をする座標
	int x,
	int y
);

// ファイル選択処理情報を削除する
extern void ToolFileSelect_Delete(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
);

// 全てのファイル選択処理情報を削除する
extern void ToolFileSelect_Delete_All( void );

// ファイル選択処理情報に関連付ける表示状態情報をセットする
extern void ToolFileSelect_SetVisibleHandle(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
);

// ファイル選択処理情報と関連付けるウインドウ情報を設定する
extern void ToolFileSelect_SetWindow(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// ファイル選択処理情報の表示位置を変更する
extern void ToolFileSelect_SetPosition(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 変更後の座標
	int x,
	int y
);

// ファイル選択処理情報の表示状態を変更する
extern void ToolFileSelect_SetVisible(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// 新しい表示状態
	bool Visible
);

// ファイル選択処理情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolFileSelect_GetVisible(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle
);

// ファイル選択処理情報の選択処理が完了しているかどうかを取得する
//     戻り値 : 処理が完了しているかどうか( true:完了している  false:まだ終わっていない )
extern bool ToolFileSelect_CheckEnd(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,

	// 処理が完了しているかどうかのフラグを倒すかどうか
	bool Reset
);

// ファイル選択処理情報のファイルパスを設定する
extern void ToolFileSelect_SetPath(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ファイルパス文字列が格納されたメモリ領域の先頭アドレス
	const char *FilePath
);

// ファイル選択処理情報のファイルパスを取得する
extern void ToolFileSelect_GetPath(
	// ファイル選択処理情報ハンドル
	int TFileSelectHandle,
	
	// ファイルパスを格納するバッファの先頭アドレス
	char *DestBuffer
);

// 全てのファイル選択処理情報の状態推移処理を行う
extern void ToolFileSelect_Step_All( void );

// 全てのファイル選択処理情報の描画移処理を行う
extern void ToolFileSelect_Draw_All( void );

#endif


