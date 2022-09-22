//=============================================================================
//
// Boss�X�e�[�W���� [boss_stage.cpp]
// Author : �с@���
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
// ����������
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
// �I������
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
// �X�V����
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
// �`�揈��
//=============================================================================
void DrawBossStage(void)
{
	DrawBG();
	DrawEnemyBoss();
	DrawPlayer();
	DrawBullet();
	DrawUI();
}
