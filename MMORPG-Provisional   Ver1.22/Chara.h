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
角度を返却する
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


//能力値割り振りによって転職できるジョブが変わるシステムとか素敵
//ゆうなれば自由自在
//0:プレイヤー 1-50:エネミー 51-100:NPC 予定

struct _CHARA_MGR{

	//キャラのヒットポイントステータス
	int HP;
	//キャラのマジックポイントステータス
	int MP;
	//キャラの物理攻撃力ステータス
	int Str;
	//キャラの魔法攻撃力ステータス
	int Int;
	//キャラの物理防御力ステータス
	int Vit;
	//キャラの魔法防御力ステータス
	int Mgr;
	//キャラの回避力＆確率UPステータス
	int Luc;
	//キャラのモデル種類
	int Model_N;
	//キャラのアニメーション種類
	int Anim_N;
	//キャラの出現フラグ
	int Flg;
	//キャラの向き
	int Direction;
	//動いているか
	bool Move;
};

