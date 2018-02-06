#pragma once 

#include "ScriptFlag.h"


#define SCRIPT_MAX_STRING_LENGTH 256
#define SCRIPT_MAX_STRING_LINE 1000
#define DROW_MAX_STRING_LINE 8
#define STRING_NEXT_LINE_NUM 32
#define SHIFT_JIS_AREA 0x81

#define YESNO_POSITION_X 500
#define YESNO_POSITION_Y 300

#define YESNO_BOX_WIDHT 140
#define YESNO_BOX_HEIGHT 80

#define MESSAGE_WINDOW_WIDTH 24
#define MESSAGE_WINDOW_HEIHGT 30

#define SCRIPT_FLAG_NUM 10

class StringBase;



class Script{
private:
	
	ScriptFlag* mFlag;
	StringBase* mStringBase;

	char mScript[SCRIPT_MAX_STRING_LINE][SCRIPT_MAX_STRING_LENGTH];
	char mMessage[DROW_MAX_STRING_LINE][SCRIPT_MAX_STRING_LENGTH];
	char* mFileName;
	int mLineNum;
	int mMaxLine;
	int mDrawLine;
	int mCouter;

	bool isWaitKey;
	bool mIsBattle;	//バトル開始用フラグ
	bool mIsGameClear; //ゲームクリアフラグ

	int mGraphicHandle;
	int mGraphicsDrawPosX;
	int mGraphicsDrawPosY;


	void LoadScript(const char*);	//スクリプトファイルの読み込み

	void YesNoFlag(const char* Yes = "はい",const char* No = "いいえ");
	void SetOnFlag(const char* Name);
	void SetOffFlag(const char* Name);
	bool GetFlag(const char* Name);
	bool SetDrawGraph(int id,int drawX,int drawY);
	void SetMessage(char*);
	void AddScriptLine();

	void GameClearOn();
	void GameClearOff();

public :

	Script(const char* FileName);
	~Script();


	
	void ScriptSplit(const char*,char*[],const char*,int);
	void AnalysisString();

	void SetScript(const char* FileName);


	int SerchLabel(char*);
	
	void BattleOn();
	void BattleOff();
	bool GetBattleFlag();

	void WaitKeyOn();
	void WaitKeyOff();
	bool GetWaitKeyFlag();
	bool GetScriptEnd();

	bool GetGameClearFlag();


	void draw(int X,int Y);
	void updata();

};