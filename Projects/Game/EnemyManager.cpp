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

namespace
{
	constexpr int kSpawnTime = 600; // 敵生成までの時間

	// 敵スポーン位置(今後はプレイヤーとマップ基準で可変になる)
	constexpr float kEnemySpawnPosY = 120.0f;
}

EnemyManager::EnemyManager(Camera* camera, Player* player, GameManager* gameManager, Map* map) :
	m_pCamera(camera),
	m_pPlayer(player),
	m_pGameManager(gameManager),
	m_pMap(map),
	m_frameCount(0)
{
	m_enemies.clear(); // 敵リストを初期化
}

void EnemyManager::Update(Input& input)
{
	m_frameCount++;
	if (m_frameCount % kSpawnTime == 0) // 敵のスポーン時間になったら
	{
		// スポーンをする(今後は細かく条件を作る)
		SpawnEnemy(Position2{ m_pPlayer->GetPos().x,m_pPlayer->GetPos().y - kEnemySpawnPosY });
	}


	for (auto& enemy : m_enemies)
	{
		enemy->Update(input);
	}

	// 敵に存在しないものがある場合削除
	m_enemies.remove_if(
		[](std::shared_ptr<EnemyBase> enemy)
		{
			return enemy->IsDead() || !enemy ->IsOnScreen();
		}
	);
}

void EnemyManager::Draw()
{

}

void EnemyManager::SpawnEnemy(const Position2& pos)
{
	auto enemy = std::make_shared<TransformEnemy>(pos,m_pPlayer, m_pMap, TransformEnemy::EnemyForm::PlayerSeeker);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

const Position2& EnemyManager::FindSpawnPosition() const
{
	return Position2{};
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