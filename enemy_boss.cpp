//=============================================================================
//
// Boss処理 [enemy_boss.cpp]
// Author : 林　劭羲
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200)	// キャラサイズ
#define TEXTURE_HEIGHT				(200)	// 
#define TEXTURE_MAX					(6)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define frameNum					(5)		// 毎コマ5フレームにつぎ止まる
#define idleFrameNum				(6)		// idleフレーム数
#define atkFrameNum					(8)
#define DeathFrameNum				(22)		// 死亡フレーム数


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Monster/demon-idle.png",
	"data/TEXTURE/Monster/boss_death.png",
	"data/TEXTURE/Monster/bg.png",
	"data/TEXTURE/Monster/demon-attack.png",
	"data/TEXTURE/Monster/breath-fire.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BOSS	g_Boss[BOSS_MAX];		// エネミー構造体

static INTERPOLATION_DATA g_ToRightDownTbl[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1750.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1750.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToLeftDownTbl[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(150.0f,  750.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(150.0f,  750.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToLeftUpTbl[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(300.0f,  450.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(300.0f,  500.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToRightUpTbl[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1650.0f, 620.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1650.0f, 620.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_ToCenterTbl[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1920.0f / 2, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},	
	{ XMFLOAT3(1920.0f / 2, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		120},
};

static INTERPOLATION_DATA g_ToPlayerTbl[] = {
	//座標									回転率							拡大率							時間
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
// 初期化処理
//=============================================================================
HRESULT InitEnemyBoss(void)
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


	// エネミー構造体の初期化
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_Boss[i].use = TRUE;
		g_Boss[i].pos = XMFLOAT3(1920.0f / 2, 800.0f, 0.0f);	// 中心点から表示
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

		g_Boss[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
		g_Boss[i].dir = DIRECTION::CHAR_DIR_RIGHT;

		g_Boss[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Boss[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Boss[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

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
// 終了処理
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[4];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[5];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[1];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[2];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[0];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[3];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				XMFLOAT3 dir;
				XMStoreFloat3(&dir, Pos);

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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

			// 攻撃された判定
			for (int j = 0; j < 2; j++)
			{
				if ((player[0].use == TRUE) && (player[0].attack == TRUE) && (player[0].patternAnim > 5))
				{
					BOOL ans = CollisionBB(g_Boss[i].hb[j].HBpos, g_Boss[j].hb[j].HBw, g_Boss[i].hb[j].HBh,
						player[0].atkHBpos, player[0].atkHBw, player[0].atkHBh);
					// 当たっている？
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

			// 攻撃判定
			if (g_Boss[i].atkHB.use == TRUE)
			{
				if ((player[0].noDamage == FALSE))
				{
					BOOL ans = CollisionBB(player[0].HBpos, player[0].HBw, player[0].HBh,
						g_Boss[i].atkHB.HBpos, g_Boss[i].atkHB.HBw, g_Boss[i].atkHB.HBh);
					// 当たっている？
					if ((ans == TRUE) && (player[0].hurt == FALSE))
					{
						// 当たった時の処理
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


#ifdef _DEBUG	// デバッグ情報を表示する

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
			// アニメーション  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// 次のフレームに
					g_Boss[0].patternAnim += 1;
				}
			}

			if (g_Boss[0].patternAnim >= idleFrameNum)
			{
				g_Boss[0].patternAnim = 0;
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Boss[0].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			px = g_Boss[0].pos.x - bg->pos.x;	// エネミーの表示位置X
			py = g_Boss[0].pos.y - bg->pos.y;	// エネミーの表示位置Y
			pw = g_Boss[0].w;		// エネミーの表示幅
			ph = g_Boss[0].h;		// エネミーの表示高さ

			// アニメーション用
			if (g_Boss[0].dir == DIRECTION::CHAR_DIR_LEFT)
			{
				tw = 1.0f / idleFrameNum;	// テクスチャの幅
				tx = (float)(g_Boss[0].patternAnim) * tw;	// テクスチャの左上X座標

			}
			else if (g_Boss[0].dir == DIRECTION::CHAR_DIR_RIGHT)
			{
				tw = -1.0f / idleFrameNum;	// テクスチャの幅
				px -= 15.0f;				// 描画位置の調整
				tx = (float)(idleFrameNum - g_Boss[0].patternAnim) * tw;	// テクスチャの左上X座標
			}

			th = 1.0f / 1;	// テクスチャの高さ
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
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

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		else if (g_Boss[0].death)
		{

			// アニメーション  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// 次のフレームに
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

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//エネミーの位置やテクスチャー座標を反映
			px = g_Boss[0].pos.x - bg->pos.x;	// エネミーの表示位置X
			py = g_Boss[0].pos.y - bg->pos.y;	// エネミーの表示位置Y
			pw = g_Boss[0].w;		// エネミーの表示幅
			ph = g_Boss[0].h;		// エネミーの表示高さ

			// アニメーション用
			tw = 1.0f / DeathFrameNum;	// テクスチャの幅
			tx = (float)(g_Boss[0].patternAnim) * tw;	// テクスチャの左上X座標
			th = 1.0f / 1;	// テクスチャの高さ
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[0].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		else if ((g_Boss[0].attack) || (g_Boss[0].SpAtk))
		{
		// アニメーション  
			{
				g_Boss[0].countAnim += 1.0f;
				if (g_Boss[0].countAnim > frameNum)
				{
					g_Boss[0].countAnim = 0.0f;
					// 次のフレームに
					g_Boss[0].patternAnim += 1;
				}
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			//エネミーの位置やテクスチャー座標を反映
			px = g_Boss[0].pos.x - 10.0f - bg->pos.x;	// エネミーの表示位置X
			py = g_Boss[0].pos.y - 20.0f - bg->pos.y;	// エネミーの表示位置Y
			pw = g_Boss[0].w + 15.0f;		// エネミーの表示幅
			ph = g_Boss[0].h + 15.0f;		// エネミーの表示高さ

			// アニメーション用
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

				tw = 1.0f / atkFrameNum;	// テクスチャの幅
				tx = (float)(g_Boss[0].patternAnim) * tw;	// テクスチャの左上X座標

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

				tw = -1.0f / atkFrameNum;	// テクスチャの幅
				tx = (float)(atkFrameNum - g_Boss[0].patternAnim) * tw;	// テクスチャの左上X座標
			}

			th = 1.0f / 1;	// テクスチャの高さ
			ty = (float)(g_Boss[0].patternAnim / 1) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
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

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			if (g_Boss[0].atkHB.use)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				px = g_Boss[0].atkPos.x - bg->pos.x;	// ヒットボックスの表示位置X
				py = g_Boss[0].atkPos.y - bg->pos.y;	// ヒットボックスの表示位置Y
				pw = g_Boss[0].atkW;		// ヒットボックスの表示幅
				ph = g_Boss[0].atkH;		// ヒットボックスの表示高さ

				int pattern = (g_Boss[0].attackCnt / frameNum) % 5;

				if (g_Boss[0].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = 1.0f / 5;	// テクスチャの幅
					tx = (float)(pattern)*tw;	// テクスチャの左上X座標
				}
				else if(g_Boss[0].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = -1.0f / 5;	// テクスチャの幅
					tx = (float)(5 - pattern)*tw;	// テクスチャの左上X座標
				}
				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Boss[0].patternAnim / 1) * th;	// テクスチャの左上Y座標

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Boss[0].rot.z);

				// ポリゴン描画
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
				// エネミーの当たり範囲を表示する
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Boss[i].hb[j].HBpos.x - bg->pos.x;	// ヒットボックスの表示位置X
				float py = g_Boss[i].hb[j].HBpos.y - bg->pos.y;	// ヒットボックスの表示位置Y
				float pw = g_Boss[i].hb[j].HBw;		// ヒットボックスの表示幅
				float ph = g_Boss[i].hb[j].HBh;		// ヒットボックスの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
					g_Boss[0].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			if (g_Boss[i].atkHB.use)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Boss[i].atkHB.HBpos.x - bg->pos.x;	// ヒットボックスの表示位置X
				float py = g_Boss[i].atkHB.HBpos.y - bg->pos.y;	// ヒットボックスの表示位置Y
				float pw = g_Boss[i].atkHB.HBw;		// ヒットボックスの表示幅
				float ph = g_Boss[i].atkHB.HBh;		// ヒットボックスの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
					g_Boss[0].rot.z);

				// ポリゴン描画
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
