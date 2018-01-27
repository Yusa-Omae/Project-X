#include "DxLib.h"

//�E�B���h�E�T�C�Y�̃}�N��
#define WINDOW_WIDHT 800
#define WINDOW_HEIGHT 600

//�ȉ��A�}�b�v�̃}�N��
#define MAP_CHIP_SIZE 100
#define MAP_CHIP_XNUM 40
#define MAP_CHIP_YNUM 40
#define MAP_LAYER_NUM 4

#define MAP_WIDTH (MAP_CHIP_XNUM * MAP_CHIP_SIZE)
#define MAP_HEIGHT (MAP_CHIP_YNUM * MAP_CHIP_SIZE)

#define DIV_SPRITE_NUM 100

#define MAP_FILENAME "data/stg%d.csv"

//�ȉ��A�}�b�v�`�b�v�̃}�N��
#define MAP_CHIP_NONE       -1					//�}�b�v�`�b�v�Ȃ�
#define MAP_CHIP_BOM_YWLLOW  25					//���j�@���F
#define MAP_CHIP_BOM_RED	30					//���j�@�ԐF
#define MAP_CHIP_BOM_BLUE	35					//���j�@�F
#define MAP_CHIP_BOM_WHITE  40					//���j�@���F
#define MAP_CHIP_BOM_BLACK  45					//���j�@���F
#define MAP_CHIP_DOOR	    55					//���h�A
#define MAP_CHIP_PAINT		50					//�y���L
#define MAP_CHIP_WARP		65					//���[�v
#define MAP_CHIP_SET__PLAYER_POSITION 97		//�v���C���[�����ʒu
#define MAP_CHIP_GOAL 98						//�S�[��
#define MAP_CHIP_KEY 60							//��
#define MAP_CHIP_ENEMY 80						//�G�l�~�[
#define MAP_CHIP_WATER 75						//��

//�ȉ��A�L�����N�^�[�̃}�N��
#define CHARACTOR_COLOR_NUM 6
#define PLAYER_SPEED 5
#define MAX_PLAYER_HP 200
#define HAVE_ITEM_NUM 10

//�ȉ��A�G�̃}�N��
#define ENEMY_NUM 20
#define ENEMY_KIND 6
#define ENEMY_ANIMATION_NUM 12

//�ړ������̋��p��
enum eVector{

	eVector_Right,
	eVector_Up,
	eVector_Down,
	eVector_Left,

	eVector_None,

};

//�F�̎��
enum eColorKind{
	eColorKind_Yellow,	//���F
	eColorKind_Red,		//�ԐF
	eColorKind_Blue,	//�F
	eColorKind_White,	//���F
	eColorKind_Black,	//���F


	eColorKind_Nomal,	//�ʏ�F


};

enum eMapGimic{

	//�ʏ�}�b�v�`�b�v

	eMapGimic_None = MAP_CHIP_NONE,			//�����Ȃ��`�b�v

	eMapGimic_Mud,					//�D
	eMapGimic_Soil,					//�y
	eMapGimic_Stone,				//��
	eMapGimic_Bone,					//��
	eMapGimic_Rock,					//��

	eMapGimic_MoistSoil,			//�������y
	eMapGimic_Turf,					//��
	eMapGimic_Flower,				//��
	eMapGimic_TurfBone,				//�ł̍�
	eMapGimic_Wood,					//��

	eMapGimic_IceSoil,				//�X�y
	eMapGimic_Ice,					//�X
	eMapGimic_IceFlower,			//�X��
	eMapGimic_IceBone,				//�X��
	eMapGimic_IceRock,				//�X��

	eMapGimic_SandSoil,				//���y
	eMapGimic_Sand,					//��
	eMapGimic_Sandpile,				//���R
	eMapGimic_SandBone,				//����
	eMapGimic_Cactus,				//�T�{�e��

	eMapGimic_StonePaveMentSoil,	//�Ώ��y
	eMapGimic_StonePavement,		//�Ώ�
	eMapGimic_StonePavementSotne,	//�Ώ���
	eMapGimic_StonePavementBone,	//�Ώ�
	eMapGimic_rubble,				//���I

	//���j ��
	eMapGimic_BomYellowUp = MAP_CHIP_BOM_YWLLOW,
	eMapGimic_BomYellowDowm,
	eMapGimic_BomYellowLeft,
	eMapGimic_BomYellowRight,

	//���j ��
	eMapGimic_BomRedUp = MAP_CHIP_BOM_RED,
	eMapGimic_BomRedDowm,
	eMapGimic_BomRedLeft,
	eMapGimic_BomRedRight,

	//���j ��
	eMapGimic_BomBuleUp = MAP_CHIP_BOM_BLUE,
	eMapGimic_BomBuleDowm,
	eMapGimic_BomBuleLeft,
	eMapGimic_BomBuleRight,

	//���j ��
	eMapGimic_BomWhiteUp = MAP_CHIP_BOM_WHITE,
	eMapGimic_BomWhiteDowm,
	eMapGimic_BomWhiteLeft,
	eMapGimic_BomWhiteRight,

	//���j ��
	eMapGimic_BomBlackUp = MAP_CHIP_BOM_BLACK,
	eMapGimic_BomBlackDowm,
	eMapGimic_BomBlackLeft,
	eMapGimic_BomBlackRight,

	//�y�C���g
	eMapGimic_PaintYellow = MAP_CHIP_PAINT,
	eMapGimic_PaintRed,
	eMapGimic_PaintBule,
	eMapGimic_PaintWhite,
	eMapGimic_PaintBlack,

	//����
	eMapGimic_DoorYellow = MAP_CHIP_DOOR,
	eMapGimic_DoorRed,
	eMapGimic_DoorBule,
	eMapGimic_DoorWhite,
	eMapGimic_DoorBlack,	

	//��
	eMapGimic_KeyYellow = MAP_CHIP_KEY,
	eMapGimic_KeyRed,
	eMapGimic_KeyBlue,
	eMapGimic_KeyWhite,
	eMapGimic_KeyBlack,

	//���[�v
	eMapGimic_WarpYellow = MAP_CHIP_WARP,
	eMapGimic_WarpRed,
	eMapGimic_WarpBlue,
	eMapGimic_WarpWhite,
	eMapGimic_WarpBlack,

	//���]
	eMapGimic_BomRotat,
	eMapGimic_BomRota2,

	//�
	eMapGimic_Trap,
	
	//���Ƃ���
	eMapGimic_Pitfall,
	
	//�񕜂̐�
	eMapGimic_Water = MAP_CHIP_WATER,

	//���v
	eMapGimic_Clock,

	//��
	eMapGimic_Paper,

	//��
	eMapGimic_Treasure,

	//�v���C���[�����ʒu
	eMapGimic_SetPlayerPosition = MAP_CHIP_SET__PLAYER_POSITION,

	//�S�[��
	eMapGimic_Goal = MAP_CHIP_GOAL,


	

	//??(�j��\�ʍs�s�\�`�b�v??)

	//??(�j��s�\�ʍs�s�\�`�b�v??)

	//�G�l�~�[
	eMapGimic_Enemy1 = MAP_CHIP_ENEMY,
	eMapGimic_Enemy2,
	eMapGimic_Enemy3,
	eMapGimic_Enemy4,
	eMapGimic_Enemy5,
	eMapGimic_Enemy6,

};

typedef struct{
	int id;			//�Ǘ��ԍ�
	int kind;		//�A�C�e���ԍ�
}Item_t;

//�L�����N�^�[�̍\����
typedef struct{
	int x,y;			//�\�����W
	int width,height;	//�T�C�Y
	int colorKind;		//�L�����N�^�[�̎��
	int vec;			//����
	int hp;				//�̗�
	int maxhp;			//�ő�HP
	bool isWalk;		//�����Ă��邩?
	int walkCnt;		//���s�J�E���g

	Item_t item[HAVE_ITEM_NUM];

}Charactor_t;

typedef struct{
	int x,y;				//���W
	int width,height;		//�摜�T�C�Y
	int kind;				//�摜�̎��
	int movePattern;		//�s���p�^�[��
	int live;				//����
}Enemy_t;


//�}�b�v�̃f�[�^��ێ�����\����
typedef struct {
	int x, y;							//�\�����W
	int mapChipKind[MAP_LAYER_NUM];		//�}�b�v�`�b�v�̎��(���C���[)
	int gimicKind;						//�M�~�b�N�̎��
	int itemKind;						//�A�C�e���̎��
	int enemyKind;						//�G�l�~�[�̎��
	int passible;						//�ʍs��
	int setPlayer;						//�v���C���[�̏����z�u�ꏊ
}Map_t;

//�}�b�v�������ׂẴf�[�^��ێ�����\����
typedef struct {
	char mapName[33];				//�}�b�v�̃^�C�g��
	char mapDescription[1024];		//�}�b�v�̐���
	int gimicVisibleKind[10];		//���݂��Ă���M�~�b�N�̎��
	Map_t map[MAP_CHIP_YNUM][MAP_CHIP_XNUM];	//�}�b�v�f�[�^
}MapMgr_t;


MapMgr_t mMap;
Charactor_t ch;	//�L�����N�^�[
Enemy_t enemy[ENEMY_NUM];


int MapChipImage[DIV_SPRITE_NUM];
int CharactorImage[CHARACTOR_COLOR_NUM];

//�G�L�����̐�
int SetEnemyNum;
int EnemyImage[ENEMY_KIND][ENEMY_ANIMATION_NUM];	//�G�̉摜��ǂݍ���

int trun;	//�o�ߎ���

//���ۂɕ`�������ʒu
int CameraPositionX;
int CameraPositionY;

//�L�[���͗p�ϐ�
int InputKey[256];

//�S�[���������H
bool isGoal;
//�Q�[���I�[�o�[���H
bool IsGameOver;

//�ȉ��A�L�[����Ɋւ��鏈��

void KeyUpdata(){

	char keyBuffer[256];
	
	GetHitKeyStateAll(keyBuffer);

	for(int i = 0;i < 256;i++){
		if(keyBuffer[i] != 0){
			InputKey[i]++;
		}else{
			InputKey[i] = 0;
		}
	}

}

int Key(int Code){
	return InputKey[Code];
}


//�ȉ��A�J�����Ɋւ��鏈��

void CameraUpdata(){

	//��ʂ̒��S���v�Z
	int wndCenterX = WINDOW_WIDHT/2;
	int wndCenterY = WINDOW_HEIGHT/2;


	//�L�����N�^�[�̎��̂̍��W����ʒ��S�����������Ȃ�
	if(ch.x < wndCenterX){
		//�J�������W�ɃL�����N�^�[�̎��̍��W��������
		CameraPositionX = ch.x;
	}else{
		//�L�����N�^�[�̎��̍��W���}�b�v�̉���(40chip * 100px) - ���S���W�����������W�����傫�����
		if(ch.x > MAP_WIDTH - wndCenterX){
			//�J�������W�ɃL�����N�^�[���̍��W����ʃT�C�Y�ɕϊ��������W����
			CameraPositionX =  ch.x % WINDOW_WIDHT;
		}else{
			//�J�����ɉ�ʂ̒��S���W����
			CameraPositionX = wndCenterX;
		}
	}

	//�L�����N�^�[�̎��̂̍��W����ʒ��S�����������Ȃ�
	if(ch.y < wndCenterY){
		//�J�������W�ɃL�����N�^�[�̎��̍��W��������
		CameraPositionY = ch.y;
	}else{
		//�L�����N�^�[�̎��̍��W���}�b�v�̏c��(40chip * 100px) - ���S���W�����������W�����傫�����
		if(ch.y > MAP_HEIGHT - wndCenterY){
			//�J�������W�ɃL�����N�^�[���̍��W����ʃT�C�Y�ɕϊ��������W����
			CameraPositionY =  ch.y % WINDOW_HEIGHT + MAP_CHIP_SIZE*2;
		}else{
			//�J�����ɉ�ʂ̒��S���W����
			CameraPositionY = wndCenterY;
		}
	}

}

/////////////////////////////////////////////////�ȉ��A�}�b�v�Ɋւ��鏈��

//�G�̐ݒu�ƃL�����N�^�[�̐ݒu
void SetCharactor(){

	SetEnemyNum = 0;

	for (int i = 0; i < MAP_CHIP_YNUM; i++) {
		for (int j = 0; j < MAP_CHIP_XNUM; j++) {
			switch(mMap.map[i][j].mapChipKind[3]){
			case eMapGimic_Enemy1:

				enemy[SetEnemyNum].kind = 0;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			case eMapGimic_Enemy2:

				enemy[SetEnemyNum].kind = 1;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			case eMapGimic_Enemy3:

				enemy[SetEnemyNum].kind = 2;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			case eMapGimic_Enemy4:

				enemy[SetEnemyNum].kind = 3;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			case eMapGimic_Enemy5:

				enemy[SetEnemyNum].kind = 4;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			case eMapGimic_Enemy6:

				enemy[SetEnemyNum].kind = 5;
				enemy[SetEnemyNum].x = j * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].y = i * MAP_CHIP_SIZE;
				enemy[SetEnemyNum].width = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].height = MAP_CHIP_SIZE;
				enemy[SetEnemyNum].live = 1;

				mMap.map[i][j].mapChipKind[3] = -1;

				SetEnemyNum++;
				break;
			}
		}
	}

}


//�}�b�v�̃f�[�^��ǂݍ���
bool MapDataLoad(const char* FileName) {

	FILE* fp;

	if (fopen_s(&fp,FileName, "r") != NULL) {
		return false;
	}

	//�}�b�v�̃^�C�g���ǂݍ���
	fgets(mMap.mapName,33,fp);

	//���s���Ȃ���
	for(int i = 0;i < 33;i++){
		if(mMap.mapName[i] == '\n'){
			mMap.mapName[i] = '\0';
			break;
		}
	}

	//�}�b�v�̐�����ǂݍ���
	fgets(mMap.mapDescription,1024,fp);

	//���s���Ȃ���
	for(int i = 0;i < 1024;i++){
		if(mMap.mapDescription[i] == '\n'){
			mMap.mapDescription[i] = '\0';
			break;
		}
	}

	//�o������M�~�b�N�f�[�^�̓ǂݍ���
	fscanf_s(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &mMap.gimicVisibleKind[0], &mMap.gimicVisibleKind[1], &mMap.gimicVisibleKind[2], &mMap.gimicVisibleKind[3], &mMap.gimicVisibleKind[4],
		&mMap.gimicVisibleKind[5], &mMap.gimicVisibleKind[6], &mMap.gimicVisibleKind[7], &mMap.gimicVisibleKind[8], &mMap.gimicVisibleKind[9]);


	//�}�b�v�f�[�^�ǂݍ���
	/*for (int i = 0; i < MAP_CHIP_YNUM; i++) {
		for (int j = 0; j < MAP_CHIP_XNUM; j++) {
			fscanf_s(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d",
				&mMap.map[j][i].mapChipKind[0], &mMap.map[j][i].mapChipKind[1], &mMap.map[j][i].mapChipKind[2], &mMap.map[j][i].mapChipKind[3],
				&mMap.map[j][i].passible, &mMap.map[j][i].gimicKind, &mMap.map[j][i].enemyKind, &mMap.map[j][i].itemKind, &mMap.map[j][i].setPlayer);
		}
	}*/

	for (int i = 0; i < MAP_CHIP_YNUM; i++) {
		for (int j = 0; j < MAP_CHIP_XNUM; j++) {
			fscanf_s(fp, "%d,%d,%d,%d,%d",
				&mMap.map[j][i].mapChipKind[0], &mMap.map[j][i].mapChipKind[1], &mMap.map[j][i].mapChipKind[2], &mMap.map[j][i].mapChipKind[3],
				&mMap.map[j][i].passible);
		}
	}

	fclose(fp);


	return true;
}


//�ȉ��A�}�b�v�M�~�b�N�Ɋւ��鏈��

void Warp(int VecX,int VecY,int MapLayer){

	static int y = 0;
	static int x = 0;
	static int k = 0;

	for(y = 0;y < MAP_CHIP_YNUM;y++){
		for(x = 0;x < MAP_CHIP_XNUM;x++){
			if(VecX == x && VecY == y) continue;
				for(k = 0;k < MAP_LAYER_NUM;k++){
					if(mMap.map[y][x].mapChipKind[k] != eMapGimic_None){
						if(mMap.map[y][x].mapChipKind[k] == mMap.map[VecY][VecX].mapChipKind[MapLayer]){
							ch.x = (x * MAP_CHIP_SIZE);
							ch.y = (y * MAP_CHIP_SIZE);
							break;
						}
					}
				}
		}
	}

}

//�ȉ��A�}�b�v�Ɋւ��鏈��

void DrawMap(int playerX,int playerY){
	
	//��ʂɕ`�悷��}�b�v�`�b�v�̐�
	int screanDrawMapChipX = WINDOW_WIDHT/MAP_CHIP_SIZE + 1;
	int screanDrawMapChipY = WINDOW_HEIGHT/MAP_CHIP_SIZE+ 1;

	//�v���C���[������}�b�v�`�b�v�̏ꏊ
	int playerChipPointX = (playerX - CameraPositionX) / MAP_CHIP_SIZE;
	int playerChipPointY = (playerY - CameraPositionY) / MAP_CHIP_SIZE;

	//�`�悷��`�b�v�̔ԍ������߂�
	int drawMapChipX = (playerX - CameraPositionX) % MAP_CHIP_SIZE;
	int drawMapChipY = (playerY - CameraPositionY) % MAP_CHIP_SIZE;

	for(int i = 0;i < screanDrawMapChipY;i++){
		for(int j = 0;j < screanDrawMapChipX;j++){
			//�}�b�v�`�b�v�̈ʒu���W
			int chipPointX = j + playerChipPointX;
			int chipPointY = i + playerChipPointY;

			if(chipPointX >= 0 && chipPointX < MAP_CHIP_XNUM &&
				chipPointY >= 0 && chipPointY < MAP_CHIP_YNUM){
			
				//�摜�`���������W
				int drawX = (j * MAP_CHIP_SIZE) - drawMapChipX;
				int drawY = (i * MAP_CHIP_SIZE) - drawMapChipY;

				for(int k = 0;k < MAP_LAYER_NUM;k++){
					if(mMap.map[chipPointY][chipPointX].mapChipKind[k] != -1){
						DrawGraph(drawX,drawY,MapChipImage[mMap.map[chipPointY][chipPointX].mapChipKind[k]],TRUE);
					}
				}
				//�}�b�v�`�b�v�̍��W��`��
				DrawFormatString(drawX,drawY,GetColor(255,255,255),"%d:%d",chipPointX,chipPointY);
			}
			
		}
	}
	
}

//�}�b�v�`�b�v�̔���
void MapChipGimicEffect(int VecX,int VecY){

	const int WARP_KIND_NUM = 5;	//���[�v�M�~�b�N�̎��
	const int RECYCLE_TIME = 60;	//�ēx�g�p�̂ɂȂ�܂ł̎���

	static bool isUse[WARP_KIND_NUM] = {false,false,false,false,false};
	static int useGimicCnt[WARP_KIND_NUM] = {0,0,0,0,0};

	static bool isHoleUse = false;

	static bool isRotetBom = false;
	static int rotetCnt = 0;

	clsDx();
	for(int i = 0;i < MAP_LAYER_NUM;i++){
		
		printfDx("�}�b�v�`�b�v�ԍ�:l:%d m:%d\n",i,mMap.map[VecY][VecX].mapChipKind[i]);
		
		//�}�b�v�`�b�v�����݂��Ȃ���Ώ������΂�
		if(mMap.map[VecY][VecX].mapChipKind[i] == eMapGimic_None) continue;

		

		switch(mMap.map[VecY][VecX].mapChipKind[i]){
		case eMapGimic_Mud:		//�D

			break;
		case eMapGimic_Soil://�y

										//�̗͂�����������
			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;


			break;
		case eMapGimic_Stone://��

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Bone://��

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Rock://��

			break;
		case eMapGimic_MoistSoil://�������y

			break;
		case eMapGimic_Turf://��

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Flower://��

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_TurfBone://�ł̍�
			
			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Wood://��

			break;
		case eMapGimic_IceSoil://�X�y

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = 0;

			break;
		case eMapGimic_Ice://�X

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceFlower://�X��

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceBone://�X��

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceRock://�X��

			break;
		case eMapGimic_SandSoil://���y

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = 0;

			break;
		case eMapGimic_Sand://��

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Sandpile://���R

			ch.hp-=3;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_SandBone://����

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Cactus://�T�{�e��
	 
			break;
		case eMapGimic_StonePaveMentSoil://�Ώ��y

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = eMapGimic_Mud;

			break;
		case eMapGimic_StonePavement://�Ώ�

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_StonePavementSotne://�Ώ���

			ch.hp-=3;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_StonePavementBone://�Ώ�

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_rubble://���I
	
			break;
		//���j ��
		case eMapGimic_BomYellowUp:

			//

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX+1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX  ].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomYellowDowm:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX-1].passible = 0;	//�����̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-1][VecX  ].passible = 0;	//�㑤�̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomYellowLeft:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX+1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX  ].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomYellowRight:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX-1].passible = 0;	//�����̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX  ].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		//���j ��
		case eMapGimic_BomRedUp:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX+1].passible = 0;	//��E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-1][VecX  ].passible = 0;	//�㑤�̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomRedDowm:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY+1][VecX-1].passible = 0;	//�㍶���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX  ].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY+1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomRedLeft:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX-1].passible = 0;	//�����̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-1][VecX-1].passible = 0;	//�������̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomRedRight:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY  ][VecX+1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX+1].passible = 0;	//�E�����̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

	
			break;
		//���j ��
		case eMapGimic_BomBuleUp:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX-1].passible = 0;	//�㍶���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-1][VecX+1].passible = 0;	//��E���̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomBuleDowm:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY+1][VecX-1].passible = 0;	//�������̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX+1].passible = 0;	//���E���̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY+1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomBuleLeft:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX-1].passible = 0;	//�㍶���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX-1].passible = 0;	//�������̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX-1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		case eMapGimic_BomBuleRight:
	 
			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX+1].passible = 0;	//��E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX+1].passible = 0;	//���E���̃}�X��ʍs�\�ɂ���

		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		 //���j ��
		case eMapGimic_BomWhiteUp:


			//�L�����N�^�[�̐F�����łȂ����
			

			for(int y = VecY-1;y > 0;y--){
				//�}�b�v�`�b�v�����݂��Ă���Ȃ�
				if(mMap.map[y][VecX].mapChipKind[0] != eMapGimic_None){
					//�ʍs�s�\�܂��ł����
					if(mMap.map[y][VecX].passible == 1){
						//�ʍs�\�ɂ���
						mMap.map[y][VecX].passible = 0;

						//�}�b�v�`�b�v��ݒu���� 0 ~ 9:�D�@10 ~ 19:�y�@20 ~ 29:�� 30 ~ 39:��
						mMap.map[y][VecX].mapChipKind[0] = eMapGimic_Mud + (((VecY-y)/10)%3);

						//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[y][VecX].mapChipKind[i] > eMapGimic_Mud && mMap.map[y][VecX].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[y][VecX].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//�ʍs�\�}�X���������甲����
					else{
						break;
					}
				}
				//�}�b�v�`�b�v�����݂��Ă��Ȃ��܂��ł���Ȃ甲����
				else{
					break;
				}
			}


			break;
		case eMapGimic_BomWhiteDowm:

			//�L�����N�^�[�̐F�����łȂ����
			

			for(int y = VecY+1;y < MAP_CHIP_YNUM;y++){
				//�}�b�v�`�b�v�����݂��Ă���Ȃ�
				if(mMap.map[y][VecX].mapChipKind[0] != eMapGimic_None){
					//�ʍs�s�\�܂��ł����
					if(mMap.map[y][VecX].passible == 1){
						//�ʍs�\�ɂ���
						mMap.map[y][VecX].passible = 0;

						//�}�b�v�`�b�v��ݒu���� 0 ~ 9:�D�@10 ~ 19:�y�@20 ~ 29:�� 30 ~ 39:��
						mMap.map[y][VecX].mapChipKind[0] = eMapGimic_Mud + (((y-VecY)/10)%3);

						//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[y][VecX].mapChipKind[i] > eMapGimic_Mud && mMap.map[y][VecX].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[y][VecX].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//�ʍs�\�}�X���������甲����
					else{
						break;
					}
				}
				//�}�b�v�`�b�v�����݂��Ă��Ȃ��܂��ł���Ȃ甲����
				else{
					break;
				}
			}



			break;
		case eMapGimic_BomWhiteLeft:

			//�L�����N�^�[�̐F�����łȂ����
			

			for(int x = VecX-1;x > 0;x--){
				//�}�b�v�`�b�v�����݂��Ă���Ȃ�
				if(mMap.map[VecY][x].mapChipKind[0] != eMapGimic_None){
					//�ʍs�s�\�܂��ł����
					if(mMap.map[VecY][x].passible == 1){
						//�ʍs�\�ɂ���
						mMap.map[VecY][x].passible = 0;

						//�}�b�v�`�b�v��ݒu���� 0 ~ 9:�D�@10 ~ 19:�y�@20 ~ 29:�� 30 ~ 39:��
						mMap.map[VecY][x].mapChipKind[0] = eMapGimic_Mud + (((VecX-x)/10)%3);


						//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[VecY][x].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY][x].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[VecY][x].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//�ʍs�\�}�X���������甲����
					else{
						break;
					}
				}
				//�}�b�v�`�b�v�����݂��Ă��Ȃ��܂��ł���Ȃ甲����
				else{
					break;
				}
			}

			break;
		case eMapGimic_BomWhiteRight:

			//�L�����N�^�[�̐F�����łȂ����
			

			for(int x = VecX+1;x < MAP_CHIP_XNUM;x++){
				//�}�b�v�`�b�v�����݂��Ă���Ȃ�
				if(mMap.map[VecY][x].mapChipKind[0] != eMapGimic_None){
					//�ʍs�s�\�܂��ł����
					if(mMap.map[VecY][x].passible == 1){
						//�ʍs�\�ɂ���
						mMap.map[VecY][x].passible = 0;


						//�}�b�v�`�b�v��ݒu���� 0 ~ 9:�D�@10 ~ 19:�y�@20 ~ 29:�� 30 ~ 39:��
						mMap.map[VecY][x].mapChipKind[0] = eMapGimic_Mud + (((x-VecX)/10)%3);

						//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[VecY][x].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY][x].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[VecY][x].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//�ʍs�\�}�X���������甲����
					else{
						break;
					}
				}
				//�}�b�v�`�b�v�����݂��Ă��Ȃ��܂��ł���Ȃ甲����
				else{
					break;
				}
			}

			break;
		//���j ��
		case eMapGimic_BomBlackUp:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-2][VecX-1].passible = 0;	//�㍶���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-2][VecX  ].passible = 0;	//�㑤�̃}�X��ʍs�\�ɂ���
			mMap.map[VecY-2][VecX+1].passible = 0;	//��E���̃}�X��ʍs�\�ɂ���
		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-2][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-2][VecX  ].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-2][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-2][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-2][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-2][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-2][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-2][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-2][VecX  ].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY-2][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-2][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-2][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
			}

			break;
		case eMapGimic_BomBlackDowm:

			
			

			//�ʍs�\�ɂ���
			mMap.map[VecY+2][VecX-1].passible = 0;	//�������̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+2][VecX  ].passible = 0;	//�����̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+2][VecX+1].passible = 0;	//���E���̃}�X��ʍs�\�ɂ���
		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY+2][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+2][VecX  ].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+2][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY+2][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+2][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+2][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+2][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+2][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+2][VecX  ].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+2][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+2][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+2][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
			}

			break;
		case eMapGimic_BomBlackLeft:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX-2].passible = 0;	//�㍶���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY  ][VecX-2].passible = 0;	//�����̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX-2].passible = 0;	//�������̃}�X��ʍs�\�ɂ���
		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX-2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY  ][VecX-2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX-2].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX-2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX-2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX-2].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY  ][VecX-2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX-2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX-2].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX-2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX-2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX-2].mapChipKind[i] = eMapGimic_None;
				}
			}

			break;
		case eMapGimic_BomBlackRight:

			

			//�ʍs�\�ɂ���
			mMap.map[VecY-1][VecX+2].passible = 0;	//��E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY  ][VecX+2].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
			mMap.map[VecY+1][VecX+2].passible = 0;	//���E���̃}�X��ʍs�\�ɂ���
		
			//�D�̃}�b�v�`�b�v��ݒu����
			mMap.map[VecY-1][VecX+2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY  ][VecX+2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+2].mapChipKind[0] = eMapGimic_Mud;

			//�ʍs�\�ɂ����}�X�̕s�K�v�ȃ}�X���폜����
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX+2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX+2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX+2].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY  ][VecX+2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX+2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX+2].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+2].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+2].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+2].mapChipKind[i] = eMapGimic_None;
				}
			}

			break;
	
		//����
		case eMapGimic_DoorYellow:

			break;
		case eMapGimic_DoorRed:

			break;
		case eMapGimic_DoorBule:

			break;
		case eMapGimic_DoorWhite:

			break;
		case eMapGimic_DoorBlack:
	
			break;
		//�y�C���g
		case eMapGimic_PaintYellow:

			ch.colorKind = eColorKind_Yellow;

			break;
		case eMapGimic_PaintRed:

			ch.colorKind = eColorKind_Red;

			break;
		case eMapGimic_PaintBule:

			ch.colorKind = eColorKind_Blue;

			break;
		case eMapGimic_PaintWhite:

			ch.colorKind = eColorKind_White;

			break;
		case eMapGimic_PaintBlack:
	
			ch.colorKind = eColorKind_Black;

			break;
		//���[�v
		case eMapGimic_WarpYellow:

			if(!isUse[0]){
	
				Warp(VecX,VecY,i);
				isUse[0] = true;

			}

			break;
		case eMapGimic_WarpRed:

			if(!isUse[1]){
				Warp(VecX,VecY,i);
				isUse[1] = true;
			}

			break;
		case eMapGimic_WarpBlue:

			if(!isUse[2]){
				Warp(VecX,VecY,i);
				isUse[2] = true;
			}

			break;
		case eMapGimic_WarpWhite:

			if(!isUse[3]){
				Warp(VecX,VecY,i);
				isUse[3] = true;
			}

			break;
		case eMapGimic_WarpBlack:
	
			if(!isUse[4]){
				Warp(VecX,VecY,i);
				isUse[4] = true;
			}

			break;
		//���]
		case eMapGimic_BomRotat:
			

			
			if(!isRotetBom){
				for(int i = 0;i < MAP_CHIP_YNUM;i++){
					for(int j = 0;j < MAP_CHIP_XNUM;j++){
						for(int k = 0;k < MAP_LAYER_NUM;k++){
							if(mMap.map[i][j].mapChipKind[k] != eMapGimic_None){
								if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomYellowUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomYellowRight){

									mMap.map[i][j].mapChipKind[k] = eMapGimic_BomYellowUp + (mMap.map[i][j].mapChipKind[k] + 1) % 4;

								}if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomRedUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomRedRight){

									mMap.map[i][j].mapChipKind[k] = eMapGimic_BomRedUp + ((mMap.map[i][j].mapChipKind[k]+5) + 1) % 4;

								}if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomBuleUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomBuleRight){
							
									mMap.map[i][j].mapChipKind[k] = eMapGimic_BomBuleUp + (mMap.map[i][j].mapChipKind[k] + 1) % 4;
							
								}if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomWhiteUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomWhiteRight){
							
									mMap.map[i][j].mapChipKind[k] = eMapGimic_BomWhiteUp + ((mMap.map[i][j].mapChipKind[k]+5) + 1) % 4;
							
								}if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomBlackUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomBlackRight){

									mMap.map[i][j].mapChipKind[k] = eMapGimic_BomBlackUp + (mMap.map[i][j].mapChipKind[k] + 1) % 4;
							
								}
							}
						}
					}
				}
				isRotetBom = true;
			}

			break;
		//�
		case eMapGimic_Trap:
	 
			break;
		 //���Ƃ���
		case eMapGimic_Pitfall:
			
			isHoleUse = false;

			while(!isHoleUse){
				int setX = GetRand(MAP_CHIP_XNUM-1);
				int setY = GetRand(MAP_CHIP_YNUM-1);

				for(int i = 0;i < MAP_LAYER_NUM;i++){
					if(mMap.map[setY][setX].mapChipKind[i] != eMapGimic_None &&
						(mMap.map[setY][setX].mapChipKind[i] >= eMapGimic_Mud && mMap.map[setY][setX].mapChipKind[i] <= eMapGimic_rubble)){
							if(mMap.map[setY][setX].passible == 0){
								ch.x = setX * MAP_CHIP_SIZE;
								ch.y = setY * MAP_CHIP_SIZE;
								isHoleUse = true;
								break;
							}
					}
				}


			}


			break;
		 //�v���C���[�����ʒu
		case eMapGimic_SetPlayerPosition:
	
			break;
		//�S�[��
		case eMapGimic_Goal:
			isGoal = true;
			break;
		//��
		case eMapGimic_KeyYellow:

			for(int j = 0;j < HAVE_ITEM_NUM;j++){
				if(ch.item[j].id == 0){
					ch.item[j].kind = eMapGimic_KeyYellow;
					ch.item[j].id = i + 1;
					mMap.map[VecY][VecX].mapChipKind[i] = eMapGimic_None;
					break;
				}
			}

			break;
		case eMapGimic_KeyRed:

			break;
		case eMapGimic_KeyBlue:

			break;
		case eMapGimic_KeyWhite:

			break;
		case eMapGimic_KeyBlack:
 
			break;
		//�񕜂̐�
		case eMapGimic_Water:
			
			//�擾�����̂ŏ���
			mMap.map[VecY][VecX].mapChipKind[i] = eMapGimic_None;

			ch.hp += 50;

			if(ch.hp > MAX_PLAYER_HP){
				ch.hp = MAX_PLAYER_HP;
			}



			break;
		//���v
		case eMapGimic_Clock:
	 
			break;
		 //��
		case eMapGimic_Paper:
		
			break;
		 //��
		case eMapGimic_Treasure:
	 
			break;
		 //??(�j��\�ʍs�s�\�`�b�v??)
	 
		 //??(�j��s�\�ʍs�s�\�`�b�v??)
	 
		 //�G�l�~�[
		case eMapGimic_Enemy1:

			break;
		case eMapGimic_Enemy2:

			break;
		case eMapGimic_Enemy3:
				
			break;
		case eMapGimic_Enemy4:

			break;
		case eMapGimic_Enemy5:
		
			break;
		case eMapGimic_Enemy6:

			break;
		}
		
	}

	for(int i = 0;i < WARP_KIND_NUM;i++){
		if(isUse[i]){
			if(useGimicCnt[i] < RECYCLE_TIME){
				useGimicCnt[i]++;
			}else{
				useGimicCnt[i] = 0;
				isUse[i] = false;
			}
		}
	}

	if(isRotetBom){
		if(rotetCnt < 60){
			rotetCnt++;
		}else{
			isRotetBom = false;
			rotetCnt = 0;
		}
	}

	if(ch.colorKind != eColorKind_Nomal){
		for(int i = 0;i < MAP_CHIP_YNUM;i++){
			for(int j = 0;j < MAP_CHIP_XNUM;j++){
				for(int k = 0;k < MAP_LAYER_NUM;k++){
					if(mMap.map[i][j].mapChipKind[k] != eMapGimic_None){
						if(mMap.map[i][j].mapChipKind[k] >= eMapGimic_BomYellowUp && mMap.map[i][j].mapChipKind[k] <= eMapGimic_BomBlackRight){
							mMap.map[i][j].mapChipKind[k] = eMapGimic_BomYellowUp + (mMap.map[i][j].mapChipKind[k]%5) + (5 * ch.colorKind);
						}
					}
				}
			}
		}
	}else{
		//���̔z�F�ɖ߂�
	}

}

void GimicEffect(int VecX,int VecY){
	
	static bool isUse = false;

	switch(mMap.map[VecY][VecX].gimicKind){
	case eMapGimic_WarpYellow:	//���[�v���F

		if(!isUse){
			for(int i = 0;i < MAP_CHIP_YNUM;i++){
				for(int j = 0;j < MAP_CHIP_XNUM;j++){
					if(VecX != j && VecY != i){
						if(mMap.map[i][j].gimicKind == mMap.map[VecY][VecX].gimicKind){
							ch.x = j * MAP_CHIP_SIZE;
							ch.y = i * MAP_CHIP_SIZE;
							break;
						}
					}
				}
			}
			isUse = true;
		}

		break;
	case eMapGimic_WarpRed:	//���[�v��
		if(!isUse){
			for(int i = 0;i < MAP_CHIP_YNUM;i++){
				for(int j = 0;j < MAP_CHIP_XNUM;j++){
					if(VecX != j && VecY != i){
						if(mMap.map[i][j].gimicKind == mMap.map[VecY][VecX].gimicKind){
							ch.x = j * MAP_CHIP_SIZE;
							ch.y = i * MAP_CHIP_SIZE;
							break;
						}
					}
				}
			}
			isUse = true;
		}
		break;
	case eMapGimic_WarpBlue:	//���[�v��
		if(!isUse){
			for(int i = 0;i < MAP_CHIP_YNUM;i++){
				for(int j = 0;j < MAP_CHIP_XNUM;j++){
					if(VecX != j && VecY != i){
						if(mMap.map[i][j].gimicKind == mMap.map[VecY][VecX].gimicKind){
							ch.x = j * MAP_CHIP_SIZE;
							ch.y = i * MAP_CHIP_SIZE;
							break;
						}
					}
				}
			}
			isUse = true;
		}
		break;
	case eMapGimic_WarpWhite:	//���[�v��
		if(!isUse){
			for(int i = 0;i < MAP_CHIP_YNUM;i++){
				for(int j = 0;j < MAP_CHIP_XNUM;j++){
					if(VecX != j && VecY != i){
						if(mMap.map[i][j].gimicKind == mMap.map[VecY][VecX].gimicKind){
							ch.x = j * MAP_CHIP_SIZE;
							ch.y = i * MAP_CHIP_SIZE;
							break;
						}
					}
				}
			}
			isUse = true;
		}
		break;
	case eMapGimic_WarpBlack:	//���[�v��
		if(!isUse){
			for(int i = 0;i < MAP_CHIP_YNUM;i++){
				for(int j = 0;j < MAP_CHIP_XNUM;j++){
					if(VecX != j && VecY != i){
						if(mMap.map[i][j].gimicKind == mMap.map[VecY][VecX].gimicKind){
							ch.x = j * MAP_CHIP_SIZE;
							ch.y = i * MAP_CHIP_SIZE;
							break;
						}
					}
				}
			}
			isUse = true;
		}
		break;
	case eMapGimic_BomYellowUp:

		//�L�����N�^�[�̐F�����F�łȂ���Ώ����𔲂���
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//�ʍs�\�ɂ���
		mMap.map[VecY][VecX+1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
		mMap.map[VecY+1][VecX].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
		//�}�b�v�`�b�v�ԍ�����ł���Ȃ�D�ɂ���
		if(mMap.map[VecY+1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY+1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX+1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX+1].mapChipKind[0] = 0;
		}

		//�}�b�v�`�b�v���폜����

		for(int i =	1;i < MAP_LAYER_NUM;i++){
			if(mMap.map[VecY+1][VecX].gimicKind == -1 &&
				mMap.map[VecY+1][VecX].itemKind == -1 &&
				mMap.map[VecY+1][VecX].enemyKind == -1){
					mMap.map[VecY+1][VecX].mapChipKind[i] = -1;
		
			}
			if(mMap.map[VecY][VecX+1].gimicKind == -1 &&
				mMap.map[VecY][VecX+1].itemKind == -1 &&
				mMap.map[VecY][VecX+1].enemyKind == -1){
					mMap.map[VecY][VecX+1].mapChipKind[i] = -1;
		
			}
		}

		break;
	case eMapGimic_BomYellowDowm:

		//�L�����N�^�[�̐F�����F�łȂ���Ώ����𔲂���
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//�ʍs�\�ɂ���
		mMap.map[VecY][VecX-1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
		mMap.map[VecY-1][VecX].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
		//�}�b�v�`�b�v�ԍ�����ł���Ȃ�D�ɂ���
		if(mMap.map[VecY-1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY-1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}

		//�}�b�v�`�b�v���폜����

		for(int i =	1;i < MAP_LAYER_NUM;i++){
			if(mMap.map[VecY-1][VecX].gimicKind == -1 &&
				mMap.map[VecY-1][VecX].itemKind == -1 &&
				mMap.map[VecY-1][VecX].enemyKind == -1){
					mMap.map[VecY-1][VecX].mapChipKind[i] = -1;
		
			}
			if(mMap.map[VecY][VecX-1].gimicKind == -1 &&
				mMap.map[VecY][VecX-1].itemKind == -1 &&
				mMap.map[VecY][VecX-1].enemyKind == -1){
					mMap.map[VecY][VecX-1].mapChipKind[i] = -1;
		
			}
		}


		break;
	case eMapGimic_BomYellowLeft:

		//�L�����N�^�[�̐F�����F�łȂ���Ώ����𔲂���
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//�ʍs�\�ɂ���
		mMap.map[VecY][VecX-1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
		mMap.map[VecY+1][VecX].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
		//�}�b�v�`�b�v�ԍ�����ł���Ȃ�D�ɂ���
		if(mMap.map[VecY+1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY+1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}

		//�}�b�v�`�b�v���폜����

		for(int i =	1;i < MAP_LAYER_NUM;i++){
			if(mMap.map[VecY+1][VecX].gimicKind == -1 &&
				mMap.map[VecY+1][VecX].itemKind == -1 &&
				mMap.map[VecY+1][VecX].enemyKind == -1){
					mMap.map[VecY+1][VecX].mapChipKind[i] = -1;
		
			}
			if(mMap.map[VecY][VecX-1].gimicKind == -1 &&
				mMap.map[VecY][VecX-1].itemKind == -1 &&
				mMap.map[VecY][VecX-1].enemyKind == -1){
					mMap.map[VecY][VecX-1].mapChipKind[i] = -1;
		
			}
		}

		break;
	case eMapGimic_BomYellowRight:

		//�L�����N�^�[�̐F�����F�łȂ���Ώ����𔲂���
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//�ʍs�\�ɂ���
		mMap.map[VecY][VecX+1].passible = 0;	//�E���̃}�X��ʍs�\�ɂ���
		mMap.map[VecY][VecX-1].passible = 0;	//�����̃}�X��ʍs�\�ɂ���

		
		//�}�b�v�`�b�v�ԍ�����ł���Ȃ�D�ɂ���
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX+1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX+1].mapChipKind[0] = 0;
		}

		//�}�b�v�`�b�v���폜����

		for(int i =	1;i < MAP_LAYER_NUM;i++){
			if(mMap.map[VecY][VecX-1].gimicKind == -1 &&
				mMap.map[VecY][VecX-1].itemKind == -1 &&
				mMap.map[VecY][VecX-1].enemyKind == -1){
					mMap.map[VecY][VecX-1].mapChipKind[i] = -1;
		
			}
			if(mMap.map[VecY][VecX+1].gimicKind == -1 &&
				mMap.map[VecY][VecX+1].itemKind == -1 &&
				mMap.map[VecY][VecX+1].enemyKind == -1){
					mMap.map[VecY][VecX+1].mapChipKind[i] = -1;
		
			}
		}

		break;
	case eMapGimic_Goal:	//�S�[��
		isGoal = true;
		break;
	default:
		isUse = false;
		break;
	}


}

//�A�C�e���̌���
void ItemEffect(int VecX,int VecY){

	static bool isUse  = false;

	switch(mMap.map[VecY][VecX].itemKind){
	case 5:
		ch.hp += 50;
		if(ch.hp > MAX_PLAYER_HP){
			ch.hp = MAX_PLAYER_HP;
		}
		isUse = true;
		break;
	}

	if(isUse == true){
		mMap.map[VecY][VecX].itemKind = -1;
		mMap.map[VecY][VecX].mapChipKind[1] = -1;
		mMap.map[VecY][VecX].mapChipKind[2] = -1;
		mMap.map[VecY][VecX].mapChipKind[3] = -1;
		isUse = false;
	}
}

bool IsPassibleMap(int VecX,int VecY){

	//�s��Ƀ}�b�v�`�b�v���Ȃ����
	if(mMap.map[VecY][VecX].mapChipKind[0] == -1){
		//�ʍs�s��
		return false;
	}

	//�}�b�v�`�b�v�����݂���͈͊O�ɍs�����Ƃ��Ă���̂Ȃ�
	if(VecX < 0 || VecX >= MAP_CHIP_XNUM ||
		VecY < 0 || VecY >= MAP_CHIP_YNUM ){
			//�ʍs�s��
			return false;
	}

	//�ʍs�s�\
	if(mMap.map[VecY][VecX].passible == 1){
		return false;
	}

	//�ʍs�\
	return true;
}

//�ȉ��A�v���C���[�Ɋւ��鏈��

void PlayerMove(){

	static int x = 0;
	static int y = 0;


	//�L�����N�^�[�̐F�̐؂�ւ�
	if(Key(KEY_INPUT_C) == 1){
		ch.colorKind = (ch.colorKind + 1 ) % CHARACTOR_COLOR_NUM;
	}


	if(ch.hp > 0){

		//�����Ă��Ȃ��Ȃ�@�L�[���͂��󂯕t����
		if(!ch.isWalk){
		
			int vecX = ch.x/MAP_CHIP_SIZE;	//���W�̒l��z��̒l�ɕϊ�
			int vecY = ch.y/MAP_CHIP_SIZE;

			MapChipGimicEffect(vecX,vecY);

			if(CheckHitKey(KEY_INPUT_RIGHT) != 0){
				if(IsPassibleMap(vecX+1,vecY) == true){
					ch.vec = eVector_Right;
					//ch.hp--;

					ch.walkCnt = -1;
					x = ch.x;
					y = ch.y;

				}
			}
			else if(CheckHitKey(KEY_INPUT_UP) != 0){
				if(IsPassibleMap(vecX,vecY-1) == true){
					ch.vec = eVector_Up;
					//ch.hp--;

					ch.walkCnt = -1;
					x = ch.x;
					y = ch.y;

				}
			}
			else if(CheckHitKey(KEY_INPUT_DOWN) != 0){
				if(IsPassibleMap(vecX,vecY+1) == true){
					ch.vec = eVector_Down;
					//ch.hp--;

					ch.walkCnt = -1;
					x = ch.x;
					y = ch.y;

				}
			}
			else if(CheckHitKey(KEY_INPUT_LEFT) != 0){
				if(IsPassibleMap(vecX - 1,vecY) == true){
					ch.vec = eVector_Left;
					//ch.hp--;

					ch.walkCnt = -1;
					x = ch.x;
					y = ch.y;

				}
			}else{
				ch.walkCnt = -1;
				ch.vec = eVector_None;
				ch.isWalk = false;
				x = ch.x;
				y = ch.y;
			}
			
			//ItemEffect(vecX,vecY);
			//GimicEffect(vecX,vecY);

			

		}

		//�ړ��������Ȃ���Ԃł͂Ȃ��Ȃ�
		if(ch.vec != eVector_None){
			//�����t���O�𗧂Ă�
			ch.isWalk = true;
		}
	}
	
	//�����t���O�������Ă���̂Ȃ�
	if(ch.isWalk){


		//�ړ������ɍ��킹�Ĉړ�������
		switch(ch.vec){
		case eVector_Right:

			//�ړ���̍��W��菬�������
			if(ch.x < x + 100){
				ch.x += PLAYER_SPEED;
			}
			//�����łȂ����
			else{
				ch.isWalk = false;
			}


			break;
		case eVector_Up:

			if(ch.y > y - 100){
				ch.y -= PLAYER_SPEED;
			}
			else{
				ch.isWalk = false;
			}

			break;
		case eVector_Down:

			if(ch.y < y + 100){
				ch.y += PLAYER_SPEED;
			}
			else{
				ch.isWalk = false;
			}

			break;
		case eVector_Left:

			if(ch.x > x - 100){
				ch.x -= PLAYER_SPEED;
			}
			else{
				ch.isWalk = false;
			}

			break;
		}
	}
}

void DrawPlayer(){

	//�L�����N�^�[�̕\���ʒu���J�������W�ɐݒ肷��
	int drawX = CameraPositionX;
	int drawY = CameraPositionY;
	
	//�L�����N�^�[�摜�`��
	DrawGraph(drawX,drawY,CharactorImage[ch.colorKind],TRUE);

	//�̗̓Q�[�W�̕`��
	DrawBox(drawX,drawY-10,drawX + MAP_CHIP_SIZE * ch.hp/MAX_PLAYER_HP,drawY-10 + 20,GetColor(100,200,255),TRUE);
	DrawFormatString(drawX,drawY-10,GetColor(0,0,255),"HP:%d",ch.hp);

	//�L�����N�^�[�̎��̍��W
	DrawFormatString(0,0,GetColor(255,255,255),"�L�����N�^�[���̍��W:%d:%d",ch.x,ch.y);

	//�L�����N�^�[�̕`����W
	DrawFormatString(0,20,GetColor(255,255,255),"�L�����N�^�[�`����W:%d:%d",drawX,drawY);


}


void DrawEnemy(){
	
	//�G�l�~�[�̐���0�ł���Ȃ珈���𔲂���
	if(SetEnemyNum == 0) return ;
	

	int mapPosX;
	int mapPosY;

	int drawX;
	int drawY;

	for(int i = 0;i < SetEnemyNum;i++){
		
		mapPosX = enemy[i].x - CameraPositionX;
		mapPosY = enemy[i].y - CameraPositionY;

		drawX = mapPosX + ch.x;
		drawY = mapPosY +ch.y;


		DrawGraph(drawX,drawY,EnemyImage[enemy[i].kind][0],TRUE);

		printfDx("%d:%d\n",drawX,drawY);


	}
	
}

//�ȉ��A�}�b�v�t�@�C�����m�F����

int MapDataNum(){
	
	FILE* fp;

	char fileName[256];
	int num = 0;

	while(1){
		
		sprintf(fileName,MAP_FILENAME,num+1);

		if((fp = fopen(fileName,"r")) != NULL){
			num++;		
			fclose(fp);
		}else{
			break;
		}
	}

	
	return num;
}

////////////////////////////////////////////////////////////////////�ȉ��A���C���Ɋւ��鏈��
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

	SetGraphMode(WINDOW_WIDHT,WINDOW_HEIGHT, 32);		//�E�B���h�E�T�C�Y�ݒ�

	ChangeWindowMode(TRUE);

	//���d�N����������
	SetDoubleStartValidFlag( TRUE );

	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);	//����ʐݒ�

	int Scene = 0;
	int backImage = LoadGraph("data/back.png");

	LoadDivGraph("data/map_chip.png",DIV_SPRITE_NUM,5,20,MAP_CHIP_SIZE,MAP_CHIP_SIZE,MapChipImage);

	//�G�̉摜�ǂݍ���
	LoadDivGraph("data/enemy_chip00.png",12,3,4,100,100,EnemyImage[0]);
	LoadDivGraph("data/enemy_chip01.png",12,3,4,100,100,EnemyImage[1]);
	LoadDivGraph("data/enemy_chip02.png",12,3,4,100,100,EnemyImage[2]);
	LoadDivGraph("data/enemy_chip03.png",12,3,4,100,100,EnemyImage[3]);
	LoadDivGraph("data/enemy_chip04.png",12,3,4,100,100,EnemyImage[4]);
	LoadDivGraph("data/enemy_chip05.png",12,3,4,100,100,EnemyImage[5]);


	for(int i = 0;i < CHARACTOR_COLOR_NUM;i++){
		char file[256];
		sprintf(file,"data/ch%d.png",i);               
		CharactorImage[i] = LoadGraph(file);
	}
	if(!MapDataLoad("data/stg1.csv")){
		MessageBox(NULL,TEXT("�f�[�^������܂���"),TEXT("�x��!"),MB_OK|MB_ICONWARNING);
		DxLib_End();
	}


	SetCharactor();

	//�v���C���[�L�����N�^�[�����ʒu�ݒ�
	for (int i = 0; i < MAP_CHIP_YNUM; i++) {
		for (int j = 0; j < MAP_CHIP_XNUM; j++) {
			for(int k = 0;k < MAP_LAYER_NUM;k++){
				if(mMap.map[i][j].mapChipKind[k] == eMapGimic_SetPlayerPosition){
					ch.x = j * MAP_CHIP_SIZE;
					ch.y = i * MAP_CHIP_SIZE;
					break;
				}
			}
		}
	}

	ch.hp = MAX_PLAYER_HP;

	isGoal = false;

	int stage = 0;
	int oldStage = 99;

	int MaxStageNum = MapDataNum();


	int color[10] = {GetColor(255,0,0),GetColor(255,255,0),GetColor(0,255,0),GetColor(0,255,255),GetColor(0,0,255),
	GetColor(255,0,255),GetColor(255,100,100),GetColor(100,255,100),GetColor(100,100,255),GetColor(255,255,255),};

	char loadMapfile[256];

	// while( ����ʂ�\��ʂɔ��f, ���b�Z�[�W����, ��ʃN���A )
    while( ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){
		

		KeyUpdata();

		DrawGraph(0,0,backImage,TRUE);

		switch(Scene){
		case 0:

			if(Key(KEY_INPUT_RIGHT) == 1){
				stage = (stage + 1) % MaxStageNum;
			}

			if(Key(KEY_INPUT_LEFT) == 1){
				stage = (stage + (MaxStageNum-1)) % MaxStageNum;
			}

			if(oldStage != stage){

				

				sprintf(loadMapfile,MAP_FILENAME,stage+1);

				if(!MapDataLoad(loadMapfile)){
					DrawString(0,100,"None�@Data",GetColor(255,255,255));
				}else{
					//�v���C���[�L�����N�^�[�����ʒu�ݒ�
					for (int i = 0; i < MAP_CHIP_YNUM; i++) {
						for (int j = 0; j < MAP_CHIP_XNUM; j++) {
							for(int k = 0;k < MAP_LAYER_NUM;k++){
								if(mMap.map[i][j].mapChipKind[k] == eMapGimic_SetPlayerPosition){
									ch.x = j * MAP_CHIP_SIZE;
									ch.y = i * MAP_CHIP_SIZE;
									break;
								}
							}
						}
					}

					SetCharactor();

				}



				//�L�����N�^�[�̏�����
				ch.hp = MAX_PLAYER_HP;
				ch.maxhp = ch.hp;
				ch.colorKind = eColorKind_Nomal;			//���F�ɐݒ�

				oldStage = stage;
			}

			if(Key(KEY_INPUT_Z) == 1){

				isGoal = false;

				Scene = 1;
			}
			
			DrawFormatString(0,0,GetColor(255,255,255),"�^�C�g��:%s",mMap.mapName);
			DrawFormatString(0,20,GetColor(255,255,255),"���@�@��:%s",mMap.mapDescription);

			DrawString(0,50,"�o���M�~�b�N",GetColor(255,255,255));

			//�o������M�~�b�N�̕`��
			for(int i = 0;i < 10;i++){
				if(mMap.gimicVisibleKind[i] == 1){
					DrawBox(i * 20,70,(i+1)*20,90,color[i],TRUE);
				}
			}

			DrawFormatString(0,100,GetColor(255,255,255),"1 - %d",stage+1);
			DrawFormatString(0,120,GetColor(255,255,255),"FileName:%s",loadMapfile);

			DrawString(100,200,"Z�L�[�ŃX�e�[�W��",GetColor(255,255,255));
			DrawString(100,220,"���E�L�[�ŃX�e�[�W�ύX",GetColor(255,255,255));


			

			break;
		case 1:
			CameraUpdata();

			if(!isGoal)
			PlayerMove();

			DrawMap(ch.x,ch.y);
			DrawPlayer();
			DrawEnemy();


			if(isGoal){

				static int cnt = 0;

				if(cnt < 60*2){
					SetFontSize(60);
					DrawString(WINDOW_WIDHT/2-200,WINDOW_HEIGHT/2-20,"�X�e�[�W�N���A�I",GetColor(255,255,0));
					SetFontSize(-1);
					cnt++;
				}else{
					cnt = 0;
					isGoal = false;
					Scene = 0;
				}
			}

			DrawString(0,WINDOW_HEIGHT-20,"X�L�[�ŃX�e�[�W�I���ɖ߂�܂�",GetColor(255,255,255));


			if(Key(KEY_INPUT_X) == 1){
				Scene = 0;
			}

			break;
		}

		
	}


	
    // �c�w���C�u�����̌�n��
    DxLib_End() ;

    // �\�t�g�̏I��
    return 0 ;
}