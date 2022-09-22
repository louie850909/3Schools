//=============================================================================
//
// BG���� [bg.cpp]
// Author : �с@���
//
//=============================================================================
#include "bg.h"
#include "player.h"
#include "collision.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(1920)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(9)			// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/MAP/tutorial_clouds.png",
	"data/TEXTURE/MAP/tutorial_town.png",
	"data/TEXTURE/MAP/tutorial_terrain.png",
	"data/TEXTURE/MAP/clouds02.png",
	"data/TEXTURE/MAP/town02.png",
	"data/TEXTURE/MAP/boss_map.png",
	"data/TEXTURE/MAP/indoor.png",
	"data/TEXTURE/MAP/stage_terrain.png",
	"data/TEXTURE/bar_white.png"
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;

static HitBox g_Terrain[TERRAIN_HITBOX_MAX];
static HitBox g_DeadZone[TERRAIN_HITBOX_MAX];
static HitBox g_MSG[MSG_HITBOX_MAX];
static MovePoint g_MovePoint[MOVE_POINT_MAX];



//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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

	for (int i = 0; i < TERRAIN_HITBOX_MAX; i++)
	{
		g_Terrain[i].use = FALSE;
		g_Terrain[i].HBh = 0;
		g_Terrain[i].HBw = 0;
		g_Terrain[i].HBpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		
		g_DeadZone[i].use = FALSE;
		g_DeadZone[i].HBh = 0;
		g_DeadZone[i].HBw = 0;
		g_DeadZone[i].HBpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	for (int i = 0; i < MSG_HITBOX_MAX; i++)
	{
		g_MSG[i].use = FALSE;
		g_MSG[i].HBh = 0;
		g_MSG[i].HBw = 0;
		g_MSG[i].HBpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	for (int i = 0; i < MOVE_POINT_MAX; i++)
	{
		g_MovePoint[i].hitbox.use = FALSE;
		g_MovePoint[i].hitbox.HBh = 0;
		g_MovePoint[i].hitbox.HBw = 0;
		g_MovePoint[i].hitbox.HBpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MovePoint[i].destination = GetMode();
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_BG.w = TEXTURE_WIDTH;
	g_BG.h = TEXTURE_HEIGHT;
	g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;

	g_BG.scrl = 0.0f;		// TEX�X�N���[��
	g_BG.scrl2 = 0.0f;		// TEX�X�N���[��

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// �P�t���O�̏���ۑ�
	
	// �n�ʓ����蔻��A�]���|�C���g�A���b�Z�[�W�ݒ�
	switch (GetMode())
	{
	case MODE_TUTORIAL:

		g_Terrain[0].use = TRUE;
		g_Terrain[0].HBw = 700.0f;
		g_Terrain[0].HBh = 150.0f;
		g_Terrain[0].HBpos = XMFLOAT3(350.0f, 1020.0f, 0.0f);
				
		g_Terrain[1].use = TRUE;
		g_Terrain[1].HBw = 1120.0f;
		g_Terrain[1].HBh = 130.0f;
		g_Terrain[1].HBpos = XMFLOAT3(1360.0f, 1010.0f, 0.0f);

		g_DeadZone[0].use = TRUE;
		g_DeadZone[0].HBw = 1720.0f;
		g_DeadZone[0].HBh = 10.0f;
		g_DeadZone[0].HBpos = XMFLOAT3(960.0f, 1075.0f, 0.0f);

		// ���E�K�C�h
		g_MSG[0].use = TRUE;
		g_MSG[0].HBw = 400.0f;
		g_MSG[0].HBh = 150.0f;
		g_MSG[0].HBpos = XMFLOAT3(200.0f, 850.0f, 0.0f);

		// �W�����v�K�C�h
		g_MSG[1].use = TRUE;
		g_MSG[1].HBw = 200.0f;
		g_MSG[1].HBh = 150.0f;
		g_MSG[1].HBpos = XMFLOAT3(600.0f, 850.0f, 0.0f);

		// �U���K�C�h
		g_MSG[2].use = TRUE;
		g_MSG[2].HBw = 400;
		g_MSG[2].HBh = 150;
		g_MSG[2].HBpos = XMFLOAT3(1000.0f, 850.0f, 0.0f);

		// ���̃}�b�v�K�C�h
		g_MSG[3].use = TRUE;
		g_MSG[3].HBw = 300;
		g_MSG[3].HBh = 150;
		g_MSG[3].HBpos = XMFLOAT3(1600.0f, 850.0f, 0.0f);

		// �]���|�C���g
		g_MovePoint[0].hitbox.use = TRUE;
		g_MovePoint[0].hitbox.HBw = 90.0f;
		g_MovePoint[0].hitbox.HBh = 100.0f;
		g_MovePoint[0].hitbox.HBpos = XMFLOAT3(1870.0f, 900.0f, 0.0f);
		g_MovePoint[0].destination = MODE_STAGE1;
		
		break;
		
	case MODE_STAGE1:

		g_Terrain[0].use = TRUE;
		g_Terrain[0].HBw = 1470.0f;
		g_Terrain[0].HBh = 90.0f;
		g_Terrain[0].HBpos = XMFLOAT3(735.0f, 200.0f, 0.0f);

		g_Terrain[1].use = TRUE;
		g_Terrain[1].HBw = 800.0f;
		g_Terrain[1].HBh = 50.0f;
		g_Terrain[1].HBpos = XMFLOAT3(1485.0f, 425.0f, 0.0f);

		g_Terrain[2].use = TRUE;
		g_Terrain[2].HBw = 170.0f;
		g_Terrain[2].HBh = 480.0f;
		g_Terrain[2].HBpos = XMFLOAT3(1845.0f, 225.0f, 0.0f);

		g_Terrain[3].use = TRUE;
		g_Terrain[3].HBw = 130.0f;
		g_Terrain[3].HBh = 480.0f;
		g_Terrain[3].HBpos = XMFLOAT3(865.0f, 435.0f, 0.0f);

		g_Terrain[4].use = TRUE;
		g_Terrain[4].HBw = 130.0f;
		g_Terrain[4].HBh = 350.0f;
		g_Terrain[4].HBpos = XMFLOAT3(1150.0f, 620.0f, 0.0f);

		g_Terrain[5].use = TRUE;
		g_Terrain[5].HBw = 1100.0f;
		g_Terrain[5].HBh = 60.0f;
		g_Terrain[5].HBpos = XMFLOAT3(650.0f, 800.0f, 0.0f);

		g_Terrain[6].use = TRUE;
		g_Terrain[6].HBw = 100.0f;
		g_Terrain[6].HBh = 30.0f;
		g_Terrain[6].HBpos = XMFLOAT3(560.0f, 715.0f, 0.0f);

		g_Terrain[7].use = TRUE;
		g_Terrain[7].HBw = 100.0f;
		g_Terrain[7].HBh = 30.0f;
		g_Terrain[7].HBpos = XMFLOAT3(433.0f, 655.0f, 0.0f);

		g_Terrain[8].use = TRUE;
		g_Terrain[8].HBw = 100.0f;
		g_Terrain[8].HBh = 30.0f;
		g_Terrain[8].HBpos = XMFLOAT3(303.0f, 590.0f, 0.0f);

		g_Terrain[9].use = TRUE;
		g_Terrain[9].HBw = 100.0f;
		g_Terrain[9].HBh = 30.0f;
		g_Terrain[9].HBpos = XMFLOAT3(173.0f, 530.0f, 0.0f);

		g_Terrain[10].use = TRUE;
		g_Terrain[10].HBw = 30.0f;
		g_Terrain[10].HBh = 340.0f;
		g_Terrain[10].HBpos = XMFLOAT3(110.0f, 663.0f, 0.0f);

		g_Terrain[11].use = TRUE;
		g_Terrain[11].HBw = 100.0f;
		g_Terrain[11].HBh = 30.0f;
		g_Terrain[11].HBpos = XMFLOAT3(50.0f, 963.0f, 0.0f);

		g_Terrain[12].use = TRUE;
		g_Terrain[12].HBw = 100.0f;
		g_Terrain[12].HBh = 30.0f;
		g_Terrain[12].HBpos = XMFLOAT3(273.0f, 963.0f, 0.0f);

		g_Terrain[13].use = TRUE;
		g_Terrain[13].HBw = 100.0f;
		g_Terrain[13].HBh = 30.0f;
		g_Terrain[13].HBpos = XMFLOAT3(496.0f, 963.0f, 0.0f);

		g_Terrain[14].use = TRUE;
		g_Terrain[14].HBw = 100.0f;
		g_Terrain[14].HBh = 30.0f;
		g_Terrain[14].HBpos = XMFLOAT3(719.0f, 963.0f, 0.0f);

		g_Terrain[15].use = TRUE;
		g_Terrain[15].HBw = 100.0f;
		g_Terrain[15].HBh = 30.0f;
		g_Terrain[15].HBpos = XMFLOAT3(942.0f, 963.0f, 0.0f);

		g_Terrain[16].use = TRUE;
		g_Terrain[16].HBw = 100.0f;
		g_Terrain[16].HBh = 30.0f;
		g_Terrain[16].HBpos = XMFLOAT3(1165.0f, 963.0f, 0.0f);

		g_Terrain[17].use = TRUE;
		g_Terrain[17].HBw = 100.0f;
		g_Terrain[17].HBh = 30.0f;
		g_Terrain[17].HBpos = XMFLOAT3(1388.0f, 963.0f, 0.0f);

		g_Terrain[18].use = TRUE;
		g_Terrain[18].HBw = 295.0f;
		g_Terrain[18].HBh = 125.0f;
		g_Terrain[18].HBpos = XMFLOAT3(1780.0f, 1020.0f, 0.0f);

		g_MovePoint[0].hitbox.use = TRUE;
		g_MovePoint[0].hitbox.HBw = 100.0f;
		g_MovePoint[0].hitbox.HBh = 100.0f;
		g_MovePoint[0].hitbox.HBpos = XMFLOAT3(1870.0f, 950.0f, 0.0f);
		g_MovePoint[0].destination = MODE_BOSS;
		
		g_DeadZone[0].use = TRUE;
		g_DeadZone[0].HBw = 1920.0f;
		g_DeadZone[0].HBh = 10.0f;
		g_DeadZone[0].HBpos = XMFLOAT3(960.0f, 1075.0f, 0.0f);

		break;

	case MODE_STAGE2:
		break;

	case MODE_BOSS:
		
		g_Terrain[0].use = TRUE;
		g_Terrain[0].HBw = 190.0f;
		g_Terrain[0].HBh = 130.0f;
		g_Terrain[0].HBpos = XMFLOAT3(95.0f, 1020.0f, 0.0f);

		g_Terrain[1].use = TRUE;
		g_Terrain[1].HBw = 160.0f;
		g_Terrain[1].HBh = 20.0f;
		g_Terrain[1].HBpos = XMFLOAT3(305.0f, 905.0f, 0.0f);

		g_Terrain[2].use = TRUE;
		g_Terrain[2].HBw = 160.0f;
		g_Terrain[2].HBh = 20.0f;
		g_Terrain[2].HBpos = XMFLOAT3(465.0f, 813.0f, 0.0f);

		g_Terrain[3].use = TRUE;
		g_Terrain[3].HBw = 160.0f;
		g_Terrain[3].HBh = 20.0f;
		g_Terrain[3].HBpos = XMFLOAT3(465.0f, 627.0f, 0.0f);

		g_Terrain[4].use = TRUE;
		g_Terrain[4].HBw = 160.0f;
		g_Terrain[4].HBh = 20.0f;
		g_Terrain[4].HBpos = XMFLOAT3(625.0f, 905.0f, 0.0f);

		g_Terrain[5].use = TRUE;
		g_Terrain[5].HBw = 160.0f;
		g_Terrain[5].HBh = 20.0f;
		g_Terrain[5].HBpos = XMFLOAT3(687.0f, 720.0f, 0.0f);

		g_Terrain[6].use = TRUE;
		g_Terrain[6].HBw = 160.0f;
		g_Terrain[6].HBh = 20.0f;
		g_Terrain[6].HBpos = XMFLOAT3(945.0f, 627.0f, 0.0f);

		g_Terrain[7].use = TRUE;
		g_Terrain[7].HBw = 160.0f;
		g_Terrain[7].HBh = 20.0f;
		g_Terrain[7].HBpos = XMFLOAT3(1200.0f, 720.0f, 0.0f);

		g_Terrain[8].use = TRUE;
		g_Terrain[8].HBw = 160.0f;
		g_Terrain[8].HBh = 20.0f;
		g_Terrain[8].HBpos = XMFLOAT3(1265.0f, 905.0f, 0.0f);

		g_Terrain[9].use = TRUE;
		g_Terrain[9].HBw = 160.0f;
		g_Terrain[9].HBh = 20.0f;
		g_Terrain[9].HBpos = XMFLOAT3(1553.0f, 905.0f, 0.0f);

		g_Terrain[10].use = TRUE;
		g_Terrain[10].HBw = 160.0f;
		g_Terrain[10].HBh = 20.0f;
		g_Terrain[10].HBpos = XMFLOAT3(1425.0f, 813.0f, 0.0f);

		g_Terrain[11].use = TRUE;
		g_Terrain[11].HBw = 160.0f;
		g_Terrain[11].HBh = 20.0f;
		g_Terrain[11].HBpos = XMFLOAT3(1455.0f, 627.0f, 0.0f);

		g_Terrain[12].use = TRUE;
		g_Terrain[12].HBw = 160.0f;
		g_Terrain[12].HBh = 130.0f;
		g_Terrain[12].HBpos = XMFLOAT3(1840.0f, 1020.0f, 0.0f);

		g_DeadZone[0].use = TRUE;
		g_DeadZone[0].HBw = 1720.0f;
		g_DeadZone[0].HBh = 10.0f;
		g_DeadZone[0].HBpos = XMFLOAT3(960.0f, 1075.0f, 0.0f);
		

		// �]���|�C���g
		g_MovePoint[0].hitbox.use = TRUE;
		g_MovePoint[0].hitbox.HBw = 90.0f;
		g_MovePoint[0].hitbox.HBh = 100.0f;
		g_MovePoint[0].hitbox.HBpos = XMFLOAT3(95.0f, 920.0f, 0.0f);
		g_MovePoint[0].destination = MODE_STAGE1;
		
		break;
		
	case MODE_HIDE_BOSS:
		break;
		
	case MODE_MID_BOSS:
		break;
	}

	// �]���|�C���g�����蔻��
	for (int i = 0; i < MOVE_POINT_MAX; i++)
	{
		if (g_MovePoint[i].hitbox.use)
		{
			PLAYER* player = GetPlayer();
			for (int j = 0; j < PLAYER_MAX; j++)
			{
				if (player[j].use)
				{
					BOOL ans = CollisionBB(player[j].HBpos, player[j].HBw, player[j].HBh, g_MovePoint[i].hitbox.HBpos, g_MovePoint[i].hitbox.HBw, g_MovePoint[i].hitbox.HBh);
					if ((ans == TRUE) && (GetKeyboardTrigger(DIK_UP)))
					{
						player[j].preStage = GetMode();
						SetFade(FADE_OUT, g_MovePoint[i].destination);
					}
				}
			}
		}
	}
	
#ifdef _DEBUG	// �f�o�b�O����\������

	PrintDebugProc("TERRAIN HITBOX right pos x: %f\n", g_Terrain[0].HBpos.x + g_Terrain[0].HBw / 2.0f);
	PrintDebugProc("TERRAIN HITBOX top pos y: %f\n", g_Terrain[0].HBpos.y - g_Terrain[0].HBh / 2.0f);


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	bool layer1Use	= FALSE;
	bool layer2Use	= FALSE;
	bool terrainUse	= FALSE;
	
	int layer1		= -1;
	int layer2		= -1;
	int terrain		= -1;

	switch (GetMode())
	{
	case MODE_TUTORIAL:

		layer1Use	= TRUE;
		layer2Use	= TRUE;
		terrainUse	= TRUE;
		
		layer1		= 0;
		layer2		= 1;
		terrain		= 2;
		
		break;
		
	case MODE_STAGE1:
		
		layer1Use	= FALSE;
		layer2Use	= TRUE;
		terrainUse	= TRUE;

		layer1		= -1;
		layer2		= 6;
		terrain		= 7;
		
		break;
		
	case MODE_STAGE2:
		
		layer1Use	= TRUE;
		layer2Use	= TRUE;
		terrainUse	= TRUE;
		
		layer1		= 6;
		layer2		= 7;
		terrain		= 8;
		
		break;

	case MODE_BOSS:
		
		layer1Use	= TRUE;
		layer2Use	= TRUE;
		terrainUse	= TRUE;
		
		layer1		= 3;
		layer2		= 4;
		terrain		= 5;
		
		break;

	case MODE_HIDE_BOSS:
		
		layer1Use	= FALSE;
		layer2Use	= TRUE;
		terrainUse	= TRUE;
		
		layer2		= 10;
		terrain		= 11;
		break;

	case MODE_MID_BOSS:

		layer1Use = FALSE;
		layer2Use = TRUE;
		terrainUse = TRUE;

		layer2 = 7;
		terrain = 12;
		break;
	}

	// ���`��
	if(layer1Use)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[layer1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		//g_BG.scrl += tx * 0.001f;
		g_BG.scrl += 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
			g_BG.scrl, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	// ��
	if(layer2Use)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[layer2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		g_BG.scrl2 += tx * 0.001f;
		//g_BG.scrl2 += 0.003f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
			g_BG.scrl2, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �X�e�[�W��`��
	if(terrainUse)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[terrain]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


#ifdef _DEBUG	// �f�o�b�O����\������

	// �n�`�͈͂ƃf�b�h�]�[����\������
	for (int i = 0; i < TERRAIN_HITBOX_MAX; i++)
	{
		if (g_Terrain[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//�ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Terrain[i].HBpos.x - g_BG.pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
			float py = g_Terrain[i].HBpos.y - g_BG.pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
			float pw = g_Terrain[i].HBw;		// �q�b�g�{�b�N�X�̕\����
			float ph = g_Terrain[i].HBh;		// �q�b�g�{�b�N�X�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		
		if (g_DeadZone[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//�ʒu��e�N�X�`���[���W�𔽉f
			float px = g_DeadZone[i].HBpos.x - g_BG.pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
			float py = g_DeadZone[i].HBpos.y - g_BG.pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
			float pw = g_DeadZone[i].HBw;		// �q�b�g�{�b�N�X�̕\����
			float ph = g_DeadZone[i].HBh;		// �q�b�g�{�b�N�X�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���b�Z�[�W�̕\���͈�
	for (int i = 0; i < MSG_HITBOX_MAX; i++)
	{
		if (g_MSG[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//�ʒu��e�N�X�`���[���W�𔽉f
			float px = g_MSG[i].HBpos.x - g_BG.pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
			float py = g_MSG[i].HBpos.y - g_BG.pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
			float pw = g_MSG[i].HBw;		// �q�b�g�{�b�N�X�̕\����
			float ph = g_MSG[i].HBh;		// �q�b�g�{�b�N�X�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// �]���|�C���g�͈̔͂�\��
	for (int i = 0; i < MOVE_POINT_MAX; i++)
	{
		if (g_MovePoint[i].hitbox.use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//�ʒu��e�N�X�`���[���W�𔽉f
			float px = g_MovePoint[i].hitbox.HBpos.x - g_BG.pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
			float py = g_MovePoint[i].hitbox.HBpos.y - g_BG.pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
			float pw = g_MovePoint[i].hitbox.HBw;		// �q�b�g�{�b�N�X�̕\����
			float ph = g_MovePoint[i].hitbox.HBh;		// �q�b�g�{�b�N�X�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

#endif
}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}


//=============================================================================
// Terrain�\���̂̐擪�A�h���X���擾
//=============================================================================
HitBox* GetTHB(void)
{
	return &g_Terrain[0];
}

//=============================================================================
// DeadZone�\���̂̐擪�A�h���X���擾
//=============================================================================
HitBox* GetDZ(void)
{
	return &g_DeadZone[0];
}

//=============================================================================
// MSG�\���̂̐擪�A�h���X���擾
//=============================================================================
HitBox* GetMSG(void)
{
	return &g_MSG[0];
}

//=============================================================================
// MovePoint�\���̂̐擪�A�h���X���擾
//=============================================================================
MovePoint* GetMVP(void)
{
	return &g_MovePoint[0];
}





