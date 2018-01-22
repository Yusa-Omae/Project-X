#pragma once

void Set_Exit_Game();
bool Get_Exit_Game();
void SystemInitialize();
void SystemMain();
//void Fade_io(int Color,int Fade_Flg);


enum eCol{
	eCol_White,
	eCol_EmeraldGreen,
	eCol_Black,
	eCol_Max,
};

//�J���[�e�[�u��0:��1:�G�������h�O���[��2:��
const int Color_ValTbl[eCol_Max] = {
	GetColor(255,255,255),
	GetColor(0,255,127),
	GetColor(0,0,0)
};

//�V�X�e���n���̍\����
struct _SYSTEM_INFO{

	//�Q�[�����I�����邩�ǂ���
	bool Exit_Game;
	//�S�̂̃t�F�[�h�A�E�g�Ŏg�p����J�E���g�ϐ�
	int Fade_Out_Count;
	//Window�̃n���h��
	HWND WinHndl;
	//Window�̍��W�l
	LPRECT WinAxis;
	//Window�̃^�C�g��
	char WinTitle;

};