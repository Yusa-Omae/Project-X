#if false
#include"DxLib.h"
#include"key.h"
#include"SceneMgr.h"
#include"System.h"
#include"Gamemain.h"
#include"Title.h"
#include"Define.h"
#include"math.h"



_TITLE_DATA *TITLE_DATA;


int Title_BackImage,Title_BackImage2,Title_BackImage_Scroll,Blend_count;
bool Count_Switch_flg,Corsol_Alpha_flg;
int Exit_Charimage,KeyConfig_Charimage,Login_Charimage,Mark_Backimage,Title_Charimage,Corsol_Charimage;
int Mark_Count,Corsol_Count,Corsol_Alpha_Count/*Sin_Count,Next_Corsolpos_X,Next_Corsolpos_Y*/;
int Color_Val[3] = {GetColor(255,255,255),GetColor(0,255,127),GetColor(0,0,0)};//���A�G�������h�O���[��,��
int Corsol_flg,Choice_flg;
int Fade_Out_Count;

_CORSOL Corsol[3];


//������
void TitleInitialize()
{
	////�������̊m��
	//_TITLE_DATA *ix  = (_TITLE_DATA*)malloc(sizeof(_TITLE_DATA));
	////�������̊m�ۂ��ł��Ă��邩�̔��f
	//if(ix != NULL){

	//	SetUseASyncLoadFlag(TRUE);
	//	ix->img[eImg_Back] = LoadGraph("Data/Title_Back.jpg");
	//	ix->img[eImg_Back2] = LoadGraph("Data/Title_Back2.jpg");
	//	ix->img[eImg_Exit] = LoadGraph("Data/Exit.png");
	//	ix->img[eImg_KeyConfig] = LoadGraph("Data/Keyconfig.png");
	//	ix->img[eImg_Login] = LoadGraph("Data/Login.png");
	//	ix->img[eImg_Mark] = LoadGraph("Data/Mark.png");
	//	ix->img[eImg_TitleLogo] = LoadGraph("Data/Title.png");
	//	ix->img[eImg_Corsol] = LoadGraph("Data/Corsol.png");
	//	SetUseASyncLoadFlag(FALSE);
	//Count_Switch_flg = true;
	//Corsol_flg = 0;
	//Choice_flg = 0;
	//Fade_Out_Count =0;
	//Corsol_Count = 0;
	//Corsol_Alpha_Count = 255;
	//Corsol_Alpha_flg = true;
	//	ix->CORSOL[0].PosX = CORSOL_POINT_X1;
	//	ix->CORSOL[0].PosY = CORSOL_POINT_Y1;
	//	ix->CORSOL[1].PosX = CORSOL_POINT_X2;
	//	ix->CORSOL[1].PosY = CORSOL_POINT_Y2;
	//	ix->CORSOL[2].PosX = CORSOL_POINT_X3;
	//	ix->CORSOL[2].PosY = CORSOL_POINT_Y3;
	//}

	SetUseASyncLoadFlag(TRUE);
	Title_BackImage = LoadGraph("Data/Title/Title_Back.jpg");
	Title_BackImage2 = LoadGraph("Data/Title/Title_Back2.jpg");
	Exit_Charimage = LoadGraph("Data/Title/Exit.png");
	KeyConfig_Charimage = LoadGraph("Data/Title/Keyconfig.png");
	Login_Charimage = LoadGraph("Data/Title/Login.png");
	Mark_Backimage = LoadGraph("Data/Title/Mark.png");
	Title_Charimage = LoadGraph("Data/Title/Title.png");
	Corsol_Charimage = LoadGraph("Data/Title/Corsol.png");
	SetUseASyncLoadFlag(FALSE);
	Count_Switch_flg = true;
	Corsol_flg = 0;
	Choice_flg = 0;
	Fade_Out_Count =0;
	Corsol_Count = 0;
	Corsol_Alpha_Count = 255;
	Corsol_Alpha_flg = false;
	Corsol[0].PosX = CORSOL_POINT_X1;
	Corsol[0].PosY = CORSOL_POINT_Y1;
	Corsol[1].PosX = CORSOL_POINT_X2;
	Corsol[1].PosY = CORSOL_POINT_Y2;
	Corsol[2].PosX = CORSOL_POINT_X3;
	Corsol[2].PosY = CORSOL_POINT_Y3;


}

//�^�C�g����ʌv�Z
void Titleupdate()
{
	//�w�i��ʃX�N���[�����u�����h
	if(Count_Switch_flg == true){                           //
		if(Title_BackImage_Scroll < 240){					//
			Title_BackImage_Scroll ++;						//
			Blend_count = Title_BackImage_Scroll;			//Count_Switch_flg��true�̊Ԃ̓^�C�g���t�F�[�Y1
			if(Title_BackImage_Scroll >= 220){				//
				Blend_count = Title_BackImage_Scroll*2;		//Title_BackImage_Scroll��220����������u�����h�𑁂߂�
			}												//
			if(Title_BackImage_Scroll >= 230){
				Count_Switch_flg = false;
			}
		}
	}else{
		if(Title_BackImage_Scroll > 50){					//
			Title_BackImage_Scroll --;						//�t�F�[�Y2�Ɉڂ�u�����h��
			Blend_count = Title_BackImage_Scroll;			//���X�ɉ������Ă���
			if (CheckHitKeyAll() == 1) {
				Count_Switch_flg = false;
			}
		}
	}

	if(Count_Switch_flg == false){
		//if(key(KEY_INPUT_DOWN) == 1){
		//	Sin_Count = 0;
		//	if(Corsol_Count == 2){
		//		Next_Corsolpos_Y = Corsol[0].Y - Corsol[2].Y;
		//		Next_Corsolpos_X = Corsol[0].X - Corsol[2].X;
		//		Corsol_Count = 0;
		//		Corsol_flg = false;
		//	}else{
		//		Next_Corsolpos_Y = Corsol[Corsol_Count+1].Y - Corsol[Corsol_Count].Y;
		//		Next_Corsolpos_X = Corsol[Corsol_Count+1].X - Corsol[Corsol_Count].X;
		//		Corsol_Count ++;
		//		Corsol_flg = true;

		//	}
		//}
		//if(key(KEY_INPUT_UP) == 1){
		//	Sin_Count = 0;
		//	if(Corsol_Count == 0){
		//		Next_Corsolpos_Y = Corsol[2].Y - Corsol[0].Y;
		//		Next_Corsolpos_X = Corsol[2].X - Corsol[0].X;
		//		Corsol_Count = 2;
		//		Corsol_flg = true;
		//	}else{
		//		Next_Corsolpos_Y = Corsol[Corsol_Count].Y - Corsol[Corsol_Count-1].Y;
		//		Next_Corsolpos_X = Corsol[Corsol_Count].X - Corsol[Corsol_Count-1].X;
		//		Corsol_Count --;
		//		Corsol_flg = false;
		//	}
		//}

		//if(Sin_Count < 20){
		//	Sin_Count ++;
		//}
		//if(Corsol_flg == 0){                    //�L�[���͂̎�t�𐧌�
			if(key(KEY_INPUT_UP) == 1){
				Corsol_flg = 1;					//
			}									//Corsol_flg�ɐ��������A�ǂ������������
			if(key(KEY_INPUT_DOWN) == 1){		//���킩��悤�ɂ���
				Corsol_flg = 2;					//
			}
		//}

		if(key(KEY_INPUT_SPACE) == 1){
			Choice_flg = 1;
		}

		if(Choice_flg == 2){
			switch(Corsol_Count){
			case 0:
				SceneMgr_ChangeScene(eScene_Gamemain);
				break;
			case 1:
				break;
			case 2:
				Set_Exit_Game();
				break;
			}
		}

		if(Corsol_flg != 0){
			if(Corsol_Alpha_flg == false){
				Corsol_Alpha_Count -= 40;		//
			}else{								//�L�[��������Ă���A���t�@�u�����h
				Corsol_Alpha_Count += 40;		//���J�n����B
			}

			if(Corsol_Alpha_Count >= 255){
				Corsol_Alpha_flg = false;		//Corsol_Alpha_Count��255�ȏ�ɂȂ�����
				Corsol_flg = 0;					//Corsol_flg�����Z�b�g����B
			}else if(Corsol_Alpha_Count <= 0){
				Corsol_Alpha_flg = true;		//
				if(Corsol_flg == 1){			//Corsol_Alpha_Count��0�ȉ��ɂȂ�����
					if(Corsol_Count == 0){		//�ړ��ł���悤��Corsol_Count��+����B
						Corsol_Count = 2;		//Corsol_flg���P�̏ꍇ�̓}�C�i�X
					}else{						//      �Q�̏ꍇ�̓v���X
						Corsol_Count --;		//
					}							//
				}else if(Corsol_flg == 2){
					if(Corsol_Count == 2){
						Corsol_Count = 0;
					}else{
						Corsol_Count ++;
					}
				}
				
			}
		}


	}




	if(Choice_flg == 1){
		Fade_Out_Count += 20;
		if(Fade_Out_Count >= 255) Choice_flg = 2;
	}

	//�w�i�}�[�N�̍X�V
	Mark_Count --;
	if(Mark_Count <= 0) Mark_Count = 360;



	//�f�o�b�O����X�L�[�Ńe�X�g���j���[�ֈڍs����
#ifdef __MY_DEBUG__
	if (key(KEY_INPUT_X) == 1) {
		SceneMgr_ChangeScene(eScene_TestMemu);
	}
#endif	//__MY_DEBUG__


}
//�`��
void TitleDraw()
{	
	if(Count_Switch_flg  == true){
		SetDrawBlendMode(DX_BLENDMODE_ADD,Blend_count);
		DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color_Val[0],TRUE);							//�^�C�g���t�F�[�Y��
		DrawGraph(TITLE_BACKIMAGE_POS_X,TITLE_BACKIMAGE_POS_Y+Title_BackImage_Scroll/2,Title_BackImage,TRUE);	//���{�b�N�X�̉��Z�u�����h�\��
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	}else{
		if(Choice_flg != 2){
			DrawGraph(INIT_AREA_X1,INIT_AREA_Y1,Title_BackImage2,TRUE);												//�w�i�`��
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,70);
			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color_Val[1],TRUE);							//�΂����Z���邽�߂Ƀ{�b�N�X���A���t�@�u�����h���ď������
			DrawRotaGraph(MARK_POS_X,MARK_POS_Y,2.0,DX_PI/180*Mark_Count,Mark_Backimage,TRUE);						//�}�[�N�̓��߉�]�`��
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,305-Blend_count);						
			DrawGraph(TITLE_POS_X,TITLE_POS_Y,Title_Charimage,TRUE);												//
			DrawGraph(LOGIN_POS_X,LOGIN_POS_Y,Login_Charimage,TRUE);												//�A���t�@�u�����h��
			DrawGraph(KEYCONFIG_POS_X,KEYCONFIG_POS_Y,KeyConfig_Charimage,TRUE);									//�^�C�~���O�����炵�`��
			DrawGraph(EXIT_POS_X,EXIT_POS_Y,Exit_Charimage,TRUE);													//
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

			SetDrawBlendMode(DX_BLENDMODE_ADD,Blend_count);
			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color_Val[0],TRUE);							//���̃t�F�[�h�A�E�g
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

//			DrawGraph(Corsol[Corsol_Count].X,Corsol[Corsol_Count].Y,Corsol_Charimage,TRUE);
			//if(Corsol_flg == true){
			//	DrawGraph(Corsol[Corsol_Count].X+sin(DX_PI/2/20*Sin_Count)*Next_Corsolpos_X,
			//				Corsol[Corsol_Count].Y+sin(DX_PI/2/20*Sin_Count)*Next_Corsolpos_Y,Corsol_Charimage,TRUE);
			//}else{
			//	DrawGraph(Corsol[Corsol_Count].X-sin(DX_PI/2/20*Sin_Count)*-abs(Next_Corsolpos_X),
			//				Corsol[Corsol_Count].Y-sin(DX_PI/2/20*Sin_Count)*-abs(Next_Corsolpos_Y),Corsol_Charimage,TRUE);
			//}
			//DrawFormatString(0,0,Color_Val[2],"Corsol[0].X:%d\nCorsol[0].Y:%d\nCorsol[1].X:%d\nCorsol[1].Y:%d\nCorsol[2].X:%d\nCorsol[2].Y:%d\nCorsol_Count:%d\nSin_Count%d\nCorsol_flg%d\nNext_Corsolpos_X:%d\nNext_Corsolpos_Y:%d\nCorsolposX:%f,Y:%f",
			//	Corsol[0].X,Corsol[0].Y,Corsol[1].X,Corsol[1].Y,Corsol[2].X,Corsol[2].Y,Corsol_Count,Sin_Count,Corsol_flg,Next_Corsolpos_X,Next_Corsolpos_Y,Corsol[Corsol_Count].X-sin(DX_PI/2/20*Sin_Count)*Next_Corsolpos_X,
			//				Corsol[Corsol_Count].Y-sin(DX_PI/2/20*Sin_Count)*Next_Corsolpos_Y);
			
			
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,Corsol_Alpha_Count);
			DrawRotaGraph(Corsol[Corsol_Count].PosX+25,Corsol[Corsol_Count].PosY+25,1.0,DX_PI/180*Mark_Count*5,Corsol_Charimage,TRUE);//�摜�T�C�Y�̔����𑫂��A���S�_�����߃J�[�\���̕`��
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		}

		if(Choice_flg == 1){
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,Fade_Out_Count);
			DrawBox(INIT_AREA_X1,INIT_AREA_Y1,INIT_AREA_X2,INIT_AREA_Y2,Color_Val[3],TRUE);							//�^�C�g���t�F�[�Y��
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);	
		//	Fade_io(Color_Val[3],0);
		}



	}






}
//Trush
void TitleFainalize()
{
	DeleteGraph(Title_BackImage);
	DeleteGraph(Title_BackImage2);
	DeleteGraph(Exit_Charimage);
	DeleteGraph(KeyConfig_Charimage);
	DeleteGraph(Login_Charimage);
	DeleteGraph(Mark_Backimage);
	DeleteGraph(Title_Charimage);
}
#endif