//=============================================================================
//
// Bossステージ処理 [boss_stage.cpp]
// Author : 林　劭羲
//
//=============================================================================
#include "boss_stage.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "score.h"
#include "UI.h"
#include "enemy_boss.h"

//=============================================================================
// 初期化処理
//=============================================================================
void InitBossStage(void)
{
	InitBG();
	InitPlayer();
	InitEnemyBoss();
	InitBullet();
	InitUI();
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBossStage(void)
{
	UninitBG();
	UninitPlayer();
	UninitEnemyBoss();
	UninitBullet();
	UninitUI();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBossStage(void)
{
	UpdateBG();
	UpdatePlayer();
	UpdateEnemyBoss();
	UpdateBullet();
	UpdateUI();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBossStage(void)
{
	DrawBG();
	DrawEnemyBoss();
	DrawPlayer();
	DrawBullet();
	DrawUI();
}
