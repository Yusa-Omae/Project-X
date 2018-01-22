/*
				ファイル名		:FontMgr.cpp
				作成者			:
				作成日時		:2017/08/30
				ソース説明		:
				
				
				備考
				
				
				
!*/

//	------- インクルード宣言

#include "DxLib.h"
#include "FontMgr.h"

//	------- 固定定数宣言

//	-------	マクロ定義

//	------- 列挙体

//	------- 構造体&共用体

//	------- データテーブル

//	------- スタティック変数宣言

//	------- グローバル変数宣言

//	------- スタティックプロトタイプ宣言(内部関数)

//	------- プロトタイプ宣言していないスタティック関数(内部関数)

//	------- グローバル関数

//コンストラクタ
FontMgr::FontMgr(){

	//念のため初期化の前に削除　
	DeleteFont();

	strcpyDx(m_FileName,"");
	m_FontHandle = 0;
	m_Size = 0;
	m_EdgeColor = 0;

}
	
//デストラクタ
FontMgr::~FontMgr(){
	DeleteFont();
}
	
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
bool FontMgr::CreateFontHandle(const char* fontName,int size,int thick,int fontType,unsigned int edgeColor){

	//すでに文字ハンドルを設定済みなので文字の生成を行わない
	if(m_FontHandle != 0){
		
		return false;
	}

	m_FontHandle = CreateFontToHandle(fontName,size,thick,fontType);
	
	//文字ハンドルの生成に失敗した
	if(m_FontHandle == -1){
		m_FontHandle = 0;
		return false;
	}
	
	m_Size = size;
	m_EdgeColor = edgeColor;

	return true;

}
/*
	フォントファイルを読み込みフォントデータの作成(DxLibのTool(CreateDXFontData.exe)を使用しないと使えない)
	
	const char* fileName	:ＤＸフォントデータファイルのパス
	int edgeSize			:エッジのサイズ(0を指定するとなし)
	unsigned int edgeColor	:エッジの色
	return	true	:成功
			false	:失敗

*/
bool FontMgr::CreateFontHandle(const char* fileName,int edgeSize,unsigned int edgeColor){
	
	if(m_FontHandle != 0){

		return false;
	}

	m_FontHandle = LoadFontDataToHandle(fileName,edgeSize);
	
	if(m_FontHandle == -1){
		return false;	
	}
	
	m_EdgeColor = edgeColor;
	strcpyDx(m_FileName,fileName);

	return true;

}


/*
	文字列を描画する
	
	int posX			:X座標
	int posY			:Y座標
	const char* str		:文字列
	unsigned int color	:色
*/
void FontMgr::Draw(int posX,int posY,const char* str,unsigned int color){
	
	DrawStringToHandle(posX,posY,str,color,m_FontHandle,m_EdgeColor);

}


/*
	書式付き文字列を描画する

	int posX			:X座標
	int posY			:Y座標
	unsigned int color	:色
	int alpha			:アルファ
	const char* str		:文字列
	...
	
*/
void FontMgr::Draw(int posX,int posY,unsigned int color,int alpha,const char* str,...){

	char buffer[1024];

	va_list ap;
	va_start(ap,str);
	vsprintf_s(buffer,str,ap);
	va_end(ap);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA,alpha);
	Draw(posX,posY,buffer,color);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
}

/*
	フォントハンドルの削除
*/
void FontMgr::DeleteFont(){

	if(m_FontHandle == 0) return ;

	DeleteFontToHandle(m_FontHandle);

	strcpyDx(m_FileName,"");
	m_FontHandle = 0;
	m_Size = 0;
	m_EdgeColor = 0;


}


