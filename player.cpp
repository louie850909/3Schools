//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : �с@���
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 

#define MSG_WIDTH					(200)	// ���b�Z�[�W�T�C�Y
#define MSG_HEIGHT					(200)

#define TEXTURE_MAX					(15)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

//	�v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

#define frameNum					(5)	// ���𓮍�Ɏ���10�t���[���~�܂�

#define IdleNum						(6)		// �ҋ@��Ԃ̕�����
#define JumpNum						(3)		// �W�����v�̕�����
#define RunNum						(8)		// ����̕�����
#define HurtNum						(4)		// �_���[�W�󂯂̕�����
#define FallNum						(3)		// �����̕�����
#define DeathNum					(8)		// ���S�̕�����
#define AttackNum					(12)	// �U���̕�����
#define DashNum						(5)		// �_�b�V���̕�����

//	�W�����v����
#define	PLAYER_JUMP_Y_MAX			(150.0f)	// �W�����v�̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/PLAYER/idle.png",
	"data/TEXTURE/PLAYER/jump.png",
	"data/TEXTURE/PLAYER/run.png",
	"data/TEXTURE/PLAYER/run.png",
	"data/TEXTURE/PLAYER/wall_slide.png",
	"data/TEXTURE/PLAYER/fall.png",
	"data/TEXTURE/PLAYER/hurt.png",
	"data/TEXTURE/PLAYER/explosion.png",
	"data/TEXTURE/PLAYER/attack.png",
	"data/TEXTURE/PLAYER/slide.png",
	"data/TEXTURE/PLAYER/msg1.png",
	"data/TEXTURE/PLAYER/msg2.png",
	"data/TEXTURE/PLAYER/msg3.png",
	"data/TEXTURE/PLAYER/msg4.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����

static int g_counter;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
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


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		switch (GetMode())
		{
		case MODE_TUTORIAL:
			if (g_Player[i].preStage > MODE_TUTORIAL)
			{
				g_Player[i].pos = XMFLOAT3(1880.0f, 900.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				g_Player[i].pos = XMFLOAT3(100.0f, 0.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			g_Player[i].HP = 10;
			break;

		case MODE_STAGE1:
			if (g_Player[i].preStage > MODE_TUTORIAL)
			{
				g_Player[i].pos = XMFLOAT3(1880.0f, 900.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				g_Player[i].pos = XMFLOAT3(100.0f, 0.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			break;

		case MODE_STAGE2:
			break;

		case MODE_BOSS:
			g_Player[i].pos = XMFLOAT3(100.0f, 912.0f, 0.0f);
			g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			break;

		case MODE_HIDE_BOSS:

			if (g_Player[i].preStage > MODE_HIDE_BOSS)
			{
				g_Player[i].pos = XMFLOAT3(1800.0f, 900.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				g_Player[i].pos = XMFLOAT3(100.0f, 0.0f, 0.0f);
				g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			break;

		case MODE_MID_BOSS:
			break;
		}
		g_Player[i].use = TRUE;
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].HBw = 40.0f;
		g_Player[i].HBh = 75.0f;

		g_Player[i].atkHBw = 40.0f;
		g_Player[i].atkHBh = 75.0f;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, 8.0f, 0.0f);		// �ړ���

		g_Player[i].dir = CHAR_DIR_RIGHT;					// �E�����ɂ��Ƃ���
		g_Player[i].moving = FALSE;							// �ړ����t���O
		g_Player[i].patternAnim = 0;

		// �W�����v�̏�����
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].Multijump = 0;

		g_Player[i].death		= FALSE;
		g_Player[i].hurt		= FALSE;
		g_Player[i].noDamage	= FALSE;

		g_Player[i].recovery = FALSE;
		g_Player[i].recoveryCnt = 0;
		
		g_Player[i].stage		= GetMode();

		g_Player[i].FALL = FALSE;

		// ���g�p
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}

		g_Player[i].noDamage = FALSE;

		// �X�e�[�^�X
		g_Player[i].state = IDLE;

		for (int j = 0; j < MSG_HITBOX_MAX; j++)
		{
			g_Player[i].msg[j] = FALSE;
		}
	}

#ifdef _DEBUG

	g_Player[0].HP = 10;

#endif // _DEBUG
	g_counter = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			int state_old = g_Player[i].state;
			
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Player[i].pos;
			
			g_Player[i].HBpos = XMFLOAT3(g_Player[i].pos.x - 10.0f, g_Player[i].pos.y + 10.0f, 0.0f);
			if (g_Player[i].dir == CHAR_DIR_RIGHT)
			{
				g_Player[i].atkHBpos = XMFLOAT3(g_Player[i].HBpos.x + g_Player[i].HBw, g_Player[i].HBpos.y, 0.0f);
			}
			else if (g_Player[i].dir == CHAR_DIR_LEFT)
			{
				g_Player[i].atkHBpos = XMFLOAT3(g_Player[i].HBpos.x - g_Player[i].HBw, g_Player[i].HBpos.y, 0.0f);
			}
			g_Player[i].FALL = TRUE;

			// ���g�p
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// �n�ʂƐڐG���邩�H
			HitBox* THB = GetTHB();

			for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
			{
				if (THB[j].use == TRUE)
				{
					BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
						THB[j].HBpos, THB[j].HBw, THB[j].HBh);

					if (ans == TRUE)
					{
						float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
						float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
						float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
						float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));
						
						float minimum = min(x1, min(x2, min(y1, y2)));

						if (minimum == y1)
						{
							g_Player[i].Multijump = 0;
							g_Player[i].FALL = FALSE;
							break;
						}
					}
				}
			}
			// ����������
			if (g_Player[i].FALL)
			{
				g_Player[i].pos.y += g_Player[i].move.y;
				g_Player[i].state = FALL;
			}
			else
			{
				g_Player[i].state = IDLE;
			}


			// �L�[���͂ňړ� 
			{
				float speed = g_Player[i].move.x;

				g_Player[i].moving = FALSE;

				if (GetKeyboardPress(DIK_RIGHT) && (g_Player[i].hurt != TRUE) && (g_Player[i].attack != TRUE) && (g_Player[i].death != TRUE))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].moving = TRUE;
					if (g_Player[i].FALL != TRUE)
					{
						g_Player[i].state = RUN_RIGHT;
					}
					else
					{
						g_Player[i].state = FALL;
					}

					for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
					{
						if (THB[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
								THB[j].HBpos, THB[j].HBw, THB[j].HBh);

							if (ans == TRUE)
							{
								float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
								float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
								float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
								float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));

								float minimum = min(x1, min(x2, min(y1, y2)));

								if (minimum == x1)
								{
									g_Player[i].Multijump = 0;
									g_Player[i].pos.x = pos_old.x;
									break;
								}
							}
						}
					}
				}
				else if (GetKeyboardPress(DIK_LEFT) && (g_Player[i].hurt != TRUE) && (g_Player[i].attack != TRUE) && (g_Player[i].death != TRUE))
				{
					g_Player[i].pos.x -= speed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].moving = TRUE;
					if (g_Player[i].FALL != TRUE)
					{
						g_Player[i].state = RUN_LEFT;
					}
					else
					{
						g_Player[i].state = FALL;
					}

					for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
					{
						if (THB[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
								THB[j].HBpos, THB[j].HBw, THB[j].HBh);

							if (ans == TRUE)
							{
								float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
								float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
								float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
								float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));

								float minimum = min(x1, min(x2, min(y1, y2)));

								if (minimum == x2)
								{
									g_Player[i].Multijump = 0;
									g_Player[i].pos.x = pos_old.x;
									break;
								}
							}
						}
					}
				}

				// �Q�[���p�b�h�łňړ�����
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					g_Player[i].pos.y += speed;
					g_Player[i].dir = CHAR_DIR_DOWN;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					g_Player[i].pos.y -= speed;
					g_Player[i].dir = CHAR_DIR_UP;
					g_Player[i].moving = TRUE;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].pos.x -= speed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}


				// �W�����v�������H
				if (g_Player[i].jump == TRUE)
				{
					g_Player[i].state = JUMP;
					g_Player[i].jumpCnt++;
					g_Player[i].jumpY -= 2.0f;
					g_Player[i].pos.y -= g_Player[i].jumpY;

					for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
					{
						if (THB[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
								THB[j].HBpos, THB[j].HBw, THB[j].HBh);

							if (ans == TRUE)
							{
								float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
								float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
								float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
								float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));

								float minimum = min(x1, min(x2, min(y1, y2)));
								if (minimum == y2)
								{
									g_Player[i].pos.y = pos_old.y;
								}
							}
						}
					}

					if (g_Player[i].jumpCnt > frameNum * JumpNum)
					{
						g_Player[i].jump = FALSE;
					}
				}
				// �W�����v�{�^���������H
				if (((g_Player[i].hurt != TRUE) && (g_Player[i].death != TRUE)) && ((GetKeyboardTrigger(DIK_SPACE)) && (g_Player[i].Multijump <= 0)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 0;
					g_Player[i].jumpY = 32.0f;
					g_Player[i].state = JUMP;
					g_Player[i].patternAnim = 0;
					g_Player[i].Multijump++;
				}

				// �_�b�V��������
				if (g_Player[i].dash == TRUE)
				{
					if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
					{
						g_Player[i].pos.x += speed * 2;

						for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
						{
							if (THB[j].use == TRUE)
							{
								BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
									THB[j].HBpos, THB[j].HBw, THB[j].HBh);

								if (ans == TRUE)
								{
									float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
									float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
									float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
									float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));

									float minimum = min(x1, min(x2, min(y1, y2)));

									if (minimum == x1)
									{
										g_Player[i].Multijump = 0;
										g_Player[i].pos.x = pos_old.x;
										break;
									}
								}
							}
						}
					}
					else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
					{
						g_Player[i].pos.x -= speed * 2;

						for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
						{
							if (THB[j].use == TRUE)
							{
								BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
									THB[j].HBpos, THB[j].HBw, THB[j].HBh);

								if (ans == TRUE)
								{
									float x1 = fabsf((g_Player[i].HBpos.x + g_Player[i].HBw / 2) - (THB[j].HBpos.x - THB[j].HBw / 2));
									float x2 = fabsf((g_Player[i].HBpos.x - g_Player[i].HBw / 2) - (THB[j].HBpos.x + THB[j].HBw / 2));
									float y1 = fabsf((g_Player[i].HBpos.y + g_Player[i].HBh / 2) - (THB[j].HBpos.y - THB[j].HBh / 2));
									float y2 = fabsf((g_Player[i].HBpos.y - g_Player[i].HBh / 2) - (THB[j].HBpos.y + THB[j].HBh / 2));

									float minimum = min(x1, min(x2, min(y1, y2)));

									if (minimum == x2)
									{
										g_Player[i].Multijump = 0;
										g_Player[i].pos.x = pos_old.x;
										break;
									}
								}
							}
						}
					}

					g_Player[i].dashCnt++;

					g_Player[i].state = DASH;

					if (g_Player[i].dashCnt > (frameNum * DashNum))
					{
						g_Player[i].dash = FALSE;
						g_Player[i].noDamage = FALSE;
						g_Player[i].dashCnt = 0;
						g_Player[i].state = IDLE;
						g_Player[i].patternAnim = 0;
					}
				}
				// �_�b�V���{�^���������H
				else if ((g_Player[i].dash == FALSE) && (GetKeyboardTrigger(DIK_LSHIFT)) && (g_Player[i].hurt != TRUE) && (g_Player[i].death != TRUE) && (g_Player[i].attack != TRUE))
				{
					g_Player[i].dash = TRUE;
					g_Player[i].noDamage = TRUE;
					g_Player[i].dashCnt = 0;
					g_Player[i].state = DASH;
					g_Player[i].patternAnim = 0;
					PlaySound(SOUND_LABEL_SE_player_dash);
				}


				// MAP�O�`�F�b�N
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				ENEMY* enemy = GetSkull();
				
				// �_���[�W���󂯂��サ�΂炭���G�ɂ���
				if (g_Player[i].recovery)
				{
					g_Player[i].recoveryCnt++;
					g_Player[i].noDamage = TRUE;

					if (g_Player[i].recoveryCnt >= 60)
					{
						g_Player[i].recovery = FALSE;
						g_Player[i].noDamage = FALSE;
						g_Player[i].recoveryCnt = 0;
					}
				}

				// �_���[�W������
				if (g_Player[i].hurt)
				{
					g_Player[i].hurtCnt++;
					g_Player[i].state = HURT;

					if (g_Player[i].hurtCnt > (frameNum * HurtNum))
					{
						g_Player[i].hurt = FALSE;
						g_Player[i].patternAnim = 0;
						g_Player[i].recovery = TRUE;
					}
				}

				// �U��������
				if (g_Player[i].attack)
				{
					g_Player[i].attackCnt++;
					g_Player[i].state = ATTACK;

					if (g_Player[i].patternAnim == 3)
					{
						PlaySound(SOUND_LABEL_SE_sword_miss);
					}

					if (g_Player[i].attackCnt > (frameNum * AttackNum))
					{
						g_Player[i].attack = FALSE;
						g_Player[i].attackCnt = 0;
						g_Player[i].state = IDLE;
						g_Player[i].patternAnim = 0;
						g_Player[i].noDamage = FALSE;
					}
				}
				// �U���H
				else if (GetKeyboardTrigger(DIK_Z) && (g_Player[i].hurt != TRUE) && (g_Player[i].death != TRUE))
				{
					g_Player[i].attack = TRUE;
					g_Player[i].attackCnt = 0;
					g_Player[i].patternAnim = 0;
					g_Player[i].noDamage = TRUE;
					g_Player[i].dash = FALSE;
				}

				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					//SetBullet(pos);
				}

				// �v���C���[�̎��S�������H
				if (g_Player[i].death)
				{
					g_Player[i].state = DEATH;
					g_Player[i].deathCnt++;
					g_Player[i].noDamage = TRUE;
					g_Player[i].pos = pos_old;

					if (g_Player[i].deathCnt > (frameNum * DeathNum))
					{
						SetFade(FADE_OUT, MODE_RESULT);
					}
				}
				// ���S�H
				else if (g_Player[i].HP <= 0)
				{
					g_Player[i].death = TRUE;
					g_Player[i].deathCnt = 0;
					g_Player[i].patternAnim = 0;
					g_Player[i].dash = FALSE;
					PlaySound(SOUND_LABEL_SE_player_explo);
				}
				// �f�b�h�]�[�������蔻��
				HitBox* deadzone = GetDZ();
				for (int j = 0; j < TERRAIN_HITBOX_MAX; j++)
				{
					if ((deadzone[j].use) && (g_Player[i].death == FALSE))
					{
						BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh, deadzone[j].HBpos, deadzone[j].HBw, deadzone[j].HBh);
						if (ans == TRUE)
						{
							g_Player[i].death = TRUE;
							g_Player[i].deathCnt = 0;
							g_Player[i].patternAnim = 0;
							g_Player[i].dash = FALSE;
							PlaySound(SOUND_LABEL_SE_player_explo);
						}
					}
				}

				// �`���[�g���A�����b�Z�[�W����
				HitBox* msgHB = GetMSG();
				switch (GetMode())
				{
				case MODE_TUTORIAL:

					for (int j = 0; j < MSG_HITBOX_MAX; j++)
					{
						if (msgHB[j].use)
						{
							BOOL ans = CollisionBB(g_Player[i].HBpos, g_Player[i].HBw, g_Player[i].HBh,
								msgHB[j].HBpos, msgHB[j].HBw, msgHB[j].HBh);
							
							if (ans)
							{
								g_Player[i].msg[j] = TRUE;
							}
							else
							{
								g_Player[i].msg[j] = FALSE;
							}
						}
					}
					break;

				default:
					break;
				}

				// �����Ŏ��̃X�e�[�W��
				if (GetKeyboardTrigger(DIK_P))
				{
					switch (GetMode())
					{
					case MODE_TUTORIAL:
						g_Player[i].preStage = GetMode();
						SetFade(FADE_OUT, MODE_STAGE1);
						break;
						
					case MODE_STAGE1:
						g_Player[i].preStage = GetMode();
						SetFade(FADE_OUT, MODE_BOSS);
						break;

					case MODE_BOSS:
						g_Player[i].preStage = GetMode();
						SetFade(FADE_OUT, MODE_CLEAR);
						break;
					}
				}
			}
		}
	}


	// ������Z�[�u����
	/*if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}*/


#ifdef _DEBUG	// �f�o�b�O����\������

	PrintDebugProc("Player X Pos: %f\n", g_Player[0].pos.x);
	PrintDebugProc("Player Y Pos: %f\n", g_Player[0].pos.y);
	PrintDebugProc("HitBox right X Pos: %f\n", g_Player[0].HBpos.x + g_Player[0].HBw / 2);
	PrintDebugProc("Hitbox bottom Y Pos: %f\n", g_Player[0].HBpos.y + g_Player[0].HBh / 2);
	PrintDebugProc("Player  aniPattern: %d\n", g_Player[0].patternAnim);
	PrintDebugProc("\n");
	PrintDebugProc("Player  prestage : %d\n", g_Player[0].preStage);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			float px = 0.0f;
			float py = 0.0f;
			float pw = 0.0f;
			float ph = 0.0f;
			float tw = 0.0f;
			float th = 0.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �v���C���[�̕��g��`��
			if (g_Player[i].dash)
			{	// �_�b�V�����������g����
				DrawPlayerOffset(i);
			}

			switch (g_Player[i].state)
			{
			case IDLE:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= IdleNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / IdleNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W

				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / IdleNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(IdleNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}

				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case JUMP:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= JumpNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / JumpNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / JumpNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(JumpNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W

				}
				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case RUN_RIGHT:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= RunNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				tw = 1.0f / RunNum;	// �e�N�X�`���̕�
				th = 1.0f / 1;	// �e�N�X�`���̍���
				tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				if (g_counter >= 25)
				{
					g_counter = 0;
				}

				if (g_counter == 0)
				{
					PlaySound(SOUND_LABEL_SE_walk);
				}
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
				g_counter++;

				break;

			case RUN_LEFT:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= RunNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				px -= 15.0f;			// �`��ʒu�̒���

				// �A�j���[�V�����p
				tw = -1.0f / RunNum;	// �e�N�X�`���̕�
				th = 1.0f / 1;	// �e�N�X�`���̍���
				tx = (float)(RunNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				if (g_counter >= 25)
				{
					g_counter = 0;
				}

				if (g_counter == 0)
				{
					PlaySound(SOUND_LABEL_SE_walk);
				}
				
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				g_counter++;
				break;

			case HURT:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= HurtNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / HurtNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W

				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / HurtNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(HurtNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case FALL:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= FallNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / FallNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / FallNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(FallNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case DEATH:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= DeathNum)
				{
					g_Player[i].use = FALSE;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DeathNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DeathNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(DeathNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���

				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case ATTACK:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= AttackNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / AttackNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / AttackNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(AttackNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���

				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;

			case DASH:

				// �A�j���[�V����  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// ���̃t���[����
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= DashNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
				py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
				pw = g_Player[i].w;		// �v���C���[�̕\����
				ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DashNum;	// �e�N�X�`���̕�
					tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DashNum;	// �e�N�X�`���̕�
					px -= 15.0f;				// �`��ʒu�̒���
					tx = (float)(DashNum - g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
				}
				th = 1.0f / 1;	// �e�N�X�`���̍���
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W

				if (g_Player[i].recovery)
				{
					if ((g_Player[i].recoveryCnt / 10) % 2 == 0)
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
							g_Player[i].rot.z);
					}
					else
					{
						SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
							XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
							g_Player[i].rot.z);
					}
				}
				else
				{
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				break;
			}

			for (int j = 0; j < MSG_HITBOX_MAX; j++)
			{
				if (g_Player[i].msg[j] == TRUE)
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10 + j]);

					//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
					px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
					py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
					pw = MSG_WIDTH;
					ph = MSG_HEIGHT;

					// �A�j���[�V�����p
					if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
					{
						px += 50.0f;
						py -= 120.0f;
					}
					else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
					{
						px += 50.0f;				// �`��ʒu�̒���
						py -= 120.0f;
					}
					tx = 0.0f;
					ty = 0.0f;
					tw = 1.0f;	// �e�N�X�`���̕�
					th = 1.0f;	// �e�N�X�`���̍���

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������

	// �v���C���[�̓�����͈͂�\������
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Player[0].HBpos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
	float py = g_Player[0].HBpos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
	float pw = g_Player[0].HBw;		// �q�b�g�{�b�N�X�̕\����
	float ph = g_Player[0].HBh;		// �q�b�g�{�b�N�X�̕\������

	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
		g_Player[0].rot.z);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	if (g_Player[0].attack)
	{
		// �v���C���[�̓�����͈͂�\������
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[0].atkHBpos.x - bg->pos.x;	// �q�b�g�{�b�N�X�̕\���ʒuX
		float py = g_Player[0].atkHBpos.y - bg->pos.y;	// �q�b�g�{�b�N�X�̕\���ʒuY
		float pw = g_Player[0].atkHBw;		// �q�b�g�{�b�N�X�̕\����
		float ph = g_Player[0].atkHBh;		// �q�b�g�{�b�N�X�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
			g_Player[0].rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

#endif
}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].state]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;		// �v���C���[�̕\����
		float ph = g_Player[no].h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 0;
		float tx = 0;
		if (g_Player[no].dir == DIRECTION::CHAR_DIR_RIGHT)
		{
			tw = 1.0f / DashNum;	// �e�N�X�`���̕�
			tx = (float)(g_Player[no].patternAnim) * tw;	// �e�N�X�`���̍���X���W
		}
		else if (g_Player[no].dir == DIRECTION::CHAR_DIR_LEFT)
		{
			tw = -1.0f / DashNum;	// �e�N�X�`���̕�
			tx = (float)(DashNum - g_Player[no].patternAnim) * tw;	// �e�N�X�`���̍���X���W
		}
		float th = 1.0f / 1;	// �e�N�X�`���̍���
		float ty = (float)(g_Player[no].patternAnim / 1) * th;	// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}



