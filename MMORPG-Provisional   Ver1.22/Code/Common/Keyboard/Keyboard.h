/*
		�t�@�C����		:Keyboard.h
		�쐬��			:
		�쐬����		:2018/01/22
		�\�[�X����		:
			
			�L�[�{�[�h���͕⊮�p�֐�

		���l



!*/

#ifndef __KEY_BOARD_H__
#define __KEY_BOARD_H__

//�L�[���͂���Ă���
bool Keyboard_On(int keyCode);

//�L�[���͂���Ă��Ȃ�
bool Keyboard_Off(int keyCode);

//�L�[�������ꂽ��Ԃ��������ꂽ
bool Keyboard_Relese(int keyCode);

//�L�[�������ꑱ���Ă���
bool Keyboard_Repeat(int keyCode);

//�L�[�������ꂽ
bool Keyboard_Press(int keyCode);

#endif	//__KEY_BOARD_H__