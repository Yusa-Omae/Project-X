#include "DxLib.h"

//ウィンドウサイズのマクロ
#define WINDOW_WIDHT 800
#define WINDOW_HEIGHT 600

//以下、マップのマクロ
#define MAP_CHIP_SIZE 100
#define MAP_CHIP_XNUM 40
#define MAP_CHIP_YNUM 40
#define MAP_LAYER_NUM 4

#define MAP_WIDTH (MAP_CHIP_XNUM * MAP_CHIP_SIZE)
#define MAP_HEIGHT (MAP_CHIP_YNUM * MAP_CHIP_SIZE)

#define DIV_SPRITE_NUM 100

#define MAP_FILENAME "data/stg%d.csv"

//以下、マップチップのマクロ
#define MAP_CHIP_NONE       -1					//マップチップなし
#define MAP_CHIP_BOM_YWLLOW  25					//爆破　黄色
#define MAP_CHIP_BOM_RED	30					//爆破　赤色
#define MAP_CHIP_BOM_BLUE	35					//爆破　青色
#define MAP_CHIP_BOM_WHITE  40					//爆破　白色
#define MAP_CHIP_BOM_BLACK  45					//爆破　黒色
#define MAP_CHIP_DOOR	    55					//鍵ドア
#define MAP_CHIP_PAINT		50					//ペンキ
#define MAP_CHIP_WARP		65					//ワープ
#define MAP_CHIP_SET__PLAYER_POSITION 97		//プレイヤー初期位置
#define MAP_CHIP_GOAL 98						//ゴール
#define MAP_CHIP_KEY 60							//鍵
#define MAP_CHIP_ENEMY 80						//エネミー
#define MAP_CHIP_WATER 75						//水

//以下、キャラクターのマクロ
#define CHARACTOR_COLOR_NUM 6
#define PLAYER_SPEED 5
#define MAX_PLAYER_HP 200
#define HAVE_ITEM_NUM 10

//以下、敵のマクロ
#define ENEMY_NUM 20
#define ENEMY_KIND 6
#define ENEMY_ANIMATION_NUM 12

//移動方向の共用体
enum eVector{

	eVector_Right,
	eVector_Up,
	eVector_Down,
	eVector_Left,

	eVector_None,

};

//色の種類
enum eColorKind{
	eColorKind_Yellow,	//黄色
	eColorKind_Red,		//赤色
	eColorKind_Blue,	//青色
	eColorKind_White,	//白色
	eColorKind_Black,	//黒色


	eColorKind_Nomal,	//通常色


};

enum eMapGimic{

	//通常マップチップ

	eMapGimic_None = MAP_CHIP_NONE,			//何もないチップ

	eMapGimic_Mud,					//泥
	eMapGimic_Soil,					//土
	eMapGimic_Stone,				//石
	eMapGimic_Bone,					//骨
	eMapGimic_Rock,					//岩

	eMapGimic_MoistSoil,			//湿った土
	eMapGimic_Turf,					//芝
	eMapGimic_Flower,				//花
	eMapGimic_TurfBone,				//芝の骨
	eMapGimic_Wood,					//木

	eMapGimic_IceSoil,				//氷土
	eMapGimic_Ice,					//氷
	eMapGimic_IceFlower,			//氷花
	eMapGimic_IceBone,				//氷骨
	eMapGimic_IceRock,				//氷岩

	eMapGimic_SandSoil,				//砂土
	eMapGimic_Sand,					//砂
	eMapGimic_Sandpile,				//砂山
	eMapGimic_SandBone,				//砂骨
	eMapGimic_Cactus,				//サボテン

	eMapGimic_StonePaveMentSoil,	//石畳土
	eMapGimic_StonePavement,		//石畳
	eMapGimic_StonePavementSotne,	//石畳石
	eMapGimic_StonePavementBone,	//石畳骨
	eMapGimic_rubble,				//瓦礫

	//爆破 黄
	eMapGimic_BomYellowUp = MAP_CHIP_BOM_YWLLOW,
	eMapGimic_BomYellowDowm,
	eMapGimic_BomYellowLeft,
	eMapGimic_BomYellowRight,

	//爆破 赤
	eMapGimic_BomRedUp = MAP_CHIP_BOM_RED,
	eMapGimic_BomRedDowm,
	eMapGimic_BomRedLeft,
	eMapGimic_BomRedRight,

	//爆破 青
	eMapGimic_BomBuleUp = MAP_CHIP_BOM_BLUE,
	eMapGimic_BomBuleDowm,
	eMapGimic_BomBuleLeft,
	eMapGimic_BomBuleRight,

	//爆破 白
	eMapGimic_BomWhiteUp = MAP_CHIP_BOM_WHITE,
	eMapGimic_BomWhiteDowm,
	eMapGimic_BomWhiteLeft,
	eMapGimic_BomWhiteRight,

	//爆破 黒
	eMapGimic_BomBlackUp = MAP_CHIP_BOM_BLACK,
	eMapGimic_BomBlackDowm,
	eMapGimic_BomBlackLeft,
	eMapGimic_BomBlackRight,

	//ペイント
	eMapGimic_PaintYellow = MAP_CHIP_PAINT,
	eMapGimic_PaintRed,
	eMapGimic_PaintBule,
	eMapGimic_PaintWhite,
	eMapGimic_PaintBlack,

	//鍵扉
	eMapGimic_DoorYellow = MAP_CHIP_DOOR,
	eMapGimic_DoorRed,
	eMapGimic_DoorBule,
	eMapGimic_DoorWhite,
	eMapGimic_DoorBlack,	

	//鍵
	eMapGimic_KeyYellow = MAP_CHIP_KEY,
	eMapGimic_KeyRed,
	eMapGimic_KeyBlue,
	eMapGimic_KeyWhite,
	eMapGimic_KeyBlack,

	//ワープ
	eMapGimic_WarpYellow = MAP_CHIP_WARP,
	eMapGimic_WarpRed,
	eMapGimic_WarpBlue,
	eMapGimic_WarpWhite,
	eMapGimic_WarpBlack,

	//反転
	eMapGimic_BomRotat,
	eMapGimic_BomRota2,

	//罠
	eMapGimic_Trap,
	
	//落とし穴
	eMapGimic_Pitfall,
	
	//回復の水
	eMapGimic_Water = MAP_CHIP_WATER,

	//時計
	eMapGimic_Clock,

	//紙
	eMapGimic_Paper,

	//宝
	eMapGimic_Treasure,

	//プレイヤー初期位置
	eMapGimic_SetPlayerPosition = MAP_CHIP_SET__PLAYER_POSITION,

	//ゴール
	eMapGimic_Goal = MAP_CHIP_GOAL,


	

	//??(破壊可能通行不可能チップ??)

	//??(破壊不可能通行不可能チップ??)

	//エネミー
	eMapGimic_Enemy1 = MAP_CHIP_ENEMY,
	eMapGimic_Enemy2,
	eMapGimic_Enemy3,
	eMapGimic_Enemy4,
	eMapGimic_Enemy5,
	eMapGimic_Enemy6,

};

typedef struct{
	int id;			//管理番号
	int kind;		//アイテム番号
}Item_t;

//キャラクターの構造体
typedef struct{
	int x,y;			//表示座標
	int width,height;	//サイズ
	int colorKind;		//キャラクターの種類
	int vec;			//方向
	int hp;				//体力
	int maxhp;			//最大HP
	bool isWalk;		//歩いているか?
	int walkCnt;		//歩行カウント

	Item_t item[HAVE_ITEM_NUM];

}Charactor_t;

typedef struct{
	int x,y;				//座標
	int width,height;		//画像サイズ
	int kind;				//画像の種類
	int movePattern;		//行動パターン
	int live;				//生存
}Enemy_t;


//マップのデータを保持する構造体
typedef struct {
	int x, y;							//表示座標
	int mapChipKind[MAP_LAYER_NUM];		//マップチップの種類(レイヤー)
	int gimicKind;						//ギミックの種類
	int itemKind;						//アイテムの種類
	int enemyKind;						//エネミーの種類
	int passible;						//通行可否
	int setPlayer;						//プレイヤーの初期配置場所
}Map_t;

//マップがもつすべてのデータを保持する構造体
typedef struct {
	char mapName[33];				//マップのタイトル
	char mapDescription[1024];		//マップの説明
	int gimicVisibleKind[10];		//存在しているギミックの種類
	Map_t map[MAP_CHIP_YNUM][MAP_CHIP_XNUM];	//マップデータ
}MapMgr_t;


MapMgr_t mMap;
Charactor_t ch;	//キャラクター
Enemy_t enemy[ENEMY_NUM];


int MapChipImage[DIV_SPRITE_NUM];
int CharactorImage[CHARACTOR_COLOR_NUM];

//敵キャラの数
int SetEnemyNum;
int EnemyImage[ENEMY_KIND][ENEMY_ANIMATION_NUM];	//敵の画像を読み込む

int trun;	//経過時間

//実際に描画をする位置
int CameraPositionX;
int CameraPositionY;

//キー入力用変数
int InputKey[256];

//ゴールしたか？
bool isGoal;
//ゲームオーバーか？
bool IsGameOver;

//以下、キー操作に関する処理

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


//以下、カメラに関する処理

void CameraUpdata(){

	//画面の中心を計算
	int wndCenterX = WINDOW_WIDHT/2;
	int wndCenterY = WINDOW_HEIGHT/2;


	//キャラクターの実体の座標が画面中心よりも小さいなら
	if(ch.x < wndCenterX){
		//カメラ座標にキャラクターの実体座標を代入する
		CameraPositionX = ch.x;
	}else{
		//キャラクターの実体座標がマップの横幅(40chip * 100px) - 中心座標を引いた座標よりも大きければ
		if(ch.x > MAP_WIDTH - wndCenterX){
			//カメラ座標にキャラクター実体座標を画面サイズに変換した座標を代入
			CameraPositionX =  ch.x % WINDOW_WIDHT;
		}else{
			//カメラに画面の中心座標を代入
			CameraPositionX = wndCenterX;
		}
	}

	//キャラクターの実体の座標が画面中心よりも小さいなら
	if(ch.y < wndCenterY){
		//カメラ座標にキャラクターの実体座標を代入する
		CameraPositionY = ch.y;
	}else{
		//キャラクターの実体座標がマップの縦幅(40chip * 100px) - 中心座標を引いた座標よりも大きければ
		if(ch.y > MAP_HEIGHT - wndCenterY){
			//カメラ座標にキャラクター実体座標を画面サイズに変換した座標を代入
			CameraPositionY =  ch.y % WINDOW_HEIGHT + MAP_CHIP_SIZE*2;
		}else{
			//カメラに画面の中心座標を代入
			CameraPositionY = wndCenterY;
		}
	}

}

/////////////////////////////////////////////////以下、マップに関する処理

//敵の設置とキャラクターの設置
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


//マップのデータを読み込む
bool MapDataLoad(const char* FileName) {

	FILE* fp;

	if (fopen_s(&fp,FileName, "r") != NULL) {
		return false;
	}

	//マップのタイトル読み込み
	fgets(mMap.mapName,33,fp);

	//改行をなくす
	for(int i = 0;i < 33;i++){
		if(mMap.mapName[i] == '\n'){
			mMap.mapName[i] = '\0';
			break;
		}
	}

	//マップの説明を読み込み
	fgets(mMap.mapDescription,1024,fp);

	//改行をなくす
	for(int i = 0;i < 1024;i++){
		if(mMap.mapDescription[i] == '\n'){
			mMap.mapDescription[i] = '\0';
			break;
		}
	}

	//出現するギミックデータの読み込み
	fscanf_s(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &mMap.gimicVisibleKind[0], &mMap.gimicVisibleKind[1], &mMap.gimicVisibleKind[2], &mMap.gimicVisibleKind[3], &mMap.gimicVisibleKind[4],
		&mMap.gimicVisibleKind[5], &mMap.gimicVisibleKind[6], &mMap.gimicVisibleKind[7], &mMap.gimicVisibleKind[8], &mMap.gimicVisibleKind[9]);


	//マップデータ読み込み
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


//以下、マップギミックに関する処理

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

//以下、マップに関する処理

void DrawMap(int playerX,int playerY){
	
	//画面に描画するマップチップの数
	int screanDrawMapChipX = WINDOW_WIDHT/MAP_CHIP_SIZE + 1;
	int screanDrawMapChipY = WINDOW_HEIGHT/MAP_CHIP_SIZE+ 1;

	//プレイヤーがいるマップチップの場所
	int playerChipPointX = (playerX - CameraPositionX) / MAP_CHIP_SIZE;
	int playerChipPointY = (playerY - CameraPositionY) / MAP_CHIP_SIZE;

	//描画するチップの番号を求める
	int drawMapChipX = (playerX - CameraPositionX) % MAP_CHIP_SIZE;
	int drawMapChipY = (playerY - CameraPositionY) % MAP_CHIP_SIZE;

	for(int i = 0;i < screanDrawMapChipY;i++){
		for(int j = 0;j < screanDrawMapChipX;j++){
			//マップチップの位置座標
			int chipPointX = j + playerChipPointX;
			int chipPointY = i + playerChipPointY;

			if(chipPointX >= 0 && chipPointX < MAP_CHIP_XNUM &&
				chipPointY >= 0 && chipPointY < MAP_CHIP_YNUM){
			
				//画像描画をする座標
				int drawX = (j * MAP_CHIP_SIZE) - drawMapChipX;
				int drawY = (i * MAP_CHIP_SIZE) - drawMapChipY;

				for(int k = 0;k < MAP_LAYER_NUM;k++){
					if(mMap.map[chipPointY][chipPointX].mapChipKind[k] != -1){
						DrawGraph(drawX,drawY,MapChipImage[mMap.map[chipPointY][chipPointX].mapChipKind[k]],TRUE);
					}
				}
				//マップチップの座標を描画
				DrawFormatString(drawX,drawY,GetColor(255,255,255),"%d:%d",chipPointX,chipPointY);
			}
			
		}
	}
	
}

//マップチップの判定
void MapChipGimicEffect(int VecX,int VecY){

	const int WARP_KIND_NUM = 5;	//ワープギミックの種類
	const int RECYCLE_TIME = 60;	//再度使用可のになるまでの時間

	static bool isUse[WARP_KIND_NUM] = {false,false,false,false,false};
	static int useGimicCnt[WARP_KIND_NUM] = {0,0,0,0,0};

	static bool isHoleUse = false;

	static bool isRotetBom = false;
	static int rotetCnt = 0;

	clsDx();
	for(int i = 0;i < MAP_LAYER_NUM;i++){
		
		printfDx("マップチップ番号:l:%d m:%d\n",i,mMap.map[VecY][VecX].mapChipKind[i]);
		
		//マップチップが存在しなければ処理を飛ばす
		if(mMap.map[VecY][VecX].mapChipKind[i] == eMapGimic_None) continue;

		

		switch(mMap.map[VecY][VecX].mapChipKind[i]){
		case eMapGimic_Mud:		//泥

			break;
		case eMapGimic_Soil://土

										//体力を減少させる
			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;


			break;
		case eMapGimic_Stone://石

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Bone://骨

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Rock://岩

			break;
		case eMapGimic_MoistSoil://湿った土

			break;
		case eMapGimic_Turf://芝

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Flower://花

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_TurfBone://芝の骨
			
			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Wood://木

			break;
		case eMapGimic_IceSoil://氷土

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = 0;

			break;
		case eMapGimic_Ice://氷

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceFlower://氷花

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceBone://氷骨

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_IceRock://氷岩

			break;
		case eMapGimic_SandSoil://砂土

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = 0;

			break;
		case eMapGimic_Sand://砂

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Sandpile://砂山

			ch.hp-=3;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_SandBone://砂骨

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_Cactus://サボテン
	 
			break;
		case eMapGimic_StonePaveMentSoil://石畳土

			ch.hp--;
			mMap.map[VecY][VecX].mapChipKind[i] = eMapGimic_Mud;

			break;
		case eMapGimic_StonePavement://石畳

			ch.hp-=2;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_StonePavementSotne://石畳石

			ch.hp-=3;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_StonePavementBone://石畳骨

			ch.hp-=5;
			mMap.map[VecY][VecX].mapChipKind[i]--;

			break;
		case eMapGimic_rubble://瓦礫
	
			break;
		//爆破 黄
		case eMapGimic_BomYellowUp:

			//

			//通行可能にする
			mMap.map[VecY  ][VecX+1].passible = 0;	//右側のマスを通行可能にする
			mMap.map[VecY+1][VecX  ].passible = 0;	//下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY  ][VecX-1].passible = 0;	//左側のマスを通行可能にする
			mMap.map[VecY-1][VecX  ].passible = 0;	//上側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY  ][VecX+1].passible = 0;	//右側のマスを通行可能にする
			mMap.map[VecY+1][VecX  ].passible = 0;	//下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY  ][VecX-1].passible = 0;	//左側のマスを通行可能にする
			mMap.map[VecY+1][VecX  ].passible = 0;	//下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX-1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX-1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX-1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX  ].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX  ].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX  ].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		//爆破 赤
		case eMapGimic_BomRedUp:

			

			//通行可能にする
			mMap.map[VecY-1][VecX+1].passible = 0;	//上右側のマスを通行可能にする
			mMap.map[VecY-1][VecX  ].passible = 0;	//上側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY+1][VecX-1].passible = 0;	//上左側のマスを通行可能にする
			mMap.map[VecY+1][VecX  ].passible = 0;	//下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX  ].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY  ][VecX-1].passible = 0;	//左側のマスを通行可能にする
			mMap.map[VecY-1][VecX-1].passible = 0;	//左下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY  ][VecX+1].passible = 0;	//右側のマスを通行可能にする
			mMap.map[VecY+1][VecX+1].passible = 0;	//右下側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY  ][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY  ][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY  ][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY  ][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

	
			break;
		//爆破 青
		case eMapGimic_BomBuleUp:

			

			//通行可能にする
			mMap.map[VecY-1][VecX-1].passible = 0;	//上左側のマスを通行可能にする
			mMap.map[VecY-1][VecX+1].passible = 0;	//上右側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY+1][VecX-1].passible = 0;	//下左側のマスを通行可能にする
			mMap.map[VecY+1][VecX+1].passible = 0;	//下右側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY-1][VecX-1].passible = 0;	//上左側のマスを通行可能にする
			mMap.map[VecY+1][VecX-1].passible = 0;	//下左側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX-1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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
	 
			

			//通行可能にする
			mMap.map[VecY-1][VecX+1].passible = 0;	//上右側のマスを通行可能にする
			mMap.map[VecY+1][VecX+1].passible = 0;	//下右側のマスを通行可能にする

		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX+1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
			for(int i = 1;i < MAP_LAYER_NUM;i++){
				if(mMap.map[VecY-1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY-1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY-1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}
				if(mMap.map[VecY+1][VecX+1].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY+1][VecX+1].mapChipKind[i] <= eMapGimic_rubble){
					mMap.map[VecY+1][VecX+1].mapChipKind[i] = eMapGimic_None;
				}

			}

			break;
		 //爆破 白
		case eMapGimic_BomWhiteUp:


			//キャラクターの色が白でなければ
			

			for(int y = VecY-1;y > 0;y--){
				//マップチップが存在しているなら
				if(mMap.map[y][VecX].mapChipKind[0] != eMapGimic_None){
					//通行不可能ますであれば
					if(mMap.map[y][VecX].passible == 1){
						//通行可能にする
						mMap.map[y][VecX].passible = 0;

						//マップチップを設置する 0 ~ 9:泥　10 ~ 19:土　20 ~ 29:石 30 ~ 39:骨
						mMap.map[y][VecX].mapChipKind[0] = eMapGimic_Mud + (((VecY-y)/10)%3);

						//通行可能にしたマスの不必要なマスを削除する
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[y][VecX].mapChipKind[i] > eMapGimic_Mud && mMap.map[y][VecX].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[y][VecX].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//通行可能マスを見つけたら抜ける
					else{
						break;
					}
				}
				//マップチップが存在していないますであるなら抜ける
				else{
					break;
				}
			}


			break;
		case eMapGimic_BomWhiteDowm:

			//キャラクターの色が白でなければ
			

			for(int y = VecY+1;y < MAP_CHIP_YNUM;y++){
				//マップチップが存在しているなら
				if(mMap.map[y][VecX].mapChipKind[0] != eMapGimic_None){
					//通行不可能ますであれば
					if(mMap.map[y][VecX].passible == 1){
						//通行可能にする
						mMap.map[y][VecX].passible = 0;

						//マップチップを設置する 0 ~ 9:泥　10 ~ 19:土　20 ~ 29:石 30 ~ 39:骨
						mMap.map[y][VecX].mapChipKind[0] = eMapGimic_Mud + (((y-VecY)/10)%3);

						//通行可能にしたマスの不必要なマスを削除する
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[y][VecX].mapChipKind[i] > eMapGimic_Mud && mMap.map[y][VecX].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[y][VecX].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//通行可能マスを見つけたら抜ける
					else{
						break;
					}
				}
				//マップチップが存在していないますであるなら抜ける
				else{
					break;
				}
			}



			break;
		case eMapGimic_BomWhiteLeft:

			//キャラクターの色が白でなければ
			

			for(int x = VecX-1;x > 0;x--){
				//マップチップが存在しているなら
				if(mMap.map[VecY][x].mapChipKind[0] != eMapGimic_None){
					//通行不可能ますであれば
					if(mMap.map[VecY][x].passible == 1){
						//通行可能にする
						mMap.map[VecY][x].passible = 0;

						//マップチップを設置する 0 ~ 9:泥　10 ~ 19:土　20 ~ 29:石 30 ~ 39:骨
						mMap.map[VecY][x].mapChipKind[0] = eMapGimic_Mud + (((VecX-x)/10)%3);


						//通行可能にしたマスの不必要なマスを削除する
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[VecY][x].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY][x].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[VecY][x].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//通行可能マスを見つけたら抜ける
					else{
						break;
					}
				}
				//マップチップが存在していないますであるなら抜ける
				else{
					break;
				}
			}

			break;
		case eMapGimic_BomWhiteRight:

			//キャラクターの色が白でなければ
			

			for(int x = VecX+1;x < MAP_CHIP_XNUM;x++){
				//マップチップが存在しているなら
				if(mMap.map[VecY][x].mapChipKind[0] != eMapGimic_None){
					//通行不可能ますであれば
					if(mMap.map[VecY][x].passible == 1){
						//通行可能にする
						mMap.map[VecY][x].passible = 0;


						//マップチップを設置する 0 ~ 9:泥　10 ~ 19:土　20 ~ 29:石 30 ~ 39:骨
						mMap.map[VecY][x].mapChipKind[0] = eMapGimic_Mud + (((x-VecX)/10)%3);

						//通行可能にしたマスの不必要なマスを削除する
						for(int i = 1;i < MAP_LAYER_NUM;i++){
							if(mMap.map[VecY][x].mapChipKind[i] > eMapGimic_Mud && mMap.map[VecY][x].mapChipKind[i] <= eMapGimic_rubble){
								mMap.map[VecY][x].mapChipKind[i] = eMapGimic_None;
							}

						}

					}
					//通行可能マスを見つけたら抜ける
					else{
						break;
					}
				}
				//マップチップが存在していないますであるなら抜ける
				else{
					break;
				}
			}

			break;
		//爆破 黒
		case eMapGimic_BomBlackUp:

			

			//通行可能にする
			mMap.map[VecY-2][VecX-1].passible = 0;	//上左側のマスを通行可能にする
			mMap.map[VecY-2][VecX  ].passible = 0;	//上側のマスを通行可能にする
			mMap.map[VecY-2][VecX+1].passible = 0;	//上右側のマスを通行可能にする
		
			//泥のマップチップを設置する
			mMap.map[VecY-2][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-2][VecX  ].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY-2][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			
			

			//通行可能にする
			mMap.map[VecY+2][VecX-1].passible = 0;	//下左側のマスを通行可能にする
			mMap.map[VecY+2][VecX  ].passible = 0;	//下側のマスを通行可能にする
			mMap.map[VecY+2][VecX+1].passible = 0;	//下右側のマスを通行可能にする
		
			//泥のマップチップを設置する
			mMap.map[VecY+2][VecX-1].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+2][VecX  ].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+2][VecX+1].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY-1][VecX-2].passible = 0;	//上左側のマスを通行可能にする
			mMap.map[VecY  ][VecX-2].passible = 0;	//左側のマスを通行可能にする
			mMap.map[VecY+1][VecX-2].passible = 0;	//下左側のマスを通行可能にする
		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX-2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY  ][VecX-2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX-2].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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

			

			//通行可能にする
			mMap.map[VecY-1][VecX+2].passible = 0;	//上右側のマスを通行可能にする
			mMap.map[VecY  ][VecX+2].passible = 0;	//右側のマスを通行可能にする
			mMap.map[VecY+1][VecX+2].passible = 0;	//下右側のマスを通行可能にする
		
			//泥のマップチップを設置する
			mMap.map[VecY-1][VecX+2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY  ][VecX+2].mapChipKind[0] = eMapGimic_Mud;
			mMap.map[VecY+1][VecX+2].mapChipKind[0] = eMapGimic_Mud;

			//通行可能にしたマスの不必要なマスを削除する
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
	
		//鍵扉
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
		//ペイント
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
		//ワープ
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
		//反転
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
		//罠
		case eMapGimic_Trap:
	 
			break;
		 //落とし穴
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
		 //プレイヤー初期位置
		case eMapGimic_SetPlayerPosition:
	
			break;
		//ゴール
		case eMapGimic_Goal:
			isGoal = true;
			break;
		//鍵
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
		//回復の水
		case eMapGimic_Water:
			
			//取得したので消す
			mMap.map[VecY][VecX].mapChipKind[i] = eMapGimic_None;

			ch.hp += 50;

			if(ch.hp > MAX_PLAYER_HP){
				ch.hp = MAX_PLAYER_HP;
			}



			break;
		//時計
		case eMapGimic_Clock:
	 
			break;
		 //紙
		case eMapGimic_Paper:
		
			break;
		 //宝
		case eMapGimic_Treasure:
	 
			break;
		 //??(破壊可能通行不可能チップ??)
	 
		 //??(破壊不可能通行不可能チップ??)
	 
		 //エネミー
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
		//元の配色に戻す
	}

}

void GimicEffect(int VecX,int VecY){
	
	static bool isUse = false;

	switch(mMap.map[VecY][VecX].gimicKind){
	case eMapGimic_WarpYellow:	//ワープ黄色

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
	case eMapGimic_WarpRed:	//ワープ赤
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
	case eMapGimic_WarpBlue:	//ワープ青
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
	case eMapGimic_WarpWhite:	//ワープ白
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
	case eMapGimic_WarpBlack:	//ワープ黒
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

		//キャラクターの色が黄色でなければ処理を抜ける
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//通行可能にする
		mMap.map[VecY][VecX+1].passible = 0;	//右側のマスを通行可能にする
		mMap.map[VecY+1][VecX].passible = 0;	//下側のマスを通行可能にする

		
		//マップチップ番号が岩であるなら泥にする
		if(mMap.map[VecY+1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY+1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX+1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX+1].mapChipKind[0] = 0;
		}

		//マップチップを削除する

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

		//キャラクターの色が黄色でなければ処理を抜ける
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//通行可能にする
		mMap.map[VecY][VecX-1].passible = 0;	//右側のマスを通行可能にする
		mMap.map[VecY-1][VecX].passible = 0;	//下側のマスを通行可能にする

		
		//マップチップ番号が岩であるなら泥にする
		if(mMap.map[VecY-1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY-1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}

		//マップチップを削除する

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

		//キャラクターの色が黄色でなければ処理を抜ける
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//通行可能にする
		mMap.map[VecY][VecX-1].passible = 0;	//右側のマスを通行可能にする
		mMap.map[VecY+1][VecX].passible = 0;	//下側のマスを通行可能にする

		
		//マップチップ番号が岩であるなら泥にする
		if(mMap.map[VecY+1][VecX].mapChipKind[0] % 7 == 4){
			mMap.map[VecY+1][VecX].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}

		//マップチップを削除する

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

		//キャラクターの色が黄色でなければ処理を抜ける
		if(ch.colorKind != eColorKind_Yellow ){
			return;
		}

		//通行可能にする
		mMap.map[VecY][VecX+1].passible = 0;	//右側のマスを通行可能にする
		mMap.map[VecY][VecX-1].passible = 0;	//下側のマスを通行可能にする

		
		//マップチップ番号が岩であるなら泥にする
		if(mMap.map[VecY][VecX-1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX-1].mapChipKind[0] = 0;
		}
		
		if(mMap.map[VecY][VecX+1].mapChipKind[0] % 7 == 4){
			mMap.map[VecY][VecX+1].mapChipKind[0] = 0;
		}

		//マップチップを削除する

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
	case eMapGimic_Goal:	//ゴール
		isGoal = true;
		break;
	default:
		isUse = false;
		break;
	}


}

//アイテムの効果
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

	//行先にマップチップがなければ
	if(mMap.map[VecY][VecX].mapChipKind[0] == -1){
		//通行不可
		return false;
	}

	//マップチップが存在する範囲外に行こうとしているのなら
	if(VecX < 0 || VecX >= MAP_CHIP_XNUM ||
		VecY < 0 || VecY >= MAP_CHIP_YNUM ){
			//通行不可
			return false;
	}

	//通行不可能
	if(mMap.map[VecY][VecX].passible == 1){
		return false;
	}

	//通行可能
	return true;
}

//以下、プレイヤーに関する処理

void PlayerMove(){

	static int x = 0;
	static int y = 0;


	//キャラクターの色の切り替え
	if(Key(KEY_INPUT_C) == 1){
		ch.colorKind = (ch.colorKind + 1 ) % CHARACTOR_COLOR_NUM;
	}


	if(ch.hp > 0){

		//歩いていないなら　キー入力を受け付ける
		if(!ch.isWalk){
		
			int vecX = ch.x/MAP_CHIP_SIZE;	//座標の値を配列の値に変換
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

		//移動方向がない状態ではないなら
		if(ch.vec != eVector_None){
			//歩くフラグを立てる
			ch.isWalk = true;
		}
	}
	
	//歩くフラグが立っているのなら
	if(ch.isWalk){


		//移動方向に合わせて移動させる
		switch(ch.vec){
		case eVector_Right:

			//移動先の座標より小さければ
			if(ch.x < x + 100){
				ch.x += PLAYER_SPEED;
			}
			//そうでなければ
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

	//キャラクターの表示位置をカメラ座標に設定する
	int drawX = CameraPositionX;
	int drawY = CameraPositionY;
	
	//キャラクター画像描画
	DrawGraph(drawX,drawY,CharactorImage[ch.colorKind],TRUE);

	//体力ゲージの描画
	DrawBox(drawX,drawY-10,drawX + MAP_CHIP_SIZE * ch.hp/MAX_PLAYER_HP,drawY-10 + 20,GetColor(100,200,255),TRUE);
	DrawFormatString(drawX,drawY-10,GetColor(0,0,255),"HP:%d",ch.hp);

	//キャラクターの実体座標
	DrawFormatString(0,0,GetColor(255,255,255),"キャラクター実体座標:%d:%d",ch.x,ch.y);

	//キャラクターの描画座標
	DrawFormatString(0,20,GetColor(255,255,255),"キャラクター描画座標:%d:%d",drawX,drawY);


}


void DrawEnemy(){
	
	//エネミーの数が0であるなら処理を抜ける
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

//以下、マップファイル数確認処理

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

////////////////////////////////////////////////////////////////////以下、メインに関する処理
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

	SetGraphMode(WINDOW_WIDHT,WINDOW_HEIGHT, 32);		//ウィンドウサイズ設定

	ChangeWindowMode(TRUE);

	//多重起動を許可する
	SetDoubleStartValidFlag( TRUE );

	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);	//裏画面設定

	int Scene = 0;
	int backImage = LoadGraph("data/back.png");

	LoadDivGraph("data/map_chip.png",DIV_SPRITE_NUM,5,20,MAP_CHIP_SIZE,MAP_CHIP_SIZE,MapChipImage);

	//敵の画像読み込み
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
		MessageBox(NULL,TEXT("データがありません"),TEXT("警告!"),MB_OK|MB_ICONWARNING);
		DxLib_End();
	}


	SetCharactor();

	//プレイヤーキャラクター初期位置設定
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

	// while( 裏画面を表画面に反映, メッセージ処理, 画面クリア )
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
					DrawString(0,100,"None　Data",GetColor(255,255,255));
				}else{
					//プレイヤーキャラクター初期位置設定
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



				//キャラクターの初期化
				ch.hp = MAX_PLAYER_HP;
				ch.maxhp = ch.hp;
				ch.colorKind = eColorKind_Nomal;			//黄色に設定

				oldStage = stage;
			}

			if(Key(KEY_INPUT_Z) == 1){

				isGoal = false;

				Scene = 1;
			}
			
			DrawFormatString(0,0,GetColor(255,255,255),"タイトル:%s",mMap.mapName);
			DrawFormatString(0,20,GetColor(255,255,255),"説　　明:%s",mMap.mapDescription);

			DrawString(0,50,"出現ギミック",GetColor(255,255,255));

			//出現するギミックの描画
			for(int i = 0;i < 10;i++){
				if(mMap.gimicVisibleKind[i] == 1){
					DrawBox(i * 20,70,(i+1)*20,90,color[i],TRUE);
				}
			}

			DrawFormatString(0,100,GetColor(255,255,255),"1 - %d",stage+1);
			DrawFormatString(0,120,GetColor(255,255,255),"FileName:%s",loadMapfile);

			DrawString(100,200,"Zキーでステージへ",GetColor(255,255,255));
			DrawString(100,220,"左右キーでステージ変更",GetColor(255,255,255));


			

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
					DrawString(WINDOW_WIDHT/2-200,WINDOW_HEIGHT/2-20,"ステージクリア！",GetColor(255,255,0));
					SetFontSize(-1);
					cnt++;
				}else{
					cnt = 0;
					isGoal = false;
					Scene = 0;
				}
			}

			DrawString(0,WINDOW_HEIGHT-20,"Xキーでステージ選択に戻ります",GetColor(255,255,255));


			if(Key(KEY_INPUT_X) == 1){
				Scene = 0;
			}

			break;
		}

		
	}


	
    // ＤＸライブラリの後始末
    DxLib_End() ;

    // ソフトの終了
    return 0 ;
}