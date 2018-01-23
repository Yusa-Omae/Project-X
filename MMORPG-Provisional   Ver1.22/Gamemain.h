

#ifndef __GAME_MAIN_H__
#define __GAME_MAIN_H__


enum eFadeType {
	eFadeType_None = -1,	//��ԂȂ�
	eFadeType_In,			//�t�F�[�h�C��
	eFadeType_Out,			//�t�F�[�h�A�E�g
	eFadeType_CrossFade,	//�N���X�t�F�[�h
};

enum eGameState {
	eGameState_None = -1,	//��ԂȂ�
	eGameState_Initialize,	//������
	eGameState_Fade,		//�t�F�[�h
	eGameState_Tutorial,	//�`���[�g���A��
	eGameState_MainGame,	//���C���Q�[��
	eGameState_Pouse,		//�|�[�Y���
	eGameState_Shop,		//�V���b�v
};


void GameMainInitialize();
void GameMainupdate();
void GameMainDraw();
void GameMainFainalize();


/*
�Q�[�����C���̃X�e�[�g��؂�ւ���
eGameState gameState				:���̃Q�[���X�e�[�g��ݒ�
eFadeType fadeType = eFadeType_None	:�t�F�[�h�̃^�C�v��ݒ�@��{�̓t�F�[�h���Ȃ���ԂɂȂ��Ă���

�Q�[���X�e�[�g
eGameState_None,		//��ԂȂ�
eGameState_Initialize,	//������
eGameState_Fade,		//�t�F�[�h		����������̃X�e�[�g�ɐݒ肷��ƃV�[�����؂�ւ��Ȃ����Ȃ�܂�
eGameState_Tutorial,	//�`���[�g���A��
eGameState_MainGame,	//���C���Q�[��
eGameState_Pouse,		//�|�[�Y���
eGameState_Shop,		//�V���b�v

�t�F�[�h�^�C�v
eFadeType_None,			//��ԂȂ�
eFadeType_In,			//�t�F�[�h�C��
eFadeType_Out,			//�t�F�[�h�A�E�g
eFadeType_CrossFade,	//�N���X�t�F�[�h

*/
void GameMain_ChangeGameState(eGameState gameState, eFadeType fadeType = eFadeType_None);


#endif	//__GAME_MAIN_H__