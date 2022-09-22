//=============================================================================
//
// Boss���� [enemy_boss.cpp]
// Author : �с@���
//
//=============================================================================
#include "enemy_boss.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "input.h"
#include "bullet.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(200)	// 
#define TEXTURE_MAX					(6)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define frameNum					(5)		// ���R�}5�t���[���ɂ��~�܂�
#define idleFrameNum				(6)		// idle�t���[����
#define atkFrameNum					(8)
#define DeathFrameNum				(22)		// ���S�t���[����


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Monster/demon-idle.png",
	"data/TEXTURE/Monster/boss_death.png",
	"data/TEXTURE/Monster/bg.png",
	"data/TEXTURE/Monster/demon-attack.png",
	"data/TEXTURE/Monster/breath-fire.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BOSS	g_Boss[BOSS_MAX];		// �G�l�~�[�\����

static INTERPOLATION_DATA g_ToRightDownTbl[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1750.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1750.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToLeftDownTbl[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(150.0f,  750.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(150.0f,  750.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToLeftUpTbl[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(300.0f,  450.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(300.0f,  500.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToRightUpTbl[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1650.0f, 620.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1650.0f, 620.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToCenterTbl[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1920.0f / 2, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},	
	{ XMFLOAT3(1920.0f / 2, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},
};

static INTERPOLATION_DATA g_ToPlayerTbl[] = {
	//���W									��]��							�g�嗦							����
	{ g_Boss[0].pos,						XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},
	{ XMFLOAT3(1050.0f, 700.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_ToRightDownTbl,
	g_ToLeftDownTbl,
	g_ToLeftUpTbl,
	g_ToRightUpTbl,
	g_ToCenterTbl,
	g_ToPlayerTbl,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyBoss(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_Boss[i].use = TRUE;
		g_Boss[i].pos = XMFLOAT3(1920.0f / 2, 800.0f, 0.0f);	// ���S�_����\��
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss[i].w = TEXTURE_WIDTH;
		g_Boss[i].h = TEXTURE_HEIGHT;
		g_Boss[i].texNo = 0;

		g_Boss[i].hb[0].HBpos = XMFLOAT3(g_Boss[i].pos.x + 5.0f, g_Boss[i].pos.y + 15.0f, g_Boss[i].pos.z);
		g_Boss[i].hb[0].HBw = 75.0f;
		g_Boss[i].hb[0].HBh = 125.0f;

		g_Boss[i].hb[1].HBpos = XMFLOAT3(g_Boss[i].pos.x - 15.0f, g_Boss[i].pos.y - 10.0f, g_Boss[i].pos.z);
		g_Boss[i].hb[1].HBw = 200.0f;
		g_Boss[i].hb[1].HBh = 60.0f;

		g_Boss[i].countAnim = 0;
		g_Boss[i].patternAnim = 0;

		g_Boss[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
		g_Boss[i].dir = DIRECTION::CHAR_DIR_RIGHT;

		g_Boss[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Boss[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Boss[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Boss[i].idle = TRUE;
		g_Boss[i].hurt = FALSE;
		g_Boss[i].hurtCnt = 0;

		g_Boss[i].idle = TRUE;
		g_Boss[i].idleCnt = 0;
		g_Boss[i].hurt = FALSE;
		g_Boss[i].hurtCnt = 0;
		g_Boss[i].death = FALSE;
		g_Boss[i].deathCnt = 0;
		g_Boss[i].attack = FALSE;
		g_Boss[i].attackCnt = 0;
		g_Boss[i].SpAtk = FALSE;
		g_Boss[i].SpAtkCnt = 0;
		g_Boss[i].ToLeftDown = FALSE;
		g_Boss[i].ToRightDown = FALSE;
		g_Boss[i].ToLeftUp = FALSE;
		g_Boss[i].ToRightUp = FALSE;
		g_Boss[i].ToCenter = FALSE;
		g_Boss[i].ToPlayer = FALSE;
		g_Boss[i].moveCnt = 0;

		g_Boss[i].HP = BOSS_HP_MAX;

		g_Boss[i].atkPos = g_Boss[i].pos;
		g_Boss[i].atkH = 200.0f;
		g_Boss[i].atkW = 200.0f;

		g_Boss[i].atkHB.HBpos = g_Boss[i].pos;
		g_Boss[i].atkHB.HBh = 125.0f;
		g_Boss[i].atkHB.HBw = 140.0f;
		g_Boss[i].atkHB.use = FALSE;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyBoss(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

void UpdateEnemyBoss(void)
{
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE)
		{
			PLAYER* player = GetPlayer();

			if (g_Boss[i].pos.x > player[0].pos.x)
			{
				g_Boss[i].dir = DIRECTION::CHAR_DIR_LEFT;
			}
			else
			{
				g_Boss[i].dir = DIRECTION::CHAR_DIR_RIGHT;
			}

			if (g_Boss[i].dir == DIRECTION::CHAR_DIR_RIGHT)
			{
				g_Boss[i].hb[0].HBpos = XMFLOAT3(g_Boss[i].pos.x + 5.0f, g_Boss[i].pos.y + 15.0f, g_Boss[i].pos.z);
				g_Boss[i].hb[1].HBpos = XMFLOAT3(g_Boss[i].pos.x - 15.0f, g_Boss[i].pos.y - 10.0f, g_Boss[i].pos.z);
				g_Boss[i].atkPos = XMFLOAT3(g_Boss[i].hb[0].HBpos.x + 85.0f, g_Boss[i].hb[0].HBpos.y + 60.0f, g_Boss[i].hb[0].HBpos.z);
				g_Boss[i].atkHB.HBpos = XMFLOAT3(g_Boss[i].hb[0].HBpos.x + 85.0f, g_Boss[i].hb[0].HBpos.y + 60.0f, g_Boss[i].hb[0].HBpos.z);
			}
			else if (g_Boss[i].dir == DIRECTION::CHAR_DIR_LEFT)
			{
				g_Boss[i].hb[0].HBpos = XMFLOAT3(g_Boss[i].pos.x - 20.0f, g_Boss[i].pos.y + 15.0f, g_Boss[i].pos.z);
				g_Boss[i].hb[1].HBpos = XMFLOAT3(g_Boss[i].pos.x, g_Boss[i].pos.y - 10.0f, g_Boss[i].pos.z);
				g_Boss[i].atkPos = XMFLOAT3(g_Boss[i].hb[0].HBpos.x - 85.0f, g_Boss[i].hb[0].HBpos.y + 60.0f, g_Boss[i].hb[0].HBpos.z);
				g_Boss[i].atkHB.HBpos = XMFLOAT3(g_Boss[i].hb[0].HBpos.x - 85.0f, g_Boss[i].hb[0].HBpos.y +60.0f, g_Boss[i].hb[0].HBpos.z);
			}

			if (g_Boss[i].idle == TRUE)
			{
				g_Boss[i].idleCnt++;
				if (g_Boss[i].idleCnt >= 180)
				{
					PlaySound(SOUND_LABEL_SE_boss_move);
					g_Boss[i].ToPlayer = TRUE;
					g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
					g_Boss[i].moveCnt = 0;
					g_Boss[i].patternAnim = 0;
					g_Boss[i].time = 0;
					g_Boss[i].idle = FALSE;
					g_Boss[i].idleCnt = 0;
				}
			}
			else if (g_Boss[i].attack == TRUE)
			{
				g_Boss[i].attackCnt++;
				if (g_Boss[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					if (g_Boss[i].patternAnim == 4)
					{
						PlaySound(SOUND_LABEL_SE_boss_atk);
					}

					if (g_Boss[i].patternAnim >= 6)
					{
						g_Boss[i].atkHB.use = TRUE;
					}
					else
					{
						g_Boss[i].atkHB.use = FALSE;
					}
				}
				else
				{
					if (g_Boss[i].patternAnim == 5)
					{
						PlaySound(SOUND_LABEL_SE_boss_atk);
					}

					if (g_Boss[i].patternAnim >= 7)
					{
						g_Boss[i].atkHB.use = TRUE;
					}
					else
					{
						g_Boss[i].atkHB.use = FALSE;
					}
				}

				if (g_Boss[i].attackCnt >= 120)
				{
					int next = rand() % 6;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToCenter = TRUE;
						g_ToCenterTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].idle = TRUE;
						g_Boss[i].idleCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 2:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToLeftDown = TRUE;
						g_ToLeftDownTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 3:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToLeftUp = TRUE;
						g_ToLeftUpTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 4:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToRightDown = TRUE;
						g_ToRightDownTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 5:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToRightUp = TRUE;
						g_ToRightUpTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;
					}

					g_Boss[i].attack = FALSE;
					g_Boss[i].atkHB.use = FALSE;
					g_Boss[i].attackCnt = 0;
					g_Boss[i].time = 0;
				}
			}
			else if (g_Boss[i].ToCenter)
			{
				g_Boss[i].moveCnt++;

				g_Boss[0].tblMax = sizeof(g_ToCenterTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[4];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					int next = rand() % 2;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToPlayer = TRUE;
						g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].SpAtk = TRUE;
						g_Boss[i].SpAtkCnt = 0;
						g_Boss[i].patternAnim = 0;
					}
					g_Boss[i].time = 0;
					g_Boss[i].ToCenter = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].ToPlayer)
			{
				g_Boss[i].moveCnt++;
				if (g_Boss[i].pos.x > player[0].pos.x)
				{
					g_ToPlayerTbl[1] = { XMFLOAT3(player[0].pos.x + 150.0f, player[0].pos.y - 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120};
				}
				else
				{
					g_ToPlayerTbl[1] = { XMFLOAT3(player[0].pos.x - 150.0f, player[0].pos.y - 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
				}
				g_Boss[0].tblMax = sizeof(g_ToPlayerTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[5];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					PlaySound(SOUND_LABEL_SE_boss_move);
					g_Boss[i].attack = TRUE;
					g_Boss[i].attackCnt = 0;
					g_Boss[i].patternAnim = 0;
					g_Boss[i].time = 0;
					g_Boss[i].ToPlayer = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].ToLeftDown)
			{
				g_Boss[i].moveCnt++;

				g_Boss[0].tblMax = sizeof(g_ToLeftDownTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[1];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					int next = rand() % 2;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToPlayer = TRUE;
						g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].SpAtk = TRUE;
						g_Boss[i].SpAtkCnt = 0;
						g_Boss[i].patternAnim = 0;
					}
					g_Boss[i].time = 0;
					g_Boss[i].ToLeftDown = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].ToLeftUp)
			{
				g_Boss[i].moveCnt++;

				g_Boss[0].tblMax = sizeof(g_ToLeftUpTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[2];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					int next = rand() % 2;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToPlayer = TRUE;
						g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].SpAtk = TRUE;
						g_Boss[i].SpAtkCnt = 0;
						g_Boss[i].patternAnim = 0;
					}
					g_Boss[i].time = 0;
					g_Boss[i].ToLeftUp = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].ToRightDown)
			{
				g_Boss[i].moveCnt++;

				g_Boss[0].tblMax = sizeof(g_ToRightDownTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[0];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					int next = rand() % 2;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToPlayer = TRUE;
						g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].SpAtk = TRUE;
						g_Boss[i].SpAtkCnt = 0;
						g_Boss[i].patternAnim = 0;
					}
					g_Boss[i].time = 0;
					g_Boss[i].ToRightDown = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].ToRightUp)
			{
				g_Boss[i].moveCnt++;

				g_Boss[0].tblMax = sizeof(g_ToRightUpTbl) / sizeof(INTERPOLATION_DATA);

				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[3];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

				if (g_Boss[i].moveCnt >= 120)
				{
					int next = rand() % 2;
					switch (next)
					{
					case 0:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].ToPlayer = TRUE;
						g_ToPlayerTbl[0] = { g_Boss[0].pos,	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	120 };
						g_Boss[i].moveCnt = 0;
						g_Boss[i].patternAnim = 0;
						break;

					case 1:
						PlaySound(SOUND_LABEL_SE_boss_move);
						g_Boss[i].SpAtk = TRUE;
						g_Boss[i].SpAtkCnt = 0;
						g_Boss[i].patternAnim = 0;
					}
					g_Boss[i].time = 0;
					g_Boss[i].ToRightUp = FALSE;
					g_Boss[i].moveCnt = 0;
				}
			}
			else if (g_Boss[i].SpAtk)
			{
				g_Boss[i].SpAtkCnt++;

				if (g_Boss[i].SpAtkCnt == 24)
				{
					PlaySound(SOUND_LABEL_SE_boss_spatk);
				}

				if (g_Boss[i].SpAtkCnt == 36)
				{
					if (g_Boss[i].HP > 50)
					{
						for (int j = 0; j < 8; j++)
						{
							SetBullet(XMFLOAT3(g_Boss[i].pos.x + 100 * cosf(j * 45 * (XM_PI / 180)), g_Boss[i].pos.y + 100 * sinf(j * 45 * (XM_PI / 180)), 0.0f), (j * 45));
						}
					}
					else
					{
						for (int j = 0; j < 12; j++)
						{
							SetBullet(XMFLOAT3(g_Boss[i].pos.x + 100 * cosf(j * 30 * (XM_PI / 180)), g_Boss[i].pos.y + 100 * sinf(j * 30 * (XM_PI / 180)), 0.0f), (j * 30));
						}
					}
				}
				
				if (g_Boss[i].SpAtkCnt >= 120)
				{
					PlaySound(SOUND_LABEL_SE_boss_move);
					g_Boss[i].idle = TRUE;
					g_Boss[i].idleCnt = 0;
					g_Boss[i].patternAnim = 0;
					g_Boss[i].SpAtk = FALSE;
					g_Boss[i].atkHB.use = FALSE;
					g_Boss[i].SpAtkCnt = 0;
					g_Boss[i].time = 0;
				}
			}

			// �U�����ꂽ����
			for (int j = 0; j < 2; j++)
			{
				if ((player[0].use == TRUE) && (player[0].attack == TRUE) && (player[0].patternAnim > 5))
				{
					BOOL ans = CollisionBB(g_Boss[i].hb[j].HBpos, g_Boss[j].hb[j].HBw, g_Boss[i].hb[j].HBh,
						player[0].atkHBpos, player[0].atkHBw, player[0].atkHBh);
					// �������Ă���H
					if ((ans == TRUE) && (g_Boss[i].hurt == FALSE))
					{
						g_Boss[i].hurt = TRUE;
						g_Boss[i].HP -= 10;
						PlaySound(SOUND_LABEL_SE_sword_hit);
					}
				}
			}
			if (g_Boss[i].hurt)
			{
				g_Boss[i].hurtCnt++;
				if (g_Boss[i].hurtCnt >= 60)
				{
					g_Boss[i].hurt = FALSE;
					g_Boss[i].hurtCnt = 0;
				}
			}

			// �U������
			if (g_Boss[i].atkHB.use == TRUE)
			{
				if ((player[0].noDamage == FALSE))
				{
					BOOL ans = CollisionBB(player[0].HBpos, player[0].HBw, player[0].HBh,
						g_Boss[i].atkHB.HBpos, g_Boss[i].atkHB.HBw, g_Boss[i].atkHB.HBh);
					// �������Ă���H
					if ((ans == TRUE) && (player[0].hurt == FALSE))
					{
						// �����������̏���
						player[0].hurt = TRUE;
						player[0].hurtCnt = 0;
						player[0].patternAnim = 0;
						player[0].noDamage = TRUE;
						player[0].HP -= 2;
						PlaySound(SOUND_LABEL_SE_player_hitten);
					}
				}
			}

			if (g_Boss[i].HP <=0)
			{
				g_Boss[i].death = TRUE;
				g_Boss[i].idle = FALSE;
				g_Boss[i].ToCenter = FALSE;
				g_Boss[i].ToLeftDown = FALSE;
				g_Boss[i].ToLeftUp = FALSE;
				g_Boss[i].ToPlayer = FALSE;
				g_Boss[i].ToRightDown = FALSE;
				g_Boss[i].ToRightUp = FALSE;
				g_Boss[i].attack = FALSE;
				g_Boss[i].SpAtk = FALSE;
			}
						
		}


#ifdef _DEBUG	// �f�o�b�O����\������

		PrintDebugProc("BOSS Idle:    %d\n", g_Boss[i].idle);
		PrintDebugProc("BOSS Attack:  %d\n", g_Boss[i].attack);
		PrintDebugProc("BOSS ToPlayer:  %d\n", g_Boss[i].ToPlayer);
		PrintDebugProc("BOSS attackCnt:  %d\n", g_Boss[i].attackCnt);
		PrintDebugProc("BOSS  POS  X  :  %f\n", g_Boss[i].pos.x);
		PrintDebugProc("BOSS  POS  Y  :  %f\n", g_Boss[i].pos.y);
		PrintDebugProc("BOSS moveCnt:  %d\n", g_Boss[i].moveCnt);


#endif
	}
}

void DrawEnemyBoss(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	if (g_Boss[0].use)
	{
		float px = 0.0f;
		float py = 0.0f;
		float pw = 0.0f;
		float ph = 0.0f;
		float tw = 0.0f;
		float th = 0.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		if ((g_Boss[0].idle) || 
			(g_Boss[0].ToCenter)||
			(g_Boss[0].ToPlayer)||
			(g_Boss[0].ToLeftUp)||
			(g_Boss[0].ToLeftDown)||
			(g_Boss[0].ToRightUp)||
			(g_Boss[0].ToRightDown))
		{
			// �A�j���[�V����  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// ���̃t���[����
					g_Boss[0].patternAnim += 1;
				}
			}

			if (g_Boss[0].patternAnim >= idleFrameNum)
			{
				g_Boss[0].patternAnim = 0;
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Boss[0].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			px = g_Boss[0].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			py = g_Boss[0].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			pw = g_Boss[0].w;		// �G�l�~�[�̕\����
			ph = g_Boss[0].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			if (g_Boss[0].dir == DIRECTION::CHAR_DIR_LEFT)
			{
				tw = 1.0f / idleFrameNum;	// �e�N�X�`���̕�
				tx = (float)(g_Boss[0].patternAnim) * tw;	// �e�N�X�`���̍���X���W

			}
			else if (g_Boss[0].dir == DIRECTION::CHAR_DIR_RIGHT)
			{
				tw = -1.0f / idleFrameNum;	// �e�N�X�`���̕�
				px -= 15.0f;				// �`��ʒu�̒���
				tx = (float)(idleFrameNum - g_Boss[0].patternAnim) * tw;	// �e�N�X�`���̍���X���W
			}

			th = 1.0f / 1;	// �e�N�X�`���̍���
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[0].rot.z);

			if (g_Boss[0].hurt)
			{
				if ((g_Boss[0].hurtCnt / 10) % 2 == 1)
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f),
						g_Boss[0].rot.z);
				}
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		else if (g_Boss[0].death)
		{

			// �A�j���[�V����  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// ���̃t���[����
					g_Boss[0].patternAnim += 1;
				}
			}

			if (g_Boss[0].patternAnim >= DeathFrameNum)
			{
				g_Boss[0].use = FALSE;
				SetFade(FADE_OUT, MODE_CLEAR);
			}

			if (g_Boss[0].patternAnim >= 14 && g_Boss[0].patternAnim <= 18)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				SetSpriteColorRotation(g_VertexBuffer, g_Boss[0].pos.x - bg->pos.x, g_Boss[0].pos.y - bg->pos.y, 3840, 2160, 0, 0, 1, 1,
					XMFLOAT4(1.0f,1.0f,1.0f, 1.0f),
					g_Boss[0].rot.z);

				GetDeviceContext()->Draw(4, 0);
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			px = g_Boss[0].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			py = g_Boss[0].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			pw = g_Boss[0].w;		// �G�l�~�[�̕\����
			ph = g_Boss[0].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			tw = 1.0f / DeathFrameNum;	// �e�N�X�`���̕�
			tx = (float)(g_Boss[0].patternAnim) * tw;	// �e�N�X�`���̍���X���W
			th = 1.0f / 1;	// �e�N�X�`���̍���
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[0].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		else if ((g_Boss[0].attack) || (g_Boss[0].SpAtk))
		{
		// �A�j���[�V����  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// ���̃t���[����
					g_Boss[0].patternAnim += 1;
				}
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			px = g_Boss[0].pos.x - 10.0f - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			py = g_Boss[0].pos.y - 20.0f - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			pw = g_Boss[0].w + 15.0f;		// �G�l�~�[�̕\����
			ph = g_Boss[0].h + 15.0f;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			if (g_Boss[0].dir == DIRECTION::CHAR_DIR_LEFT)
			{
				if (g_Boss[0].patternAnim >= atkFrameNum)
				{
					g_Boss[0].patternAnim = 6;
					if (g_Boss[0].attackCnt >= 110)
					{
						g_Boss[0].patternAnim = 0;
					}
				}

				tw = 1.0f / atkFrameNum;	// �e�N�X�`���̕�
				tx = (float)(g_Boss[0].patternAnim) * tw;	// �e�N�X�`���̍���X���W

			}
			else if (g_Boss[0].dir == DIRECTION::CHAR_DIR_RIGHT)
			{
				if (g_Boss[0].patternAnim > atkFrameNum)
				{
					g_Boss[0].patternAnim = 7;
					if (g_Boss[0].attackCnt >= 110)
					{
						g_Boss[0].patternAnim = 0;
					}
				}

				tw = -1.0f / atkFrameNum;	// �e�N�X�`���̕�
				tx = (float)(atkFrameNum - g_Boss[0].patternAnim) * tw;	// �e�N�X�`���̍���X���W
			}

			th = 1.0f / 1;	// �e�N�X�`���̍���
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[0].rot.z);
			
			if (g_Boss[0].hurt)
			{
				if ((g_Boss[0].hurtCnt / 10) % 2 == 1)
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f),
						g_Boss[0].rot.z);
				}
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			if (g_Boss[0].atkHB.use)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				px = g_Boss[0].atkPos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
				py = g_Boss[0].atkPos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
				pw = g_Boss[0].atkW;		// �q�b�g�{�b�N�X�̕\����
				ph = g_Boss[0].atkH;		// �q�b�g�{�b�N�X�̕\������

				int pattern = (g_Boss[0].attackCnt / frameNum) % 5;

				if (g_Boss[0].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = 1.0f / 5;	// �e�N�X�`���̕�
					tx = (float)(pattern)*tw;	// �e�N�X�`���̍���X���W
				}
				else if(g_Boss[0].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = -1.0f / 5;	// �e�N�X�`���̕�
					tx = (float)(5 - pattern)*tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Boss[0].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Boss[0].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use)
		{
			for (int j = 0; j < 2; j++)
			{
				// �G�l�~�[�̓�����͈͂�\������
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Boss[i].hb[j].HBpos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
				float py = g_Boss[i].hb[j].HBpos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
				float pw = g_Boss[i].hb[j].HBw;		// �q�b�g�{�b�N�X�̕\����
				float ph = g_Boss[i].hb[j].HBh;		// �q�b�g�{�b�N�X�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
					g_Boss[0].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			if (g_Boss[i].atkHB.use)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Boss[i].atkHB.HBpos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
				float py = g_Boss[i].atkHB.HBpos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
				float pw = g_Boss[i].atkHB.HBw;		// �q�b�g�{�b�N�X�̕\����
				float ph = g_Boss[i].atkHB.HBh;		// �q�b�g�{�b�N�X�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
					g_Boss[0].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
#endif // _DEBUG
}

BOSS* GetBoss(void)
{
	return &g_Boss[0];
}
