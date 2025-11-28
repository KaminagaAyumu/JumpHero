#include <vector>
#include <list>
#include <memory>
#include "../Utility/Geometry.h"
#include "EnemyManager.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/Map.h"
#include "Actor.h"
#include "EnemyBase.h"
#include "TransformEnemy.h"
#include "Player.h"

EnemyManager::EnemyManager(Camera* camera, Player* player, GameManager* gameManager) :
	m_pCamera(camera),
	m_pPlayer(player),
	m_pGameManager(gameManager)
{
	m_enemies.clear(); // 敵リストを初期化
}

void EnemyManager::Update(Input& input)
{
	for (auto& enemy : m_enemies)
	{
		enemy->Update(input);
	}

	// 敵に存在しないものがある場合削除
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

void EnemyManager::SpawnEnemy(const Position2& pos,Map* map)
{
	auto enemy = std::make_shared<TransformEnemy>(pos,m_pPlayer, map, TransformEnemy::EnemyForm::Normal);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

void EnemyManager::PushActors(std::vector<Actor*>& actors)
{
	for (auto& enemy : m_enemies)
	{
		actors.push_back(enemy.get());
	}
}

size_t EnemyManager::GetEnemyNum()
{
	if (m_enemies.empty())
	{
		return 0;
	}
	return m_enemies.size();
}