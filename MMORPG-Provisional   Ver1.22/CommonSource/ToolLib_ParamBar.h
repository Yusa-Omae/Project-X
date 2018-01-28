#ifndef TOOLLIB_PARAMBAR_H
#define TOOLLIB_PARAMBAR_H

// パラメータバー情報を作成する
//     戻り値 : パラメータバー情報ハンドル( 失敗した場合は -1 が返る )
extern int ToolParamBar_Create(
	// 名前の文字列が格納されたメモリアドレス
	const char *Name,

	// 表示座標
	int PositionX,
	int PositionY,

	// バーを表示する相対X座標
	int BarPositionX,

	// バーの横幅
	int BarWidth,

	// 表示する数値の桁数
	int NumberLength,
	
	// バーで操作できる範囲の値の最小値と最大値
	float ParamMin,
	float ParamMax,

	// 初期値
	float InitParam
);

// パラメータバー情報を削除する
extern void ToolParamBar_Delete(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// 全てのパラメータバー情報を削除する
extern void ToolParamBar_Delete_All( void );

// パラメータバー情報に関連付ける表示状態情報をセットする
extern void ToolParamBar_SetVisibleHandle(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
);

// パラメータバー情報の表示状態を変更する
extern void ToolParamBar_SetVisible(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 新しい表示状態
	bool Visible
);

// パラメータバー情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
extern bool ToolParamBar_GetVisible(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// パラメータバー情報と関連付けるウインドウ情報を設定する
extern void ToolParamBar_SetWindow(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// パラメータバー情報の座標を変更する
extern void ToolParamBar_SetPosition(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 変更後の座標
	int PositionX,
	int PositionY
);

// パラメータバー情報の数値を変更する
extern void ToolParamBar_SetParam(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 変更後の数値
	float Param
);

// パラメータバー情報のバーで変更できる数値の最大値と最小値を変更する
extern void ToolParamBar_SetMinMax(
	// パラメータバー情報ハンドル
	int TParamBarHandle,
	
	// 新しい最小値と最大値
	float ParamMin,
	float ParamMax
);

// パラメータバー情報の数値編集が行われていた場合に無条件で終了させる
extern void ToolParamBar_AlwaysEnd(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// パラメータバー情報の数値に変化があったかどうかを取得する
//     戻り値 : 数値に変化があったかどうか( true:変化あり false:変化なし )
extern bool ToolParamBar_CheckChange(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// パラメータバー情報の現在の数値を取得する
//     戻り値 : パラメータバーの値
extern float ToolParamBar_GetParam(
	// パラメータバー情報ハンドル
	int TParamBarHandle
);

// 全てのパラメータバー情報の状態推移処理を行う
extern void ToolParamBar_Step_All( void );

// 全てのパラメータバー情報の描画移処理を行う
extern void ToolParamBar_Draw_All( void );

#endif
