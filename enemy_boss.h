//=============================================================================
//
// Boss���� [enemy_boss.h]
// Author : �с@���
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BOSS_MAX		(1)		// �G�l�~�[��Max�l��
#define BOSS_HP_MAX		(100)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BOSS
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	atkPos;			// �U���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		atkW, atkH;		// �U���̕��ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x
	int			dir;			// �ړ�����


	BOOL		idle;			// ���ʏ�ԃt���O
	int			idleCnt;
	BOOL		hurt;			// �_���[�W�t���O
	int			hurtCnt;		// �_���[�W�J�E���g
	BOOL		death;
	int			deathCnt;
	BOOL		attack;			// �U���t���O	
	int			attackCnt;
	BOOL		SpAtk;			// ����U���t���O	
	int			SpAtkCnt;
	BOOL		ToLeftDown;		// �s���p�^��
	BOOL		ToRightDown;
	BOOL		ToLeftUp;
	BOOL		ToRightUp;
	BOOL		ToCenter;
	BOOL		ToPlayer;
	int			moveCnt;

	HitBox		hb[2];			// �q�b�g�{�b�N�X
	HitBox		atkHB;			// �U���q�b�g�{�b�N�X

	int			HP;				// HP
	int			state;			// �s�����


	float		time;			// ���`��ԗp
	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	//INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	//int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	//float				move_time;			// ���s����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyBoss(void);
void UninitEnemyBoss(void);
void UpdateEnemyBoss(void);
void DrawEnemyBoss(void);

BOSS* GetBoss(void);
