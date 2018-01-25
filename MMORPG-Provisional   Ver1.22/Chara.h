#pragma once


void Chara_Initialize();
void Chara_Update();
void Chara_Draw();
void Chara_Fainalize();
void Set_Chara_Direction(int charanum,int direction);
int Get_Chara_Direction(int charanum);
void Set_Anim_Flg(int anim_Kind,bool flg);
void Set_Move_flg(int model_Kind, bool flg);
bool Get_Move_flg(int model_Kind);
/*
�p�x��ԋp����
*/
float Get_Chara_Rotation(int charanum);

void Debag_Chara();


enum{
	model_Player,
	model_Goblin,
	model_Golem,


	model_Max,
};

enum{
	anim_Neutral,
	anim_Run,
	anim_Attack,
	

	anim_Max,
};

enum{
	direction_Down,
	direction_Left,
	direction_Up,
	direction_Right,
};


//�\�͒l����U��ɂ���ē]�E�ł���W���u���ς��V�X�e���Ƃ��f�G
//�䂤�Ȃ�Ύ��R����
//0:�v���C���[ 1-50:�G�l�~�[ 51-100:NPC �\��

struct _CHARA_MGR{

	//�L�����̃q�b�g�|�C���g�X�e�[�^�X
	int HP;
	//�L�����̃}�W�b�N�|�C���g�X�e�[�^�X
	int MP;
	//�L�����̕����U���̓X�e�[�^�X
	int Str;
	//�L�����̖��@�U���̓X�e�[�^�X
	int Int;
	//�L�����̕����h��̓X�e�[�^�X
	int Vit;
	//�L�����̖��@�h��̓X�e�[�^�X
	int Mgr;
	//�L�����̉��́��m��UP�X�e�[�^�X
	int Luc;
	//�L�����̃��f�����
	int Model_N;
	//�L�����̃A�j���[�V�������
	int Anim_N;
	//�L�����̏o���t���O
	int Flg;
	//�L�����̌���
	int Direction;
	//�����Ă��邩
	bool Move;
};

