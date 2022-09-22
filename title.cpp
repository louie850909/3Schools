//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "player.h"
#include "UI.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg.png",
	"data/TEXTURE/Misc/logo.png",
	"data/TEXTURE/UI/start.png",
	"data/TEXTURE/UI/quit.png",
	"data/TEXTURE/effect000.jpg",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static double	g_time;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	g_Load = TRUE;
	g_time = 0;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	PLAYER* player = GetPlayer();

	UiItem* UI = GetUiItem();
	
	// Enter��������A�X�e�[�W��؂�ւ���
	if (GetKeyboardTrigger(DIK_RETURN) && UI[0].pos.y == 380)
	{
		player[0].preStage = GetMode();
		SetFade(FADE_OUT, MODE_TUTORIAL);
		
	}

	if (GetKeyboardTrigger(DIK_RETURN) && UI[0].pos.y == 475)
	{
		PostQuitMessage(WM_CLOSE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}

	g_time++;

	// �Z�[�u�f�[�^�����[�h����H
	/*if (GetKeyboardTrigger(DIK_L))
	{
		SetLoadGame(TRUE);
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}*/

	

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	UiItem* UI = GetUiItem();

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		SetSpriteLeftTop(g_VertexBuffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1);

		GetDeviceContext()->Draw(4, 0);
	}

	// ���S�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float px = 135.0f;			// ���_���W
		float py = 150.0f;
		float pw = 729.0f;
		float ph = 112.0f;

		float tx = 0.0f;			// Tex���W
		float ty = 0.0f;
		float tw = 1.0f / pw;
		float th = 1.0f;

		for (int i = px; i < px + pw; i += 1)
		{
			double rate = 1.5f * sinf((3.0f * (i + g_time) * XM_PI / 180.0f));//���W�A���ϊ��Ɗg�嗦�ւ̕ϊ�
			//	�X���C�X���ĕ`�悷��
			float x1 = px + rate;
			float y1 = py + rate;

			SetSpriteLeftTop(g_VertexBuffer, i, y1, 1, ph, (i - px) / pw,  ty, tw, th);

			GetDeviceContext()->Draw(4, 0);
		}
	}

	// Start
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float pw = 200;
		if (UI[0].pos.y == 380)
		{
			pw = 250;
		}
		float ph = 80;
		if (UI[0].pos.y == 380)
		{
			ph = 100;
		}

		SetSprite(g_VertexBuffer, 500, 400, pw, ph, 0, 0, 1, 1);

		GetDeviceContext()->Draw(4, 0);
	}

	// Quit
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		float pw = 200;
		if (UI[0].pos.y == 475)
		{
			pw = 250;
		}
		float ph = 80;
		if (UI[0].pos.y == 475)
		{
			ph = 100;
		}

		SetSprite(g_VertexBuffer, 500, 495, pw, ph, 0, 0, 1, 1);

		GetDeviceContext()->Draw(4, 0);
	}
}





