#include "ToolLib_Window.h"
#include "ToolLib_Visible.h"
#include "ToolLib_SpaceManage.h"
#include "ToolLib_Static.h"
#include "ToolLib.h"
#include <string.h>

// 作成できるウインドウ情報の最大数
#define TOOL_WINDOW_MAX				(256)

// サイズ変更可能な表示領域を使用する場合の表示領域右端から空けるスペースのサイズ
#define DRAWAREA_BASERIGHT_SPACE	(20)

// スクロールバー操作用のボタンのサイズ
#define SCROLLBAR_BUTTON_SIZE		(8)

// スクロールバーの幅
#define SCROLLBAR_WIDTH				(8)

// スクロールバーをボタンを押して操作する場合の移動速度
#define SCROLLBAR_SPEED				(180)

// ウインドウ構造体
typedef struct _SToolWindow
{
	// スペース管理情報のハンドル
	int               TSpaceManageHandle;

	// 表示状態情報のハンドル
	int               TVisibleHandle;

	// ボタンの名前の文字列が格納されているメモリアドレスを保存しているポインタ
	char             *Name;

	// ウインドウの情報表示領域
	RECT              InfoArea;

	// ウインドウのクライアント領域のサイズ
	SIZE              ClientSize;

	// ウインドウのクライアント領域の内容を描画する際の基点となる左上座標
	// ( スクロールバーによるウインドウ内情報の表示位置が変更される処理に使用 )
	int               DrawLeftUpX;
	int               DrawLeftUpY;

	// 表示位置を決定するスクロール座標
	int               HScroll;
	int               VScroll;

	// スクロールバーを掴んでいるかどうか
	bool              HScrollGrips;
	bool              VScrollGrips;

	// 表示状態
	bool              Visible;

	// 最後にボタン押下スクロールをした時間
	int               LastButtonBottomTime;

	// それぞれ上下左右スクロールボタンが押された状態かどうか
	// ( true:押されている  false:押されていない)
	bool              UpButBottom;
	bool              DownButBottom;
	bool              LeftButBottom;
	bool              RightButBottom;

	// それぞれ上下左右スクロールボタンの矩形内にマウスカーソルがあるかどうか
	bool              UpButIn;
	bool              DownButIn;
	bool              LeftButIn;
	bool              RightButIn;

	// 名前表示部分のスペース情報
	SToolSpace        NameSpace;

	// 情報表示部分のスペース情報
	SToolSpace        InfoSpace;

	// 水平スクロールバーのスペース情報
	SToolSpace        HScrollBarSpace;
} SToolWindow;

static int          g_TWindowNum;
static SToolWindow *g_TWindow[ TOOL_WINDOW_MAX ];

// スペース情報を使用している場合にスペース情報に基づいた情報表示領域の更新を行う
static void ToolWindow_UpdateInfoArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
);

// ウインドウ情報の状態推移処理を行う
static void ToolWindow_Step(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 進める時間( 単位 : 秒 )
	float StepTime
);

// ウインドウ情報を画面に描画する
static void ToolWindow_Draw(
	// ウインドウ情報ハンドル
	int TWindowHandle
);


// ウインドウ情報を作成する
//     戻り値 : ウインドウ情報ハンドル( 失敗した場合は -1 が返る )
int  ToolWindow_Create(
	// スペース管理情報ハンドル( 必要のない場合は -1 を渡す )
	int TSpaceManageHandle,
	
	// 残りのスペースを全て使用するかどうか( true を渡すと全て使用する )
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool SpaceLast,

	// 固定タイプのスペースかどうか
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool LockSpace,
	
	// 水平スクロールバー用のスペースも確保するかどうか
	// ( TSpaceManageHandle が -1 以外の場合のみ有効 )
	bool HScrollSpace,

	// ウインドウの名前
	const char *Name,

	// ウインドウの左端座標
	int DrawX,
	
	// ウインドウの左端Y座標
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawY,

	// ウインドウの幅
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawW,
	
	// ウインドウの高さ
	// ( TSpaceManageHandle が -1 の場合のみ有効 )
	int DrawH,
	
	// ウインドウのクライアント領域の幅
	int ClientW,
	
	// ウインドウのクライアント領域の高さ
	int ClientH
)
{
	int          NewHandle;
	SToolWindow *TWindow;

	// 既にウインドウ情報が最大数に達している場合はエラー終了
	if( g_TWindowNum == TOOL_WINDOW_MAX )
	{
		return -1;
	}

	// 使用されていない配列要素を探す
	for( NewHandle = 0; g_TWindow[ NewHandle ]; NewHandle++ ){}

	// 新しいウインドウ情報を格納するためのメモリの確保
	TWindow = ( SToolWindow * )calloc( 1, sizeof( SToolWindow ) );
	if( TWindow == NULL )
	{
		return -1;
	}

	// 確保したメモリ領域を保存
	g_TWindow[ NewHandle ] = TWindow;

	// ウインドウ情報の数を増やす
	g_TWindowNum++; 

	// スペース管理情報ハンドルを保存
	TWindow->TSpaceManageHandle = TSpaceManageHandle;

	// スペース管理情報ハンドルが有効な場合はウインドウのスペース情報を追加する
	if( TSpaceManageHandle >= 0 )
	{
		// 名前表示部分のスペースを追加
		if( !ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->NameSpace,
				TOOL_WINDOW_NAME_HEIGHT,
				true ) )
		{
			return -1;
		}

		// 情報表示領域のスペースを追加
		if( !ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->InfoSpace,
				SpaceLast ?
					ToolSpaceManage_GetTotalSpace( TSpaceManageHandle ) - 
					ToolSpaceManage_GetNextSpace(  TSpaceManageHandle ) -
					TOOL_SCROLLBAR_WIDTH : DrawH,
				LockSpace ) )
		{
			return -1;
		}

		// 水平スクロールバー用のスペースを確保する指定がある場合は
		// 水平スクロールバー用のスペースを追加する
		if( HScrollSpace )
		{
			ToolSpaceManage_AddSpace(
				TSpaceManageHandle,
				&TWindow->HScrollBarSpace,
				TOOL_SCROLLBAR_WIDTH,
				true
			);
		}
	}

	// 情報を初期化
	TWindow->TVisibleHandle  = -1;

	TWindow->InfoArea.left    = DrawX;
	TWindow->InfoArea.right   = DrawX + DrawW;
	if( TWindow->TSpaceManageHandle >= 0 )
	{
		TWindow->InfoArea.top = TWindow->InfoSpace.SpaceStart;
	}
	else
	{
		TWindow->InfoArea.top = DrawY;
	}
	TWindow->InfoArea.bottom  = TWindow->InfoArea.top + DrawH;
	TWindow->ClientSize.cx    = ClientW;
	TWindow->ClientSize.cy    = ClientH;

	if( !ToolWindow_SetName( NewHandle, Name ) )
	{
		return -1;
	}

	TWindow->DrawLeftUpX      = 0;
	TWindow->DrawLeftUpY      = 0;
	TWindow->HScroll          = 0;
	TWindow->VScroll          = 0;
	TWindow->Visible          = true;

	// ハンドルを返す
	return NewHandle;
}

// ウインドウ情報を削除する
void ToolWindow_Delete(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// 名前保存用にメモリを確保していた場合は解放
	if( TWindow->Name )
	{
		free( TWindow->Name );
		TWindow->Name = NULL;
	}

	// 確保していたメモリを解放する
	free( TWindow );
	g_TWindow[ TWindowHandle ] = NULL;

	// ウインドウ情報の数を減らす
	g_TWindowNum--;
}

// 全てのウインドウ情報を削除する
void ToolWindow_Delete_All( void )
{
	int i;

	// 有効な全てのウインドウを削除する
	for( i = 0; i < TOOL_WINDOW_MAX; i++ )
	{
		if( g_TWindow[ i ] != NULL )
		{
			ToolWindow_Delete( i );
		}
	}
}

// ウインドウ情報に関連付ける表示状態情報をセットする
void ToolWindow_SetVisibleHandle( 
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 表示状態情報ハンドル
	int TVisibleHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	TWindow->TVisibleHandle = TVisibleHandle;
}

// スペース情報を使用している場合にスペース情報に基づいた情報表示領域の更新を行う
static void ToolWindow_UpdateInfoArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// スペース管理情報ハンドルが無効な場合は何もしない
	if( TWindow->TSpaceManageHandle < 0 )
	{
		return;
	}

	// 情報表示領域をスペース情報に基づいて算出
	TWindow->InfoArea.right  = Tool_GetBaseRightX() - DRAWAREA_BASERIGHT_SPACE;
	TWindow->InfoArea.top    = TWindow->InfoSpace.SpaceStart;
	TWindow->InfoArea.bottom = TWindow->InfoArea.top + TWindow->InfoSpace.UseSpace;
}

// ウインドウ情報を元に SetDrawArea で描画領域を制限する設定を行う
void ToolWindow_SetupDrawArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// SetDrawArea を使用して実際に描画領域の制限設定を行う
	SetDrawArea(
		TWindow->InfoArea.left,
		TWindow->InfoArea.top,
		TWindow->InfoArea.right,
		TWindow->InfoArea.bottom
	);
}

// ウインドウ情報の情報表示領域を取得する
//     戻り値 : 描画領域の矩形
RECT ToolWindow_GetInfoArea(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	return TWindow->InfoArea;
}

// ウインドウ情報のスクロールバーの状態も考慮した
// クライアント領域の描画原点となるスクリーン座標を取得する
void ToolWindow_GetDrawLeftUpPos(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 座標を代入する変数のアドレス
	int *x,
	int *y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	if( x )
	{
		*x = -TWindow->DrawLeftUpX + TWindow->InfoArea.left;
	}

	if( y )
	{
		*y = -TWindow->DrawLeftUpY + TWindow->InfoArea.top;
	}
}

// ウインドウ情報のスクロールバーの状態も考慮した
// クライアント領域の描画原点となる座標を取得する
void ToolWindow_GetClientArea(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// 座標を代入する変数のアドレス
	int *x,
	int *y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	if( x )
	{
		*x = TWindow->DrawLeftUpX;
	}

	if( y )
	{
		*y = TWindow->DrawLeftUpY;
	}
}

// ウインドウ情報のクライアント領域のサイズを設定する
void ToolWindow_SetClientSize(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 新しいクライアント領域のサイズ
	int w,
	int h
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          ScrollW;
	int          ScrollH;

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// 現在のクライアント領域のサイズと同じ場合は何もせずに終了
	if( TWindow->ClientSize.cx == w && TWindow->ClientSize.cy == h ) 
	{
		return;
	}

	// 新しいクライアント領域のサイズを保存
	TWindow->ClientSize.cx = w;
	TWindow->ClientSize.cy = h;

	// 情報表示領域の幅と高さを算出
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// スクロールバーの水平移動範囲と垂直移動範囲を算出
	ScrollW = InfoAreaW - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );
	ScrollH = InfoAreaH - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );

	// 新しいクライアント領域のサイズが
	// 情報表示領域より狭い場合はスクロールバーの位置を調節
	if( TWindow->ClientSize.cx > InfoAreaW )
	{
		if( TWindow->DrawLeftUpX + InfoAreaW > TWindow->ClientSize.cx )
		{
			TWindow->DrawLeftUpX = TWindow->ClientSize.cx - InfoAreaW;
		}

		TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
								( TWindow->ClientSize.cx - InfoAreaW );
	}
	else
	{
		TWindow->DrawLeftUpX = 0;
		TWindow->HScroll     = 0;
	}

	if( TWindow->ClientSize.cy > InfoAreaH )
	{
		if( TWindow->DrawLeftUpY + InfoAreaH > TWindow->ClientSize.cy )
		{
			TWindow->DrawLeftUpY = TWindow->ClientSize.cy - InfoAreaH;
		}
		TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
								( TWindow->ClientSize.cy - InfoAreaH );
	}
	else
	{
		TWindow->DrawLeftUpY = 0;
		TWindow->VScroll     = 0;
	}
}

// ウインドウ情報の名前を設定する
//     戻り値 : 処理が正常に終了したかどうか( true:正常に終了した  false:エラー発生 )
bool ToolWindow_SetName(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 名前となる文字列のアドレス
	const char *Name
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// 既に名前用にメモリを確保していた場合は解放する
	if( TWindow->Name )
	{
		free( TWindow->Name );
		TWindow->Name = NULL;
	}

	// 名前のアドレスが有効な場合はメモリを確保して保存する
	if( Name != NULL )
	{
		TWindow->Name = ( char * )malloc( strlen( Name ) + 1 );
		if( TWindow->Name == NULL )
		{
			return false;
		}
		strcpy( TWindow->Name, Name );
	}

	// 正常終了
	return true;
}

// ウインドウ情報の表示状態を変更する
void ToolWindow_SetVisible(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 新しい表示状態
	bool Visible
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// 新しい表示状態を保存する
	TWindow->Visible = Visible;
}

// ウインドウ情報の表示状態を取得する
//     戻り値 : 表示状態( true:表示  false:非表示 )
bool ToolWindow_GetVisible(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	// 表示状態が false の場合は false を返す
	if( !TWindow->Visible )
	{
		return false;
	}

	// 表示状態情報ハンドルが登録されていて、
	// 且つ表示状態情報ハンドルの表示状態が false の場合は false を返す
	if( TWindow->TVisibleHandle >= 0 &&
		!ToolVisible_GetVisible( TWindow->TVisibleHandle ) )
	{
		return false;
	}

	// ここにきていたら非表示ではないということなので true を返す
	return true;
}

// 指定の座標がウインドウの情報表示領域に入っているかどうかを調べる
//     戻り値 : 指定の座標がウインドウの描画領域に入っているかどうか
//              ( true:入っている   false:入っていない )
bool ToolWindow_CheckInfoAreaIn(
	// ウインドウ情報ハンドル
	int TWindowHandle,

	// ウインドウの描画領域に入っているかどうか調べる座標
	int x,
	int y
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];

	return TWindow->InfoArea.left <= x && TWindow->InfoArea.right  >= x &&
	       TWindow->InfoArea.top  <= y && TWindow->InfoArea.bottom >= y;
}

// ウインドウ情報の状態推移処理を行う
static void ToolWindow_Step(
	// ウインドウ情報ハンドル
	int TWindowHandle,
	
	// 進める時間( 単位 : 秒 )
	float StepTime
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          ScrollH;
	int          ScrollW;
	int          DeltaTime;
	int          ScrollSize;
	bool         SizeOverX;
	bool         SizeOverY;
	int          MouseInput;
	int          MouseEdgeInput;
	int          MouseX;
	int          MouseY;
	int          MouseMoveX;
	int          MouseMoveY;

	// 表示状態が false の場合は何もせず終了
	if( !ToolWindow_GetVisible( TWindowHandle ) )
	{
		return;
	}

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// マウスカーソルの位置を取得
	ToolStatic_GetMousePosition( &MouseX, &MouseY );

	// マウスカーソルの移動量を取得
	ToolStatic_GetMouseMoveVecter( &MouseMoveX, &MouseMoveY );

	// マウスの入力状態を取得
	MouseInput     = ToolStatic_GetMouseButton();
	MouseEdgeInput = ToolStatic_GetMouseEdgeButton();

	// 情報表示領域の幅と高さを算出
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// クライアント領域のサイズが情報表示領域のサイズより大きいかどうかを調べておく
	SizeOverX = InfoAreaW < TWindow->ClientSize.cx;
	SizeOverY = InfoAreaH < TWindow->ClientSize.cy;

	// スクロールバーの水平移動範囲と垂直移動範囲を算出
	ScrollW = InfoAreaW - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );
	ScrollH = InfoAreaH - ( SCROLLBAR_WIDTH + SCROLLBAR_BUTTON_SIZE * 2 );

	// スクロールバーの上下左右ボタンの表示領域の中にカーソルがあるかどうかを調べる
	TWindow->UpButIn =
		SizeOverY &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE
		);

	TWindow->DownButIn = 
		SizeOverY &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom
		);

	TWindow->LeftButIn = 
		SizeOverX &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.left,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH
		);

	TWindow->RightButIn = 
		SizeOverX &&
		ToolStatic_CheckMouseIn(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH
		);

	// 前回状態推移処理が行われてからの経過時間を算出する
	DeltaTime                     = GetNowCount() - TWindow->LastButtonBottomTime;

	// 現在の時間を保存しておく
	TWindow->LastButtonBottomTime = GetNowCount();

	// 経過時間からスクロールバーが移動する場合の移動量を算出する
	ScrollSize                    = DeltaTime * SCROLLBAR_SPEED / 1000;
	if( ScrollSize == 0 )
	{
		ScrollSize = SCROLLBAR_SPEED / 60;
	}

	// マウスの左ボタンが押されていない場合はスクロール関係のフラグを全て初期化して終了
	if( ( MouseInput & MOUSE_INPUT_LEFT ) == 0 )
	{
		TWindow->HScrollGrips   = false;
		TWindow->VScrollGrips   = false;
		TWindow->RightButBottom = false;
		TWindow->LeftButBottom  = false;
		TWindow->UpButBottom    = false;
		TWindow->DownButBottom  = false;
		return;
	}

	// クライアント領域の横幅が情報表示領域の横幅より大きい場合は
	// 横方向のスクロールバーの処理を行う
	if( SizeOverX )
	{
		// 左ボタンが押された瞬間で、且つスクロールボタンの表示領域内にマウスカーソルが
		// あった場合はスクロールボタンを押しているかどうかのフラグを立てる
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
		{
			if( TWindow->LeftButIn  )
			{
				TWindow->LeftButBottom  = true;
			}

			if( TWindow->RightButIn )
			{
				TWindow->RightButBottom = true;
			}
		}

		// スクロールボタンが押されていた場合はスクロール位置を移動する
		if( TWindow->LeftButIn || TWindow->RightButIn )
		{
			if( TWindow->LeftButBottom )
			{
				TWindow->DrawLeftUpX -= ScrollSize;
				if( TWindow->DrawLeftUpX < 0 )
				{
					TWindow->DrawLeftUpX = 0;
				}

				TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
										( TWindow->ClientSize.cx - InfoAreaW );
			}
			else
			if( TWindow->RightButBottom )
			{
				TWindow->DrawLeftUpX += ScrollSize;
				if( TWindow->DrawLeftUpX > TWindow->ClientSize.cx - InfoAreaW )
				{
					TWindow->DrawLeftUpX = TWindow->ClientSize.cx - InfoAreaW;
				}

				TWindow->HScroll = TWindow->DrawLeftUpX * ScrollW /
										( TWindow->ClientSize.cx - InfoAreaW );
			}
		}
		else
		// スクロールバーのつまみを掴んでいる場合はマウスカーソルの移動量に
		// あわせてスクロール位置を移動する
		if( TWindow->HScrollGrips )
		{
			TWindow->HScroll += MouseMoveX;
			if( TWindow->HScroll < 0 )
			{
				TWindow->HScroll = 0;
			}

			if( TWindow->HScroll > ScrollW )
			{
				TWindow->HScroll = ScrollW;
			}

			TWindow->DrawLeftUpX = 
				TWindow->HScroll * ( TWindow->ClientSize.cx - InfoAreaW ) / ScrollW;
		}
		else
		// スクロールバーの領域内にカーソルがあり、且つ左ボタンが押された瞬間の場合は
		// スクロールバー関係の処理をする
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			ToolStatic_CheckMouseIn(
				TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.bottom,
				TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH ) )
		{
			// カーソルの外をクリックした場合は１画面分移動する
			if( MouseX < TWindow->InfoArea.left + SCROLLBAR_BUTTON_SIZE +
											TWindow->HScroll )
			{
				TWindow->HScroll -= InfoAreaW * ScrollW / TWindow->ClientSize.cx;
				if( TWindow->HScroll < 0 )
				{
					TWindow->HScroll = 0;
				}
			}
			else
			if( MouseX > TWindow->InfoArea.left + SCROLLBAR_BUTTON_SIZE +
											TWindow->HScroll + SCROLLBAR_WIDTH )
			{
				TWindow->HScroll += InfoAreaW * ScrollW / TWindow->ClientSize.cx;
				if( TWindow->HScroll > ScrollW )
				{
					TWindow->HScroll = ScrollW;
				}
			}
			else
			{
				// つまみをクリックした場合はつまみ掴み状態にする
				TWindow->HScrollGrips = true;
			}

			// スクロール位置からスクロール位置を考慮した描画基点位置を算出
			TWindow->DrawLeftUpX = TWindow->HScroll *
									( TWindow->ClientSize.cx - InfoAreaW ) / ScrollW;
		}
	}

	// クライアント領域の縦幅が情報表示領域の縦幅より大きい場合は
	// 縦方向のスクロールバーの処理を行う
	if( SizeOverY )
	{
		// 左ボタンが押された瞬間で、且つスクロールボタンの表示領域内にマウスカーソルが
		// あった場合はスクロールボタンを押しているかどうかのフラグを立てる
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 )
		{
			if( TWindow->UpButIn )
			{
				TWindow->UpButBottom   = true;
			}

			if( TWindow->DownButIn )
			{
				TWindow->DownButBottom = true;
			}
		}

		// スクロールボタンが押されていた場合はスクロール位置を移動する
		if( TWindow->UpButIn || TWindow->DownButIn )
		{
			if( TWindow->UpButBottom )
			{
				TWindow->DrawLeftUpY -= ScrollSize;
				if( TWindow->DrawLeftUpY < 0 )
				{
					TWindow->DrawLeftUpY = 0;
				}

				TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
										( TWindow->ClientSize.cy - InfoAreaH );
			}
			else
			if( TWindow->DownButBottom )
			{
				TWindow->DrawLeftUpY += ScrollSize;
				if( TWindow->DrawLeftUpY > TWindow->ClientSize.cy - InfoAreaH )
				{
					TWindow->DrawLeftUpY = TWindow->ClientSize.cy - InfoAreaH;
				}

				TWindow->VScroll = TWindow->DrawLeftUpY * ScrollH /
										( TWindow->ClientSize.cy - InfoAreaH );
			}
		}
		else
		// スクロールバーのつまみを掴んでいる場合はマウスカーソルの移動量に
		// あわせてスクロール位置を移動する
		if( TWindow->VScrollGrips )
		{
			TWindow->VScroll += MouseMoveY;
			if( TWindow->VScroll < 0 )
			{
				TWindow->VScroll = 0;
			}

			if( TWindow->VScroll > ScrollH )
			{
				TWindow->VScroll = ScrollH;
			}

			TWindow->DrawLeftUpY =
				TWindow->VScroll * ( TWindow->ClientSize.cy - InfoAreaH ) / ScrollH;
		}
		else
		// スクロールバーの領域内にカーソルがあり、且つ左ボタンが押された瞬間の場合は
		// スクロールバー関係の処理をする
		if( ( MouseEdgeInput & MOUSE_INPUT_LEFT ) != 0 &&
			ToolStatic_CheckMouseIn(
				TWindow->InfoArea.right,
				TWindow->InfoArea.top    + SCROLLBAR_BUTTON_SIZE,
				TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
				TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE ) )
		{
			// つまみの外をクリックした場合は１画面分移動する
			if( MouseY < TWindow->InfoArea.top + SCROLLBAR_BUTTON_SIZE +
											TWindow->VScroll )
			{
				TWindow->VScroll -= InfoAreaH * ScrollH / TWindow->ClientSize.cy;
				if( TWindow->VScroll < 0 )
				{
					TWindow->VScroll = 0;
				}
			}
			else
			if( MouseY > TWindow->InfoArea.top + SCROLLBAR_BUTTON_SIZE +
											TWindow->VScroll + SCROLLBAR_WIDTH )
			{
				TWindow->VScroll += InfoAreaH * ScrollH / TWindow->ClientSize.cy;
				if( TWindow->VScroll > ScrollH )
				{
					TWindow->VScroll = ScrollH;
				}
			}
			else
			{
				// つまみをクリックした場合はつまみ掴み状態にする
				TWindow->VScrollGrips = true;
			}

			// スクロール位置からスクロール位置を考慮した描画基点位置を算出
			TWindow->DrawLeftUpY = TWindow->VScroll *
									( TWindow->ClientSize.cy - InfoAreaH ) / ScrollH;
		}
	}
}

// ウインドウ情報を画面に描画する
static void ToolWindow_Draw(
	// ウインドウ情報ハンドル
	int TWindowHandle
)
{
	SToolWindow *TWindow = g_TWindow[ TWindowHandle ];
	int          InfoAreaW;
	int          InfoAreaH;
	int          DrawWidth;
	bool         SizeOverX;
	bool         SizeOverY;
	bool         DownButInBottom;
	bool         UpButInBottom;
	bool         LeftButInBottom;
	bool         RightButInBottom;

	// 表示状態が false の場合は何もせず終了
	if( !ToolWindow_GetVisible( TWindowHandle ) )
	{
		return;
	}

	// スペース情報が変更されている可能性があるので情報表示領域の更新
	ToolWindow_UpdateInfoArea( TWindowHandle );

	// スクロールバーのボタンの上にカーソルがあり、
	// 且つボタンが押されている状態かどうかを調べておく
	DownButInBottom  = TWindow->DownButBottom  && TWindow->DownButIn;
	UpButInBottom    = TWindow->UpButBottom    && TWindow->UpButIn;
	LeftButInBottom  = TWindow->LeftButBottom  && TWindow->LeftButIn;
	RightButInBottom = TWindow->RightButBottom && TWindow->RightButIn;

	// 情報表示領域の幅と高さを算出
	InfoAreaW = TWindow->InfoArea.right  - TWindow->InfoArea.left;
	InfoAreaH = TWindow->InfoArea.bottom - TWindow->InfoArea.top;

	// クライアント領域のサイズが情報表示領域のサイズより大きいかどうかを調べておく
	SizeOverX = InfoAreaW < TWindow->ClientSize.cx;
	SizeOverY = InfoAreaH < TWindow->ClientSize.cy;

	// 名前が設定されている場合は名前を表示する
	if( TWindow->Name )
	{
		// 名前を表示するウインドウ上部を描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.left  - 1,
			TWindow->InfoArea.top   - TOOL_WINDOW_NAME_HEIGHT,
			TWindow->InfoArea.right + 1,
			TWindow->InfoArea.top   - 1,
			GetColor( 255,255,255 ),
			TRUE
		);

		// 名前の文字列を描画
		DrawWidth = GetDrawStringWidthToHandle(
					TWindow->Name, -1, ToolStatic_GetButtonFontHandle() );

		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawStringToHandle(
			TWindow->InfoArea.left + ( InfoAreaW - DrawWidth ) / 2,
			TWindow->InfoArea.top - TOOL_WINDOW_NAME_HEIGHT +
							( TOOL_WINDOW_NAME_HEIGHT - TOOL_BUTTON_FONT_HEIGHT ) / 2,
			TWindow->Name,
			GetColor( 0,0,0 ),
			ToolStatic_GetButtonFontHandle()
		);
	}

	// 情報表示領域の枠を描画
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	DrawBox(
		TWindow->InfoArea.left   - 1,
		TWindow->InfoArea.top    - 1,
		TWindow->InfoArea.right  + 1 + ( SizeOverY ? TOOL_SCROLLBAR_WIDTH : 0 ),
		TWindow->InfoArea.bottom + 1 + ( SizeOverX ? TOOL_SCROLLBAR_WIDTH : 0 ),
		GetColor( 255,255,255 ),
		FALSE
	);

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 64 );

	DrawBox(
		TWindow->InfoArea.left   ,
		TWindow->InfoArea.top    ,
		TWindow->InfoArea.right  + ( SizeOverY ? TOOL_SCROLLBAR_WIDTH : 0 ),
		TWindow->InfoArea.bottom + ( SizeOverX ? TOOL_SCROLLBAR_WIDTH : 0 ),
		GetColor( 0,0,0 ),
		TRUE
	);

	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

	// クライアント領域の縦幅が情報表示領域の縦幅より大きい場合は
	// 縦方向のスクロールバーを描画する
	if( SizeOverY )
	{
		// バーの上下移動操作ボタンを描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE,
			GetColor( 255,255,255 ),
			UpButInBottom ? TRUE : FALSE
		);

		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom,
			GetColor( 255,255,255 ),
			DownButInBottom ? TRUE : FALSE
		);

		DrawTriangle(
			TWindow->InfoArea.right + 2,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE - 2, 
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.top   + 2, 
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH - 2,
			TWindow->InfoArea.top   + SCROLLBAR_BUTTON_SIZE - 2,
			UpButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		DrawTriangle(
			TWindow->InfoArea.right  + 2,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE + 2, 
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.bottom - 2,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH - 2,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE + 2,
			DownButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		// バーの枠を描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top    + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right  + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom - SCROLLBAR_BUTTON_SIZE,
			GetColor( 255,255,255 ),
			TRUE
		);

		// バーの現在のスクロール位置を示すつまみ部分を描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawBox(
			TWindow->InfoArea.right,
			TWindow->InfoArea.top   + TWindow->VScroll + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.right + TOOL_SCROLLBAR_WIDTH,
			TWindow->InfoArea.top   + TWindow->VScroll +
								SCROLLBAR_BUTTON_SIZE + SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);
	}

	// クライアント領域の横幅が情報表示領域の横幅より大きい場合は
	// 横方向のスクロールバーを描画する
	if( SizeOverX )
	{
		// バーの左右移動操作ボタンを描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );

		DrawBox(
			TWindow->InfoArea.left,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			LeftButInBottom ? TRUE : FALSE
		);

		DrawBox(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			RightButInBottom ? TRUE : FALSE
		);

		DrawTriangle(
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE - 2,
			TWindow->InfoArea.bottom + 2,
			TWindow->InfoArea.left   + 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE - 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH - 2,
			LeftButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		DrawTriangle(
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE + 2,
			TWindow->InfoArea.bottom + 2,
			TWindow->InfoArea.right  - 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH / 2,
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE + 2,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH - 2,
			RightButInBottom ? GetColor( 0,0,0 ) : GetColor( 255,255,255 ),
			TRUE
		);

		// バーの枠を描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		DrawBox(
			TWindow->InfoArea.left   + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.right  - SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);

		// バーの現在のスクロール位置を示すつまみ部分を描画
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 200 );
		DrawBox(
			TWindow->InfoArea.left   + TWindow->HScroll + SCROLLBAR_BUTTON_SIZE,
			TWindow->InfoArea.bottom,
			TWindow->InfoArea.left   + TWindow->HScroll +
									SCROLLBAR_BUTTON_SIZE + SCROLLBAR_WIDTH,
			TWindow->InfoArea.bottom + TOOL_SCROLLBAR_WIDTH,
			GetColor( 255,255,255 ),
			TRUE
		);
	}

	// 描画ブレンドモードを元に戻す
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
}

// 全てのウインドウ情報の状態推移処理を行う
void ToolWindow_Step_All(
	// 進める時間( 単位 : 秒 )
	float StepTime
)
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TWindowNum; i++ )
	{
		if( g_TWindow[ i ] == NULL )
		{
			continue;
		}

		ToolWindow_Step( i, StepTime );
		j++;
	}
}

// 全てのウインドウ情報の描画移処理を行う
void ToolWindow_Draw_All( void )
{
	int i;
	int j;

	j = 0;
	for( i = 0; j < g_TWindowNum; i++ )
	{
		if( g_TWindow[ i ] == NULL )
		{
			continue;
		}

		ToolWindow_Draw( i );
		j++;
	}
}

