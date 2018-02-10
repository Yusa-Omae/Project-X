#ifndef ANIMATION_H
#define ANIMATION_H

#include "Sound.h"

// アニメーションイベントのサウンドファイルパスで使用できる最大文字数
#define ANIM_EVENT_SOUNDPATH_MAX_LENGTH			(64)

// アニメーションイベントの最大数
#define ANIM_EVENT_MAX_NUM						(64)

// アニメーションのタイムを整数型に変換する際のスケール
#define ANIM_TIME_INT_SCALE						(10000.0f)

// アニメーションで再生する効果音のファイルが格納されているフォルダ
#define ANIM_SE_FILE_DIRECTORY_PATH				"SE\\"

// アニメーションイベントのタイプ
typedef enum _EAnimEventType
{
	EAnimEventType_Sound,			// 音を再生
	EAnimEventType_PhysicsSound,	// 物理音を再生
	EAnimEventType_AnimCancel,		// アニメーションキャンセル
	EAnimEventType_AtkStart,		// 攻撃判定開始
	EAnimEventType_AtkEnd,		// 攻撃判定終了
	EAnimEventType_Other,			// その他

	EAnimEventType_Num				// アニメーションイベントのタイプの数
} EAnimEventType;

// アニメーション物理音のタイプ
typedef enum _EAnimPhysicsSound
{
	EAnimPhysicsSound_Footstep,		// 足音
	EAnimPhysicsSound_Down,			// 倒れる

	EAnimPhysicsSound_Num			// アニメーション物理音のタイプの数
} EAnimPhysicsSound;

// アニメーションイベント情報
typedef struct _SAnimEventInfo
{
	// アニメーションイベントタイプ
	EAnimEventType         Type;

	// イベントタイムの浮動小数点型値
	float                  TimeF;

	// イベントタイムの整数型値
	int                    TimeI;

	// 攻撃番号
	// ( Type が EAnimEventType_AtkStart 又は EAnimEventType_AtkEnd の場合に使用 )
	int                    AtkNo;

	// 攻撃位置番号
	// ( Type が EAnimEventType_AtkStart の場合に使用 )
	int                    AtkPosIndex;

	// アニメーション物理音タイプ
	// ( Type が EAnimEventType_PhysicsSound の場合に使用 )
	EAnimPhysicsSound      PhysicsSound;

	// サウンドファイルのパス
	// ( Type が EAnimEventType_Sound の場合に使用 )
	char                   SoundPath[ANIM_EVENT_SOUNDPATH_MAX_LENGTH];

	// サウンドハンドル
	// ( Type が EAnimEventType_Sound の場合に使用 )
	int                    SoundHandle;
} SAnimEventInfo;

// アニメーション情報
typedef struct _SAnimInfo
{
	// イベント情報の数
	int                    EventNum;

	// イベント情報の配列
	SAnimEventInfo         Event[ANIM_EVENT_MAX_NUM];

	// 内容が変更されたかどうかのフラグ( ツールで使用 )
	bool                   Change;
} SAnimInfo;

// 指定時間範囲のアニメーションイベント取得用情報
typedef struct _SAnimEventCheck
{
	// 指定時間範囲のアニメーションイベントの数
	int                    EventNum;

	// 指定時間範囲のアニメーションイベント情報へのポインタ
	const SAnimEventInfo * Event[ANIM_EVENT_MAX_NUM];
} SAnimEventCheck;


// アニメーション情報をファイルから読み込む
//     戻り値 : 読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool LoadAnimInfoFile(
	// アニメーション情報構造体のアドレス
	SAnimInfo *AInfo,

	// アニメーション情報ファイルパス
	const char *FilePath
);

// アニメーション情報をファイルに保存する
//     戻り値 : 保存に成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool SaveAnimInfoFile(
	// アニメーション情報構造体のアドレス
	const SAnimInfo *AInfo,

	// アニメーション情報ファイルパス
	const char *FilePath
);

// アニメーション情報で使用するサウンドファイルを読み込む
//     戻り値 : 読み込みに成功したかどうか
//              ( true:成功した  false:失敗した )
extern bool LoadAnimInfoSound(
	// アニメーション情報構造体のアドレス
	SAnimInfo *AInfo
);

// 指定の時間範囲にあるアニメーションイベントを取得する
extern void AnimInfoEventCheck(
	// アニメーション情報構造体のアドレス
	const SAnimInfo *AInfo,

	// 検索結果の情報を格納する構造体へのアドレス
	SAnimEventCheck *AECheck,

	// 検索開始時間
	float StartTime,

	// 検索終了時間
	float EndTime
);

// アニメーションイベントタイプの名前を取得する
//     戻り値 : アニメーションイベントタイプ名文字列のアドレス
extern const char *GetAnimEventTypeName(
	// アニメーションイベントタイプ名
	EAnimEventType AnimEventType
);

// アニメーション物理音タイプの名前を取得する
//     戻り値 : アニメーション物理音タイプ名文字列のアドレス
extern const char *GetAnimPhysicsSoundName(
	// アニメーション物理音タイプ
	EAnimPhysicsSound AnimPhysicsSound
);

#endif
