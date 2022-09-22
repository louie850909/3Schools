//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : 林　劭羲
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define frameNum					(5)		// 毎コマ5フレームにつぎ止まる
#define SkullFrameNum				(4)		// スカルのフレーム数
#define DeathFrameNum				(8)		// 死亡フレーム数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Monster/skull-no-fire.png",
	"data/TEXTURE/Monster/death.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Skull[ENEMY_MAX];		// エネミー構造体

static int		g_EnemyCnt = ENEMY_MAX;

static INTERPOLATION_DATA g_TutorialMoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1400.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
	{ XMFLOAT3(1600.0f,  900.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		60 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1440.0f,  100.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		300 },
	{ XMFLOAT3(100.0f,  100.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		300 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1110.0f,  350.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1700.0f,  350.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl2[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(680.0f,  720.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1025.0f,  720.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl3[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(180.0f,  470.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(750.0f,  470.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl4[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(50.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(550.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl5[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(725.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(1175.0f,  890.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl6[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(620.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
	{ XMFLOAT3(120.0f,  1030.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),		180 },
};

static INTERPOLATION_DATA g_Stage1MoveTbl7[] = {
	//座標									回転率							拡大率							時間
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
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

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
	g_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Skull[i].use = FALSE;
		g_Skull[i].pos = XMFLOAT3(1200.0f + i*200.0f, 100.0f, 0.0f);	// 中心点から表示
		g_Skull[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Skull[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Skull[i].w = TEXTURE_WIDTH;
		g_Skull[i].h = TEXTURE_HEIGHT;
		g_Skull[i].texNo = 0;

		g_Skull[i].HBw = 80.0f;
		g_Skull[i].HBh = 80.0f;

		g_Skull[i].countAnim = 0;
		g_Skull[i].patternAnim = 0;

		g_Skull[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
		g_Skull[i].dir = 1;

		g_Skull[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Skull[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Skull[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Skull[i].idle = TRUE;
		g_Skull[i].hurt = FALSE;
		g_Skull[i].hurtCnt = 0;

		g_Skull[i].attack = FALSE;

		g_EnemyCnt++;
	}

	switch (GetMode())
	{

	case MODE_TUTORIAL:
		// 0番だけ線形補間で動かしてみる
		g_Skull[0].time = 0.0f;		// 線形補間用のタイマーをクリア
		g_Skull[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
		g_Skull[0].tblMax = sizeof(g_TutorialMoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Skull[0].attack = TRUE;
		g_Skull[0].use = TRUE;
		break;

	case MODE_STAGE1:
		
		for (int i = 0; i < 8; i++)
		{
			g_Skull[i].time = 0.0f;		// 線形補間用のタイマーをクリア
			g_Skull[i].tblNo = i;		// 再生するアニメデータの先頭アドレスをセット
			g_Skull[i].tblMax = sizeof(g_Stage1MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
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
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// 生きてるエネミーの数
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
		// 生きてるエネミーだけ処理をする
		if (g_Skull[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Skull[i].pos;
			
			g_Skull[i].HBpos = XMFLOAT3(g_Skull[i].pos.x - 10.0f, g_Skull[i].pos.y + 5.0f, 0.0f);
			
			// 移動処理
			if (g_Skull[i].idle)
			{
				// 移動処理
				if (g_Skull[i].tblMax > 0)	// 線形補間を実行する？
				{	// 線形補間の処理
					int nowNo = (int)g_Skull[i].time;			// 整数分であるテーブル番号を取り出している
					int maxNo = g_Skull[i].tblMax;				// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = 0;
					
					switch (GetMode())
					{
					case MODE_TUTORIAL:
						tbl = g_TutorialMoveTblAdr[g_Skull[i].tblNo];	// 行動テーブルのアドレスを取得
						break;
						
					case MODE_STAGE1:
						tbl = g_Stage1MoveTblAdr[g_Skull[i].tblNo];	// 行動テーブルのアドレスを取得
					}

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_Skull [i] .time - nowNo;	// 時間部分である少数を取り出している

					Pos *= nowTime;								// 現在の移動量を計算している
					Rot *= nowTime;								// 現在の回転量を計算している
					Scl *= nowTime;								// 現在の拡大率を計算している

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

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_Skull[i].pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_Skull[i].rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_Skull[i].scl, nowScl + Scl);
					g_Skull[i].w = TEXTURE_WIDTH * g_Skull[i].scl.x;
					g_Skull[i].h = TEXTURE_HEIGHT * g_Skull[i].scl.y;

					// frameを使て時間経過処理をする
					g_Skull[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_Skull[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
					{
						g_Skull[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
					}
				}
			}

			// 移動が終わったらエネミーとの当たり判定
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
					// プレイヤーに攻撃された処理
					if ((player[j].attack == TRUE) && (player[j].patternAnim > 5))
					{
						BOOL ans = CollisionBB(g_Skull[i].pos, g_Skull[i].w, g_Skull[i].h,
							player[j].atkHBpos, player[j].atkHBw, player[j].atkHBh);
						// 当たっている？
						if ((ans == TRUE) && (g_Skull[i].hurt == FALSE))
						{
							g_Skull[i].hurt = TRUE;
							g_Skull[i].idle = FALSE;
							g_Skull[i].attack = FALSE;
							PlaySound(SOUND_LABEL_SE_sword_hit);
						}
					}

					// 攻撃判定
					if ((player[j].noDamage == FALSE) && g_Skull[i].attack == TRUE)
					{
						BOOL ans = CollisionBB(player[j].HBpos, player[j].HBw, player[j].HBh,
							g_Skull[i].HBpos, g_Skull[i].HBw, g_Skull[i].HBh);
						// 当たっている？
						if ((ans == TRUE) && (player[j].hurt == FALSE))
						{
							// 当たった時の処理
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
			g_EnemyCnt++;		// 生きてた敵の数
		}
	}


	// エネミー全滅チェック
	/*if (g_EnemyCnt <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}*/

#ifdef _DEBUG	// デバッグ情報を表示する

	PrintDebugProc("Enemy counts : %d\n", g_EnemyCnt);
	PrintDebugProc("skull0 use: %d\n", g_Skull[0].use);
	PrintDebugProc("skull0 hurt: %d\n", g_Skull[0].hurt);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Skull[i].use == TRUE)			// このエネミーが使われている？
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
				// アニメーション  
				{
					g_Skull[i].countAnim += 1.0f;
					if (g_Skull[i].countAnim > frameNum)
					{
						g_Skull[i].countAnim = 0.0f;
						// 次のフレームに
						g_Skull[i].patternAnim += 1;
					}
				}

				if (g_Skull[i].patternAnim >= SkullFrameNum)
				{
					g_Skull[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Skull[i].texNo]);

				//エネミーの位置やテクスチャー座標を反映
				px = g_Skull[i].pos.x - bg->pos.x;	// エネミーの表示位置X
				py = g_Skull[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
				pw = g_Skull[i].w;		// エネミーの表示幅
				ph = g_Skull[i].h;		// エネミーの表示高さ

				// アニメーション用
				if (g_Skull[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					px -= 5.0f;
					tw = 1.0f / SkullFrameNum;	// テクスチャの幅
					tx = (float)(g_Skull[i].patternAnim) * tw;	// テクスチャの左上X座標

				}
				else if (g_Skull[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / SkullFrameNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(SkullFrameNum - g_Skull[i].patternAnim) * tw;	// テクスチャの左上X座標
				}

				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Skull[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Skull[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			
			else if (g_Skull[i].hurt)
			{
				// アニメーション  
				{
					g_Skull[i].countAnim += 1.0f;
					if (g_Skull[i].countAnim > frameNum)
					{
						g_Skull[i].countAnim = 0.0f;
						// 次のフレームに
						g_Skull[i].patternAnim += 1;
					}
				}

				if (g_Skull[i].patternAnim >= DeathFrameNum)
				{
					g_Skull[i].patternAnim = 0;
				}

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				//エネミーの位置やテクスチャー座標を反映
				px = g_Skull[i].pos.x - bg->pos.x;	// エネミーの表示位置X
				py = g_Skull[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
				pw = g_Skull[i].w;		// エネミーの表示幅
				ph = g_Skull[i].h;		// エネミーの表示高さ

				// アニメーション用
				if (g_Skull[i].dir == DIRECTION::CHAR_DIR_RIGHT)
				{
					tw = 1.0f / DeathFrameNum;	// テクスチャの幅
					tx = (float)(g_Skull[i].patternAnim) * tw;	// テクスチャの左上X座標

				}
				else if (g_Skull[i].dir == DIRECTION::CHAR_DIR_LEFT)
				{
					tw = -1.0f / DeathFrameNum;	// テクスチャの幅
					px -= 15.0f;				// 描画位置の調整
					tx = (float)(DeathFrameNum - g_Skull[i].patternAnim) * tw;	// テクスチャの左上X座標
				}

				th = 1.0f / 1;	// テクスチャの高さ
				ty = (float)(g_Skull[i].patternAnim / 1) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Skull[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Skull[i].use)
		{
			// エネミーの当たり範囲を表示する
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Skull[i].HBpos.x - bg->pos.x;	// ヒットボックスの表示位置X
			float py = g_Skull[i].HBpos.y - bg->pos.y;	// ヒットボックスの表示位置Y
			float pw = g_Skull[i].HBw;		// ヒットボックスの表示幅
			float ph = g_Skull[i].HBh;		// ヒットボックスの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
				g_Skull[0].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
#endif // _DEBUG



}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetSkull(void)
{
	return &g_Skull[0];
}





