#include "DxLib.h"
#include "Script.h"
#include "ScriptGraphicsList.h"
#include "../Input.h"
#include "../Code/Common/String/StringBase.h"
#include <math.h>

/*
入力状態を返却する
*/
static bool Input(int inputType) {

	int EdgeInput = GetEdgeInput();
	int Input = GetInput();

	bool result = false;

#if true
	result = ((EdgeInput & (1 << inputType)) != 0);
#else
	if (inputType >= EInputType_Left && inputType <= EInputType_Down) {
		result = ((Input     & (1 << inputType)) != 0);
	}
	else {
		result = ((EdgeInput & (1 << inputType)) != 0);
	}
#endif
	return result;
}
Script::Script(const char* FileName){
	
	mFlag = new ScriptFlag[SCRIPT_FLAG_NUM];
	
	mStringBase = new StringBase();
	mStringBase->FontCreate("ＭＳ 明朝", 24, 1, -1);
	mStringBase->SetColor(GetColor(255, 255, 255));

	memset(mScript,0,sizeof(mScript));
	memset(mMessage,0,sizeof(mMessage));
	mFileName = "";
	mLineNum = 0;
	mMaxLine = 0;
	mDrawLine = 0;
	mCouter = 0;
	mGraphicHandle = -1;
	

	WaitKeyOff();
	BattleOff();
	GameClearOff();

	LoadScript(FileName);

}

Script::~Script() {
	delete[] mFlag;
	mFlag = NULL;
	delete mStringBase;
	mStringBase = NULL;
}

void Script::WaitKeyOn(){
	isWaitKey = true;
}

void Script::WaitKeyOff(){
	isWaitKey = false;
}

bool Script::GetWaitKeyFlag(){
	return isWaitKey;
}

//スクリプトを最後の行まで読み終えているならtrueを返す
bool Script::GetScriptEnd(){
	return mLineNum >= mMaxLine;
}

void Script::LoadScript(const char* FileName){

	FILE* fp;
	char Char;
	int Pos;

	//ファイルの読み込みに失敗したらその時点で読み込みを中断させる
	if((fp = fopen(FileName,"r")) == NULL){
		printfDx("スクリプトの読み込みに失敗しました(%s)\n",FileName);	
		return;
	}

	Pos = 0;
	while(1){

		Char = fgetc(fp);

		//ファイルの読み込み位置が末端まで来ていたら読み込みをやめ、ループから抜ける
		if(feof(fp))	break;

		//読み込んだ文字が半角スペースかタブで
		//かつ読み込み位置が最初で
		//かつファイルの読み込みが末端でなければ
		//読み飛ばす
		while((Char == ' ' || Char == '\t') && Pos == 0 && !feof(fp)){
			Char = fgetc(fp);
		}

		//読み込み位置が「文字列の最大数-1」より小さければ
		if(Pos >= SCRIPT_MAX_STRING_LENGTH -1){
			printfDx("スクリプトの文字数が多すぎます(%dまで)\n",SCRIPT_MAX_STRING_LENGTH);
			return;
		}

		//読み込んだ文字が改行であれば次の行へ移動する
		if(Char == '\n'){
			
			//読み込んだ行が何も書かれていなければこれより下の処理を飛ばす
			if(Pos == 0){
				continue;
			}

			mScript[mLineNum][Pos] = '\0';	//文字列の末尾にヌル文字を入れる
			mLineNum++;						//行数を加算する()
			Pos = 0;						//書き込み位置を0番目にする

		}else{	//読み込んだ文字が改行でないなら

			mScript[mLineNum][Pos] = Char;	//読み込んだ文字を入れる
			Pos++;							//書き込み位置を加算する(右へずらす)

		}

	}

	fclose(fp);

	mMaxLine = mLineNum;		//最大行数を入れる
	mLineNum = -1;				//行数の数を0に初期化
	mFileName = (char*)FileName;//読み込んだファイル名を入れる

}

//文字列を分割する
//Src		:分割する文字
//Dest		:分割された文字
//Delim		:区切り文字
//SplitNum	:最大分割数
void Script::ScriptSplit(const char* Src,char* Dest[],const char* Delim,int SplitNum){

	int i;
	char* Cp;
	char* CpySrc;

	//必要数のメモリを確保する
	CpySrc = new char[SCRIPT_MAX_STRING_LENGTH+1];
	//分割する文字をコピーする
	strcpy(CpySrc,Src);

	Cp = CpySrc;

	//文字列を分割
	for(i = 0;i < SplitNum;i++){
		//分割対象文字列が無くなるまで分割
		if((Dest[i] = strtok(Cp,Delim)) == NULL){
			break;
		}
		Cp = NULL;
	}

	Dest[i] = NULL;

}

//スクリプトの解析を行う関数
//talk	:会話文を表示させる
//goto	:指定されたラベル名を検索し見つけたらその行へ読み込み位置を移動させる
//seton	:フラグをtrueで宣言する
//setoff:フラグをfalseで宣言する
//if	:フラグがtrueでればendifまでの間のスクリプトを読み込む
//ifn	:フラグがfalseであればendifまでの間のスクリプトを読み込む
//endif	:if,ifnの終わりを示す
//end	:スクリプトの終了
void Script::AnalysisString(){

	
	//スクリプトが読み終わっているなら抜ける
	if(GetScriptEnd())	return;


	char* Message[SCRIPT_MAX_STRING_LENGTH];


	//文字列を半角スペースで分割する
	ScriptSplit(mScript[mLineNum],Message," ",SCRIPT_MAX_STRING_LENGTH);
	
	if(strncmp(Message[0],"talk",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetMessage(Message[1]);
		WaitKeyOn();

	}//ラベルを検索してラベルの位置へ飛ぶ
	else if(strncmp(Message[0],"goto",SCRIPT_MAX_STRING_LENGTH) == 0){
		int Line = SerchLabel(Message[1]);

		//エラーであれば抜ける
		if(Line == -1){
			return;
		}
		//スクリプトの読み込み業をしてしたラベル行にする
		mLineNum = Line;

	}else if(strncmp(Message[0],"label",SCRIPT_MAX_STRING_LENGTH) == 0){

	}//コメント
	else if(strncmp(Message[0],"//",SCRIPT_MAX_STRING_LENGTH) == 0){
		//printfDx("コメント\n");
	}else if(strncmp(Message[0],"seton",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetOnFlag(Message[1]);
	}else if(strncmp(Message[0],"if",SCRIPT_MAX_STRING_LENGTH) == 0){

		//フラグがtrueでなければendifまでスクリプトの行を飛ばす
		if(!GetFlag(Message[1])){
			while(strncmp(Message[0],"endif",SCRIPT_MAX_STRING_LENGTH)!=0){
				ScriptSplit(mScript[mLineNum++],Message," ",SCRIPT_MAX_STRING_LENGTH);
			}
		}

	}else if(strncmp(Message[0],"setoff",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetOffFlag(Message[1]);
	}else if(strncmp(Message[0],"ifn",SCRIPT_MAX_STRING_LENGTH) == 0){

		//フラグがfalseでなければendifまでスクリプトの行を飛ばす
		if(GetFlag(Message[1])){
			while(strncmp(Message[0],"endif",SCRIPT_MAX_STRING_LENGTH)!=0){
				ScriptSplit(mScript[mLineNum++],Message," ",SCRIPT_MAX_STRING_LENGTH);
			}
		}

	}else if(strncmp(Message[0],"endif",SCRIPT_MAX_STRING_LENGTH) == 0){

	}else if(strncmp(Message[0],"Draw",SCRIPT_MAX_STRING_LENGTH) == 0){
		int id = atoi(Message[1]);
		int drawX = atoi(Message[2]);
		int drawY = atoi(Message[3]);

		SetDrawGraph(id, drawX, drawY);

	}else if(strncmp(Message[0],"yesno",SCRIPT_MAX_STRING_LENGTH) == 0){
		char yes[256],no[256];

		strcpy(yes,Message[1]);
		strcpy(no,Message[2]);

		if(strcmp(yes,"none") == 0 && strcmp(no,"none") == 0){
			YesNoFlag();
		}else if(strcmp(yes,"none") != 0 && strcmp(no,"none") == 0){
			YesNoFlag(yes);
		}else if(strcmp(yes,"none") == 0 && strcmp(no,"none") != 0){
			YesNoFlag("はい",no);
		}else{
			YesNoFlag(yes,no);
		}
		
	}else if(strncmp(Message[0],"end",SCRIPT_MAX_STRING_LENGTH) == 0){

	}else{

	}

}
//メッセージをセットする
//受け取ったメッセージを半角文字で32文字で改行させ
//文字列を表示させるためのmMessage変数に入れる
void Script::SetMessage(char* Message){

	mStringBase->SetString(Message);
}

//指定されたラベルをすくりぷテキストの上部から検索し発見をしたら指定したラベル名の行数を返す
//LabelName	:ラベルの名前
//-1		:エラー
//Line　　	:指定したラベル名の行数を返却する
int Script::SerchLabel(char* LabelName){

	char* Message[SCRIPT_MAX_STRING_LENGTH];
	static int Line = 0;
	static int i = 0;

	for(i = 0;i <= mMaxLine;i++){
		//文字列を半角スペースで分割する
		ScriptSplit(mScript[i],Message," ",SCRIPT_MAX_STRING_LENGTH);
		//ラベル命令で
		if(strncmp(Message[0],"label",SCRIPT_MAX_STRING_LENGTH) == 0){
			//指定したラベルの名前と一致したら
			if(strncmp(Message[1],LabelName,SCRIPT_MAX_STRING_LENGTH) == 0){
				Line = i;	//ラベルの行数を入れる
				break;
			}
		}

	}

	//スクリプトの末端まで来てしまったら
	if(i == mMaxLine+1){
		printf("指定したラベルを発見することはできませんでした(%s)\n",LabelName);
		return -1;	//エラーを返す
	}

	return Line;	//指定したラベルの行数を返却する
}

bool Script::SetDrawGraph(int id, int drawX, int drawY) {

	
	

	//画像一覧を参照
	const SCRIPT_GRAPHICS_t* graph = ScriptGraphicsList_GetGraphics(id);
	
	mGraphicHandle = graph->image;
	mGraphicsDrawPosX = drawX;
	mGraphicsDrawPosY = drawY;

	return true;
}

void Script::SetOffFlag(const char* Name){

	for(int i = 0;i < SCRIPT_FLAG_NUM;i++){
		//まだ、フラグがセットされていない場所があるか同じ名前のフラグ名があればフラグをセットする
		if(!(mFlag+i)->GetAlreadySet() || strcmp((mFlag+i)->GetName(),Name) == 0){
			(mFlag+i)->SetOffFlag(Name);	
			break;
		}
	}

}

void Script::SetOnFlag(const char* Name){
	for(int i = 0;i < SCRIPT_FLAG_NUM;i++){
		//まだ、フラグがセットされていない場所があるか同じ名前のフラグ名があればフラグをセットする
		if(!(mFlag+i)->GetAlreadySet() || strcmp((mFlag+i)->GetName(),Name) == 0){
			(mFlag+i)->SetOnFlag(Name);
			break;
		}
	}
}

bool Script::GetFlag(const char* Name){


	for(int i = 0;i < SCRIPT_FLAG_NUM;i++){
		if(strncmp((mFlag+i)->GetName(),Name,STRING_LENGHT) == 0){
			return (mFlag+i)->Get();
		}
	}
}

void Script::YesNoFlag(const char* Yes,const char* No){
	
	const char* yesno[2] = {Yes,No};
	const char name[3] = {"yn"};
	static int select = 0;


	WaitKeyOn();

	DrawBox(YESNO_POSITION_X, YESNO_POSITION_Y, YESNO_POSITION_X + YESNO_BOX_WIDHT, YESNO_POSITION_Y + YESNO_BOX_HEIGHT, GetColor(123,123,123), TRUE);
	DrawBox(YESNO_POSITION_X, YESNO_POSITION_Y, YESNO_POSITION_X + YESNO_BOX_WIDHT, YESNO_POSITION_Y + YESNO_BOX_HEIGHT, GetColor(255, 255, 255), FALSE);
	

	if(Input(EInputType_Down) == true){
		select = (select + 1) % 2;
	}

	if(Input(EInputType_Up) == true){
		select = (select + 1) % 2;
	}

	//キー入力待ちじゃなければ
	if(Input(EInputType_Atk) == true){
		if(select == 0){
			SetOnFlag(name);	//trueでセット
		}else{
			SetOffFlag(name);//falseでセット
		}
	}

	for(int i = 0;i < 2;i++){
		if(select == i){
			DrawString(YESNO_POSITION_X+5,YESNO_POSITION_Y+20+40*i,yesno[i],GetColor(255,255,255));
		}else{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,123);
			DrawString(YESNO_POSITION_X+5,YESNO_POSITION_Y+20+40*i,yesno[i],GetColor(255,255,255));
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		}
	}

}

void Script::BattleOff(){
	mIsBattle = false;
}

void Script::BattleOn(){
	mIsBattle = true;
}

bool Script::GetBattleFlag(){
	return mIsBattle;
}
void Script::AddScriptLine(){
	if(GetWaitKeyFlag())	return;

	if(mLineNum < mMaxLine){
		mLineNum++;
	}
}

void Script::GameClearOn(){
	mIsGameClear = true;
}

void Script::GameClearOff(){
	mIsGameClear = false;
}

bool Script::GetGameClearFlag(){
	return mIsGameClear;
}
//
//void Script::ShopFlagOn(){
//	mIsShop = true;
//}
//
//void Script::ShopFlagOff(){
//	mIsShop = false;
//}
//
//bool Script::GetShopFlag(){
//	return mIsShop;
//}


void Script::updata(){

	
	
	AddScriptLine();	
	AnalysisString();
	
	mStringBase->Update(true, 40, 5);

	//スクリプト読み込みを先に進める
	if (Input(EInputType_Atk) == true) {
		WaitKeyOff();
	}
	
}
//文字列の表示
//X:表示する座標を受け取る
//Y:
void Script::draw(int X,int Y){

	//画像描画処理を記述
	if (mGraphicHandle != -1) {
		DrawGraph(mGraphicsDrawPosX, mGraphicsDrawPosY, mGraphicHandle, TRUE);
	}

	//スクリプトを読み終えているなら抜ける
	if(GetScriptEnd()) return;

	//キー入力待ちでなければ抜ける
	if(!GetWaitKeyFlag()) return;

	//int length = strlen(mMessage[0]);								//最初の一行の文字の長さを取得
	int length = mStringBase->DrawString(X, Y, false);				//最初の一行の文字の長さを取得
	//int endLength = strlen(mMessage[mDrawLine]);					//最後の一行の文字列の長さを取得
	int endLength = mStringBase->DrawString(X, Y, false);			//最後の一行の文字列の長さを取得
	




	//受け取った表示座標と文字列の長さと行数でサイズを設定し表示させる
	DrawBox(X, Y, X + MESSAGE_WINDOW_WIDTH * length, Y + MESSAGE_WINDOW_HEIHGT * (mDrawLine + 1), GetColor(123, 123, 123), TRUE);

	//文字列を表示する
	mStringBase->DrawString(X,Y,true);


	//次の行へ送ることができることを知らせる
	DrawString(X+MESSAGE_WINDOW_WIDTH*(endLength-1),Y+(mDrawLine+1)*20+sin(DX_PI/2/60* mCouter)*5,"▼",GetColor(255,255,255));

	//スクリプトの表示を行数
	//DrawFormatString(0,Y-20,GetColor(255,255,255),"%d",mLineNum);

	mCouter++;											//▼を揺らすためのカウンター
}