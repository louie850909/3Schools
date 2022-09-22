//=============================================================================
//
// �v���C���[���� [player.h]
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
#define PLAYER_MAX			(1)		// �v���C���[��Max�l��

#define PLAYER_HP_MAX		(10)	// HP�ő吔�l

#define	PLAYER_OFFSET_CNT	(16)	// 16���g

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
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	int			HP;				// HP

	BOOL		idle;			// �ҋ@�t���O

	int			dir;			// �����i0:�� 1:�E 2:�� 3:���j
	BOOL		moving;			// �ړ����t���O

	BOOL		dash;			// �_�b�V�����t���O
	int			dashCnt;		// �_�b�V�����̃J�E���g

	XMFLOAT3	move;			// �ړ����x
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W

	BOOL		jump;			// �W�����v�t���O
	float		jumpY;			// �W�����v�̍���
	int			jumpCnt;		// �W�����v���̃J�E���g
	int			Multijump;       // ��d�W�����v

	BOOL		FALL;			// �����t���O

	BOOL		hurt;			// �_���[�W�t���O
	int			hurtCnt;		// �_���[�W�J�E���g

	BOOL		recovery;		// �񕜃t���O
	int			recoveryCnt;	// �񕜃J�E���g

	BOOL		attack;			// �U���t���O
	int			attackCnt;		// �U���J�E���g
	XMFLOAT3	atkHBpos;		// �U���q�b�g�{�b�N�X�̍��W
	float		atkHBw, atkHBh;	// �U���q�b�g�{�b�N�X�̕��ƍ���

	BOOL		death;			// ���S�t���O
	int			deathCnt;		// ���S�J�E���g

	XMFLOAT3	HBpos;			// �v���C���[�q�b�g�{�b�N�X�̍��W
	float		HBw, HBh;		// �v���C���[�q�b�g�{�b�N�X�̕��ƍ���

	BOOL		noDamage;		// ���G�t���O

	BOOL		msg[MSG_HITBOX_MAX]; // ���b�Z�[�W�t���O

	int			state;			// �v���C���[�̃X�e�[�^�X
	int			stage;			// 
	int			preStage;		//
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);
