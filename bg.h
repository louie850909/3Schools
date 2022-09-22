//=============================================================================
//
// BG処理 [bg.h]
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
struct BG
{
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号

	float		scrl;		// スクロール
	XMFLOAT3	old_pos;	// １フレ前のポリゴンの座標
	float		scrl2;		// スクロール
};

struct HitBox
{
	BOOL		use;
	XMFLOAT3	HBpos;
	float		HBw, HBh;
};

struct MovePoint
{
	HitBox		hitbox;
	int			destination;
};


#define	SKY_H					(150.0f)	// 空のポリゴンの高さ

#define TERRAIN_HITBOX_MAX		(20)		// 地形のヒットボックスの数
#define MSG_HITBOX_MAX			(4)		// メッセージのヒットボックスの数
#define MOVE_POINT_MAX			(5)		// 転送ポイントのヒットボックスの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);

BG* GetBG(void);
HitBox* GetTHB(void);
HitBox* GetDZ(void);
HitBox* GetMSG(void);
MovePoint* GetMVP(void);



