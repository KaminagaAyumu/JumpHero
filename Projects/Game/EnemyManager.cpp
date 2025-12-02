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
#include "DxLib.h"

namespace
{
	constexpr int kSpawnTime = 600; // 敵生成までの時間

	// 敵スポーン位置(今後はプレイヤーとマップ基準で可変になる)
	constexpr float kEnemySpawnPosY = 120.0f;

	// 敵のアイテム化時間
	constexpr int kChangeToItemTime = 360;
	
	constexpr int kEnemyCoinAddScore = 1000;
}

EnemyManager::EnemyManager(Camera* camera, Player* player, GameManager* gameManager, Map* map) :
	m_pCamera(camera),
	m_pPlayer(player),
	m_pGameManager(gameManager),
	m_pMap(map),
	m_frameCount(0),
	m_itemTime(0)
{
	m_enemies.clear(); // 敵リストを初期化
}

void EnemyManager::Update(Input& input)
{
	m_frameCount++;
	m_itemTime--;

	// 敵の生成処理
	if (m_frameCount % kSpawnTime == 0) // 敵のスポーン時間になったら
	{
		if (IsChangeToItem()) // アイテム化状態の場合
		{
#ifdef _DEBUG
			printfDx(L"アイテム化中のため敵が生成できない\n");
#endif
		}
		else
		{
			// スポーンをする(今後は細かく条件を作る)
			SpawnEnemy(Position2{ m_pPlayer->GetPos().x,m_pPlayer->GetPos().y - kEnemySpawnPosY });
		}
	}

	// 敵の更新
	for (auto& enemy : m_enemies)
	{
		enemy->Update(input);
		if (IsChangeToItem()) // アイテム化している場合
		{
			if(enemy->IsDead()) // アイテム化で取得されたら
			{
				m_pGameManager->AddScore(kEnemyCoinAddScore); // コインを取った時と同じ処理をする
			}
		}
	}

	// 敵の削除
	if (IsChangeToItem()) // アイテム化状態の場合
	{
		// 敵に存在しないものがある場合削除
		m_enemies.remove_if(
			[](std::shared_ptr<EnemyBase> enemy)
			{
				// 死んだ判定のときにのみ削除
				return enemy->IsDead();
			}
		);
	}
	else // アイテム化状態ではない場合
	{
		// 敵に存在しないものがある場合削除
		m_enemies.remove_if(
			[](std::shared_ptr<EnemyBase> enemy)
			{
				// 死んだもしくは画面外にいるときに削除
				return enemy->IsDead() || !enemy->IsOnScreen();
			}
		);
	}
}

void EnemyManager::Draw()
{

}

void EnemyManager::SpawnEnemy(const Position2& pos)
{
	auto enemy = std::make_shared<TransformEnemy>(pos, m_pPlayer, m_pMap, TransformEnemy::EnemyForm::Skull);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

const Position2& EnemyManager::FindSpawnPosition() const
{
	return Position2{};
}

bool EnemyManager::IsChangeToItem()
{
	// アイテム化状態のカウントが進んでいるときにtrueを返す
	return m_itemTime > 0;
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

void EnemyManager::ChangeToItemAll()
{
	m_itemTime = kChangeToItemTime;
	for (auto& enemy : m_enemies)
	{
		enemy->ChangeToItem(kChangeToItemTime);
	}
}
