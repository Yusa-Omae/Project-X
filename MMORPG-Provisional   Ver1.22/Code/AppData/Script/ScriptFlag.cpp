#include "DxLib.h"
#include "ScriptFlag.h"



ScriptFlag::ScriptFlag(){
	memset(mName,0,sizeof(mName));
	mIsFlag = false;
	mIsSet = false;
}

//�󂯎�����t���O����true�ɐݒ肷��
void ScriptFlag::SetOnFlag(const char* Name){

	strcpy(mName,Name);	
	mIsFlag = true;
	mIsSet =  true;
	

}

//�󂯎�����t���O����false�ɐݒ肷��
void ScriptFlag::SetOffFlag(const char* Name){

	
	strcpy(mName,Name);
	mIsFlag = false;
	mIsSet =  true;
	

}

bool ScriptFlag::Get(){
	return mIsFlag;
}

char* ScriptFlag::GetName(){
	return mName;
}

bool ScriptFlag::GetAlreadySet(){
	return mIsSet;
}