
#if false
void UI_Initialize();
void UI_Update();
void UI_Draw();
void UI_Fainalize();

//UI�\�����W�ۑ��\����
typedef struct _MENU_AXIS{

	int X;
	int Y;

}_MENU_AXIS;

//���j���[�n�\����
typedef struct _MENU_INFO {

	//�W�J�t���O
	int flg;

	//�|�b�v�A�b�v���J�E���g
	int Move_Cnt;

	//�|�b�v�A�b�v�t���O
	bool Move_Flg;//true = �A�C�h�� false = �W�J�ς�

	//1�t���[���O�̃|�b�v�A�b�v�t���O
	bool Back_Move_Flg;//1�t���[���O�Ƃ̃t���O���قɂ��A�j���[�V����(Sin�h���g�p�������W�ړ�)���s��

	//���j���[�`����W�l�Ǘ�
	_MENU_AXIS MENU_AXIS[6];

}_MENU_INFO;

//UIclass�Ŏg�p���邻�̑��̂���
typedef struct _UI_INFO {


}_UI_INFO;
#endif