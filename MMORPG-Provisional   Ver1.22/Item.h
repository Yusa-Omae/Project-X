#ifndef ITEM_H
#define ITEM_H


typedef struct _ITEM_PARAM{

	//�A�C�e���i���o�[
	int Num;
	//�A�C�e���̃��x���i�K
	int Lv;
	//�A�C�e���̒l�i
	int Pre;
	//�A�C�e���̑ϋv�l
	int Db;
	//���������ۂ̒ǉ�HP
	int Hp;
	//���������ۂ̒ǉ��U����
	int Atk;
	//���������ۂ̒ǉ��h���
	int Def;
	//���������ۂ̒ǉ��N���e�B�J����
	float Crt;
	//���������ۂ̒ǉ��ړ����x
	float Spd;
	//�U�������ۂ̒ǉ�HP�z����
	float Abp;
	//���������ۂ̒ǉ����W�F�l
	int Ah;
	//�G��|�����ۂ̃h���b�v��������
	float Gp;
	//�i����
	int Evol[4];

}ITEM_PARAM;










#endif