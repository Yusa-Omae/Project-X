//戦闘データのやり取りなど。
#include"Chara.h"
#include"DxLib.h"
#include"Chara_Player.h"
#include"Key.h"
#include"Camera.h"
#include"System.h"


_CHARA_MGR CHARA_MGR[100];

int Model_Kind[model_Max];
int Model_Anim[model_Max][anim_Max];
int Attach_Model;
float Total_Anim_time;
float Play_Anim_time;
bool Anim_Flg[10];
int Root_flm;




float RATETION_TBL[] = {

	0.0f,		//direction_Down
	270.0f,		//direction_Left
	180.0f,		//direction_Up
	90.0f,		//direction_Right

};

/*

	アニメーションを設定する
	int modelHandle			:モデルハンドル
	int *attachModleHandle	:アタッチモデルハンドル
	int ChangeAnimHandle	:切り替えるアニメハンドル
	int animIdex = 0		:アニメーション番号 何も入力がなければ0が設定される
	return	アニメーションの総時間
*/
static int Change_AttachAnim(int modelHandle,int *attachModleHandle,int ChangeAnimHandle,int animIdex = 0) {

	MV1DetachAnim(modelHandle, *attachModleHandle);
	*attachModleHandle = MV1AttachAnim(modelHandle, animIdex, ChangeAnimHandle);
	return MV1GetAttachAnimTotalTime(modelHandle, *attachModleHandle);
}

//初期化
void Chara_Initialize(){

	memset(Anim_Flg,0,10);
	CHARA_MGR[model_Player].Move = true;
	//プレイヤー
	Model_Kind[model_Player] = MV1LoadModel("Data/Charactor/Player/PC.mv1");
	//エネミー
	Model_Kind[model_Goblin] = MV1LoadModel("Data/Charactor/Goblin/Goblin.mv1");
	//プレイヤーアニメーション
	Model_Anim[model_Player][anim_Neutral] = MV1LoadModel("Data/Charactor/Player/Anim_Neutral.mv1");
	Model_Anim[model_Player][anim_Run] = MV1LoadModel("Data/Charactor/Player/Anim_Run.mv1");
	Model_Anim[model_Player][anim_Attack] = MV1LoadModel("Data/Charactor/Player/Anim_Attack1.mv1");
	//エネミーアニメーション
	Model_Anim[model_Goblin][anim_Neutral] = MV1LoadModel("Data/Charactor/Goblin/Anim_Neutral.mv1");
	Model_Anim[model_Goblin][anim_Run] = MV1LoadModel("Data/Charactor/Goblin/Anim_Run.mv1");
	//プレイヤー初期アニメーション設定
	Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);

}

void Chara_Update(){

	//アニメーション進行
	Play_Anim_time += 0.5f;
	if(Play_Anim_time > Total_Anim_time){
		Play_Anim_time = 0.0f;
	}
	MV1SetAttachAnimTime(Model_Kind[model_Player],Attach_Model,Play_Anim_time);

	//プレイヤーモデル設定(走る、idol 切り替え)
	if(CHARA_MGR[model_Player].Move == true){
		if(Anim_Flg[anim_Run] == true){
			Anim_Flg[anim_Run] = false;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Neutral]);
#if false
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Neutral]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}else{
		if(Anim_Flg[anim_Run] == false){
			Anim_Flg[anim_Run] = true;
			Total_Anim_time = Change_AttachAnim(Model_Kind[model_Player], &Attach_Model, Model_Anim[model_Player][anim_Run]);

#if false	//関数を作成したのでコメントアウト By.Syara
			MV1DetachAnim(Model_Kind[model_Player],Attach_Model);
			Attach_Model = MV1AttachAnim(Model_Kind[model_Player],0,Model_Anim[model_Player][anim_Run]);
			Total_Anim_time = MV1GetAttachAnimTotalTime(Model_Kind[model_Player],Attach_Model);
#endif
		}
	}
	
	

	//モデルのフレームを検索
	Root_flm = MV1SearchFrame(Model_Kind[model_Player],"root");
	MV1SetFrameUserLocalMatrix(Model_Kind[model_Player],Root_flm,MGetIdent());

}

void Chara_Draw(){
	
	//キャラの回転
	MV1SetRotationXYZ(Model_Kind[model_Player],VGet(0.0f,1.57f*CHARA_MGR[model_Player].Direction/*CHARA_MGR[model_Player].Direction*90.0f+GetRad_Direction()*/,0.0f));
	//キャラの移動
	MV1SetPosition(Model_Kind[model_Player],Get_Player_Pos());
	//キャラの描画
	MV1DrawModel(Model_Kind[model_Player]);

	MV1DrawModel(Model_Kind[model_Goblin]);

}

void Chara_Fainalize(){
	
	for(int i = 0; i > model_Max-1;i++){
		MV1DeleteModel(Model_Kind[i]);
	}

}

void Set_Chara_Direction(int charanum,int direction){
	CHARA_MGR[charanum].Direction = direction;
}

int Get_Chara_Direction(int charanum){
	return CHARA_MGR[charanum].Direction;
}

/*
	角度を返却する
*/
float Get_Chara_Rotation(int charanum) {
	return RATETION_TBL[CHARA_MGR[charanum].Direction];
}

void Set_Anim_Flg(int anim_Kind,bool flg){
	Anim_Flg[anim_Kind] = flg;
}

void Set_Move_flg(int model_Kind, bool flg){
	CHARA_MGR[model_Kind].Move = flg;
}

bool Get_Move_flg(int model_Kind){
	return	CHARA_MGR[model_Kind].Move;
}


void Debag_Chara(){

}