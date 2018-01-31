#include "Input.h"
#include "Code\Common\Mouse\Mouse.h"
#include "DxLib.h"
#include "BinaryFile.h"
#include "System.h"
#include <string.h>
#include <math.h>

// 入力コンフィグファイルのパス
#define INPUTCONFIG_PATH		"Data\\InputConfig.dat"

// チェックするゲームパッドの数
#define MAX_GAMEPAD_NUM				(16)

// キーボードの入力状態を取得するためのバッファのサイズ
#define KEY_STATE_BUFFER_SIZE		(256)

// DirectInput から得られる値の最大値
#define DIRECTINPUT_MAX_VALUE		(1000)

// DirectInput の入力情報タイプ
typedef enum _EDInputType
{
	EDInputType_X,					// スティックのＸ軸
	EDInputType_Y,					// スティックのＹ軸
	EDInputType_Z,					// スティックのＺ軸
	EDInputType_Rx,					// スティックのＸ軸回転
	EDInputType_Ry,					// スティックのＹ軸回転
	EDInputType_Rz,					// スティックのＺ軸回転
	EDInputType_POV,				// 方向コントローラ
	EDInputType_Button,				// ボタン
} EDInputType;

// キーボードの１キー辺りの情報
typedef struct _SKeyInfo
{
	// 入力コード( KEY_INPUT_LEFT など )
	int              KeyInput;

	// キーの名前
	const char *     Name;
} SKeyInfo;

// ゲームでの入力とキーやパッドなどの入力との対応情報
typedef struct _SInputTypeInfo
{
	// パッド番号
	int              PadNo;

	// DirectInput の入力情報タイプ
	EDInputType      DirectInputType;

	// サブ情報( DirectInputType によって意味が変わる )
	int              SubInfo[ 2 ];

	// 対応するキーボードのキー( KEY_INPUT_UP など )
	int              KeyInput;

} SInputTypeInfo;

// 入力処理用の情報
typedef struct _SInputSystemData
{
	// ゲーム中の入力とキーやパッドなどの入力との対応情報
	SInputTypeInfo   InputTypeInfo[ EInputType_Num ];
	
	// DirectInput のパッドの入力状態
	DINPUT_JOYSTATE  DirectInputState[ MAX_GAMEPAD_NUM ];

	// キーの入力状態
	char             KeyInput[ KEY_STATE_BUFFER_SIZE ];

	// ボタンの入力状態
	int              ButtonInput;

	// ボタンの押された瞬間のみの入力状態
	int              ButtonEdgeInput;

	// カメラ操作用のスティック入力状態
	VECTOR           CameraStickInput;
} SInputSystemData;

// 入力処理用の情報
static SInputSystemData g_InpSys;

// ゲームでの各入力とキーやパッドなどの入力とのデフォルトの対応設定
static SInputTypeInfo g_DefaultInputTypeInfo[ EInputType_Num ] =
{
	0, EDInputType_X,      -1, 0, KEY_INPUT_A,		// EInputType_Left
	0, EDInputType_X,       1, 0, KEY_INPUT_D,		// EInputType_Right
	0, EDInputType_Y,      -1, 0, KEY_INPUT_W,		// EInputType_Up 
	0, EDInputType_Y,       1, 0, KEY_INPUT_S,		// EInputType_Down 
	0, EDInputType_Rx,     -1, 0, KEY_INPUT_V,		// EInputType_Camera_Left
	0, EDInputType_Rx,      1, 0, KEY_INPUT_N,		// EInputType_Camera_Right
	0, EDInputType_Ry,     -1, 0, KEY_INPUT_G,		// EInputType_Camera_Up
	0, EDInputType_Ry,      1, 0, KEY_INPUT_B,		// EInputType_Camera_Down
	0, EDInputType_Button,  0, 0, KEY_INPUT_Z,		// EInputType_Attack
	0, EDInputType_Button,  1, 0, KEY_INPUT_X,		// EInputType_Defence
	0, EDInputType_Button,  2, 0, KEY_INPUT_C,		// EInputType_Jump
	0, EDInputType_Button,  3, 0, KEY_INPUT_SPACE,	// EInputType_Pause
};

// パッドの識別子テーブル
static int g_PadIDTable[ MAX_GAMEPAD_NUM ] =
{
	DX_INPUT_PAD1,
	DX_INPUT_PAD2,
	DX_INPUT_PAD3,
	DX_INPUT_PAD4,
	DX_INPUT_PAD5,
	DX_INPUT_PAD6,
	DX_INPUT_PAD7,
	DX_INPUT_PAD8,
	DX_INPUT_PAD9,
	DX_INPUT_PAD10,
	DX_INPUT_PAD11,
	DX_INPUT_PAD12,
	DX_INPUT_PAD13,
	DX_INPUT_PAD14,
	DX_INPUT_PAD15,
	DX_INPUT_PAD16,
};

// キーコードの情報
static SKeyInfo g_KeyInfoTable[] =
{
	KEY_INPUT_BACK,			"BACK",			// バックスペースキー
	KEY_INPUT_TAB,			"TAB",			// タブキー
	KEY_INPUT_RETURN,		"ENTER",		// エンターキー

	KEY_INPUT_LSHIFT,		"L SHIFT",		// 左シフトキー
	KEY_INPUT_RSHIFT,		"R SHIFT",		// 右シフトキー
	KEY_INPUT_LCONTROL,		"L CONTROL",	// 左コントロールキー
	KEY_INPUT_RCONTROL,		"R CONTROL",	// 右コントロールキー
	KEY_INPUT_ESCAPE,		"ESCAPE",		// エスケープキー
	KEY_INPUT_SPACE,		"SPACE",		// スペースキー
	KEY_INPUT_PGUP,			"PAGE UP",		// ＰａｇｅＵＰキー
	KEY_INPUT_PGDN,			"PAGE DOWN",	// ＰａｇｅＤｏｗｎキー
	KEY_INPUT_END,			"END",			// エンドキー
	KEY_INPUT_HOME,			"HOME",			// ホームキー
	KEY_INPUT_LEFT,			"LEFT",			// 左キー
	KEY_INPUT_UP,			"UP",			// 上キー
	KEY_INPUT_RIGHT,		"RIGHT",		// 右キー
	KEY_INPUT_DOWN,			"DOWN",			// 下キー
	KEY_INPUT_INSERT,		"INSERT",		// インサートキー
	KEY_INPUT_DELETE,		"DELETE",		// デリートキー

	KEY_INPUT_MINUS,		"-",			// －キー
	KEY_INPUT_YEN,			"\\",			// ￥キー
	KEY_INPUT_PREVTRACK,	"^",			// ＾キー
	KEY_INPUT_PERIOD,		".",			// ．キー
	KEY_INPUT_SLASH,		"/",			// ／キー
	KEY_INPUT_LALT,			"L ALT",		// 左ＡＬＴキー
	KEY_INPUT_RALT,			"R ALT",		// 右ＡＬＴキー
	KEY_INPUT_SCROLL,		"SCROLL LOCK",	// ScrollLockキー
	KEY_INPUT_SEMICOLON,	";",			// ；キー
	KEY_INPUT_COLON,		":",			// ：キー
	KEY_INPUT_LBRACKET,		"[",			// ［キー
	KEY_INPUT_RBRACKET,		"]",			// ］キー
	KEY_INPUT_AT,			"@",			// ＠キー
	KEY_INPUT_BACKSLASH,	"BACK SLASH",	// ＼キー
	KEY_INPUT_COMMA,		",",			// ，キー
	KEY_INPUT_KANJI,		"KANJI",		// 漢字キー
	KEY_INPUT_CONVERT,		"CONVERT",		// 変換キー
	KEY_INPUT_NOCONVERT,	"NO CONVERT",	// 無変換キー
	KEY_INPUT_KANA,			"KANA",			// カナキー
	KEY_INPUT_APPS,			"APPS",			// アプリケーションメニューキー
	KEY_INPUT_CAPSLOCK,		"CAPS LOCK",	// CaspLockキー
	KEY_INPUT_SYSRQ,		"PRINT SCREEN",	// PrintScreenキー
	KEY_INPUT_PAUSE,		"PAUSE BREAK",	// PauseBreakキー
	KEY_INPUT_LWIN,			"L WIN",		// 左Ｗｉｎキー
	KEY_INPUT_RWIN,			"R WIN",		// 右Ｗｉｎキー

	KEY_INPUT_NUMLOCK,		"NUM LOCK",		// テンキー０
	KEY_INPUT_NUMPAD0,		"NUMPAD 0",		// テンキー０
	KEY_INPUT_NUMPAD1,		"NUMPAD 1",		// テンキー１
	KEY_INPUT_NUMPAD2,		"NUMPAD 2",		// テンキー２
	KEY_INPUT_NUMPAD3,		"NUMPAD 3",		// テンキー３
	KEY_INPUT_NUMPAD4,		"NUMPAD 4",		// テンキー４
	KEY_INPUT_NUMPAD5,		"NUMPAD 5",		// テンキー５
	KEY_INPUT_NUMPAD6,		"NUMPAD 6",		// テンキー６
	KEY_INPUT_NUMPAD7,		"NUMPAD 7",		// テンキー７
	KEY_INPUT_NUMPAD8,		"NUMPAD 8",		// テンキー８
	KEY_INPUT_NUMPAD9,		"NUMPAD 9",		// テンキー９
	KEY_INPUT_MULTIPLY,		"NUMPAD *",		// テンキー＊キー
	KEY_INPUT_ADD,			"NUMPAD +",		// テンキー＋キー
	KEY_INPUT_SUBTRACT,		"NUMPAD -",		// テンキー－キー
	KEY_INPUT_DECIMAL,		"NUMPAD .",		// テンキー．キー
	KEY_INPUT_DIVIDE,		"NUMPAD /",		// テンキー／キー
	KEY_INPUT_NUMPADENTER,	"NUMPAD ENTER",	// テンキーのエンターキー

	KEY_INPUT_F1,			"F1",			// Ｆ１キー
	KEY_INPUT_F2,			"F2",			// Ｆ２キー
	KEY_INPUT_F3,			"F3",			// Ｆ３キー
	KEY_INPUT_F4,			"F4",			// Ｆ４キー
	KEY_INPUT_F5,			"F5",			// Ｆ５キー
	KEY_INPUT_F6,			"F6",			// Ｆ６キー
	KEY_INPUT_F7,			"F7",			// Ｆ７キー
	KEY_INPUT_F8,			"F8",			// Ｆ８キー
	KEY_INPUT_F9,			"F9",			// Ｆ９キー
	KEY_INPUT_F10,			"F10",			// Ｆ１０キー
	KEY_INPUT_F11,			"F11",			// Ｆ１１キー
	KEY_INPUT_F12,			"F12",			// Ｆ１２キー

	KEY_INPUT_A,			"A",			// Ａキー
	KEY_INPUT_B,			"B",			// Ｂキー
	KEY_INPUT_C,			"C",			// Ｃキー
	KEY_INPUT_D,			"D",			// Ｄキー
	KEY_INPUT_E,			"E",			// Ｅキー
	KEY_INPUT_F,			"F",			// Ｆキー
	KEY_INPUT_G,			"G",			// Ｇキー
	KEY_INPUT_H,			"H",			// Ｈキー
	KEY_INPUT_I,			"I",			// Ｉキー
	KEY_INPUT_J,			"J",			// Ｊキー
	KEY_INPUT_K,			"K",			// Ｋキー
	KEY_INPUT_L,			"L",			// Ｌキー
	KEY_INPUT_M,			"M",			// Ｍキー
	KEY_INPUT_N,			"N",			// Ｎキー
	KEY_INPUT_O,			"O",			// Ｏキー
	KEY_INPUT_P,			"P",			// Ｐキー
	KEY_INPUT_Q,			"Q",			// Ｑキー
	KEY_INPUT_R,			"R",			// Ｒキー
	KEY_INPUT_S,			"S",			// Ｓキー
	KEY_INPUT_T,			"T",			// Ｔキー
	KEY_INPUT_U,			"U",			// Ｕキー
	KEY_INPUT_V,			"V",			// Ｖキー
	KEY_INPUT_W,			"W",			// Ｗキー
	KEY_INPUT_X,			"X",			// Ｘキー
	KEY_INPUT_Y,			"Y",			// Ｙキー
	KEY_INPUT_Z,			"Z",			// Ｚキー

	KEY_INPUT_0, 			"0",			// ０キー
	KEY_INPUT_1,			"1",			// １キー
	KEY_INPUT_2,			"2",			// ２キー
	KEY_INPUT_3,			"3",			// ３キー
	KEY_INPUT_4,			"4",			// ４キー
	KEY_INPUT_5,			"5",			// ５キー
	KEY_INPUT_6,			"6",			// ６キー
	KEY_INPUT_7,			"7",			// ７キー
	KEY_INPUT_8,			"8",			// ８キー
	KEY_INPUT_9,			"9",			// ９キー

	-1,						NULL,
};

// 入力処理の初期化を行う
void InputInitialize( void )
{
	// ゲームでの入力とキーやパッドなどの入力との対応情報をファイルから読み込む
	if( !LoadInputSetting() )
	{
		// ファイルの読み込みに失敗した場合はデフォルト設定にする
		SetDefaultSetting();
	}

}

// ProcessInput 用の軸入力タイプの処理を行う補助関数
static void ProcessInput_Assist(
	// 処理結果を代入する変数のアドレス
	int *InputState,

	// 軸入力タイプのサブ情報
	//    (  1:InputValue がプラスの値の場合に入力ありとする
	//      -1:InputValue がマイナスの値の場合に入力ありとする )
	int SubInfo,

	// 軸入力の値
	int InputValue
)
{
	// サブ情報が 1 か -1 かで処理を分岐
	switch( SubInfo )
	{
	case 1:		// 1の場合は InputValue がプラスの場合のみ入力ありとする
		if( InputValue > 0 )
		{
			*InputState = InputValue;
		}
		break;

	case -1:	// -1の場合は InputValue がプラスの場合のみ入力ありとする
		if( InputValue < 0 )
		{
			*InputState = -InputValue;
		}
		break;
	}
}

// 入力処理を行う
void ProcessInput( void )
{
	int              i;
	SInputTypeInfo  *ITInfo;
	int              InputState[ EInputType_Num ];
	int              PadNum;
	DINPUT_JOYSTATE *DIJState;
	int              ButtonPrevInput;
	int              Total;

	// パッドの数を取得する
	PadNum = GetJoypadNum();

	// パッドの数がプログラムが対応している数より多い場合は、対応している数に制限する
	if( PadNum > MAX_GAMEPAD_NUM )
	{
		PadNum = MAX_GAMEPAD_NUM;
	}

	// パッドの入力情報を DirectInput から取得する
	for( i = 0; i < PadNum; i++ )
	{
		GetJoypadDirectInputState( g_PadIDTable[ i ], &g_InpSys.DirectInputState[ i ] );
	}

	// 現在のキーの入力状態を取得する
	GetHitKeyStateAll( g_InpSys.KeyInput );

	// ゲームで使用する入力情報を構築する
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		// 『入力なし』状態にしておく
		InputState[ i ] = 0;

		// 対応しているキーが押されていたら『入力あり』にする
		if( g_InpSys.KeyInput[ ITInfo->KeyInput ] != 0 )
		{
			InputState[ i ] = DIRECTINPUT_MAX_VALUE;
		}


		// 対応する DirectInput の情報タイプによって処理を分岐
		DIJState = &g_InpSys.DirectInputState[ ITInfo->PadNo ];
		switch( ITInfo->DirectInputType )
		{
		case EDInputType_X:	// スティックのＸ軸の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->X );
			break;

		case EDInputType_Y:	// スティックのＹ軸の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Y );
			break;

		case EDInputType_Z:	// スティックのＺ軸の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Z );
			break;

		case EDInputType_Rx:	// スティックのＸ軸回転の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Rx );
			break;

		case EDInputType_Ry:	// スティックのＹ軸回転の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Ry );
			break;

		case EDInputType_Rz:	// スティックのＺ軸回転の場合
			ProcessInput_Assist( &InputState[ i ], ITInfo->SubInfo[ 0 ], DIJState->Rz );
			break;

		case EDInputType_POV:	// 方向コントローラの場合
			// 設定されている方向に入力されていたら『入力あり』にする
			if( DIJState->POV[ ITInfo->SubInfo[ 0 ] ] == ITInfo->SubInfo[ 1 ] )
			{
				InputState[ i ] = DIRECTINPUT_MAX_VALUE;
			}
			break;

		case EDInputType_Button:	// ボタンの場合
			// 設定されているボタンが押されていたら『入力あり』にする
			if( DIJState->Buttons[ ITInfo->SubInfo[ 0 ] ] == 128 )
			{
				InputState[ i ] = DIRECTINPUT_MAX_VALUE;
			}
			break;
		}
	}



	// １フレーム前の入力状態をとっておく
	ButtonPrevInput = g_InpSys.ButtonInput;

	// 入力状態の初期化
	g_InpSys.ButtonInput = 0;

	// 各入力に対応するビットを『入力あり』の場合は立てる
	for( i = 0; i < EInputType_Num; i++ )
	{
		if( InputState[ i ] > 0 )
		{
			g_InpSys.ButtonInput |= 1 << i;
		}
	}

	// 今回のフレームで『入力あり』になり始めたビットを算出する
	g_InpSys.ButtonEdgeInput = g_InpSys.ButtonInput & ~ButtonPrevInput;

	// カメラ操作用のスティック入力状態を算出する
	Total = InputState[ EInputType_Camera_Right ] - InputState[ EInputType_Camera_Left ];
	g_InpSys.CameraStickInput.x = Total / ( float )DIRECTINPUT_MAX_VALUE;

	Total = InputState[ EInputType_Camera_Down  ] - InputState[ EInputType_Camera_Up   ];
	g_InpSys.CameraStickInput.y = Total / ( float )DIRECTINPUT_MAX_VALUE;

	g_InpSys.CameraStickInput.z = 0.0f;
}

// ボタンの入力状態を取得する
//     戻り値 : ボタンの入力状態
int GetInput( void )
{
	return g_InpSys.ButtonInput;
}

// ボタンの押された瞬間のみの入力状態を取得する
//     戻り値 : ボタンの押された瞬間のみの入力状態
int GetEdgeInput( void )
{
	return g_InpSys.ButtonEdgeInput;
}

// カメラ操作用のスティックの入力状態を取得する
//     戻り値 : カメラ操作用のスティックの入力状態
VECTOR GetCameraStickInput( void )
{
	return g_InpSys.CameraStickInput;
}

// ゲームでの入力とキーやパッドなどの入力との対応情報をファイルに保存する
void SaveInputSetting( void )
{
	SBinaryFileData BinFileData;
	SInputTypeInfo *ITInfo;
	int             i;

	// 対応情報を保存するファイルを開く
	if( !WriteBinFile_Open( &BinFileData, INPUTCONFIG_PATH ) )
	{
		// 開けなかったら何もせずに終了
		return;
	}

	// ゲームの各入力とキーやパッドなどの入力との対応情報をファイルに書き出す
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		WriteBinFile_Int( &BinFileData, ITInfo->PadNo );
		WriteBinFile_Int( &BinFileData, ITInfo->DirectInputType );
		WriteBinFile_Int( &BinFileData, ITInfo->SubInfo[ 0 ] );
		WriteBinFile_Int( &BinFileData, ITInfo->SubInfo[ 1 ] );
		WriteBinFile_Int( &BinFileData, ITInfo->KeyInput );
	}

	// ファイルを閉じる
	WriteBinFile_Close( &BinFileData );
}

// ゲームでの入力とキーやパッドなどの入力との対応情報をファイルから読み込む
//     戻り値 : ファイルの読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
bool LoadInputSetting( void )
{
	SBinaryFileData BinFileData;
	SInputTypeInfo *ITInfo;
	int             i;
	int             ParamInt;

	// 対応情報を保存したファイルを開く
	if( !ReadBinFile_Open( &BinFileData, INPUTCONFIG_PATH ) )
	{
		// 開けなかった場合は何もせずに終了
		return false;
	}

	// ゲームの各入力とキーやパッドなどの入力との対応情報をファイルから読み込む
	ITInfo = g_InpSys.InputTypeInfo;
	for( i = 0; i < EInputType_Num; i++, ITInfo++ )
	{
		ReadBinFile_Int( &BinFileData, &ITInfo->PadNo );
		ReadBinFile_Int( &BinFileData, &ParamInt );
		ITInfo->DirectInputType = ( EDInputType )ParamInt;
		ReadBinFile_Int( &BinFileData, &ITInfo->SubInfo[ 0 ] );
		ReadBinFile_Int( &BinFileData, &ITInfo->SubInfo[ 1 ] );
		ReadBinFile_Int( &BinFileData, &ITInfo->KeyInput );
	}

	// ファイルを閉じる
	ReadBinFile_Close( &BinFileData );

	// 成功終了
	return true;
}

// ゲームでの入力とキーやパッドなどの入力との対応情報をデフォルト設定にする
void SetDefaultSetting( void )
{
	int i;

	// デフォルト設定の情報をコピーする
	for( i = 0; i < EInputType_Num; i++ )
	{
		g_InpSys.InputTypeInfo[ i ] = g_DefaultInputTypeInfo[ i ];
	}
}

// UpdateInputTypeInfo 用の軸入力タイプの処理を行う補助関数
static void UpdateInputTypeInfo_Assist(
	// キーやパッドなどの入力情報
	SInputTypeInfo   *ITInfo,

	// MaxInput に有効な値が入っているかどうかを保存している変数のアドレス
	bool             *ValidMaxDInput,

	// 有効な入力値の最大値を保存している変数のアドレス
	int              *MaxDInput,

	// DirectInput の入力情報タイプ
	EDInputType       DirectInputType,

	// DirectInput の入力値
	int               DirectInputValue
)
{
	// DirectInput の入力値がマイナスかプラスかで処理を分岐
	if( DirectInputValue < 0 )
	{
		// MaxDirectInputValue にまだ有効な値が入っていないか
		// 又は MaxDirectInputValue の値よりも DirectInputValue の値が大きいかをチェック
		if( *ValidMaxDInput == false || *MaxDInput < -DirectInputValue )
		{
			// 既に ITInfo に設定されている入力タイプ情報と異なるかをチェック
			if( ITInfo->DirectInputType != DirectInputType || ITInfo->SubInfo[ 0 ] != -1 )
			{
				// 条件をクリアした場合は情報を更新する
				ITInfo->DirectInputType   = DirectInputType;
				ITInfo->SubInfo[ 0 ]      = -1;
				ITInfo->SubInfo[ 1 ]      = 0;

				*ValidMaxDInput = true;
				*MaxDInput      = -DirectInputValue;
			}
		}
	}
	else
	if( DirectInputValue > 0 )
	{
		// MaxDirectInputValue にまだ有効な値が入っていないか
		// 又は MaxDirectInputValue の値よりも DirectInputValue の値が大きいかをチェック
		if( *ValidMaxDInput == false || *MaxDInput < DirectInputValue )
		{
			// 既に ITInfo に設定されている入力タイプ情報と異なるかをチェック
			if( ITInfo->DirectInputType != DirectInputType || ITInfo->SubInfo[ 0 ] != 1 )
			{
				// 条件をクリアした場合は情報を更新する
				ITInfo->DirectInputType   = DirectInputType;
				ITInfo->SubInfo[ 0 ]      = 1;
				ITInfo->SubInfo[ 1 ]      = 0;

				*ValidMaxDInput = true;
				*MaxDInput      = DirectInputValue;
			}
		}
	}
}

// 現在押下されているキーやパッドの入力情報をチェックして、入力があった場合は情報更新する
//     戻り値 : 入力があり、情報が更新されたかどうか
//              ( true:更新された  false:更新されなかった )
bool UpdateInputTypeInfo(
	// 情報を更新する入力情報
	EInputType UpdateInputType
)
{
	int              i;
	int              j;
	bool             ValidMaxDInput;
	int              MaxDInput;
	int              PadNum;
	DINPUT_JOYSTATE *DIJState;
	SInputTypeInfo  *ITInfo = &g_InpSys.InputTypeInfo[ UpdateInputType ];
	bool             Result;

	// 戻り値は『情報の更新が無かった』にしておく
	Result = false;

	// 接続されているパッドの数を取得する
	PadNum = GetJoypadNum();

	// パッドの数がプログラムが対応している数より多い場合は、対応している数に制限する
	if( PadNum > MAX_GAMEPAD_NUM )
	{
		PadNum = MAX_GAMEPAD_NUM;
	}

	// 『MaxDInput に有効な値が入っているかどうか』のフラグを倒し、
	// MaxDInput の値も初期化しておく
	ValidMaxDInput = false;
	MaxDInput      = 0;

	// 全てのパッドの入力状態をチェック
	DIJState = g_InpSys.DirectInputState;
	for( i = 0; i < PadNum; i++, DIJState++ )
	{
		// 方向入力状態をチェック( DIJState->X と DIJState->Y は固定なのでチェックしない )
		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Z,  DIJState->Z );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Rx, DIJState->Rx );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Ry, DIJState->Ry );

		UpdateInputTypeInfo_Assist(
			ITInfo, &ValidMaxDInput, &MaxDInput, EDInputType_Rz, DIJState->Rz );

		// 方向コントローラの入力状態をチェック
		for( j = 0; j < 4; j++ )
		{
			// POV が有効な値( -1 以外の値 )で、
			// 且つ ITInfo に設定されている値と異なるかをチェック
			if( DIJState->POV[ j ] != -1 && 
				( ITInfo->DirectInputType != EDInputType_POV ||
				  ITInfo->SubInfo[ 0 ]    != j ||
				  ITInfo->SubInfo[ 1 ]    != DIJState->POV[ j ] ) )
			{
				// 条件をクリアした場合は情報を更新する
				ITInfo->DirectInputType = EDInputType_POV;
				ITInfo->SubInfo[ 0 ]    = j;
				ITInfo->SubInfo[ 1 ]    = DIJState->POV[ j ];

				// 戻り値を『情報の更新があった』にする
				Result = true;
			}
		}

		// ボタンの入力状態をチェック
		for( j = 0; j < 32; j++ )
		{
			// ボタンが押されていて、
			// 且つ ITInfo に 設定されている情報と異なるかをチェック
			if( DIJState->Buttons[ j ] == 128 &&
				( ITInfo->DirectInputType != EDInputType_Button ||
				  ITInfo->SubInfo[ 0 ]    != j ) )
			{
				// 条件をクリアした場合は情報を更新する
				ITInfo->DirectInputType = EDInputType_Button;
				ITInfo->SubInfo[ 0 ]    = j;
				ITInfo->SubInfo[ 1 ]    = 0;

				// 戻り値を『情報の更新があった』にする
				Result = true;
			}
		}
	}

	// ValidMaxDirectInputValue が true の場合は、有効な入力があり、且つ ITInfo に
	// 設定されている値が更新されたということなので戻り値を『情報の更新があった』にする
	if( ValidMaxDInput )
	{
		Result = true;
	}

	// キーの入力状態をチェック
	for( i = 0; i < KEY_STATE_BUFFER_SIZE; i++ )
	{
		// 入力があり、且つ ITInfo に設定されている情報と異なるかをチェック
		if( g_InpSys.KeyInput[ i ] && ITInfo->KeyInput != i )
		{
			// 条件をクリアした場合は情報を更新する
			ITInfo->KeyInput = i;

			// 戻り値を『情報の更新があった』にする
			Result = true;
		}
	}

	// 情報が更新されたかどうかを返す
	return Result;
}

// 指定の入力情報タイプに割り当てられているパッドの入力とキーの入力の名前を取得する
void GetInputTypeString(
	// 名前を取得したい入力情報タイプ
	EInputType InputType,

	// 割り当てられているパッド入力の名前を代入するバッファへのアドレス
	char *PadInputString,

	// 割り当てられているキー入力の名前を代入するバッファへのアドレス
	char *KeyInputString
)
{
	SInputTypeInfo *ITInfo = &g_InpSys.InputTypeInfo[ InputType ];
	SKeyInfo       *KInfo;
	char            String[ 256 ];

	// パッドの入力名は先頭にパッドの番号を付ける
	sprintf( String, "No.%d", ITInfo->PadNo );

	// 対応しているパッドの DirectInput の情報タイプによって分岐
	switch( ITInfo->DirectInputType )
	{
	case EDInputType_X:
		// X軸入力の場合はプラスの場合は RIGHT、マイナスの場合は LEFT を付ける
		sprintf( PadInputString, "%s %s",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "RIGHT" : "LEFT" );
		break;

	case EDInputType_Y:
		// Y軸入力の場合はプラスの場合は DOWN、マイナスの場合は UP を付ける
		sprintf( PadInputString, "%s %s",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "DOWN" : "UP" );
		break;

	case EDInputType_Z:
		// Z軸入力の場合はプラスの場合は +、マイナスの場合は - を付ける
		sprintf( PadInputString, "%s %sZ",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Rx:
		// X軸回転入力の場合はプラスの場合は +、マイナスの場合は - を付ける
		sprintf( PadInputString, "%s %sRx",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Ry:
		// Y軸回転入力の場合はプラスの場合は +、マイナスの場合は - を付ける
		sprintf( PadInputString, "%s %sRy",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_Rz:
		// Z軸回転入力の場合はプラスの場合は +、マイナスの場合は - を付ける
		sprintf( PadInputString, "%s %sRz",
			String, ITInfo->SubInfo[ 0 ] > 0 ? "+" : "-" );
		break;

	case EDInputType_POV:
		// 方向コントローラ入力の場合は POV であることと、POVの情報番号と入力角度を付ける
		sprintf( PadInputString, "%s POV %d Angle %d°",
			String, ITInfo->SubInfo[ 0 ], ITInfo->SubInfo[ 1 ] / 100 );
		break;

	case EDInputType_Button:
		// ボタン入力の場合は BUTTON とボタン番号を付ける
		sprintf( PadInputString, "%s BUTTON %d",
			String, ITInfo->SubInfo[ 0 ] );
		break;
	}

	// キーコードの情報テーブルに無いキーコードである場合に備えて、
	// 最初は UNKNOWN を代入しておく
	strcpy( KeyInputString, "UNKNOWN" );

	// 一致するキーコード情報をテーブルから探す
	for( KInfo = g_KeyInfoTable; KInfo->Name != NULL; KInfo++ )
	{
		if( KInfo->KeyInput == ITInfo->KeyInput )
		{
			strcpy( KeyInputString, KInfo->Name );
			break;
		}
	}
}


