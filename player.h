//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 林　劭羲
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define PLAYER_HP_MAX		(10)	// HP最大数値

#define	PLAYER_OFFSET_CNT	(16)	// 16分身

enum DIRECTION
{
	CHAR_DIR_UP,
	CHAR_DIR_RIGHT,
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,

	CHAR_DIR_MAX
};

enum STATE
{
	IDLE,
	JUMP,
	RUN_RIGHT,
	RUN_LEFT,
	WALL_SLIDE,
	FALL,
	HURT,
	DEATH,
	ATTACK,
	DASH,

	STATE_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	int			HP;				// HP

	BOOL		idle;			// 待機フラグ

	int			dir;			// 向き（0:上 1:右 2:下 3:左）
	BOOL		moving;			// 移動中フラグ

	BOOL		dash;			// ダッシュ中フラグ
	int			dashCnt;		// ダッシュ中のカウント

	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	int			Multijump;       // 二重ジャンプ

	BOOL		FALL;			// 落下フラグ

	BOOL		hurt;			// ダメージフラグ
	int			hurtCnt;		// ダメージカウント

	BOOL		recovery;		// 回復フラグ
	int			recoveryCnt;	// 回復カウント

	BOOL		attack;			// 攻撃フラグ
	int			attackCnt;		// 攻撃カウント
	XMFLOAT3	atkHBpos;		// 攻撃ヒットボックスの座標
	float		atkHBw, atkHBh;	// 攻撃ヒットボックスの幅と高さ

	BOOL		death;			// 死亡フラグ
	int			deathCnt;		// 死亡カウント

	XMFLOAT3	HBpos;			// プレイヤーヒットボックスの座標
	float		HBw, HBh;		// プレイヤーヒットボックスの幅と高さ

	BOOL		noDamage;		// 無敵フラグ

	BOOL		msg[MSG_HITBOX_MAX]; // メッセージフラグ

	int			state;			// プレイヤーのステータス
	int			stage;			// 
	int			preStage;		//
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);
