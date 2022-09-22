//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 林　劭羲
// 
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_BOSS_FIGHT,	// BGM0
	SOUND_LABEL_BGM_TUTORIAL,	// BGM1
	SOUND_LABEL_BGM_STAGE001,	// BGM2
	SOUND_LABEL_BGM_TITLE,		// 
	SOUND_LABEL_BGM_GAME_OVER,	// 
	SOUND_LABEL_BGM_GAME_CLEAR,	// 
	SOUND_LABEL_SE_walk,		// 
	SOUND_LABEL_SE_player_explo,// 
	SOUND_LABEL_SE_sword_hit,	// 
	SOUND_LABEL_SE_player_hitten, // 
	SOUND_LABEL_SE_player_dash,		// 
	SOUND_LABEL_SE_sword_miss,		// BGM Maou
	SOUND_LABEL_SE_boss_atk,
	SOUND_LABEL_SE_boss_move,
	SOUND_LABEL_SE_boss_spatk,

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

