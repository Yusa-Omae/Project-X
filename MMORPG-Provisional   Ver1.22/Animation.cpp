#include "Animation.h"
#include "BinaryFile.h"
#include "TextParam.h"
#include "DxLib.h"
#include <stdio.h>
#include <string.h>

// アニメーションイベントタイプ名テーブル
static const char *g_AnimEventTypeName[EAnimEventType_Num] =
{
	"Sound",			// EAnimEventType_Sound
	"PhysicsSound",		// EAnimEventType_PhysicsSound
	"AnimCancel",		// EAnimEventType_AnimCancel
	"AttackStart",		// EAnimEventType_AttackStart,
	"AttackEnd",		// EAnimEventType_AttackEnd,
	"Other",			// EAnimEventType_Other,
};

// アニメーション物理音タイプ名テーブル
static const char *g_AnimPhysicsSoundName[EAnimPhysicsSound_Num] =
{
	"footstep",			// EAnimPhysicsSound_Run
	"down",				// EAnimPhysicsSound_Down
};

// アニメーション情報をファイルから読み込む
//     戻り値 : 読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
bool LoadAnimInfoFile(
	// アニメーション情報構造体のアドレス
	SAnimInfo *AInfo,

	// アニメーション情報ファイルパス
	const char *FilePath
)
{
	const char     *ParamString;
	SAnimEventInfo *Event;
	int             i;
	int             j;
	int             ParamInt;
	STextParamInfo  TextParam;
	SBinaryFileData BinFileData;
	bool            LoadSuccess;

	// アニメーション情報が変更されたかどうかのフラグを初期化
	AInfo->Change = false;

	// アニメーションイベントの数を初期化
	AInfo->EventNum = 0;

	// 読み込みが成功したかどうかのフラグを倒しておく
	LoadSuccess = false;

	// アニメーション情報のバイナリファイルを読み込む
	if (ReadBinFile_Open(&BinFileData, "%s.dat", FilePath))
	{
		// アニメーションイベントの数を読み込む
		ReadBinFile_Int(&BinFileData, &AInfo->EventNum);

		// アニメーションイベント情報の数だけ繰り返し
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// アニメーションイベントタイプを読み込み
			ReadBinFile_Int(&BinFileData, &ParamInt);
			Event->Type = (EAnimEventType)ParamInt;

			// アニメーションイベントタイムを読み込み
			ReadBinFile_Float(&BinFileData, &Event->TimeF);
			Event->TimeI = (int)(Event->TimeF * ANIM_TIME_INT_SCALE);

			// その他の情報の初期化
			Event->SoundHandle = -1;
			Event->AttackPosIndex = 0;
			Event->AttackNo = 0;
			Event->PhysicsSound = (EAnimPhysicsSound)0;
			memset(Event->SoundPath, 0, sizeof(Event->SoundPath));

			// アニメーションイベントタイプによって処理を分岐
			switch (Event->Type)
			{
			case EAnimEventType_Sound:
				// 音再生イベントの場合はサウンドファイルのパスを読み込み
				ReadBinFile_String(&BinFileData, Event->SoundPath);
				break;

			case EAnimEventType_PhysicsSound:
				// 物理音イベントの場合は物理音タイプを読み込み
				ReadBinFile_Int(&BinFileData, &ParamInt);
				Event->PhysicsSound = (EAnimPhysicsSound)ParamInt;
				break;

			case EAnimEventType_AttackStart:
				// 攻撃判定開始イベントの場合は攻撃番号と攻撃位置番号を読み込み
				ReadBinFile_Int(&BinFileData, &Event->AttackNo);
				ReadBinFile_Int(&BinFileData, &Event->AttackPosIndex);
				break;

			case EAnimEventType_AttackEnd:
				// 攻撃判定終了イベントの場合は攻撃番号を読み込み
				ReadBinFile_Int(&BinFileData, &Event->AttackNo);
				break;
			}
		}

		// バイナリファイルを閉じる
		ReadBinFile_Close(&BinFileData);

		// 読み込みが成功したフラグを立てる
		LoadSuccess = true;
	}

	// バイナリファイルが読み込めなかった場合はアニメーション情報の
	// テキストファイルを読み込む
	if (!LoadSuccess &&
		LoadTextParam(&TextParam, "%s.txt", FilePath))
	{
		// アニメーションイベントの数を読み込む
		AInfo->EventNum = GetTextParamInt(&TextParam, "EventNum");

		// アニメーションイベント情報の数だけ繰り返し
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// アニメーションイベントタイプを読み込み
			ParamString = GetTextParamString(&TextParam, "Event%d_TypeName", i);
			for (j = 0; j < EAnimEventType_Num; j++)
			{
				if (strcmp(g_AnimEventTypeName[j], ParamString) == 0)
				{
					break;
				}
			}
			if (j == EAnimEventType_Num)
			{
				return false;
			}
			Event->Type = (EAnimEventType)j;

			// アニメーションイベントタイムを読み込み
			Event->TimeF = GetTextParamFloat(&TextParam, "Event%d_Time", i);
			Event->TimeI = (int)(Event->TimeF * ANIM_TIME_INT_SCALE);

			// その他の情報の初期化
			Event->SoundHandle = -1;
			Event->AttackPosIndex = 0;
			Event->AttackNo = 0;
			Event->PhysicsSound = (EAnimPhysicsSound)0;
			memset(Event->SoundPath, 0, sizeof(Event->SoundPath));

			// アニメーションイベントタイプによって処理を分岐
			switch (Event->Type)
			{
			case EAnimEventType_Sound:
				// 音再生イベントの場合はサウンドファイルのパスを読み込み
				ParamString = GetTextParamString(&TextParam, "Event%d_SoundPath", i);
				strcpy(Event->SoundPath, ParamString);
				break;

			case EAnimEventType_PhysicsSound:
				// 物理音イベントの場合は物理音タイプを読み込み
				ParamString = GetTextParamString(&TextParam, "Event%d_PhysicsSoundType", i);
				for (j = 0; j < EAnimPhysicsSound_Num; j++)
				{
					if (strcmp(g_AnimPhysicsSoundName[j], ParamString) == 0)
					{
						break;
					}
				}
				if (j == EAnimPhysicsSound_Num)
				{
					return false;
				}
				Event->PhysicsSound = (EAnimPhysicsSound)j;
				break;

			case EAnimEventType_AttackStart:
				// 攻撃判定開始イベントの場合は攻撃番号と攻撃位置番号を読み込み
				Event->AttackNo = GetTextParamInt(&TextParam, "Event%d_AttackNo", i);
				Event->AttackPosIndex = GetTextParamInt(&TextParam, "Event%d_AttackPosIndex", i);
				break;

			case EAnimEventType_AttackEnd:
				// 攻撃判定終了イベントの場合は攻撃番号を読み込み
				Event->AttackNo = GetTextParamInt(&TextParam, "Event%d_AttackNo", i);
				break;
			}
		}

		// 読み込みが成功したフラグを立てる
		LoadSuccess = true;
	}

	// 読み込みが成功したかどうかのフラグを返す
	return LoadSuccess;
}

// アニメーション情報をファイルに保存する
//     戻り値 : 保存に成功したかどうか
//              ( true:成功した  false:失敗した )
bool SaveAnimInfoFile(
	// アニメーション情報構造体のアドレス
	const SAnimInfo *AInfo,

	// アニメーション情報ファイルパス
	const char *FilePath
)
{
	SBinaryFileData       BinFileData;
	FILE                 *fp;
	const SAnimEventInfo *Event;
	char                  FilePathS[MAX_PATH];
	int                   i;

	// バイナリファイルの出力

	// 出力用のバイナリファイルを開く
	sprintf(FilePathS, "%s.dat", FilePath);
	if (!WriteBinFile_Open(&BinFileData, FilePathS))
	{
		char MessageString[1024];
		char FullPath[1024];

		ConvertFullPath(FilePathS, FullPath);
		sprintf(MessageString, "セーブに失敗しました", FullPath);
		MessageBox(NULL, MessageString, "セーブ失敗", MB_OK);
		return false;
	}

	// アニメーションイベントの数を書き出す
	WriteBinFile_Int(&BinFileData, AInfo->EventNum);

	// アニメーションイベントの数だけ繰り返し
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// アニメーションイベントタイプを書き出し
		WriteBinFile_Int(&BinFileData, Event->Type);

		// アニメーションイベントタイムを書き出し
		WriteBinFile_Float(&BinFileData, Event->TimeF);

		// アニメーションイベントタイプによって処理を分岐
		switch (Event->Type)
		{
		case EAnimEventType_Sound:
			// 音再生イベントの場合はサウンドファイルのパスを書き出し
			WriteBinFile_String(&BinFileData, Event->SoundPath);
			break;

		case EAnimEventType_PhysicsSound:
			// 物理音イベントの場合は物理音タイプを書き出し
			WriteBinFile_Int(&BinFileData, Event->PhysicsSound);
			break;

		case EAnimEventType_AttackStart:
			// 攻撃判定開始イベントの場合は攻撃番号と攻撃位置番号を書き出し
			WriteBinFile_Int(&BinFileData, Event->AttackNo);
			WriteBinFile_Int(&BinFileData, Event->AttackPosIndex);
			break;

		case EAnimEventType_AttackEnd:
			// 攻撃判定終了イベントの場合は攻撃番号を書き出し
			WriteBinFile_Int(&BinFileData, Event->AttackNo);
			break;
		}
	}

	// バイナリファイルを閉じる
	WriteBinFile_Close(&BinFileData);


	// テキストファイルの出力

	// 出力用のテキストファイルを開く
	sprintf(FilePathS, "%s.txt", FilePath);
	fp = fopen(FilePathS, "wt");
	if (fp == NULL)
	{
		char MessageString[1024];
		char FullPath[1024];

		ConvertFullPath(FilePathS, FullPath);
		sprintf(MessageString, "セーブに失敗しました", FullPath);
		MessageBox(NULL, MessageString, "セーブ失敗", MB_OK);
		return false;
	}

	// アニメーションイベントの数を書き出す
	fprintf(fp, "int EventNum = %d\n", AInfo->EventNum);

	// アニメーションイベントの数だけ繰り返し
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// アニメーションイベントタイプを書き出し
		fprintf(fp, "string Event%d_TypeName = %s\n", i,
			g_AnimEventTypeName[Event->Type]);

		// アニメーションイベントタイムを書き出し
		fprintf(fp, "float Event%d_Time = %.3f\n", i, Event->TimeF);

		// アニメーションイベントタイプによって処理を分岐
		switch (Event->Type)
		{
		case EAnimEventType_Sound:
			// 音再生イベントの場合はサウンドファイルのパスを書き出し
			fprintf(fp, "string Event%d_SoundPath = %s\n", i, Event->SoundPath);
			break;

		case EAnimEventType_PhysicsSound:
			// 物理音イベントの場合は物理音タイプを書き出し
			fprintf(fp, "string Event%d_PhysicsSoundType = %s\n", i,
				g_AnimPhysicsSoundName[Event->PhysicsSound]);
			break;

		case EAnimEventType_AttackStart:
			// 攻撃判定開始イベントの場合は攻撃番号と攻撃位置番号を書き出し
			fprintf(fp, "int Event%d_AttackNo = %d\n", i, Event->AttackNo);
			fprintf(fp, "int Event%d_AttackPosIndex = %d\n", i, Event->AttackPosIndex);
			break;

		case EAnimEventType_AttackEnd:
			// 攻撃判定終了イベントの場合は攻撃番号を書き出し
			fprintf(fp, "int Event%d_AttackNo = %d\n", i, Event->AttackNo);
			break;
		}

		// 一つのアニメーションイベント毎に改行を入れる
		fprintf(fp, "\n");
	}

	// テキストファイルを閉じる
	fclose(fp);

	// 成功終了
	return true;
}

// アニメーション情報で使用するサウンドファイルを読み込む
//     戻り値 : 読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
bool LoadAnimInfoSound(
	// アニメーション情報構造体のアドレス
	SAnimInfo *AInfo
)
{
	int             i;
	SAnimEventInfo *Event;
	char            FilePath[256];

	// アニメーションイベントの数だけ繰り返し
	Event = AInfo->Event;
	for (i = 0; i < AInfo->EventNum; i++, Event++)
	{
		// 音を再生するイベント以外の場合はなにもしない
		if (Event->Type != EAnimEventType_Sound)
		{
			continue;
		}

		// 読み込むサウンドファイルのパスを作成する
		strcpy(FilePath, ANIM_SE_FILE_DIRECTORY_PATH);
		strcat(FilePath, Event->SoundPath);

		// サウンドファイルの読み込み
		Event->SoundHandle = Sound_AddSound(FilePath, true);
		if (Event->SoundHandle < 0)
		{
			return false;
		}
	}

	// 成功終了
	return true;
}

// 指定の時間範囲にあるアニメーションイベントを取得する
void AnimInfoEventCheck(
	// アニメーション情報構造体のアドレス
	const SAnimInfo *AInfo,

	// 検索結果の情報を格納する構造体へのアドレス
	SAnimEventCheck *AECheck,

	// 検索開始時間
	float StartTime,

	// 検索終了時間
	float EndTime
)
{
	int                   i;
	const SAnimEventInfo *Event;
	int                   StartTimeI;
	int                   EndTimeI;

	// 検索開始時間と終了時間を整数値に変換
	StartTimeI = (int)(StartTime * ANIM_TIME_INT_SCALE);
	EndTimeI = (int)(EndTime   * ANIM_TIME_INT_SCALE);

	// 検索に引っかかったイベントの数を初期化する
	AECheck->EventNum = 0;

	// 開始時間が終了時間より大きな値の場合はアニメーションがループした場合の判定なので
	// 処理を分岐する
	if (EndTimeI >= 0 && StartTimeI > EndTimeI)
	{
		// イベントの数だけ繰り返す
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// 検索範囲外かどうかを判定
			if (StartTimeI > Event->TimeI && EndTimeI <= Event->TimeI)
			{
				continue;
			}

			// 検索範囲内だった場合は見つかったアニメーションイベントとして追加する
			AECheck->Event[AECheck->EventNum] = Event;
			AECheck->EventNum++;
		}
	}
	else
	{
		// イベントの数だけ繰り返す
		Event = AInfo->Event;
		for (i = 0; i < AInfo->EventNum; i++, Event++)
		{
			// 検索範囲外かどうかを判定
			if (StartTimeI > Event->TimeI || (EndTimeI >= 0 && EndTimeI <= Event->TimeI))
			{
				continue;
			}

			// 検索範囲内だった場合は見つかったアニメーションイベントとして追加する
			AECheck->Event[AECheck->EventNum] = Event;
			AECheck->EventNum++;
		}
	}
}

// アニメーションイベントタイプの名前を取得する
//     戻り値 : アニメーションイベントタイプ名文字列のアドレス
const char *GetAnimEventTypeName(
	// アニメーションイベントタイプ名
	EAnimEventType AnimEventType
)
{
	return g_AnimEventTypeName[AnimEventType];
}

// アニメーション物理音タイプの名前を取得する
//     戻り値 : アニメーション物理音タイプ名文字列のアドレス
const char *GetAnimPhysicsSoundName(
	// アニメーション物理音タイプ
	EAnimPhysicsSound AnimPhysicsSound
)
{
	return g_AnimPhysicsSoundName[AnimPhysicsSound];
}
