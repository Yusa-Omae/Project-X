#ifndef ITEM_H
#define ITEM_H


typedef struct _ITEM_PARAM{

	//アイテムナンバー
	int Num;
	//アイテムのレベル段階
	int Lv;
	//アイテムの値段
	int Pre;
	//アイテムの耐久値
	int Db;
	//装備した際の追加HP
	int Health;
	//装備した際の追加攻撃力
	int Attack;
	//装備した際の追加防御力
	int Def;
	//装備した際の追加クリティカル率
	float Crt;
	//装備した際の追加移動速度
	float Spd;
	//攻撃した際の追加HP吸収率
	float Abp;
	//装備した際の追加リジェネ
	float Ah;
	//敵を倒した際のドロップ金増加率
	float Gp;
	//進化先
	int Evol[4];

}ITEM_PARAM;










#endif