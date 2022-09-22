//=============================================================================
//
// BG���� [bg.h]
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
struct BG
{
	XMFLOAT3	pos;		// �|���S���̍��W
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	float		scrl;		// �X�N���[��
	XMFLOAT3	old_pos;	// �P�t���O�̃|���S���̍��W
	float		scrl2;		// �X�N���[��
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


#define	SKY_H					(150.0f)	// ��̃|���S���̍���

#define TERRAIN_HITBOX_MAX		(20)		// �n�`�̃q�b�g�{�b�N�X�̐�
#define MSG_HITBOX_MAX			(4)		// ���b�Z�[�W�̃q�b�g�{�b�N�X�̐�
#define MOVE_POINT_MAX			(5)		// �]���|�C���g�̃q�b�g�{�b�N�X�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
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



