//=============================================================================
//
// Boss処理 [enemy_boss.h]
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
#define BOSS_MAX		(1)		// エネミーのMax人数
#define BOSS_HP_MAX		(100)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BOSS
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	atkPos;			// 攻撃の座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		atkW, atkH;		// 攻撃の幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度
	int			dir;			// 移動方向


	BOOL		idle;			// 普通状態フラグ
	int			idleCnt;
	BOOL		hurt;			// ダメージフラグ
	int			hurtCnt;		// ダメージカウント
	BOOL		death;
	int			deathCnt;
	BOOL		attack;			// 攻撃フラグ	
	int			attackCnt;
	BOOL		SpAtk;			// 特殊攻撃フラグ	
	int			SpAtkCnt;
	BOOL		ToLeftDown;		// 行動パタン
	BOOL		ToRightDown;
	BOOL		ToLeftUp;
	BOOL		ToRightUp;
	BOOL		ToCenter;
	BOOL		ToPlayer;
	int			moveCnt;

	HitBox		hb[2];			// ヒットボックス
	HitBox		atkHB;			// 攻撃ヒットボックス

	int			HP;				// HP
	int			state;			// 行動状態


	float		time;			// 線形補間用
	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	//INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	//int					tbl_size;			// 登録したテーブルのレコード総数
	//float				move_time;			// 実行時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyBoss(void);
void UninitEnemyBoss(void);
void UpdateEnemyBoss(void);
void DrawEnemyBoss(void);

BOSS* GetBoss(void);
