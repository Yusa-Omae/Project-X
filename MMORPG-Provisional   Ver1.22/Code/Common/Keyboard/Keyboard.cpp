/*
		�t�@�C����		:Keyboard.cpp
		�쐬��			:
		�쐬����		:2018/01/22
		�\�[�X����		:

		�L�[�{�[�h���͕⊮�p�֐�

		���l



!*/
#include "DxLib.h"
#include "Keyboard.h"
#include "../Key.h"

//-------------------------- �Œ�萔
static const int KEY_MAX_NUM = 256;

//-------------------------- �X�^�e�B�b�N�ϐ���`
static int s_ReleseCount[KEY_MAX_NUM] = {0};	//�L�[���J������Ă���ԃJ�E���g����


/*
	�L�[���͂����͂���Ă��鎞�Ԃ�ԋp����
	int		keyCode	:���͂��Ă���L�[�R�[�h�ԍ�
*/
static int _CheckHitKey(int keyCode){
	return key(keyCode);
}

/*
	�L�[�����͂���Ă���
	return	true	:	�L�[�����͂���Ă���
			false	:	�L�[�����͂���Ă��Ȃ�
*/
bool Keyboard_On(int keyCode){
	return _CheckHitKey(keyCode) > 0;
}

/*
	�L�[�����͂���Ă��Ȃ�
	return	true	:�L�[�����͂���Ă��Ȃ�
			false	:�L�[�����͂���Ă���
*/
bool Keyboard_Off(int keyCode){
	return _CheckHitKey(keyCode) == 0;
}

/*
	�L�[���J�����ꂽ
	return	true	:�L�[���J�����ꂽ
			false	:�L�[���J������Ă��Ȃ�
*/
bool Keyboard_Relese(int keyCode){
	if(_CheckHitKey(keyCode) > 0){
		s_ReleseCount[keyCode] = 0;
		return false;
	}
	s_ReleseCount[keyCode]++;
	return  s_ReleseCount[keyCode] == 1;
}

/*
	�L�[�����s�[�g����Ă���
	return	true	:�L�[��������Ă���
			false	:�L�[��������Ă��Ȃ�
*/	
bool Keyboard_Repeat(int keyCode){
	return _CheckHitKey(keyCode) == 1 || (_CheckHitKey(keyCode) > 4 && _CheckHitKey(keyCode) % 20 == 0);
}

/*
	�L�[��������Ă���
	return	true	:�L�[�������ꂽ
			false	:�L�[��������Ă��Ȃ�
*/
bool Keyboard_Press(int keyCode){
	return _CheckHitKey(keyCode) == 1;
}
