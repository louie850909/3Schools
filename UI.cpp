//=============================================================================
//
// UI処理 [UI.cpp]
// Author : 林　劭羲
//
//=============================================================================
#include "UI.h"
#include "player.h"
#include "enemy_boss.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define UI_ITEM_MAX					(3)				// UIの数



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI/heart.png",
	"data/TEXTURE/UI/skull.png",
	"data/TEXTURE/UI/arrow.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static UiItem	g_UI[UI_ITEM_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
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


	// 変数の初期化
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		g_UI[i].use = FALSE;
		
		g_UI[i].w = TEXTURE_WIDTH;
		g_UI[i].h = TEXTURE_HEIGHT;
		g_UI[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_UI[i].texNo = 0;

		g_UI[i].scrl = 0.0f;		// TEXスクロール
		g_UI[i].scrl2 = 0.0f;		// TEXスクロール
	}

	switch (GetMode())
	{
	case MODE_TITLE:

		g_UI[0].use = true;
		g_UI[0].texNo = 2;
		g_UI[0].pos = XMFLOAT3(340.0f, 380.0f, 0.0f);
		g_UI[0].w = 50;
		g_UI[0].h = 50;

		break;
	}

	g_Load = TRUE;
	return S_OK;
}




//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		if (g_UI[i].use)
		{
			g_UI[i].old_pos = g_UI[i].pos;	// １フレ前の情報を保存

			switch (GetMode())
			{
			case MODE_TITLE:

				if (GetKeyboardTrigger(DIK_DOWN))
				{
					g_UI[i].pos.y += 95;
				}

				if (GetKeyboardTrigger(DIK_UP))
				{
					g_UI[i].pos.y -= 95;
				}

				if (g_UI[i].pos.y > 475)
				{
					g_UI[i].pos.y = 380;
				}

				if (g_UI[i].pos.y < 380)
				{
					g_UI[i].pos.y = 475;
				}

				break;
			}
		}
	}


	//g_BG.scrl -= 0.0f;		// 0.005f;		// スクロール


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}




//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
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

	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (GetMode() > MODE_TITLE)
		{
			if (player[i].use)
			{
				// 下敷きのゲージ（枠的な物）
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 100.0f;		// ゲージの表示位置X
				float py = 500.0f;		// ゲージの表示位置Y
				float pw = 300.0f;		// ゲージの表示幅
				float ph = 30.0f;		// ゲージの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);


				// エネミーの数に従ってゲージの長さを表示してみる
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//ゲージの位置やテクスチャー座標を反映
				pw = pw * ((float)player[i].HP / PLAYER_HP_MAX);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// ハート描画
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
					SetSpriteLTColor(g_VertexBuffer,
						px - 80, py - 25, 80, 80,
						tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}
	}

	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (GetMode() == MODE_BOSS)
		{
			// 下敷きのゲージ（枠的な物）
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//ゲージの位置やテクスチャー座標を反映
			float px = 330.0f;		// ゲージの表示位置X
			float py = 25.0f;		// ゲージの表示位置Y
			float pw = 300.0f;		// ゲージの表示幅
			float ph = 30.0f;		// ゲージの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);


			// エネミーの数に従ってゲージの長さを表示してみる
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

			//ゲージの位置やテクスチャー座標を反映
			pw = pw * ((float)boss[i].HP / BOSS_HP_MAX);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			// スカル描画
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
				SetSpriteLTColor(g_VertexBuffer,
					px - 70, py - 20, 70, 70,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	// arrow描画
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		if (g_UI[i].use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UI[i].texNo]);
			SetSpriteLTColor(g_VertexBuffer,
				g_UI[i].pos.x, g_UI[i].pos.y, g_UI[i].w, g_UI[i].h,
				0, 0, 1, 1,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する

#endif
}

UiItem* GetUiItem(void)
{
	return &g_UI[0];
}
