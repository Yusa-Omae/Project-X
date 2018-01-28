#ifndef TOOLLIB_STRINGINPUT_H
#define TOOLLIB_STRINGINPUT_H

// 文字列入力情報を作成する
//     戻り値 : 文字列入力情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolStringInput_Create(
	// 名前の文字列が格納されたメモリアドレス
	const char *Name,
	
	// 文字列入力の表示位置
	int x,
	int y,

	// 数値のみ入力可能にするかどうか
	bool NumberOnly,

	// 入力できる数値の桁数( NumberOnly が true の場合のみ有効 )
	int NumberNum,

	// 文字列入力領域の横幅( NumberOnly が false の場合のみ有効 )
	int StringDrawWidth
);

// 文字列入力情報を削除する
extern void ToolStringInput_Delete(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 全ての文字列入力情報を削除する
extern void ToolStringInput_Delete_All( void );

// 文字列入力情報に関連付ける表示状態情報をセットする
extern void ToolStringInput_SetVisibleHandle(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
);

// 文字列入力情報の表示状態を変更する
extern void ToolStringInput_SetVisible(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 新しい表示状態
	bool Visible
);

// 文字列入力情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolStringInput_GetVisible(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報と関連付けるウインドウ情報を設定する
extern void ToolStringInput_SetWindow(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// 文字列入力情報の文字入力領域の座標を変更する
extern void ToolStringInput_SetPosition(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 変更後の文字入力領域の座標
	int x,
	int y
);

// 文字列入力情報の入力中の文字列を変更する
extern void ToolStringInput_SetString(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// セットする文字列が格納されたメモリアドレス
	const char *String
);

// 文字列入力情報の入力中の文字列を取得する
extern void ToolStringInput_GetString(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中の文字列を格納するメモリ領域の先頭アドレス
	char *String
);

// 文字列入力情報の文字列入力を無条件で終了する
extern void ToolStringInput_AlwaysEnd(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報の入力中の文字列として整数値に変更する
extern void ToolStringInput_SetIntNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中文字列として設定する整数値
	int Number
);

// 文字列入力情報の入力中の文字列として浮動小数点値に変更する
extern void ToolStringInput_SetFloatNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中文字列として設定する浮動小数点値
	float Number
);

// 文字列入力情報の入力中の文字列を整数値に変換したものを取得する
//     戻り値 : 入力中の文字列を整数値に変換した値
extern int ToolStringInput_GetIntNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報の入力中の文字列を浮動小数点値に変換したものを取得する
//     戻り値 : 入力中の文字列を浮動小数点値に変換した値
extern float ToolStringInput_GetFloatNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報の文字列の入力が終了しているかどうかを取得する
//     戻り値 : 入力が終了しているかどうか( true:終了している  false:終了していない )
extern bool ToolStringInput_CheckEnd(
	// 文字列入力情報ハンドル
	int TStringInputHandle,

	// 文字列の入力が終了しているかどうかのフラグをリセットするかどうか
	bool Reset
);

// 文字列入力情報の文字列入力がアクティブになっていた場合にアクティブ状態を解除する
extern void ToolStringInput_ResetActive(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 全ての文字列入力情報の状態推移処理を行う
//    戻り値 : 処理が正常に終了したかどうか( true:正常終了  false:エラー発生 )
extern bool ToolStringInput_Step_All( void );

// 全ての文字列入力情報の描画移処理を行う
extern void ToolStringInput_Draw_All( void );

#endif

