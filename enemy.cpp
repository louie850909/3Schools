//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : �с@���
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define frameNum					(5)		// ���R�}5�t���[���ɂ��~�܂�
#define SkullFrameNum				(4)		// �X�J���̃t���[����
#define DeathFrameNum				(8)		// ���S�t���[����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Monster/skull-no-fire.png",
	"data/TEXTURE/Monster/death.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Skull[ENEMY_MAX];		// �G�l�~�[�\����

static int		g_EnemyCnt = ENEMY_MAX;

static INTERPOLATION_DATA g_TutorialMoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1400.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1600.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1440.0f,  100.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		300 },
	{ XMFLOAT3(100.0f,  100.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		300 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1110.0f,  350.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1700.0f,  350.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(680.0f,  720.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1025.0f,  720.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl3[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(180.0f,  470.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(750.0f,  470.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl4[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(50.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(550.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl5[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(725.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1175.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl6[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(620.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(120.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl7[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1250.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(850.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};


static INTERPOLATION_DATA* g_TutorialMoveTblAdr[] =
{
	g_TutorialMoveTbl0,
};

static INTERPOLATION_DATA* g_Stage1MoveTblAdr[] =
{
	g_Stage1MoveTbl0,
	g_Stage1MoveTbl1,
	g_Stage1MoveTbl2,
	g_Stage1MoveTbl3,
	g_Stage1MoveTbl4,
	g_Stage1MoveTbl5,
	g_Stage1MoveTbl6,
	g_Stage1MoveTbl7,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

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
	g_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Skull[i].use = FALSE;
		g_Skull[i].pos = XMFLOAT3(1200.0f + i*200.0f, 100.0f, 0.0f);	// ���S�_����\��
		g_Skull[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Skull[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Skull[i].w = TEXTURE_WIDTH;
		g_Skull[i].h = TEXTURE_HEIGHT;
		g_Skull[i].texNo = 0;

		g_Skull[i].HBw = 80.0f;
		g_Skull[i].HBh = 80.0f;

		g_Skull[i].countAnim = 0;
		g_Skull[i].patternAnim = 0;

		g_Skull[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
		g_Skull[i].dir = 1;

		g_Skull[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Skull[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Skull[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Skull[i].idle = TRUE;
		g_Skull[i].hurt = FALSE;
		g_Skull[i].hurtCnt = 0;

		g_Skull[i].attack = FALSE;

		g_EnemyCnt++;
	}

	switch (GetMode())
	{

	case MODE_TUTORIAL:
		// 0�Ԃ������`��Ԃœ������Ă݂�
		g_Skull[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Skull[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Skull[0].tblMax = sizeof(g_TutorialMoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Skull[0].attack = TRUE;
		g_Skull[0].use = TRUE;
		break;

	case MODE_STAGE1:
		
		for (int i = 0; i < 8; i++)
		{
			g_Skull[i].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
			g_Skull[i].tblNo = i;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Skull[i].tblMax = sizeof(g_Stage1MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			g_Skull[i].attack = TRUE;
			g_Skull[i].use = TRUE;
		}

		for (int i = 8; i < 13; i++)
		{
			g_Skull[i].attack = TRUE;
			g_Skull[i].use = TRUE;
			g_Skull[i].pos = XMFLOAT3(1550.0f, 1030.0f - (i - 8) * 100.0f, 0.0f);
		}
		
		break;

	case MODE_BOSS:

		break;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// �����Ă�G�l�~�[�̐�
	/*int nowNo = 0;
	int maxNo = 0;
	int nextNo;*/
	/*XMVECTOR nowPos;
	XMVECTOR nowRot;
	XMVECTOR nowScl;
	XMVECTOR Pos;
	XMVECTOR Rot;
	XMVECTOR Scl;
	float nowTime;*/

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Skull[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Skull[i].pos;
			
			g_Skull[i].HBpos = XMFLOAT3(g_Skull[i].pos.x - 10.0f, g_Skull[i].pos.y + 5.0f, 0.0f);
			
			// �ړ�����
			if (g_Skull[i].idle)
			{
				// �ړ�����
				if (g_Skull[i].tblMax > 0)	// ���`��Ԃ����s����H
				{	// ���`��Ԃ̏���
					int nowNo = (int)g_Skull[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_Skull[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = 0;
					
					switch (GetMode())
					{
					case MODE_TUTORIAL:
						tbl = g_TutorialMoveTblAdr[g_Skull[i].tblNo];	// �s���e�[�u���̃A�h���X���擾
						break;
						
					case MODE_STAGE1:
						tbl = g_Stage1MoveTblAdr[g_Skull[i].tblNo];	// �s���e�[�u���̃A�h���X���擾
					}

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_Skull [i] .time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

					XMFLOAT3 dir;
					XMStoreFloat3(&dir, Pos);
					if (dir.x < 0)
					{
						g_Skull[i].dir = DIRECTION::CHAR_DIR_RIGHT;
					}
					else if (dir.x > 0)
					{
						g_Skull[i].dir = DIRECTION::CHAR_DIR_LEFT;
					}

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_Skull[i].pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Skull[i].rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Skull[i].scl, nowScl + Scl);
					g_Skull[i].w = TEXTURE_WIDTH * g_Skull[i].scl.x;
					g_Skull[i].h = TEXTURE_HEIGHT * g_Skull[i].scl.y;

					// frame���g�Ď��Ԍo�ߏ���������
					g_Skull[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_Skull[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_Skull[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
					}
				}
			}

			// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
			PLAYER* player = GetPlayer();

			if (g_Skull[i].hurt)
			{
				g_Skull[i].hurtCnt++;
				if (g_Skull[i].hurtCnt >= frameNum * DeathFrameNum)
				{
					g_Skull[i].use = FALSE;
				}
			}

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				if (player[j].use == TRUE)
				{
					// �v���C���[�ɍU�����ꂽ����
					if ((player[j].attack == TRUE) && (player[j].patternAnim > 5))
					{
						BOOL ans = CollisionBB(g_Skull[i].pos, g_Skull[i].w, g_Skull[i].h,
							player[j].atkHBpos, player[j].atkHBw, player[j].atkHBh);
						// �������Ă���H
						if ((ans == TRUE) && (g_Skull[i].hurt == FALSE))
						{
							g_Skull[i].hurt = TRUE;
							g_Skull[i].idle = FALSE;
							g_Skull[i].attack = FALSE;
							PlaySound(SOUND_LABEL_SE_sword_hit);
						}
					}

					// �U������
					if ((player[j].noDamage == FALSE) && g_Skull[i].attack == TRUE)
					{
						BOOL ans = CollisionBB(player[j].HBpos, player[j].HBw, player[j].HBh,
							g_Skull[i].HBpos, g_Skull[i].HBw, g_Skull[i].HBh);
						// �������Ă���H
						if ((ans == TRUE) && (player[j].hurt == FALSE))
						{
							// �����������̏���
							player[j].hurt = TRUE;
							player[j].hurtCnt = 0;
							player[j].patternAnim = 0;
							player[j].noDamage = TRUE;
							player[j].HP -= 1;
							PlaySound(SOUND_LABEL_SE_player_hitten);
						}
					}
				}
			}
			g_EnemyCnt++;		// �����Ă��G�̐�
		}
	}


	// �G�l�~�[�S�Ń`�F�b�N
	/*if (g_EnemyCnt <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}*/

#ifdef _DEBUG	// �f�o�b�O����\������

	PrintDebugProc("Enemy counts : %d\n", g_EnemyCnt);
	PrintDebugProc("skull0 use: %d\n", g_Skull[0].use);
	PrintDebugProc("skull0 hurt: %d\n", g_Skull[0].hurt);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Skull[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			float px = 0.0f;
			float py = 0.0f;
			float pw = 0.0f;
			float ph = 0.0f;
			float tw = 0.0f;
			float th = 0.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			if (g_Skull[i].idle)
			{
				// �A�j���[�V����  
				{
					g_Skull[i].countAnim += 1.0f;
					if (g_Skull[i].countAnim > frameNum)
					{
						g_Skull[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Skull[i].patternAnim += 1;
					}
				}

				if (g_Skull[i].patternAnim >= SkullFrameNum)
				{
					g_Skull[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Skull[i].texNo]);

				//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Skull[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
				py = g_Skull[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
				pw = g_Skull[i].w;		// �G�l�~�[�̕\����
				ph = g_Skull[i].h;		// �G�l�~�[�̕\������

				// �A�j���[�V�����p
				if (g_Skull[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					px -= 5.0f;
					tw = 1.0f / SkullFrameNum;	// �e�N�X�`���̕�
					tx = (float)(g_Skull[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W

				}
				else if (g_Skull[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / SkullFrameNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(SkullFrameNum - g_Skull[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}

				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Skull[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Skull[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			
			else if (g_Skull[i].hurt)
			{
				// �A�j���[�V����  
				{
					g_Skull[i].countAnim += 1.0f;
					if (g_Skull[i].countAnim > frameNum)
					{
						g_Skull[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Skull[i].patternAnim += 1;
					}
				}

				if (g_Skull[i].patternAnim >= DeathFrameNum)
				{
					g_Skull[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Skull[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
				py = g_Skull[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
				pw = g_Skull[i].w;		// �G�l�~�[�̕\����
				ph = g_Skull[i].h;		// �G�l�~�[�̕\������

				// �A�j���[�V�����p
				if (g_Skull[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DeathFrameNum;	// �e�N�X�`���̕�
					tx = (float)(g_Skull[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W

				}
				else if (g_Skull[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DeathFrameNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(DeathFrameNum - g_Skull[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}

				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Skull[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Skull[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Skull[i].use)
		{
			// �G�l�~�[�̓�����͈͂�\������
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Skull[i].HBpos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
			float py = g_Skull[i].HBpos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
			float pw = g_Skull[i].HBw;		// �q�b�g�{�b�N�X�̕\����
			float ph = g_Skull[i].HBh;		// �q�b�g�{�b�N�X�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
				g_Skull[0].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
#endif // _DEBUG



}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetSkull(void)
{
	return &g_Skull[0];
}





