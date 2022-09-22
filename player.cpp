//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 林　劭羲
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(100)	// 

#define MSG_WIDTH					(200)	// メッセージサイズ
#define MSG_HEIGHT					(200)

#define TEXTURE_MAX					(15)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(10)		// アニメーションの切り替わるWait値

//	プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

#define frameNum					(5)	// 分解動作に次ぎ10フレーム止まる

#define IdleNum						(6)		// 待機状態の分割数
#define JumpNum						(3)		// ジャンプの分割数
#define RunNum						(8)		// 走るの分割数
#define HurtNum						(4)		// ダメージ受けの分割数
#define FallNum						(3)		// 落下の分割数
#define DeathNum					(8)		// 死亡の分割数
#define AttackNum					(12)	// 攻撃の分割数
#define DashNum						(5)		// ダッシュの分割数

//	ジャンプ処理
#define	PLAYER_JUMP_Y_MAX			(150.0f)	// ジャンプの高さ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

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


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体

static int g_counter;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤー構造体の初期化
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

		g_Player[i].move = XMFLOAT3(4.0f, 8.0f, 0.0f);		// 移動量

		g_Player[i].dir = CHAR_DIR_RIGHT;					// 右向きにしとくか
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].patternAnim = 0;

		// ジャンプの初期化
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

		// 分身用
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}

		g_Player[i].noDamage = FALSE;

		// ステータス
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
// 終了処理
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
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			int state_old = g_Player[i].state;
			
			// 地形との当たり判定用に座標のバックアップを取っておく
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

			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// 地面と接触するか？
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
			// 落下処理中
			if (g_Player[i].FALL)
			{
				g_Player[i].pos.y += g_Player[i].move.y;
				g_Player[i].state = FALL;
			}
			else
			{
				g_Player[i].state = IDLE;
			}


			// キー入力で移動 
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

				// ゲームパッドでで移動処理
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


				// ジャンプ処理中？
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
				// ジャンプボタン押した？
				if (((g_Player[i].hurt != TRUE) && (g_Player[i].death != TRUE)) && ((GetKeyboardTrigger(DIK_SPACE)) && (g_Player[i].Multijump <= 0)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 0;
					g_Player[i].jumpY = 32.0f;
					g_Player[i].state = JUMP;
					g_Player[i].patternAnim = 0;
					g_Player[i].Multijump++;
				}

				// ダッシュ処理中
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
				// ダッシュボタン押した？
				else if ((g_Player[i].dash == FALSE) && (GetKeyboardTrigger(DIK_LSHIFT)) && (g_Player[i].hurt != TRUE) && (g_Player[i].death != TRUE) && (g_Player[i].attack != TRUE))
				{
					g_Player[i].dash = TRUE;
					g_Player[i].noDamage = TRUE;
					g_Player[i].dashCnt = 0;
					g_Player[i].state = DASH;
					g_Player[i].patternAnim = 0;
					PlaySound(SOUND_LABEL_SE_player_dash);
				}


				// MAP外チェック
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

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// 移動が終わったらエネミーとの当たり判定
				ENEMY* enemy = GetSkull();
				
				// ダメージを受けた後しばらく無敵にする
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

				// ダメージ処理中
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

				// 攻撃処理中
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
				// 攻撃？
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

				// プレイヤーの死亡処理中？
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
				// 死亡？
				else if (g_Player[i].HP <= 0)
				{
					g_Player[i].death = TRUE;
					g_Player[i].deathCnt = 0;
					g_Player[i].patternAnim = 0;
					g_Player[i].dash = FALSE;
					PlaySound(SOUND_LABEL_SE_player_explo);
				}
				// デッドゾーン当たり判定
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

				// チュートリアルメッセージ処理
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

				// 強制で次のステージに
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


	// 現状をセーブする
	/*if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}*/


#ifdef _DEBUG	// デバッグ情報を表示する

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
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			float px = 0.0f;
			float py = 0.0f;
			float pw = 0.0f;
			float ph = 0.0f;
			float tw = 0.0f;
			float th = 0.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// プレイヤーの分身を描画
			if (g_Player[i].dash)
			{	// ダッシュ中だけ分身処理
				DrawPlayerOffset(i);
			}

			switch (g_Player[i].state)
			{
			case IDLE:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= IdleNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / IdleNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標

				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / IdleNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(IdleNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}

				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
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

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case JUMP:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= JumpNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / JumpNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / JumpNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(JumpNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標

				}
				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

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

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case RUN_RIGHT:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= RunNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				tw = 1.0f / RunNum;	// テクスチャの幅
				th = 1.0f / 1;	// テクスチャの高さ
				tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

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
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
				g_counter++;

				break;

			case RUN_LEFT:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= RunNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				px -= 15.0f;			// 描画位置の調整

				// アニメーション用
				tw = -1.0f / RunNum;	// テクスチャの幅
				th = 1.0f / 1;	// テクスチャの高さ
				tx = (float)(RunNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

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
				
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				g_counter++;
				break;

			case HURT:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= HurtNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / HurtNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標

				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / HurtNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(HurtNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case FALL:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= FallNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / FallNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / FallNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(FallNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
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

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case DEATH:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= DeathNum)
				{
					g_Player[i].use = FALSE;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DeathNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DeathNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(DeathNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ

				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case ATTACK:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= AttackNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / AttackNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / AttackNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(AttackNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ

				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;

			case DASH:

				// アニメーション  
				{
					g_Player[i].countAnim += 1.0f;
					if (g_Player[i].countAnim > frameNum)
					{
						g_Player[i].countAnim = 0.0f;
						// 次のフレームに
						g_Player[i].patternAnim += 1;
					}
				}

				if (g_Player[i].patternAnim >= DashNum)
				{
					g_Player[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].state]);

				//プレイヤーの位置やテクスチャー座標を反映
				px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				pw = g_Player[i].w;		// プレイヤーの表示幅
				ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DashNum;	// テクスチャの幅
					tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DashNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(DashNum - g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Player[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

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

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				break;
			}

			for (int j = 0; j < MSG_HITBOX_MAX; j++)
			{
				if (g_Player[i].msg[j] == TRUE)
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10 + j]);

					//プレイヤーの位置やテクスチャー座標を反映
					px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
					py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
					pw = MSG_WIDTH;
					ph = MSG_HEIGHT;

					// アニメーション用
					if (g_Player[i].dir == DIRECTION::CHAR_DIR_RIGHT)
					{
						px += 50.0f;
						py -= 120.0f;
					}
					else if (g_Player[i].dir == DIRECTION::CHAR_DIR_LEFT)
					{
						px += 50.0f;				// 描画位置の調整
						py -= 120.0f;
					}
					tx = 0.0f;
					ty = 0.0f;
					tw = 1.0f;	// テクスチャの幅
					th = 1.0f;	// テクスチャの高さ

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する

	// プレイヤーの当たり範囲を表示する
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = g_Player[0].HBpos.x - bg->pos.x;	// ヒットボックスの表示位置X
	float py = g_Player[0].HBpos.y - bg->pos.y;	// ヒットボックスの表示位置Y
	float pw = g_Player[0].HBw;		// ヒットボックスの表示幅
	float ph = g_Player[0].HBh;		// ヒットボックスの表示高さ

	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
		g_Player[0].rot.z);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	if (g_Player[0].attack)
	{
		// プレイヤーの当たり範囲を表示する
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[0].atkHBpos.x - bg->pos.x;	// ヒットボックスの表示位置X
		float py = g_Player[0].atkHBpos.y - bg->pos.y;	// ヒットボックスの表示位置Y
		float pw = g_Player[0].atkHBw;		// ヒットボックスの表示幅
		float ph = g_Player[0].atkHBh;		// ヒットボックスの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
			g_Player[0].rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

#endif
}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].state]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 0;
		float tx = 0;
		if (g_Player[no].dir == DIRECTION::CHAR_DIR_RIGHT)
		{
			tw = 1.0f / DashNum;	// テクスチャの幅
			tx = (float)(g_Player[no].patternAnim) * tw;	// テクスチャの左上X座標
		}
		else if (g_Player[no].dir == DIRECTION::CHAR_DIR_LEFT)
		{
			tw = -1.0f / DashNum;	// テクスチャの幅
			tx = (float)(DashNum - g_Player[no].patternAnim) * tw;	// テクスチャの左上X座標
		}
		float th = 1.0f / 1;	// テクスチャの高さ
		float ty = (float)(g_Player[no].patternAnim / 1) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}



