#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"
#include"math.h"

bool Debug_flg;


_SYSTEM_INFO SYSTEM_INFO;


void Set_Debug_Mode(){

	if(Debug_flg == 0){
		Debug_flg = 1;
	}else{
		Debug_flg = 0;
	}
}

bool Get_Debug_mode(){
	return Debug_flg;
}

void SystemInitialize(){

	// �E�C���h�E���[�h�ŋN�����邩�m�F����
	//if( MessageBox( NULL, "�E�C���h�E���[�h�ŋN�����܂����H", "��ʃ��[�h�m�F", MB_YESNO ) == IDYES )
	//{
	//	// �u�͂��v���I�����ꂽ�ꍇ�̓E�C���h�E���[�h�ŋN��
	//	ChangeWindowMode( TRUE );
	//}
	//if(MessageBox(NULL,"�f�o�b�O���[�h�ŋN�����܂����H","�f�o�b�O�m�F",MB_YESNO) == IDYES){
	//	Set_Debug_Mode();
	//}else{
	//	Debug_flg = 0;
	//}

	//�Q�[���I���t���O�̃I�t
	SYSTEM_INFO.Exit_Game = true;
	//�E�B���h�E�n���h���̎擾
	SYSTEM_INFO.WinHndl = GetMainWindowHandle();


}

void SystemMain(){





}



void Set_Exit_Game(){

	SYSTEM_INFO.Exit_Game = false;
}

bool Get_Exit_Game(){
	return SYSTEM_INFO.Exit_Game;
}

////�t�F�[�h�C���A�E�g�֐�	true:FadeIN		false:FadeOut
//void Fade_io(int Color,int Fade_Flg){
//
//	SYSTEM_INFO.Fade_Out_Count = 0;
//	if(Fade_Flg == true){			
//		for(int i = 255;0 < i;i -= 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}else{
//		for(int i = 0;255 > i;i += 2){
//			SetDrawBlendMode(DX_BLENDMODE_ADD,i);
//			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color,TRUE);
//			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//		}
//	}
//}

HWND GetConsoleHwnd(){








	return ;

}