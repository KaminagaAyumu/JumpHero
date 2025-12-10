#include <memory>
#include <vector>
#include "ItemManager.h"
#include "ItemBase.h"
#include "Coin.h"
#include "Balloon.h"
#include "UpgradeMedal.h"
#include "LifeUp.h"
#include "ChangeToCoin.h"
#include "../Utility/Map.h"
#include "../Utility/Input.h"
#include <cassert>
#include "DxLib.h"

namespace
{
	constexpr int kBallonChipNo = 33; // 風船のマップチップ番号

	// 画像を識別するためのenum
	enum
	{
		kGraphBalloon, // 風船
		kGraphUpgradeMedal, // 強化メダル
		kGraphLifeUp, // 1UP
		kGraphCoin, // スコア加算用コイン
		kGraphChangeToCoin, // 敵をコインに変える

		kGraphNum // 画像の数
	};

	// 画像ファイル名の配列
	const std::wstring kGraphFileName[] =
	{
		L"data/balloon_red.png", // 風船
		L"data/upgrade_medal.png", // 強化メダル
		L"data/life_up.png", // 1UP
		L"data/coin.png", // スコア加算用コイン
		L"data/change_to_coin.png", // 敵をコインに変える
	};

	// 画像枚数が違うときにエラーを出せるようにする
	static_assert(kGraphNum == _countof(kGraphFileName), "画像枚数の定義が間違っています");
}

ItemManager::ItemManager(Camera* camera, GameManager* gameManager) :
	m_pCamera(camera),
	m_pGameManager(gameManager)
{
	m_pItems.clear();
	for(int i = 0; i < kGraphNum; i++)
	{
		int graphHandle = LoadGraph(kGraphFileName[i].c_str());
		assert(graphHandle != -1 && "画像の読み込みに失敗しました");
		m_graphHandles.push_back(graphHandle);
	}
}

ItemManager::~ItemManager()
{
	for(auto& handle : m_graphHandles)
	{
		DeleteGraph(handle);
	}
}

void ItemManager::Init()
{
	m_pItems.clear();
}

void ItemManager::Update(Input& input)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Update(input);
		if (!item->IsExist())
		{
			item->OnCollected(*m_pGameManager);
		}
	}

	// アイテムに存在しないものがある場合削除
	m_pItems.remove_if(
		[](std::shared_ptr<ItemBase> item )
		{ 
			return !item->IsExist();
		}
	);
}

void ItemManager::Draw()
{
	/*if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Draw();
	}*/
}

void ItemManager::SpawnItem(int x, int y)
{
	Position2 pos = { x,y };
	auto item = std::make_shared<UpgradeMedal>(pos,m_graphHandles[kGraphUpgradeMedal]);
	item->SetCamera(m_pCamera);
	m_pItems.push_back(item);
}

void ItemManager::FirstSpawnItem(Map* map)
{
	for (int y = 0; y < map->GetMapHeight(); y++)
	{
		for (int x = 0; x < map->GetMapWidth(); x++)
		{
			if (map->GetChestPosToMap(x, y) == kBallonChipNo)
			{
				// マップチップの1マスのサイズを取得(拡大を含む)
				float tileSize = map->GetTileSize();
				// 風船のスポーン位置はマップの中心にする
				Position2 pos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				// 風船生成
				auto balloon = std::make_shared<Balloon>(pos,m_graphHandles[kGraphBalloon]);
				balloon->SetCamera(m_pCamera); // カメラセット
				m_pItems.push_back(balloon); // アイテムリストに追加
			}
		}
	}
}

void ItemManager::PushActors(std::vector<Actor*>& actors)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		actors.push_back(item.get());
	}
}

size_t ItemManager::GetItemNum()
{
	if (m_pItems.empty())
	{
		return 0;
	}
	return m_pItems.size();
}
