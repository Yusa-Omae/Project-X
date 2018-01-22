/*
				ファイル名		:FontMgr.h
				作成者			:
				作成日時		:2017/08/30
				ソース説明		:
				
				
				備考
				
				
				
!*/

#ifndef __FONT_MGR_H__
#define __FONT_MGR_H__

//	------- インクルード宣言

//	------- 固定定数宣言

//	-------	マクロ定義

//	------- 列挙体

//	------- 構造体&共用体

//	------- グローバル関数


class FontMgr{

private:

	int m_FontHandle;			//フォントデータハンドル
	char m_FileName[256];		//フォントのファイル名
	int m_Size;					//フォントポイント
	unsigned int m_EdgeColor;	//エッジの色

public :

	//コンストラクタ
	FontMgr();
	
	//デストラクタ
	~FontMgr();
	
	/*
		フォント名からフォントデータの作成

		const char* fontName					:フォント名
		int size								:フォントサイズ
		int tick								:文字の太さ
		int fontType							:フォントタイプ
	
　		 -1　　　　　　　　　　　　　　			: デフォルトフォント(DX_FONTTYPE_NORMAL と同じ)
　		 DX_FONTTYPE_NORMAL　　　　　　			: ノーマルフォント
　		 DX_FONTTYPE_EDGE　　　　　　　			: エッジつきフォント
　		 DX_FONTTYPE_ANTIALIASING　　　　		: アンチエイリアスフォント
　		 DX_FONTTYPE_ANTIALIASING_4X4　　	　	: アンチエイリアスフォント( 4x4サンプリング )
　		 DX_FONTTYPE_ANTIALIASING_8X8　　　		: アンチエイリアスフォント( 8x8サンプリング )
　		 DX_FONTTYPE_ANTIALIASING_EDGE_4X4　	: アンチエイリアス＆エッジ付きフォント( 4x4サンプリング )
　		 DX_FONTTYPE_ANTIALIASING_EDGE_8X8　	: アンチエイリアス＆エッジ付きフォント( 8x8サンプリング ) )

		unsigned int edgeColor					:エッジの色
		return	true	:成功
				false	:失敗

	*/
	bool CreateFontHandle(const char* fontName,int size,int tick,int fontType,unsigned int edgeColor = GetColor(255,255,255));	//フォント名からフォントデータの作成
	
	/*
		フォントファイルを読み込みフォントデータの作成(DxLibのTool(CreateDXFontData.exe)を使用しないと使えない)
	
		const char* fileName	:ＤＸフォントデータファイルのパス
		int edgeSize			:エッジのサイズ(0を指定するとなし)
		unsigned int edgeColor	:エッジの色
		return	true	:成功
				false	:失敗

	*/
	bool CreateFontHandle(const char* fileName,int edgeSize,unsigned int edgeColor = GetColor(255,255,255));						//フォントファイルを読み込みフォントデータの作成(DxLibのTool(CreateDXFontData.exe)を使用しないと使えない)


	/*
		文字列を描画する
	
		int posX			:X座標
		int posY			:Y座標
		const char* str		:文字列
		unsigned int color	:色
	*/
	void Draw(int posX,int posY,const char* str,unsigned int color);			//文字列の描画
	
	/*
		書式付き文字列を描画する

		int posX			:X座標
		int posY			:Y座標
		unsigned int color	:色
		int alpha			:アルファ
		const char* str		:文字列
		...
	
	*/
	void Draw(int posX,int posY,unsigned int color,int alpha,const char* str,...);



	/*
		フォントハンドルの削除
	*/
	void DeleteFont();

};





//	------- インライン関数(ヘッダー内関数)

//インライン関数の書き方


#endif // __FONT_MGR_H__