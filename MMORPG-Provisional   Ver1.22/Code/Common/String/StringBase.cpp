/*
				ファイル名		:StringBase.cpp
				作成者			:
				作成日時		:2018/01/15
				ソース説明		:
					
					文字列基底クラス
				
				備考
				
				
				
!*/

#include "DxLib.h"
#include "StringBase.h"
#include "FontMgr/FontMgr.h"

StringBase::StringBase(){
	mFontMgr = NULL;
	mColor = GetColor(255, 255, 255);
	memset(mString, 0, sizeof(char) * 1024);
}

StringBase::~StringBase() {
	if (mFontMgr != NULL) {
		delete mFontMgr;
		mFontMgr = NULL;
	}

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
bool StringBase::FontCreate(const char* fontName, int size, int thick, int fontType, unsigned int edgeColor) {
	if (mFontMgr == NULL) {
		mFontMgr = new FontMgr();
	}
	return mFontMgr->CreateFontHandle(fontName, size,thick,fontType, edgeColor);
}

/*
	フォントファイルを読み込みフォントデータの作成(DxLibのTool(CreateDXFontData.exe)を使用しないと使えない)

	const char* fileName	:ＤＸフォントデータファイルのパス
	int edgeSize			:エッジのサイズ(0を指定するとなし)
	unsigned int edgeColor	:エッジの色
	return	true	:成功
			false	:失敗

*/
bool StringBase::FontCreate(const char* fileName, int edgeSize, unsigned int edgeColor) {
	if (mFontMgr == NULL) {
		mFontMgr = new FontMgr();
	}
	return mFontMgr->CreateFontHandle(fileName, edgeSize, edgeColor);
}


void StringBase::SetString(const char* string) {
	int size = strlenDx(string) + 1;
	SetString(string, size);

}

void StringBase::SetString(const char* string, size_t size) {
	memset(mString, 0, sizeof(char) * 1024);
	strcpy_sDx(mString,size,string);
}

void StringBase::SetColor(unsigned int color) {
	mColor = color;
}

/*
	文字列の描画を行う
	return 文字数を返却する
*/
int StringBase::DrawString(int posX, int posY, bool isDisp/* = true*/) {

	int length = 0;

	length = GetLength(mString);

	if (isDisp == false)	return length;

	if (mFontMgr == NULL) {
		DxLib::DrawString(posX, posY, mString, mColor);
	}
	else {
		mFontMgr->Draw(posX, posY, mString, mColor);
	}

	return length;

}

int StringBase::DrawString(int posX, int posY, int alpha, bool isDisp/* = true*/) {

	int length = 0;
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	length = DrawString(posX, posY, isDisp);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	return length;
}


int StringBase::GetCharBytes(const void* string) {

	if (string == NULL) return 0;

	const char* str = (const char*)string;

	int bytes = 0;
#ifdef __WINDOWS__
	bytes = DxLib::GetCharBytes(DX_CHARCODEFORMAT_SHIFTJIS, str);
#else

	int chrCodeFormat = DxLib::Get_wchar_t_CharCodeFormat();

	if (chrCodeFormat == -1) {
		chrCodeFormat = DX_CHARCODEFORMAT_UTF8;
	}

	bytes = DxLib::GetCharBytes(chrCodeFormat, str);

#endif

	return bytes;

}

int StringBase::GetLength(const char* string) {
	int length = 0;
	int lengthMax = strlenDx(string);
	const char* pchr = string;
	char ch = *pchr;
	while (length < lengthMax && ch != '\0') {
		int add = GetCharBytes(pchr);
		pchr += add;		
		length++;
		ch = *pchr;
	}
	return length;
}