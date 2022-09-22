//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : �с@���
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX		(100)		// �o���b�g��Max��
#define BULLET_SPEED	(6.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct BULLET
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	float				HBw, HBh;			// �q�b�g�{�b�N�X�̕��ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			HBpos;				// �q�b�g�{�b�N�X�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	float				angle;				

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(XMFLOAT3 pos, float angle);


