//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : �с@���
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(20)		// �G�l�~�[��Max�l��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x
	int			dir;			// �ړ�����

	BOOL		hurt;			// �_���[�W�t���O
	int			hurtCnt;		// �_���[�W�J�E���g
	BOOL		idle;			// ���ʏ�ԃt���O

	BOOL		attack;			// �U���t���O	

	XMFLOAT3	HBpos;			// �q�b�g�{�b�N�X�̍��W
	float		HBw, HBh;		// �q�b�g�{�b�N�X�̕��ƍ���


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
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetSkull(void);


