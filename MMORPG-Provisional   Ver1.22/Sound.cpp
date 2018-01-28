#include "Sound.h"
#include "Stage.h"
#include "TextParam.h"
#include <string.h>
#include <stdlib.h>

// ３Ｄサウンドのデフォルトの音が聞こえる範囲
#define DEFAULT_RADIUS			(5000.0f)

// サウンドのデフォルト同時再生可能数
#define DEFAULT_BUFFER_NUM		(6)

// ファイルパスの最大長
#define MAX_PATH_LENGTH			(64)

// １サウンド辺りの最大バリエーション数
#define MAX_HANDLE_NUM			(8)

// 登録できるサウンドの最大数
#define MAX_SOUNDINFO_NUM		(1024)

// サウンドファイルが存在するフォルダパス
#define FILE_DIRECTORY_PATH		"Data\\Sound\\"

// サウンドファイルの拡張子
#define FILE_EXENAME			"wav"

// サウンドの情報
typedef struct _SSoundInfo
{
	// ３Ｄサウンドかどうか( true:3Dサウンド  false:2Dサウンド )
	bool                 Is3DSound;

	// ファイルパス
	char                 FilePath[MAX_PATH_LENGTH];

	// サウンドハンドルの数
	int                  HandleNum;

	// サウンドハンドル
	int                  Handle[MAX_HANDLE_NUM];
} SSoundInfo;

// ＢＧＭの情報
typedef struct _SBgmInfo
{
	// サウンドハンドル
	int                  SoundHandle;

	// ＢＧＭのループ時に戻る位置
	int                  LoopStartPosition;

	// ＢＧＭのループをする位置
	int                  LoopEndPosition;
} SBgmInfo;

// サウンドシステムの情報
typedef struct _SSoundSystemData
{
	// サウンドの数
	int                  SoundNum;

	// サウンドの情報
	SSoundInfo           Sound[MAX_SOUNDINFO_NUM];

	// ＢＧＭの情報
	SBgmInfo             Bgm[EBGM_Num];

	// 現在鳴らしているＢＧＭ
	EBGM                 NowPlayBgm;
} SSoundSystemData;

static SSoundSystemData g_SndSysInfo;

// サウンド処理を初期化する
//     戻り値 : 初期化が成功したかどうか( true:成功  false:失敗 )
bool Sound_Initialize(void)
{
	int             i;
	SBgmInfo       *BInfo;
	STextParamInfo  TextParam;
	const char     *ParamString;
	char            String[256];

	// サウンドの数を初期化
	g_SndSysInfo.SoundNum = 0;

	// ＢＧＭの情報が記述されているテキストファイルを読み込む
	if (!LoadTextParam(&TextParam, FILE_DIRECTORY_PATH "BGM\\Param.txt"))
	{
		return false;
	}

	// 作成するサウンドのタイプをファイルストリーミング形式に変更する
	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);

	// ＢＧＭの数だけ繰り返し
	BInfo = g_SndSysInfo.Bgm;
	for (i = 0; i < EBGM_Num; i++, BInfo++)
	{
		// ＢＧＭ用のサウンドファイルのパスを取得する
		ParamString = GetTextParamString(&TextParam, "BGM%d_FilePath", i);

		// パスが見つからなかったらエラー
		if (ParamString == NULL)
		{
			return false;
		}

		// ＢＧＭ用のサウンドファイルを読み込む
		sprintf(String, FILE_DIRECTORY_PATH "BGM\\%s", ParamString);
		BInfo->SoundHandle = LoadSoundMem(String);

		// 読み込みに失敗したらエラー
		if (BInfo->SoundHandle == -1)
		{
			return false;
		}

		// BGMのループ開始位置と終了位置を取得する
		BInfo->LoopStartPosition = GetTextParamInt(&TextParam, "BGM%d_Loop_Start", i);
		BInfo->LoopEndPosition = GetTextParamInt(&TextParam, "BGM%d_Loop_End", i);

		// 有効なループ位置が得られた場合はループ位置をサウンドハンドルに設定する
		if (BInfo->LoopStartPosition >= 0 &&
			BInfo->LoopEndPosition   >  0)
		{
			SetLoopSamplePosSoundMem(BInfo->LoopStartPosition, BInfo->SoundHandle);
			SetLoopStartSamplePosSoundMem(BInfo->LoopEndPosition, BInfo->SoundHandle);
		}
	}

	// 作成するサウンドのタイプを標準形式に戻す
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

	// 『現在再生中のBGM』を初期化
	g_SndSysInfo.NowPlayBgm = EBGM_Num;

	// 成功終了
	return true;
}

// サウンド処理の後始末をする
void Sound_Terminate(void)
{
	int         i;
	int         j;
	SSoundInfo *Info;
	SBgmInfo   *BInfo;

	// 全てのサウンドのサウンドハンドルを削除する
	Info = g_SndSysInfo.Sound;
	for (i = 0; i < g_SndSysInfo.SoundNum; i++, Info++)
	{
		for (j = 0; j < Info->HandleNum; j++)
		{
			DeleteSoundMem(Info->Handle[j]);
		}
	}

	// サウンドの数を０にする
	g_SndSysInfo.SoundNum = 0;

	// 全てのＢＧＭのサウンドハンドルを削除する
	BInfo = g_SndSysInfo.Bgm;
	for (i = 0; i < EBGM_Num; i++, BInfo++)
	{
		DeleteSoundMem(BInfo->SoundHandle);

		// サウンドハンドルとして無効な値を代入しておく
		BInfo->SoundHandle = -1;
	}
}

// サウンドファイルを追加する
//     戻り値 : サウンドファイルの登録番号
int Sound_AddSound(
	// 追加するサウンドファイルのパス
	const char *FilePath,

	// ３Ｄで鳴らすサウンドかどうか( true:3Dサウンド  false:2Dサウンド )
	bool Is3DSound
)
{
	int             i;
	int             Index;
	SSoundInfo     *Info;
	char            FilePath2[MAX_PATH];
	STextParamInfo  TextParam;
	float           Radius;
	int             Volume;

	// 既に登録してあるサウンドの中に、登録しようとしているサウンドが存在するか調べる
	Info = g_SndSysInfo.Sound;
	for (Index = 0; Index < g_SndSysInfo.SoundNum; Index++, Info++)
	{
		if (Info->Is3DSound == Is3DSound && strcmp(Info->FilePath, FilePath) == 0)
		{
			// 存在した場合は、既に登録済みのサウンドの登録番号を返す
			return Index;
		}
	}

	// 登録しているサウンドの数が最大数に達していたらエラー
	if (g_SndSysInfo.SoundNum == MAX_SOUNDINFO_NUM)
	{
		return -1;
	}

	// ３Ｄサウンドかどうかを保存
	Info->Is3DSound = Is3DSound;

	// 作成するサウンドが３Ｄサウンドかどうかを設定する
	SetCreate3DSoundFlag(Is3DSound);

	// ３Ｄサウンドの場合は聞こえる距離や音量をテキストファイルから読み取る
	if (Info->Is3DSound)
	{
		Radius = DEFAULT_RADIUS;
		Volume = 255;
		if (LoadTextParam(&TextParam, "%s%s_Param.txt", FILE_DIRECTORY_PATH, FilePath))
		{
			Radius = GetTextParamFloat(&TextParam, "Radius");
			Volume = (int)(GetTextParamFloat(&TextParam, "Volume") * 255.0f);
		}
	}

	// サウンドファイルを読み込む
	Info->HandleNum = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++)
	{
		// サウンドファイルパスの作成
		sprintf(FilePath2, "%s%s_%02d.%s",
			FILE_DIRECTORY_PATH, FilePath, i, FILE_EXENAME);

		// サウンドファイルが無かったらループを抜ける
		if (FileRead_size(FilePath2) < 0)
		{
			break;
		}

		// サウンドファイルの読み込み
		Info->Handle[i] = LoadSoundMem(FilePath2, DEFAULT_BUFFER_NUM);
		if (Info->Handle[i] == -1)
		{
			return -1;
		}

		// サウンドファイルの場合は聞こえる距離と音量をサウンドハンドルに設定する
		if (Info->Is3DSound)
		{
			Set3DRadiusSoundMem(Radius, Info->Handle[i]);
			ChangeVolumeSoundMem(Volume, Info->Handle[i]);
		}
	}

	// サウンドハンドルの数を保存
	Info->HandleNum = i;

	// 一つも読み込めなかった場合はエラー
	if (Info->HandleNum == 0)
	{
		return -1;
	}

	// ファイルパスを保存
	strcpy(Info->FilePath, FilePath);

	// サウンドの数を増やす
	g_SndSysInfo.SoundNum++;

	// サウンドの登録番号を返す
	return Index;
}

// サウンドを再生する
void Sound_PlaySound(
	// 再生するサウンドの登録番号
	int SoundIndex,

	// 再生タイプ( DX_PLAYTYPE_BACK など )
	int PlayType
)
{
	SSoundInfo *Info;

	// 指定の登録番号が有効かチェック
	if (SoundIndex < 0 || SoundIndex >= g_SndSysInfo.SoundNum)
	{
		return;
	}
	Info = &g_SndSysInfo.Sound[SoundIndex];

	// サウンドハンドルが複数ある場合はランダムで鳴らすサウンドを決定する
	PlaySoundMem(Info->Handle[GetRand(Info->HandleNum - 1)], PlayType);
}

// サウンドを３Ｄ再生する
void Sound_PlaySound3D(
	// サウンドを鳴らすワールド座標
	VECTOR Position,

	// 再生するサウンドの登録番号
	int SoundIndex,

	// 再生タイプ( DX_PLAYTYPE_BACK など )
	int PlayType
)
{
	SSoundInfo *Info;
	int         PlayIndex;

	// 指定の登録番号が有効かチェック
	if (SoundIndex < 0 || SoundIndex >= g_SndSysInfo.SoundNum)
	{
		return;
	}
	Info = &g_SndSysInfo.Sound[SoundIndex];

	// サウンドハンドルが複数ある場合は鳴らすサウンドをランダムで決定する
	PlayIndex = GetRand(Info->HandleNum - 1);

	// 鳴らすワールド座標を設定する
	SetNextPlay3DPositionSoundMem(Position, Info->Handle[PlayIndex]);

	// サウンドを再生する
	PlaySoundMem(Info->Handle[PlayIndex], PlayType);
}

// ＢＧＭを再生する
void Sound_PlayBGM(
	// 再生するＢＧＭ
	EBGM BGM
)
{
	SBgmInfo *BInfo = &g_SndSysInfo.Bgm[BGM];

	// 現在再生しているBGMと同じ場合は何もせずに終了
	if (g_SndSysInfo.NowPlayBgm == BGM)
	{
		return;
	}

	// 現在他のBGMを再生していたら停止する
	if (g_SndSysInfo.NowPlayBgm != EBGM_Num)
	{
		Sound_StopBGM();
	}

	// BGMの再生を開始する
	// 有効なループポイントがある場合はループ再生する
	PlaySoundMem(BInfo->SoundHandle,
		BInfo->LoopStartPosition >= 0 ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK
	);

	// 『現在再生中のBGM』を更新する
	g_SndSysInfo.NowPlayBgm = BGM;
}

// ＢＧＭの再生を止める
void Sound_StopBGM(void)
{
	// 現在何のBGMも再生していない場合は何もせず終了
	if (g_SndSysInfo.NowPlayBgm == EBGM_Num)
	{
		return;
	}

	// BGMのサウンドハンドルの再生を停止する
	StopSoundMem(g_SndSysInfo.Bgm[g_SndSysInfo.NowPlayBgm].SoundHandle);

	// 『現在再生中のBGM』を何も再生していないことを示す値にする
	g_SndSysInfo.NowPlayBgm = EBGM_Num;
}
