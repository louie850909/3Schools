//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : 林　劭羲
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(20)		// エネミーのMax人数


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度
	int			dir;			// 移動方向

	BOOL		hurt;			// ダメージフラグ
	int			hurtCnt;		// ダメージカウント
	BOOL		idle;			// 普通状態フラグ

	BOOL		attack;			// 攻撃フラグ	

	XMFLOAT3	HBpos;			// ヒットボックスの座標
	float		HBw, HBh;		// ヒットボックスの幅と高さ


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
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetSkull(void);



