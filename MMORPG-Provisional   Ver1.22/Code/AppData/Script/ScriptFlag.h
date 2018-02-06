#pragma once 

#define STRING_LENGHT 256

class ScriptFlag{

private:

	char mName[STRING_LENGHT];
	bool mIsFlag;
	bool mIsSet;				//フラグをセットし終わっていればtrueにする

public :

	ScriptFlag();

	void SetOnFlag(const char* Name);
	void SetOffFlag(const char* Name);
	char* GetName();
	bool Get();
	bool GetAlreadySet();				//セット済みかを返却する

};