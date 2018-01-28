
#if false
void UI_Initialize();
void UI_Update();
void UI_Draw();
void UI_Fainalize();

//UI表示座標保存構造体
typedef struct _MENU_AXIS{

	int X;
	int Y;

}_MENU_AXIS;

//メニュー系構造体
typedef struct _MENU_INFO {

	//展開フラグ
	int flg;

	//ポップアップ時カウント
	int Move_Cnt;

	//ポップアップフラグ
	bool Move_Flg;//true = アイドル false = 展開済み

	//1フレーム前のポップアップフラグ
	bool Back_Move_Flg;//1フレーム前とのフラグ差異によりアニメーション(Sin派を使用した座標移動)を行う

	//メニュー描画座標値管理
	_MENU_AXIS MENU_AXIS[6];

}_MENU_INFO;

//UIclassで使用するその他のもの
typedef struct _UI_INFO {


}_UI_INFO;
#endif