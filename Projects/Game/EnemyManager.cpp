#include <vector>
#include <list>
#include <memory>
#include "EnemyManager.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/Map.h"
#include "Actor.h"
#include "EnemyBase.h"

EnemyManager::EnemyManager(Camera* camera, GameManager* gameManager) : 
	m_pCamera(camera),
	m_pGameManager(gameManager)
{
}

void EnemyManager::Update()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Update();
	}

	// 宝箱に存在しないものがある場合削除
	m_enemies.remove_if(
		[](std::shared_ptr<EnemyBase> enemy)
		{
			return enemy->IsDead();
		}
	);
}

void EnemyManager::Draw()
{

}

void EnemyManager::SpawnEnemy(Map* map)
{
}

void EnemyManager::PushActors(std::vector<Actor*>& actors)
{

}

size_t EnemyManager::GetChestNum()
{
	if (m_enemies.empty())
	{
		return 0;
	}
	return m_enemies.size();
}