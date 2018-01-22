/*
		ファイル名		:Keyboard.h
		作成者			:
		作成日時		:2018/01/22
		ソース説明		:
			
			キーボード入力補完用関数

		備考



!*/

#ifndef __KEY_BOARD_H__
#define __KEY_BOARD_H__

//キー入力されている
bool Keyboard_On(int keyCode);

//キー入力されていない
bool Keyboard_Off(int keyCode);

//キーが押された状態から解放された
bool Keyboard_Relese(int keyCode);

//キーが押され続けている
bool Keyboard_Repeat(int keyCode);

//キーが押された
bool Keyboard_Press(int keyCode);

#endif	//__KEY_BOARD_H__