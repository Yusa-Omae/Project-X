#include "ToolLib_StringInput.h"
#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include "DxLib.h"
#include <string.h>
#include <stdlib.h>

// 文字列入力情報の最大数
#define TOOL_STRINGINPUT_MAX		(256)

// 文字列入力領域の縦幅
#define STRINGINPUT_HEIGHT			(18)

// 数字一文字の仮の横幅
#define ONE_NUMBER_SIZE				(7)

// 文字列バッファのサイズ
#define STRING_BUFFER_SIZE			(2049)

// 文字列入力構造体
typedef struct _SToolStringInput
{
	// 関連付けられている表示状態情報ハンドル
	int    TVisibleHandle;

	// 関連付けられているウインドウ情報ハンドル
	int    TWindowHandle;

	// 文字列入力の表示位置
	POINT  Position;

	// 名前の文字列が格納されているメモリアドレスを保存しているポインタ
	char  *Name;

	// 数値のみ入力可能かどうか
	bool   NumberOnly;

	// 入力できる数値の桁数( NumberOnly が true の場合のみ有効 )
	int    NumberNum;

	// 文字列入力領域の横幅( NumberOnly が false の場合のみ有効 )
	int    StringDrawWidth;

	// 文字列入力が完了しているかどうか
	bool   EndFlag;

	// 表示状態
	bool   Visible;

	// 入力文字列を整数値に変換した値
	int    IntNumber;

	// 入力文字列を浮動小数点値に変換した値
	float  FloatNumber;

	// 文字列入力ハンドル
	int    InputHandle;

	// 入力文字列を取得する際に使用されるバッファ
	char   InputString[ STRING_BUFFER_SIZE ];
} SToolStringInput;

static int               g_TStringInputNum;
static SToolStringInput *g_TStringInput[ TOOL_STRINGINPUT_MAX ];

// 文字列入力情報の状態推移処理を行う
//     戻り値 : 処理が正常終了したかどうか( true:正常終了 false:エラー発生 )
static bool ToolStringInput_Step(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報を画面に描画する
static void ToolStringInput_Draw(
	// 文字列入力情報ハンドル
	int TStringInputHandle
);

// 文字列入力情報を作成する
//     戻り値 : 文字列入力情報ハンドル( 失敗した場合は -1 が返る )
int ToolStringInput_Create(
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
)
{
	int               NewHandle;
	SToolStringInput *TStringInput;

	// 既にボタン情報が最大数に達している場合はエラー終了
	if( g_TStringInputNum == TOOL_STRINGINPUT_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TStringInput[ NewHandle ]; NewHandle++ ){}

	// 新しいボタン情報を格納するためのメモリの確保
	TStringInput = ( SToolStringInput * )calloc( 1, sizeof( SToolStringInput ) );
	if( TStringInput == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TStringInput[ NewHandle ] = TStringInput;

	// 文字列入力情報の数を増やす
	g_TStringInputNum++; 

	// 情報を初期化
	TStringInput->TVisibleHandle  = -1;
	TStringInput->Visible         = true;
	TStringInput->IntNumber       = 0;
	TStringInput->FloatNumber     = 0;
	TStringInput->EndFlag         = false;
	TStringInput->NumberNum       = NumberNum;
	TStringInput->NumberOnly      = NumberOnly;
	TStringInput->StringDrawWidth = StringDrawWidth;
	TStringInput->InputHandle     = -1;
	TStringInput->Position.x      = x;
	TStringInput->Position.y      = y;
	TStringInput->TWindowHandle   = -1;

	if( Name )
	{
		TStringInput->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TStringInput->Name == NULL )
		{
			return -1;
		}
		strcpy( TStringInput->Name, Name );
	}

	// ハンドルを返す
	return NewHandle;
}

// 文字列入力情報を削除する
void ToolStringInput_Delete(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 名前保存用にメモリを確保していた場合は解放
	if( TStringInput->Name )
	{
		free( TStringInput->Name );
		TStringInput->Name = NULL;
	}

	// 文字列入力ハンドルを作成していたら削除する
	if( TStringInput->InputHandle != -1 )
	{
		DeleteKeyInput( TStringInput->InputHandle );
	}

	// 確保していたメモリを解放する
	free( TStringInput );
	g_TStringInput[ TStringInputHandle ] = NULL;

	// 文字列入力情報の数を減らす
	g_TStringInputNum--;
}

// 全ての文字列入力情報を削除する
void ToolStringInput_Delete_All( void )
{
	int i;

	// 有効な全ての文字列入力を削除する
	for( i = 0; i < TOOL_STRINGINPUT_MAX; i++ )
	{
		if( g_TStringInput[ i ] != NULL )
		{
			ToolStringInput_Delete( i );
		}
	}
}

// 文字列入力情報に関連付ける表示状態情報をセットする
void ToolStringInput_SetVisibleHandle(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->TVisibleHandle = TVisibleHandle;
}

// 文字列入力情報の表示状態を変更する
void ToolStringInput_SetVisible(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 新しい表示状態を保存する
	TStringInput->Visible = Visible;
}

// 文字列入力情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolStringInput_GetVisible(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 表示状態が false の場合は false を返す
	if( !TStringInput->Visible )
	{
		return false;
	}

	// 関連付けられているウインドウ情報の表示状態が false の場合は false を返す
	if( TStringInput->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TStringInput->TWindowHandle ) )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TStringInput->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TStringInput->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// 文字列入力情報と関連付けるウインドウ情報を設定する
void ToolStringInput_SetWindow(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->TWindowHandle = TWindowHandle;
}

// 文字列入力情報の文字入力領域の座標を変更する
void ToolStringInput_SetPosition(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 変更後の文字入力領域の座標
	int x,
	int y
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	TStringInput->Position.x = x;
	TStringInput->Position.y = y;
}

// 文字列入力情報の入力中の文字列を変更する
void ToolStringInput_SetString(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// セットする文字列が格納されたメモリアドレス
	const char *String
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 文字列を保存する
	strcpy( TStringInput->InputString, String );

	// 文字列入力ハンドルが有効な場合は文字列入力ハンドルに反映する
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputString( String, TStringInput->InputHandle );
	}
}

// 文字列入力情報の入力中の文字列を取得する
void ToolStringInput_GetString(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中の文字列を格納するメモリ領域の先頭アドレス
	char *String
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 文字列入力ハンドルが有効な場合は入力中の文字列を取得する
	if( TStringInput->InputHandle != -1 )
	{
		GetKeyInputString( String, TStringInput->InputHandle );
	}
	else
	{
		// 入力中ではない場合は入力が確定した文字列をコピーする
		strcpy( String, TStringInput->InputString );
	}
}

// 文字列入力情報の文字列入力を無条件で終了する
void ToolStringInput_AlwaysEnd(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 文字列入力ハンドルが有効な場合は入力中の文字列を取得する
	if( TStringInput->InputHandle != -1 )
	{
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
}

// 文字列入力情報の入力中の文字列として整数値に変更する
void ToolStringInput_SetIntNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中文字列として設定する整数値
	int Number
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 値を保存
	TStringInput->IntNumber   = Number;
	TStringInput->FloatNumber = ( float )Number;

	// 文字列入力ハンドルが有効な場合は文字列入力ハンドルにセットする
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputNumber( Number, TStringInput->InputHandle );
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
	else
	{
		// 文字列入力ハンドルが無効な場合は単純に数値を文字列に変換する
		sprintf( TStringInput->InputString, "%d", Number );
	}
}

// 文字列入力情報の入力中の文字列として浮動小数点値に変更する
void ToolStringInput_SetFloatNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle,
	
	// 入力中文字列として設定する浮動小数点値
	float Number
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// 値を保存
	TStringInput->FloatNumber = Number;
	TStringInput->IntNumber   = ( int )Number;

	// 文字列入力ハンドルが有効な場合は文字列入力ハンドルにセットする
	if( TStringInput->InputHandle != -1 )
	{
		SetKeyInputNumberToFloat( Number, TStringInput->InputHandle );
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}
	else
	{
		// 文字列入力ハンドルが無効な場合は単純に数値を文字列に変換する
		sprintf( TStringInput->InputString, "%f", Number );
	}
}

// 文字列入力情報の入力中の文字列を整数値に変換したものを取得する
//     戻り値 : 入力中の文字列を整数値に変換した値
int ToolStringInput_GetIntNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	return TStringInput->IntNumber;
}

// 文字列入力情報の入力中の文字列を浮動小数点値に変換したものを取得する
//     戻り値 : 入力中の文字列を浮動小数点値に変換した値
float ToolStringInput_GetFloatNumber(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	return TStringInput->FloatNumber;
}

// 文字列入力情報の文字列の入力が終了しているかどうかを取得する
//     戻り値 : 入力が終了しているかどうか( true:終了している  false:終了していない )
bool ToolStringInput_CheckEnd(
	// 文字列入力情報ハンドル
	int TStringInputHandle,

	// 文字列の入力が終了しているかどうかのフラグをリセットするかどうか
	bool Reset
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	bool              Result;

	// 戻り値を保存
	Result = TStringInput->EndFlag;

	// リセット指定がある場合はフラグを倒す
	if( Reset )
	{
		TStringInput->EndFlag = false;
	}

	// 戻り値を返す
	return Result;
}

// 文字列入力情報の文字列入力がアクティブになっていた場合にアクティブ状態を解除する
void ToolStringInput_ResetActive(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];

	// アクティブな文字列入力ハンドルとなっていたら
	// アクティブな文字列入力ハンドルが無い状態にする
	if( GetActiveKeyInput() == TStringInput->InputHandle )
	{
		SetActiveKeyInput( -1 );
	}
}

// 文字列入力情報の状態推移処理を行う
//     戻り値 : 処理が正常終了したかどうか( true:正常終了 false:エラー発生 )
static bool ToolStringInput_Step(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	bool              In;
	int               SizeX;
	int               SizeY;
	int               SelectStart;
	int               SelectEnd;
	int               AreaX;
	int               AreaY;
	int               NameDrawW;
	int               ButtonFontHandle;
	int               MouseX;
	int               MouseY;
	int               MouseSelectStartX;
	int               MouseSelectEndX;
	int               MouseInput;
	int               MouseEdgeInput;

	// 表示状態が false の場合は文字列入力ハンドルを削除して終了
	if( !ToolStringInput_GetVisible( TStringInputHandle ) )
	{
		// 文字列入力ハンドルが有効だった場合は入力中の文字列を保存した上で削除
		if( TStringInput->InputHandle != -1 )
		{
			GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
			DeleteKeyInput( TStringInput->InputHandle );
			TStringInput->InputHandle = -1;
		}
		return true;
	}

	// 入力中文字列描画用のフォントハンドルを取得
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// マウスカーソルの位置を取得
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// マウスの入力状態を取得
	MouseInput     = ToolStatic_GetMouseButton();
	MouseEdgeInput = ToolStatic_GetMouseEdgeButton();

	// マウスの選択範囲を取得する
	ToolStatic_GetMouseSelectStartPosition(
		&MouseSelectStartX, NULL,
		&MouseSelectEndX,   NULL
	);

	// 文字列入力ハンドルが作成されていない場合は作成する
	if( TStringInput->InputHandle == -1 )
	{
		// 数値入力かどうかで作成処理を変更
		if( TStringInput->NumberOnly )
		{
			// 数値しか入力できない文字列入力ハンドルを作成
			TStringInput->InputHandle =
					MakeKeyInput( TStringInput->NumberNum, FALSE, FALSE, TRUE );

			// 数値をセット
			SetKeyInputNumberToFloat(
						TStringInput->FloatNumber, TStringInput->InputHandle );
		}
		else
		{
			// 数値以外も入力できるの文字列入力ハンドルを作成
			TStringInput->InputHandle =
					MakeKeyInput( STRING_BUFFER_SIZE - 1, FALSE, FALSE, FALSE );

			// 確定している入力文字列をセット
			SetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
		}

		// 文字列入力ハンドルの作成に失敗したらエラー
		if( TStringInput->InputHandle < 0 )
		{
			return false;
		}

		// 現在の文字列入力ハンドルで設定されている文字列を取得
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );
	}

	// 文字列入力領域の幅と高さをセット
	SizeX = TStringInput->NumberOnly ?
				TStringInput->NumberNum * ONE_NUMBER_SIZE + 4 :
				TStringInput->StringDrawWidth;
	SizeY = STRINGINPUT_HEIGHT;

	// 名前の描画幅を取得する
	NameDrawW = 0;
	if( TStringInput->Name != NULL )
	{
		NameDrawW = GetDrawStringWidthToHandle( 
					TStringInput->Name, -1, ToolStatic_GetNormalFontHandle() ) + 2;
	}

	// 文字列入力領域の座標と文字列入力領域上にマウスカーソルがあるかどうかを取得する
	if( TStringInput->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TStringInput->TWindowHandle, &AreaX, &AreaY );
		AreaX += TStringInput->Position.x + NameDrawW;
		AreaY += TStringInput->Position.y;
//		In = AreaX <= MouseX && AreaX + SizeX >= MouseX &&
//		     AreaY <= MouseY && AreaY + SizeY >= MouseY &&
		In = ToolStatic_CheckMouseIn( AreaX, AreaY, AreaX + SizeX, AreaY + SizeY ) &&
			 ToolWindow_CheckInfoAreaIn( TStringInput->TWindowHandle, MouseX, MouseY );
	}
	else
	{
		AreaX = TStringInput->Position.x + NameDrawW;
		AreaY = TStringInput->Position.y;
//		In = TStringInput->Position.x <= MouseX && TStringInput->Position.x + SizeX >= MouseX &&
//		     TStringInput->Position.y <= MouseY && TStringInput->Position.y + SizeY >= MouseY;
		In = ToolStatic_CheckMouseIn( AreaX, AreaY, AreaX + SizeX, AreaY + SizeY );
	}

	// 入力中の文字列の選択範囲を取得する
	GetKeyInputSelectArea( &SelectStart, &SelectEnd, TStringInput->InputHandle );

	// マウスの左クリックをした瞬間の場合は処理を分岐
	if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
	{
		// 文字列入力領域の中でクリックしたかどうかで処理を分岐
		if( In )
		{
			// 文字列入力ハンドルをアクティブにする
			SetActiveKeyInput( TStringInput->InputHandle );

			// ダブルクリックだった場合は入力中文字列全体を選択状態にする
			if( ToolStatic_GetMouseDoubleClick() )
			{
				SetKeyInputSelectArea( 0, 9999, TStringInput->InputHandle );
			}
			else
			{
				// シングルクリックの場合は入力中文字列の選択範囲を解除する
				SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );

				// クリックした座標に合わせて入力中文字列のカーソルの位置を変更
				SetKeyInputCursorPosition(
					Tool_GetKeyInputCursorPos(
						AreaX,
						MouseX,
						TStringInput->InputHandle,
						ButtonFontHandle
					),
					TStringInput->InputHandle
				);
			}
		}
		else
		{
			// 文字列入力領域の外でクリックした場合は文字列の選択範囲を解除する
			SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );
		}
	}
	else
	// マウスの左ボタンが押されていて且つ文字列入力領域にマウスカーソルがある場合は
	// 入力中文字列の選択範囲を変更する
	if( ( MouseInput & MOUSE_INPUT_LEFT ) != 0 && In )
	{
		int Start;
		int End;

		Start = Tool_GetKeyInputCursorPos(
					AreaX,
					MouseSelectStartX,
					TStringInput->InputHandle,
					ButtonFontHandle
				);

		End   = Tool_GetKeyInputCursorPos(
					AreaX,
					MouseSelectEndX,
					TStringInput->InputHandle,
					ButtonFontHandle
				);

		if( Start != End )
		{
			SetKeyInputSelectArea( Start, End, TStringInput->InputHandle );
		}

		// 入力中文字列のカーソル位置もマウスカーソルの位置にあわせて変更
		SetKeyInputCursorPosition(
			Tool_GetKeyInputCursorPos(
				AreaX,
				MouseX,
				TStringInput->InputHandle,
				ButtonFontHandle
			),
			TStringInput->InputHandle
		);
	}

	// 文字列入力領域内で右クリックされた場合
	if( ( MouseEdgeInput & MOUSE_INPUT_RIGHT ) != 0 && In )
	{
		if( SelectStart != -1 )
		{
			int  StringLength;
			int  smin;
			int  smax;
			char TempStr[ STRING_BUFFER_SIZE ];

			// 入力中文字列の選択範囲がある場合はクリップボードに文字列を登録する

			GetKeyInputString( TempStr, TStringInput->InputHandle );

			if( SelectStart < SelectEnd )
			{
				smin = SelectStart;
				smax = SelectEnd;
			}
			else
			{
				smin = SelectEnd;
				smax = SelectStart;
			}
			StringLength = smax - smin;
			memmove( TempStr, TempStr + smin, StringLength );
			TempStr[ StringLength ] = 0;
			SetClipboardText( TempStr );
			SetKeyInputSelectArea( -1, -1, TStringInput->InputHandle );
		}
		else
		{
			char *String;
			char *p;
			int  Size;

			// 入力中文字列の選択範囲が無い場合はクリップボードの文字列を入力文字列に追加

			// クリップボード上の文字列のサイズを取得する
			Size = GetClipboardText( NULL );

			// 文字列を格納するためのメモリの確保
			String = ( char * )malloc( Size );
			if( String != NULL )
			{
				// メモリの確保に成功したらクリップボード上の文字列を取得
				GetClipboardText( String );

				// 取得した文字を入力文字列に追加
				for( p = String; *p; p ++ )
				{
					StockInputChar( *p );
				}

				// 確保したメモリの解放
				free( String );
			}
		}
	}

	// 文字列入力が完了していたら完了処理を行う
	if( CheckKeyInput( TStringInput->InputHandle ) )
	{
		// 文字列入力が完了しているかどうかのフラグを立てる
		TStringInput->EndFlag = true;

		// 完了時の文字列を取得する
		GetKeyInputString( TStringInput->InputString, TStringInput->InputHandle );

		// 完了時の文字列を数値とした場合の値を取得する
		TStringInput->IntNumber   = GetKeyInputNumber( TStringInput->InputHandle );
		TStringInput->FloatNumber = GetKeyInputNumberToFloat( TStringInput->InputHandle);

		// 文字列入力ハンドルは再度入力状態にする
		ReStartKeyInput( TStringInput->InputHandle );
	}

	// 正常終了
	return true;
}

// 文字列入力情報を画面に描画する
static void ToolStringInput_Draw(
	// 文字列入力情報ハンドル
	int TStringInputHandle
)
{
	SToolStringInput *TStringInput = g_TStringInput[ TStringInputHandle ];
	int               SizeX;
	int               SizeY;
	char              String[ STRING_BUFFER_SIZE ];
	int               AreaX;
	int               AreaY;
	RECT              DrawRect;
	int               NameDrawW;
	int               NormalFontHandle;
	int               ButtonFontHandle;

	// 非表示状態の場合は何もせず終了
	if( !ToolStringInput_GetVisible( TStringInputHandle ) )
	{
		return;
	}

	// 名前描画用のフォントハンドルを取得
	NormalFontHandle = ToolStatic_GetNormalFontHandle();

	// 入力中文字列描画用のフォントハンドルを取得
	ButtonFontHandle = ToolStatic_GetButtonFontHandle();

	// 文字列入力領域の座標を取得する
	if( TStringInput->TWindowHandle >= 0 )
	{
		ToolWindow_GetDrawLeftUpPos( TStringInput->TWindowHandle, &AreaX, &AreaY );
		AreaX += TStringInput->Position.x;
		AreaY += TStringInput->Position.y;

		// ウインドウ情報ハンドルが有効な場合は描画領域制限設定を行う
		ToolWindow_SetupDrawArea( TStringInput->TWindowHandle );
	}
	else
	{
		AreaX = TStringInput->Position.x;
		AreaY = TStringInput->Position.y;
	}

	// 名前がある場合は名前を描画する
	if( TStringInput->Name )
	{
		// 名前の描画幅を取得する
		NameDrawW =
			GetDrawStringWidthToHandle( TStringInput->Name, -1, NormalFontHandle ) + 2;

		// 名前を描画する
		DrawStringToHandle(
			AreaX,
			AreaY,
			TStringInput->Name,
			GetColor( 255,255,255 ),
			NormalFontHandle
		);

		// 文字列入力領域を名前の描画幅分だけ右にずらす
		AreaX += NameDrawW;
	}

	// 文字列入力領域の幅と高さをセット
	SizeX = TStringInput->NumberOnly ?
				TStringInput->NumberNum * ONE_NUMBER_SIZE + 4 :
				TStringInput->StringDrawWidth;
	SizeY = STRINGINPUT_HEIGHT;
	
	// 文字列入力領域の枠を描画
	Tool_DrawButton(
		AreaX - 3,
		AreaY - 3,
		SizeX + 4,
		18,
		GetColor(   0,  0,  0 ),
		GetColor(  64, 64, 64 ),
		GetColor( 255,255,255 ),
		GetColor( 191,191,191 ),
		GetColor( 220,220,220 )
	);

	// 関連付けられているウインドウ情報がある場合は
	// ウインドウ情報を考慮した描画領域制限設定を行う
	if( TStringInput->TWindowHandle >= 0 )
	{
		DrawRect.left   = AreaX;
		DrawRect.top    = AreaY;
		DrawRect.right  = AreaX + SizeX;
		DrawRect.bottom = AreaY + 17;
		RectClipping( &DrawRect, &ToolWindow_GetInfoArea( TStringInput->TWindowHandle ));
		SetDrawArea( DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom );
	}
	else
	{
		// ウインドウ情報が関連付けられていない場合は
		// 文字列入力領域をそのまま描画制限領域として設定する
		SetDrawArea( AreaX, AreaY, AreaX + SizeX, AreaY + 17 );
	}

	// 入力中文字列の文字列描画に使用するフォントを設定する
	SetKeyInputStringFont( ButtonFontHandle );

	// 現在の入力中の文字列を取得する
	GetKeyInputString( String, TStringInput->InputHandle );

	// 入力中文字列が確定した文字列と異なる場合は描画色を変更する
	if( strcmp( String, TStringInput->InputString ) != 0 )
	{
		SetKeyInputStringColor( GetColor( 0,0,255 ), -1, -1, -1, -1, -1, -1 );
	}
	else
	{
		SetKeyInputStringColor( GetColor( 0,0,  0 ), -1, -1, -1, -1, -1, -1 );
	}

	// 入力中文字列の描画
	DrawKeyInputString( AreaX, AreaY, TStringInput->InputHandle );

	// 設定を元に戻す
	SetKeyInputStringFont( -1 );
	SetKeyInputStringColor( -1, -1, -1, -1, -1, -1, -1 );
	SetDrawArea( 0, 0, 999999, 999999 );
}

// 全ての文字列入力情報の状態推移処理を行う
//    戻り値 : 処理が正常に終了したかどうか( true:正常終了  false:エラー発生 )
bool ToolStringInput_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TStringInputNum; i++ )
	{
		if( g_TStringInput[ i ] == NULL )
		{
			continue;
		}

		if( !ToolStringInput_Step( i ) )
		{
			return false;
		};
		j++;
	}

	return true;
}

// 全ての文字列入力情報の描画移処理を行う
void ToolStringInput_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TStringInputNum; i++ )
	{
		if( g_TStringInput[ i ] == NULL )
		{
			continue;
		}

		ToolStringInput_Draw( i );
		j++;
	}
}



