#include "DxLib.h"
#include "Script.h"
#include "ScriptGraphicsList.h"
#include "../Input.h"
#include "../Code/Common/String/StringBase.h"
#include <math.h>

/*
���͏�Ԃ�ԋp����
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
	mStringBase->FontCreate("�l�r ����", 24, 1, -1);
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

//�X�N���v�g���Ō�̍s�܂œǂݏI���Ă���Ȃ�true��Ԃ�
bool Script::GetScriptEnd(){
	return mLineNum >= mMaxLine;
}

void Script::LoadScript(const char* FileName){

	FILE* fp;
	char Char;
	int Pos;

	//�t�@�C���̓ǂݍ��݂Ɏ��s�����炻�̎��_�œǂݍ��݂𒆒f������
	if((fp = fopen(FileName,"r")) == NULL){
		printfDx("�X�N���v�g�̓ǂݍ��݂Ɏ��s���܂���(%s)\n",FileName);	
		return;
	}

	Pos = 0;
	while(1){

		Char = fgetc(fp);

		//�t�@�C���̓ǂݍ��݈ʒu�����[�܂ŗ��Ă�����ǂݍ��݂���߁A���[�v���甲����
		if(feof(fp))	break;

		//�ǂݍ��񂾕��������p�X�y�[�X���^�u��
		//���ǂݍ��݈ʒu���ŏ���
		//���t�@�C���̓ǂݍ��݂����[�łȂ����
		//�ǂݔ�΂�
		while((Char == ' ' || Char == '\t') && Pos == 0 && !feof(fp)){
			Char = fgetc(fp);
		}

		//�ǂݍ��݈ʒu���u������̍ő吔-1�v��菬�������
		if(Pos >= SCRIPT_MAX_STRING_LENGTH -1){
			printfDx("�X�N���v�g�̕��������������܂�(%d�܂�)\n",SCRIPT_MAX_STRING_LENGTH);
			return;
		}

		//�ǂݍ��񂾕��������s�ł���Ύ��̍s�ֈړ�����
		if(Char == '\n'){
			
			//�ǂݍ��񂾍s������������Ă��Ȃ���΂����艺�̏������΂�
			if(Pos == 0){
				continue;
			}

			mScript[mLineNum][Pos] = '\0';	//������̖����Ƀk������������
			mLineNum++;						//�s�������Z����()
			Pos = 0;						//�������݈ʒu��0�Ԗڂɂ���

		}else{	//�ǂݍ��񂾕��������s�łȂ��Ȃ�

			mScript[mLineNum][Pos] = Char;	//�ǂݍ��񂾕���������
			Pos++;							//�������݈ʒu�����Z����(�E�ւ��炷)

		}

	}

	fclose(fp);

	mMaxLine = mLineNum;		//�ő�s��������
	mLineNum = -1;				//�s���̐���0�ɏ�����
	mFileName = (char*)FileName;//�ǂݍ��񂾃t�@�C����������

}

//������𕪊�����
//Src		:�������镶��
//Dest		:�������ꂽ����
//Delim		:��؂蕶��
//SplitNum	:�ő啪����
void Script::ScriptSplit(const char* Src,char* Dest[],const char* Delim,int SplitNum){

	int i;
	char* Cp;
	char* CpySrc;

	//�K�v���̃��������m�ۂ���
	CpySrc = new char[SCRIPT_MAX_STRING_LENGTH+1];
	//�������镶�����R�s�[����
	strcpy(CpySrc,Src);

	Cp = CpySrc;

	//������𕪊�
	for(i = 0;i < SplitNum;i++){
		//�����Ώە����񂪖����Ȃ�܂ŕ���
		if((Dest[i] = strtok(Cp,Delim)) == NULL){
			break;
		}
		Cp = NULL;
	}

	Dest[i] = NULL;

}

//�X�N���v�g�̉�͂��s���֐�
//talk	:��b����\��������
//goto	:�w�肳�ꂽ���x�������������������炻�̍s�֓ǂݍ��݈ʒu���ړ�������
//seton	:�t���O��true�Ő錾����
//setoff:�t���O��false�Ő錾����
//if	:�t���O��true�ł��endif�܂ł̊Ԃ̃X�N���v�g��ǂݍ���
//ifn	:�t���O��false�ł����endif�܂ł̊Ԃ̃X�N���v�g��ǂݍ���
//endif	:if,ifn�̏I��������
//end	:�X�N���v�g�̏I��
void Script::AnalysisString(){

	
	//�X�N���v�g���ǂݏI����Ă���Ȃ甲����
	if(GetScriptEnd())	return;


	char* Message[SCRIPT_MAX_STRING_LENGTH];


	//������𔼊p�X�y�[�X�ŕ�������
	ScriptSplit(mScript[mLineNum],Message," ",SCRIPT_MAX_STRING_LENGTH);
	
	if(strncmp(Message[0],"talk",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetMessage(Message[1]);
		WaitKeyOn();

	}//���x�����������ă��x���̈ʒu�֔��
	else if(strncmp(Message[0],"goto",SCRIPT_MAX_STRING_LENGTH) == 0){
		int Line = SerchLabel(Message[1]);

		//�G���[�ł���Δ�����
		if(Line == -1){
			return;
		}
		//�X�N���v�g�̓ǂݍ��݋Ƃ����Ă������x���s�ɂ���
		mLineNum = Line;

	}else if(strncmp(Message[0],"label",SCRIPT_MAX_STRING_LENGTH) == 0){

	}//�R�����g
	else if(strncmp(Message[0],"//",SCRIPT_MAX_STRING_LENGTH) == 0){
		//printfDx("�R�����g\n");
	}else if(strncmp(Message[0],"seton",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetOnFlag(Message[1]);
	}else if(strncmp(Message[0],"if",SCRIPT_MAX_STRING_LENGTH) == 0){

		//�t���O��true�łȂ����endif�܂ŃX�N���v�g�̍s���΂�
		if(!GetFlag(Message[1])){
			while(strncmp(Message[0],"endif",SCRIPT_MAX_STRING_LENGTH)!=0){
				ScriptSplit(mScript[mLineNum++],Message," ",SCRIPT_MAX_STRING_LENGTH);
			}
		}

	}else if(strncmp(Message[0],"setoff",SCRIPT_MAX_STRING_LENGTH) == 0){
		SetOffFlag(Message[1]);
	}else if(strncmp(Message[0],"ifn",SCRIPT_MAX_STRING_LENGTH) == 0){

		//�t���O��false�łȂ����endif�܂ŃX�N���v�g�̍s���΂�
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
			YesNoFlag("�͂�",no);
		}else{
			YesNoFlag(yes,no);
		}
		
	}else if(strncmp(Message[0],"end",SCRIPT_MAX_STRING_LENGTH) == 0){

	}else{

	}

}
//���b�Z�[�W���Z�b�g����
//�󂯎�������b�Z�[�W�𔼊p������32�����ŉ��s����
//�������\�������邽�߂�mMessage�ϐ��ɓ����
void Script::SetMessage(char* Message){

	mStringBase->SetString(Message);
}

//�w�肳�ꂽ���x����������Ճe�L�X�g�̏㕔���猟����������������w�肵�����x�����̍s����Ԃ�
//LabelName	:���x���̖��O
//-1		:�G���[
//Line�@�@	:�w�肵�����x�����̍s����ԋp����
int Script::SerchLabel(char* LabelName){

	char* Message[SCRIPT_MAX_STRING_LENGTH];
	static int Line = 0;
	static int i = 0;

	for(i = 0;i <= mMaxLine;i++){
		//������𔼊p�X�y�[�X�ŕ�������
		ScriptSplit(mScript[i],Message," ",SCRIPT_MAX_STRING_LENGTH);
		//���x�����߂�
		if(strncmp(Message[0],"label",SCRIPT_MAX_STRING_LENGTH) == 0){
			//�w�肵�����x���̖��O�ƈ�v������
			if(strncmp(Message[1],LabelName,SCRIPT_MAX_STRING_LENGTH) == 0){
				Line = i;	//���x���̍s��������
				break;
			}
		}

	}

	//�X�N���v�g�̖��[�܂ŗ��Ă��܂�����
	if(i == mMaxLine+1){
		printf("�w�肵�����x���𔭌����邱�Ƃ͂ł��܂���ł���(%s)\n",LabelName);
		return -1;	//�G���[��Ԃ�
	}

	return Line;	//�w�肵�����x���̍s����ԋp����
}

bool Script::SetDrawGraph(int id, int drawX, int drawY) {

	
	

	//�摜�ꗗ���Q��
	const SCRIPT_GRAPHICS_t* graph = ScriptGraphicsList_GetGraphics(id);
	
	mGraphicHandle = graph->image;
	mGraphicsDrawPosX = drawX;
	mGraphicsDrawPosY = drawY;

	return true;
}

void Script::SetOffFlag(const char* Name){

	for(int i = 0;i < SCRIPT_FLAG_NUM;i++){
		//�܂��A�t���O���Z�b�g����Ă��Ȃ��ꏊ�����邩�������O�̃t���O��������΃t���O���Z�b�g����
		if(!(mFlag+i)->GetAlreadySet() || strcmp((mFlag+i)->GetName(),Name) == 0){
			(mFlag+i)->SetOffFlag(Name);	
			break;
		}
	}

}

void Script::SetOnFlag(const char* Name){
	for(int i = 0;i < SCRIPT_FLAG_NUM;i++){
		//�܂��A�t���O���Z�b�g����Ă��Ȃ��ꏊ�����邩�������O�̃t���O��������΃t���O���Z�b�g����
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

	//�L�[���͑҂�����Ȃ����
	if(Input(EInputType_Atk) == true){
		if(select == 0){
			SetOnFlag(name);	//true�ŃZ�b�g
		}else{
			SetOffFlag(name);//false�ŃZ�b�g
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

	//�X�N���v�g�ǂݍ��݂��ɐi�߂�
	if (Input(EInputType_Atk) == true) {
		WaitKeyOff();
	}
	
}
//������̕\��
//X:�\��������W���󂯎��
//Y:
void Script::draw(int X,int Y){

	//�摜�`�揈�����L�q
	if (mGraphicHandle != -1) {
		DrawGraph(mGraphicsDrawPosX, mGraphicsDrawPosY, mGraphicHandle, TRUE);
	}

	//�X�N���v�g��ǂݏI���Ă���Ȃ甲����
	if(GetScriptEnd()) return;

	//�L�[���͑҂��łȂ���Δ�����
	if(!GetWaitKeyFlag()) return;

	//int length = strlen(mMessage[0]);								//�ŏ��̈�s�̕����̒������擾
	int length = mStringBase->DrawString(X, Y, false);				//�ŏ��̈�s�̕����̒������擾
	//int endLength = strlen(mMessage[mDrawLine]);					//�Ō�̈�s�̕�����̒������擾
	int endLength = mStringBase->DrawString(X, Y, false);			//�Ō�̈�s�̕�����̒������擾
	




	//�󂯎�����\�����W�ƕ�����̒����ƍs���ŃT�C�Y��ݒ肵�\��������
	DrawBox(X, Y, X + MESSAGE_WINDOW_WIDTH * length, Y + MESSAGE_WINDOW_HEIHGT * (mDrawLine + 1), GetColor(123, 123, 123), TRUE);

	//�������\������
	mStringBase->DrawString(X,Y,true);


	//���̍s�֑��邱�Ƃ��ł��邱�Ƃ�m�点��
	DrawString(X+MESSAGE_WINDOW_WIDTH*(endLength-1),Y+(mDrawLine+1)*20+sin(DX_PI/2/60* mCouter)*5,"��",GetColor(255,255,255));

	//�X�N���v�g�̕\�����s��
	//DrawFormatString(0,Y-20,GetColor(255,255,255),"%d",mLineNum);

	mCouter++;											//����h�炷���߂̃J�E���^�[
}