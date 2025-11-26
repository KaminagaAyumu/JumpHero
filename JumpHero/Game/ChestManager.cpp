#include "ChestManager.h"
#include "Chest.h"
#include "../Camera.h"
#include "../Input.h"
#include "../Map.h"
#include "Actor.h"
#include "../GameManager.h"
#include "Player.h"

namespace
{
	constexpr int kChestChipNo = 46;
}


ChestManager::ChestManager(Camera* camera, GameManager* gameManager) :
	m_pCamera(camera),
	m_pGameManager(gameManager)
{
}

void ChestManager::Update(Input& input)
{
	for (auto& chest : m_chests)
	{
		chest->Update(input);
		if (chest->IsOpen())
		{
			m_pGameManager->DropItem(chest->GetPos().x, chest->GetPos().y);
		}
	}

	// 宝箱に存在しないものがある場合削除
	m_chests.remove_if(
		[](std::shared_ptr<Chest> chest)
		{
			return chest->IsOpen();
		}
	);
}

void ChestManager::Draw()
{
	/*for (auto& chest : m_chests)
	{
		chest->Draw();
	}*/
}

void ChestManager::PushActors(std::vector<Actor*>& actors)
{
	for (auto& chest : m_chests)
	{
		actors.push_back(chest.get());
	}
}

size_t ChestManager::GetChestNum()
{
	return m_chests.size();
}

void ChestManager::SpawnChest(Map* map)
{
	for (int y = 0; y < map->GetMapHeight(); y++)
	{
		for (int x = 0; x < map->GetMapWidth(); x++)
		{
			if (map->GetMapChipNum(x, y) == kChestChipNo)
			{
				auto chest = std::make_shared<Chest>(x, y, map);
				chest->SetCamera(m_pCamera);
				m_chests.push_back(chest);
			}
		}
	}
}
