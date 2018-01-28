#include "ToolLib_List.h"
#include "ToolLib_Window.h"
#include "ToolLib_Static.h"
#include "ToolLib_Visible.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>

// 作成できるリスト情報の最大数
#define TOOL_LIST_MAX				(256)

// リスト１行辺りの縦幅
#define ONE_LINE_HEIGHT				(16)

// 行のポインタバッファを拡張する際に一度に追加するバッファサイズ
#define ADD_OBJPOINTER_NUM			(100)

// リスト表示用の文字列バッファを拡張する際に一度に追加するバッファサイズ
#define ADD_STRINGBUFFER_SIZE			(16 * 1024)

// リスト構造体
typedef struct _SToolList
{
	// 関連付けられている表示状態情報ハンドル
	int    TVisibleHandle;

	// 関連付けられているウインドウ情報ハンドル
	int    TWindowHandle;

	// 追加されている行の文字列のアドレスが格納されているポインタ配列
	char **ObjList;

	// 追加可能な最大行数
	int    MaxObjNum;

	// 追加されている行の数
	int    ObjNum;

	// 追加されている文字列の中で最大の横幅
	int    Width;

	// 追加されている行の文字列が格納されているバッファ
	char  *StringBuffer;

	// StringBuffer で確保しているメモリサイズ
	int    StringBufferSize;

	// StringBuffer 中で既に使用しているメモリサイズ
	int    StringBufferUseSize;
	
	// 選択されている行
	int    SelectIndex;

	// リストの状態に変化があったかどうか
	bool   Change;

	// 表示状態
	bool   Visible;
} SToolList;

static int        g_TListNum;
static SToolList *g_TList[ TOOL_LIST_MAX ];

// リスト情報の状態推移処理を行う
static void ToolList_Step(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報を画面に描画する
static void ToolList_Draw(
	// リスト情報ハンドル
	int TListHandle
);

// リスト情報を作成する
//     戻り値 : リスト情報ハンドル( 失敗した場合は -1 が返る )
int ToolList_Create( void )
{
	int        NewHandle;
	SToolList *TList;

	// 既にリスト情報が最大数に達している場合はエラー終了
	if( g_TListNum == TOOL_LIST_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TList[ NewHandle ]; NewHandle++ ){}

	// 新しいリスト情報を格納するためのメモリの確保
	TList = ( SToolList * )calloc( 1, sizeof( SToolList ) );
	if( TList == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TList[ NewHandle ] = TList;

	// リスト情報の数を増やす
	g_TListNum++; 

	// 情報を初期化
	TList->TVisibleHandle = -1;
	TList->TWindowHandle  = -1;
	TList->Visible        = true;

	// ハンドルを返す
	return NewHandle;
}

// リスト情報を削除する
void ToolList_Delete(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// 行情報用に確保していたメモリを解放
	if( TList->ObjList )
	{
		free( TList->ObjList );
		TList->ObjList = NULL;
	}

	// 行に表示する文字列を格納していたメモリを解放
	if( TList->StringBuffer )
	{
		free( TList->StringBuffer );
		TList->StringBuffer = NULL;
	}

	// 確保していたメモリを解放する
	free( TList );
	g_TList[ TListHandle ] = NULL;

	// リスト情報の数を減らす
	g_TListNum--;
}

// 全てのリスト情報を削除する
void ToolList_Delete_All( void )
{
	int i;

	// 有効な全てのリストを削除する
	for( i = 0; i < TOOL_LIST_MAX; i++ )
	{
		if( g_TList[ i ] != NULL )
		{
			ToolList_Delete( i );
		}
	}
}

// リスト情報に関連付ける表示状態情報をセットする
void ToolList_SetVisibleHandle(
	// リスト情報ハンドル
	int TListHandle,

	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	TList->TVisibleHandle = TVisibleHandle;
}

// リスト情報を初期化する
void ToolList_Initialize(
	// リスト情報ハンドル
	int TListHandle,
	
	// 関連付けるウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// 行情報用に確保していたメモリを解放
	if( TList->ObjList )
	{
		free( TList->ObjList );
		TList->ObjList = NULL;
	}

	// 行に表示する文字列を格納していたメモリを解放
	if( TList->StringBuffer )
	{
		free( TList->StringBuffer );
		TList->StringBuffer = NULL;
	}

	// 情報を初期化する
	TList->TWindowHandle       = TWindowHandle;
	TList->SelectIndex         = -1;
	TList->ObjNum              = 0;
	TList->StringBufferUseSize = 0;
	TList->MaxObjNum           = 0;
	TList->StringBufferSize    = 0;
	TList->Change              = false;
	TList->Width               = 0;

	// ウインドウのクライアント領域を初期化
	ToolWindow_SetClientSize( TList->TWindowHandle, 16, 16 );
}

// リスト情報の選択されている行の変更などが発生したかどうかを取得する
//     戻り値 : 変更が発生したかどうか( true:変更が発生した  false:変更は発生していない )
bool ToolList_GetChange(
	// リスト情報ハンドル
	int TListHandle,

	// 変更が発生したかどうかの情報をリセットするかどうか
	bool Reset
)
{
	SToolList *TList = g_TList[ TListHandle ];
	bool       Result;

	Result = TList->Change;
	if( Reset )
	{
		TList->Change = false;
	}

	return Result;
}

// リスト情報に行情報を追加する
//     戻り値 : 正常に処理が終了したかどうか( true:正常終了  false:エラー発生 )
bool ToolList_AddObj(
	// リスト情報ハンドル
	int TListHandle,

	// 追加する行に表示する文字列
	const char *String,
	
	// 可変個数引数
	...
)
{
	SToolList *TList = g_TList[ TListHandle ];
	va_list    VaList;
	char       FixString[ 1024 ];
	int        StrLength;
	int        DrawWidth;
	int        i;
	void      *NewBuf;
	
	// 可変個引数から追加する文字列を構築する
	va_start( VaList , String );
	vsprintf( FixString , String , VaList );
	va_end( VaList );

	// 既に追加できる最大行数に達していたら最大行数を増やす
	if( TList->ObjNum == TList->MaxObjNum )
	{
		// 行の情報を格納するメモリ領域を新たに確保
		NewBuf = malloc( sizeof( char * ) * ( TList->MaxObjNum + ADD_OBJPOINTER_NUM ) );
		if( NewBuf == NULL )
		{
			return false;
		}

		// 今までの行の情報がある場合はコピー
		if( TList->ObjList != NULL )
		{
			if( TList->ObjNum != 0 )
			{
				memcpy( NewBuf, TList->ObjList, sizeof( char * ) * TList->ObjNum );
			}

			// 今まで行の情報を格納していたメモリ領域を解放
			free( TList->ObjList );
		}

		// 新しい行の情報を格納するためのメモリ領域を保存
		TList->ObjList = ( char ** )NewBuf;

		// 追加できる行の最大数を増やす
		TList->MaxObjNum += ADD_OBJPOINTER_NUM;
	}

	// 追加する文字列の長さを取得
	StrLength = strlen( FixString );

	// 追加する文字列が文字列を保存しておくバッファのサイズを超えてしまう場合は
	// 文字列を保存しておくバッファのサイズを大きくする
	if( TList->StringBufferUseSize + StrLength + 1 >= TList->StringBufferSize )
	{
		// 文字列を保存するためのバッファを新たに確保する
		NewBuf = ( char * )malloc( sizeof( char ) *
			( TList->StringBufferSize + StrLength + 1 + ADD_STRINGBUFFER_SIZE ) );
		if( NewBuf == NULL )
		{
			return false;
		}

		// 今までの文字列がある場合は内容をコピー
		if( TList->StringBuffer != NULL )
		{
			if( TList->StringBufferUseSize != 0 )
			{
				memcpy( NewBuf, TList->StringBuffer, TList->StringBufferUseSize );
			}

			// 今までのバッファを指していた行のアドレス情報を
			// 新しいバッファ用に変更する
			for( i = 0; i < TList->ObjNum; i ++ )
			{
				TList->ObjList[ i ] =
					( char * )NewBuf + ( TList->ObjList[ i ] - TList->StringBuffer );
			}

			// 今まで文字列を保存していたバッファを解放
			free( TList->StringBuffer );
		}
		
		// 新しい文字列バッファのアドレスを保存
		TList->StringBuffer = ( char * )NewBuf;

		// 文字列バッファのサイズを増やす
		TList->StringBufferSize += StrLength + 1 + ADD_STRINGBUFFER_SIZE;
	}

	// 追加する行の文字列の横幅を取得する
	DrawWidth = GetDrawStringWidthToHandle(
					FixString, StrLength, ToolStatic_GetNormalFontHandle() );

	// 今までの最大横幅より大きい場合は最大横幅を更新
	if( DrawWidth > TList->Width )
	{
		TList->Width = DrawWidth;
	}

	// 新しい行の情報を保存
	TList->ObjList[ TList->ObjNum ] = TList->StringBuffer + TList->StringBufferUseSize;
	memcpy( TList->ObjList[ TList->ObjNum ], FixString, StrLength + 1 );

	// 使用している文字列バッファのサイズを増やす
	TList->StringBufferUseSize += StrLength + 1;

	// 行の数を増やす
	TList->ObjNum ++;

	// ウインドウのクライアント領域を更新
	ToolWindow_SetClientSize(
		TList->TWindowHandle, TList->Width, ToolList_GetHeight( TListHandle ) );

	// 一つ目の行だった場合は、追加した行を選択状態にする
	if( TList->ObjNum == 1 )
	{
		ToolList_SetSelectIndex( TListHandle, 0 );
	}

	// 正常終了
	return true;
}

// リスト情報の表示状態を変更する
void ToolList_SetVisible(
	// リスト情報ハンドル
	int TListHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// 新しい表示状態を保存する
	TList->Visible = Visible;
}

// リスト情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolList_GetVisible(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	// 表示状態が false の場合は false を返す
	if( !TList->Visible )
	{
		return false;
	}

	// 関連付けられているウインドウ情報の表示状態が false の場合は false を返す
	if( TList->TWindowHandle >= 0 &&
		!ToolWindow_GetVisible( TList->TWindowHandle ) )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TList->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TList->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// リスト情報の選択行を変更する
void ToolList_SetSelectIndex(
	// リスト情報ハンドル
	int TListHandle,
	
	// 行番号
	int Index
)
{
	SToolList *TList = g_TList[ TListHandle ];

	TList->SelectIndex = Index;
}

// リスト情報の選択している行を取得する
//     戻り値 : 選択している行の番号( どの行も選択されていない場合は -1 )
int ToolList_GetSelectIndex(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->SelectIndex;
}

// リスト情報に追加した行の数を取得する
int ToolList_GetObjNum(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->ObjNum;
}

// リスト情報に追加した全ての行をあわせた縦幅を取得する
//     戻り値 : リストの全行の縦幅( ピクセル単位 )
int ToolList_GetHeight(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return ONE_LINE_HEIGHT * TList->ObjNum;
}

// リスト情報に追加した全ての行をあわせた横幅を取得する
//     戻り値 : リストの全行の横幅( ピクセル単位 )
int ToolList_GetWidth(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];

	return TList->Width;
}

// リスト情報の状態推移処理を行う
static void ToolList_Step(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];
	int        MouseOffsetX;
	int        MouseOffsetY;
	int        Index;
	int        DrawX;
	int        DrawY;
	RECT       InfoArea;
	int        MouseX;
	int        MouseY;

	// 表示状態が false の場合は何もせず終了
	if( !ToolList_GetVisible( TListHandle ) )
	{
		return;
	}

	// マウスの左ボタンが押された瞬間ではない場合は特に何もしない
	if( ( ToolStatic_GetMouseEdgeButton() & MOUSE_INPUT_LEFT ) == 0 )
	{
		return;
	}

	// ボタンが押された瞬間のマウスカーソルの位置を取得
	ToolStatic_GetMouseEdgePosition( &MouseX, &MouseY );

	// 関連付けされているウインドウ情報の情報表示領域を取得
	InfoArea = ToolWindow_GetInfoArea( TList->TWindowHandle );

	// 関連付けされているウインドウ情報のスクロールバーの状態も考慮した
	// クライアント領域の描画原点となるスクリーン座標を取得する
	ToolWindow_GetDrawLeftUpPos( TList->TWindowHandle, &DrawX, &DrawY );

	// リスト情報上でのマウスの位置を算出
	MouseOffsetX = MouseX - DrawX;
	MouseOffsetY = MouseY - DrawY;

	// マウスのボタンを押した瞬間がリストの領域から外れていたら何もせず終了
	if( MouseX < InfoArea.left || MouseX >= InfoArea.right  ||
		MouseY < InfoArea.top  || MouseY >= InfoArea.bottom ||
		MouseOffsetY                   <  0 ||
		MouseOffsetY / ONE_LINE_HEIGHT >= TList->ObjNum )
	{
		return;
	}

	// マウスカーソルの位置に該当する行を算出
	Index = MouseOffsetY / ONE_LINE_HEIGHT;

	// マウスカーソルの位置にある行の文字列部分にカーソルがなかったら何もせずに終了
	if( MouseOffsetX < 0 ||
		GetDrawStringWidthToHandle( TList->ObjList[ Index ], -1,
								ToolStatic_GetNormalFontHandle() ) <= MouseOffsetX )
	{
		return;
	}

	// 選択している行の変更
	ToolList_SetSelectIndex( TListHandle, Index );

	// 状態に変化があったかどうかのフラグを立てる
	TList->Change = true;
}

// リスト情報を画面に描画する
static void ToolList_Draw(
	// リスト情報ハンドル
	int TListHandle
)
{
	SToolList *TList = g_TList[ TListHandle ];
	int        WinDrawX;
	int        WinDrawY;
	int        i;
	int        ClientY;
	RECT       InfoArea;
	int        NormalColor;
	int        SelectColor;
	int        NormalFontHandle;

	// 表示状態が false の場合は何もせず終了
	if( !ToolList_GetVisible( TListHandle ) )
	{
		return;
	}

	// ウインドウ情報のスクロールバーの状態も考慮した
	// クライアント領域の描画原点となるスクリーン座標を取得する
	ToolWindow_GetDrawLeftUpPos( TList->TWindowHandle, &WinDrawX, &WinDrawY );

	// ウインドウ情報のスクロールバーの状態も考慮した
	// クライアント領域の描画原点となる座標を取得する
	ToolWindow_GetClientArea( TList->TWindowHandle, NULL, &ClientY );

	// ウインドウ情報を元に SetDrawArea で描画領域を制限する設定を行う
	ToolWindow_SetupDrawArea( TList->TWindowHandle );

	// ウインドウ情報の情報表示領域を取得する
	InfoArea = ToolWindow_GetInfoArea( TList->TWindowHandle );

	// 通常の文字列の描画色と選択されている文字列の描画色を取得する
	NormalColor = GetColor( 255,255,255 );
	SelectColor = GetColor( 255,  0,  0 );

	// 文字列描画用のフォントハンドルを取得
	NormalFontHandle = ToolStatic_GetNormalFontHandle();

	// 行の描画ループ
	for( i = ClientY / ONE_LINE_HEIGHT; i < TList->ObjNum; i ++ )
	{
		// 描画位置が情報表示領域から外れていたらループを抜ける
		if( i * ONE_LINE_HEIGHT + WinDrawY > InfoArea.bottom )
		{
			break;
		}

		// 行の文字列を描画
		DrawStringToHandle(
			WinDrawX,
			WinDrawY + i * ONE_LINE_HEIGHT,
			TList->ObjList[ i ],
			TList->SelectIndex == i ? SelectColor : NormalColor,
			NormalFontHandle
		);
	}

	// 描画可能範囲を元に戻す
	SetDrawArea( 0, 0, 999999, 999999 );
}

// 全てのリスト情報の状態推移処理を行う
void ToolList_Step_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TListNum; i++ )
	{
		if( g_TList[ i ] == NULL )
		{
			continue;
		}

		ToolList_Step( i );
		j++;
	}
}

// 全てのリスト情報の描画移処理を行う
void ToolList_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TListNum; i++ )
	{
		if( g_TList[ i ] == NULL )
		{
			continue;
		}

		ToolList_Draw( i );
		j++;
	}
}

