#ifndef TEXTPARAM_H
#define TEXTPARAM_H

#include "DxLib.h"

// テキストパラメータの名前に使える最大文字数
#define TEXTPARAM_NAME_MAXLENGTH		(64)

// ファイル一つに含められるテキストパラメータの最大数
#define TEXTPARAM_MAXNUM				(1024)

// 文字列タイプのパラメータの最大文字数
#define TEXTPARAM_STRING_MAXLENGTH		(64)

// パラメータタイプ
typedef enum _ETextParamType
{
	ETextParamType_Int,				// 整数値
	ETextParamType_Bool,			// bool値
	ETextParamType_Float,			// 浮動小数点数値
	ETextParamType_Vector,			// VECTOR値
	ETextParamType_String,			// 文字列
	ETextParamType_Color,			// カラー値

	ETextParamType_Num,				// パラメータタイプの数
} ETextParamType;

// パラメータ本体の共用体
typedef union _STextParam
{
	// intタイプのパラメータ用変数
	int                Int;

	// boolタイプのパラメータ用変数
	bool               Bool;

	// floatタイプのパラメータ用変数
	float              Float;

	// vectorタイプのパラメータ用変数
	VECTOR             Vector;

	// stringタイプのパラメータ用変数
	char               String[TEXTPARAM_STRING_MAXLENGTH];

	// colorタイプのパラメータ用変数
	COLOR_U8           Color;
} STextParam;

// パラメータ一つの情報
typedef struct _STextParamInfoUnit
{
	// パラメータの名前
	char               Name[TEXTPARAM_NAME_MAXLENGTH];

	// パラメータタイプ
	ETextParamType     Type;

	// パラメータ本体
	STextParam         Param;
} STextParamInfoUnit;

// １ファイルのテキストパラメータ情報
typedef struct _STextParamInfo
{
	// パラメータの数
	int                ParamNum;

	// パラメータ配列
	STextParamInfoUnit Param[TEXTPARAM_MAXNUM];
} STextParamInfo;

// パラメータファイルを読み込む
//     戻り値 : 読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool        LoadTextParam(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// ファイルパス
	const char *FilePath,

	// ファイルパス生成用可変個引数
	...
);

// 指定の名前のパラメータの配列中のインデックスを取得する
//     戻り値 : 配列インデックス
//              ( パラメータが見つからなかった場合は -1 が返る )
extern int         SearchTextParam(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のintタイプのパラメータを取得する
//     戻り値 : パラメータの整数値
extern int         GetTextParamInt(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のboolタイプのパラメータを取得する
//     戻り値 : パラメータのbool値
extern bool        GetTextParamBool(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のfloatタイプのパラメータを取得する
//     戻り値 : パラメータの浮動小数点値
extern float       GetTextParamFloat(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のvectorタイプのパラメータを取得する
//     戻り値 : パラメータのVECTOR値
extern VECTOR      GetTextParamVector(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のstringタイプのパラメータを取得する
//     戻り値 : パラメータの文字列のアドレス
extern const char *GetTextParamString(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のcolorタイプのパラメータを取得する
//     戻り値 : パラメータのCOLOR_U8値
extern COLOR_U8    GetTextParamColor(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,

	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

#endif

#if false
#include"DxLib.h"
#include"Define.h"

// パラメータタイプ
typedef enum _ETextParamType
{
	ETextParamType_Int,				// 整数値
	ETextParamType_Bool,			// bool値
	ETextParamType_Float,			// 浮動小数点数値
	ETextParamType_Vector,			// VECTOR値
	ETextParamType_String,			// 文字列
	ETextParamType_Color,			// カラー値

	ETextParamType_Num,				// パラメータタイプの数
} ETextParamType;

// パラメータ本体
typedef union _STextParam
{
	// intタイプのパラメータ用変数
	int                Int;
	// boolタイプのパラメータ用変数
	bool               Bool;
	// floatタイプのパラメータ用変数
	float              Float;
	// vectorタイプのパラメータ用変数
	VECTOR             Vector;
	// stringタイプのパラメータ用変数
	char               String[ TEXTPARAM_STRING_MAXLENGTH ];
	// colorタイプのパラメータ用変数
	COLOR_U8           Color;
} STextParam;


// パラメータ一つの情報
typedef struct _STextParamInfoUnit
{
	// パラメータの名前
	char               Name[ TEXTPARAM_NAME_MAXLENGTH ];
	// パラメータタイプ
	ETextParamType     Type;
	// パラメータ本体
	STextParam         Param;
} STextParamInfoUnit;


// １ファイルのテキストパラメータ情報
typedef struct _STextParamInfo
{
	// パラメータの数
	int                ParamNum;
	// パラメータ配列
	STextParamInfoUnit Param[ TEXTPARAM_MAXNUM ];
} STextParamInfo;

// パラメータファイルを読み込む
extern bool        LoadTextParam(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// ファイルパス
	const char *FilePath,
	// ファイルパス生成用可変個引数
	...
);

// 指定の名前のパラメータの配列を取得する
extern int         SearchTextParam(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のintのパラメータを取得する
extern int         GetTextParamInt(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のboolのパラメータを取得する
extern bool        GetTextParamBool(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のfloatのパラメータを取得する
extern float       GetTextParamFloat(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のvectorのパラメータを取得する
extern VECTOR      GetTextParamVector(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のstringのパラメータを取得する
extern const char *GetTextParamString(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,

	// パラメータ名生成用可変個引数
	...
);

// 指定の名前のcolorのパラメータを取得する
extern COLOR_U8    GetTextParamColor(
	// テキストパラメータ情報構造体のアドレス
	STextParamInfo *TxtParam,
	// パラメータ名
	const char *ParamName,
	// パラメータ名生成用可変個引数
	...
);
#endif