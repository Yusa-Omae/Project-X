/*
				ファイル名		:StringBase.h
				作成者			:
				作成日時		:2018/01/15
				ソース説明		:
					
					文字列基底クラス
				
				備考
				
				
				
!*/

#ifndef __STRING_BASE_H__
#define __STRING_BASE_H__

class FontMgr;

#include <list>

class StringBase {

public :

	static const int STRING_LINE_MAX = 6;

private:

	FontMgr * mFontMgr;
	unsigned int mColor;
	char mString[1024];
	
	int mStringSize;

	int mLine;
	int mPos;
	int mTotalPos;
	int mNextLineLength;	//次の行にする文字数
	int mStrNum;			//文字数

	char mDrawString[STRING_LINE_MAX][1024];
	bool mIsOnletter;	//一文字ずつ描画するか？
	int mInputPos;

	int mCounter;

private:

	/*
	文字列のバイト数を返却する
	return	0以上	:バイト数
	-1		:エラー
	*/
	int GetCharBytes(const void* string);
	int GetLength(const char* string);

public :

	StringBase();
	virtual ~StringBase();

	/*
		更新処理
		bool isOnletter = false	:一文字ずつ表示させるか？
									true:	一文字ずつ表示する	false:すべて描画
		int length = 30			:一行に表示させる文字数(デフォルトは30文字)
		int interval  = 20		:文字を描画する間隔
		return			-1		:エラー
						0		:描画中
						1		:描画終了
						2		:表示インターバル中
	*/
	int Update(bool isOnletter = false,int length = 30, int interval = 20);

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
	bool FontCreate(const char* fontName, int size, int thick, int fontType, unsigned int edgeColor = GetColor(255, 255, 255));

	/*
		フォントファイルを読み込みフォントデータの作成(DxLibのTool(CreateDXFontData.exe)を使用しないと使えない)

		const char* fileName	:ＤＸフォントデータファイルのパス
		int edgeSize			:エッジのサイズ(0を指定するとなし)
		unsigned int edgeColor	:エッジの色
		return	true	:成功
				false	:失敗

	*/
	bool FontCreate(const char* fileName, int edgeSize, unsigned int edgeColor = GetColor(255, 255, 255));

	void SetString(const char* string);
	void SetString(const char* string,size_t size);
	
	void SetColor(unsigned int color);

	/*
		文字列の描画を行う
		return 文字数を返却する
	*/
	virtual int DrawString(int posX, int posY,bool isDisp = true);
	virtual int DrawString(int posX, int posY, int alpha, bool isDisp = true);


	

};

#endif // __STRING_BASE_H__