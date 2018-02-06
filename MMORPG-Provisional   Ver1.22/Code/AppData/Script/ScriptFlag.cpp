#include "DxLib.h"
#include "ScriptFlag.h"



ScriptFlag::ScriptFlag(){
	memset(mName,0,sizeof(mName));
	mIsFlag = false;
	mIsSet = false;
}

//受け取ったフラグ名をtrueに設定する
void ScriptFlag::SetOnFlag(const char* Name){

	strcpy(mName,Name);	
	mIsFlag = true;
	mIsSet =  true;
	

}

//受け取ったフラグ名をfalseに設定する
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