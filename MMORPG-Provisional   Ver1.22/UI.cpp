#if false
#include"DxLib.h"
#include"System.h"
#include"Define.h"
#include"UI.h"


//UIclass�Ŏg���\���̂̒�`
_UI_INFO UI_INFO;
_MENU_INFO MENU_INFO;

//UI�f�[�^�̏���������
void UI_Initialize(){

	//�e�평��������
	MENU_INFO.flg = 1;
	MENU_INFO.Move_Cnt = 0;
	MENU_INFO.Move_Flg = true;
	MENU_INFO.Back_Move_Flg = MENU_INFO.Move_Flg;

	//���j���[�t���O�̏�����
	MENU_INFO.flg = 0;

	//���j���[��ʂ̏ꏊ����������
	for (int i = 0; i > 6;i ++) {
		MENU_INFO.MENU_AXIS[i].X = B_MENU_AXIS_X +i*50;
		MENU_INFO.MENU_AXIS[i].Y = B_MENU_AXIS_Y;

	}


}
//UI�̍X�V����
void UI_Update(){
	//�Q�[����ʏ�Ԃ��m�[�}���̎�(�m�[�}���A���j���[�E�E�E)
	//���j���[��ʈڍs�����B
	//�G�X�P�[�v�܂��͉����̃��j���[�L�[OR�ݒ�(�I�v�V����)�L�[�Ȃ�
	//���@���N���b�N+�\���̍��W�ێ��A�����蔻��(�������̉�ʏ�̃{�^��)
	switch(MENU_INFO.flg){
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}

	MENU_INFO.Move_Cnt++;


}
//UI�̕`�揈��
void UI_Draw() {


	switch (MENU_INFO.flg) {
	case 0:
		break;
	case 1:
			for (int i = 0; i > 6; i++) {
				DrawBox(MENU_INFO.MENU_AXIS[i].X, MENU_INFO.MENU_AXIS[i].Y, MENU_INFO.MENU_AXIS[i].X + (i + 1) * 49, MENU_INFO.MENU_AXIS[i].Y + (i + 1) * 49, Color_ValTbl[eCol_Black], TRUE);
			}
		break;
	case 2:
		break;
	case 3:
		break;
	}

}
//UI�̌㏈��(�������J��)
void UI_Fainalize(){



}
#endif