#pragma once 

#define STRING_LENGHT 256

class ScriptFlag{

private:

	char mName[STRING_LENGHT];
	bool mIsFlag;
	bool mIsSet;				//�t���O���Z�b�g���I����Ă����true�ɂ���

public :

	ScriptFlag();

	void SetOnFlag(const char* Name);
	void SetOffFlag(const char* Name);
	char* GetName();
	bool Get();
	bool GetAlreadySet();				//�Z�b�g�ς݂���ԋp����

};