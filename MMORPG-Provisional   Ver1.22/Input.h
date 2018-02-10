#ifndef INPUT_H
#define INPUT_H

#include "DxLib.h"

// 入力情報
typedef enum _EInputType
{
	EInputType_Left,				// 方向入力左
	EInputType_Right,				// 方向入力右
	EInputType_Up,					// 方向入力上
	EInputType_Down,				// 方向入力下

	EInputType_Camera_Left,			// カメラ用方向入力左
	EInputType_Camera_Right,		// カメラ用方向入力右
	EInputType_Camera_Up,			// カメラ用方向入力上
	EInputType_Camera_Down,			// カメラ用方向入力下

	EInputType_Attack,				// 攻撃ボタン
	EInputType_Defence,				// 防御ボタン
	EInputType_Jump,				// ジャンプボタン
	EInputType_Pause,				// ポーズボタン

	EInputType_Num,					// 入力タイプの数
} EInputType;

// 入力処理の初期化を行う
extern void   InputInitialize( void );

// 入力処理を行う
extern void   ProcessInput( void );

// ボタンの入力状態を取得する
//     戻り値 : ボタンの入力状態
extern int    GetInput( void );

// ボタンの押された瞬間のみの入力状態を取得する
//     戻り値 : ボタンの押された瞬間のみの入力状態
extern int    GetEdgeInput( void );

// カメラ操作用のスティックの入力状態を取得する
//     戻り値 : カメラ操作用のスティックの入力状態
extern VECTOR GetCameraStickInput( void );

// ゲームでの入力とキーやパッドなどの入力との対応情報をファイルに保存する
extern void   SaveInputSetting( void );

// ゲームでの入力とキーやパッドなどの入力との対応情報をファイルから読み込む
//     戻り値 : ファイルの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool   LoadInputSetting( void );

// ゲームでの入力とキーやパッドなどの入力との対応情報をデフォルト設定にする
extern void   SetDefaultSetting( void );

// 現在押下されているキーやパッドの入力情報をチェックして、入力があった場合は情報更新する
//     戻り値 : 入力があり、情報が更新されたかどうか
//              ( true:更新された  false:更新されなかった )
extern bool   UpdateInputTypeInfo(
	// 情報を更新する入力情報
	EInputType UpdateInputType
);

// 指定の入力情報タイプに割り当てられているパッドの入力とキーの入力の名前を取得する
extern void   GetInputTypeString(
	// 名前を取得したい入力情報タイプ
	EInputType InputType,

	// 割り当てられているパッド入力の名前を代入するバッファへのアドレス
	char *PadInputString,

	// 割り当てられているキー入力の名前を代入するバッファへのアドレス
	char *KeyInputString
);


#endif
