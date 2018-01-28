#ifndef TOOLLIB_BUTTON_H
#define TOOLLIB_BUTTON_H

// ボタン情報を作成する
//     戻り値 : ボタン情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolButton_Create(
	// ＯＮ状態とＯＦＦ状態が押すたびに切り替わるタイプのボタンかどうか
	bool On_Off_SwitchType,

	// 名前となる文字列のアドレス
	const char *Name,
	
	// ボタンを表示する左上座標
	int x,
	int y,
	
	// ボタンの幅と高さ
	int w,
	int h
);

// ボタン情報を削除する
extern void ToolButton_Delete(
	// ボタン情報ハンドル
	int TButtonHandle
);

// 全てのボタン情報を削除する
extern void ToolButton_Delete_All( void );

// ボタン情報に関連付ける表示状態情報をセットする
extern void ToolButton_SetVisibleHandle(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
);

// ボタン情報の名前を設定する
//     戻り値 : 処理が正常に終了したかどうか( true:正常に終了した  false:エラー発生 )
extern bool ToolButton_SetName(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 名前となる文字列のアドレス
	const char *Name
);

// ボタン情報と関連付けるウインドウ情報を設定する
extern void ToolButton_SetWindow(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// ボタン情報の位置を変更する
extern void ToolButton_SetPosition(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 変更後のボタンの座標
	int x,
	int y
);

// ボタン情報のＯＮ／ＯＦＦ状態を変更する
extern void ToolButton_SetOnOffFlag(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 変更後のＯＮ／ＯＦＦ状態( true:ON  false:OFF )
	bool Flag
);

// ボタン情報のＯＮ／ＯＦＦ状態を取得する
//     戻り値 : ＯＮ／ＯＦＦ状態( ture:ON  false:OFF )
extern bool ToolButton_GetOnOffFlag(
	// ボタン情報ハンドル
	int TButtonHandle
);

// ボタン情報のボタンがクリックされたかどうかを取得する
//     戻り値 : ボタンがクリックされたかどうか
//              ( true:クリックされた  false:クリックされていない )
extern bool ToolButton_GetClick(
	// ボタン情報ハンドル
	int TButtonHandle,

	// クリックされたかどうかの状態をリセットするかどうか
	// ( true:リセットする  false:リセットしない )
	bool Reset
);

// ボタン情報のボタンが押下されているかどうかを取得する
//     戻り値 : ボタンが押下されているかどうか
//              ( true:押下されている  false:押下されていない )
extern bool ToolButton_GetBottom(
	// ボタン情報ハンドル
	int TButtonHandle
);

// ボタン情報のボタンが押下されているかどうかを取得する( リピート機能付き版 )
//     戻り値 : ボタンが押下されているかどうか
//              ( true:押下されている  false:押下されていない )
extern bool ToolButton_GetRepeatBottom(
	// ボタン情報ハンドル
	int TButtonHandle
);

// ボタン情報の表示状態を変更する
extern void ToolButton_SetVisible(
	// ボタン情報ハンドル
	int TButtonHandle,
	
	// 新しい表示状態
	bool Visible
);

// ボタン情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolButton_GetVisible(
	// ボタン情報ハンドル
	int TButtonHandle
);

// 全てのボタン情報の状態推移処理を行う
extern void ToolButton_Step_All(
	// 進める時間( 単位 : 秒 )
	float StepTime
);

// 全てのボタン情報の描画移処理を行う
extern void ToolButton_Draw_All( void );

#endif
