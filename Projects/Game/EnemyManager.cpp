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
	constexpr int kSpawnChipNo = 2; // マップの敵スポーンチップ番号

	constexpr int kSpawnTime = 600; // 敵生成までの時間

	// 敵スポーン位置のマージン(敵生成時に床から少し浮かせる)
	constexpr float kEnemySpawnPosMarginY = 0.1f;

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
	m_spawnPositions.clear(); // スポーン位置リストを初期化
	LoadSpawnPositions(map); // 敵スポーン位置の読み込み
}

void EnemyManager::Init(Map* map)
{
	m_pMap = map;
	m_enemies.clear(); // 敵リストを初期化
	m_spawnPositions.clear(); // スポーン位置リストを初期化
	LoadSpawnPositions(map); // 敵スポーン位置の読み込み
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
			if (m_spawnPositions.empty())
			{
				return;
			}
			Position2 spawnPos = SearchNearestSpawnPosition(m_pPlayer->GetPos()); // プレイヤーに一番近いスポーン位置を取得
			spawnPos.y -= kEnemySpawnPosMarginY; // 少し上にずらす
			SpawnEnemy(spawnPos); // 敵を生成
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
#ifdef _DEBUG
	for(const auto& pos : m_spawnPositions)
	{
		Position2 nearPos = SearchNearestSpawnPosition(m_pPlayer->GetPos());
		if(pos.x == nearPos.x && pos.y == nearPos.y)
		{
			// 最近点は色を変える
			int drawX = static_cast<int>(pos.x - m_pCamera->scroll.x);
			int drawY = static_cast<int>(pos.y - m_pCamera->scroll.y);
			// スポーン位置を表示(デバッグ用)
			DrawCircle(drawX, drawY, 10, GetColor(0, 255, 0), FALSE);
			continue;
		}
		int drawX = static_cast<int>(pos.x - m_pCamera->scroll.x);
		int drawY = static_cast<int>(pos.y - m_pCamera->scroll.y);
		// スポーン位置を表示(デバッグ用)
		DrawCircle(drawX, drawY, 10, GetColor(255, 0, 0), FALSE);
	}
#endif

}

void EnemyManager::LoadSpawnPositions(Map* map)
{
	for (int y = 0; y < map->GetMapHeight(); y++)
	{
		for (int x = 0; x < map->GetMapWidth(); x++)
		{
			if (map->GetPositioningData(x, y) == kSpawnChipNo)
			{
				// マップチップの1マスのサイズを取得(拡大を含む)
				float tileSize = map->GetTileSize();
				// スポーン位置はマップの中心にする
				Position2 pos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				// スポーン位置リストに追加
				m_spawnPositions.push_back(pos);
			}
		}
	}
}

void EnemyManager::SpawnEnemy(const Position2& pos)
{
	auto enemy = std::make_shared<TransformEnemy>(pos, m_pPlayer, m_pMap, TransformEnemy::EnemyForm::Skull);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

const Position2& EnemyManager::SearchNearestSpawnPosition(const Position2& playerPos)
{
	if(m_spawnPositions.empty())
	{
		return playerPos; // スポーン位置が存在しない場合はプレイヤー位置を返す
	}
	// 最初のスポーン位置を最近点として設定
	Position2 nearestPos = m_spawnPositions.front();
	for (const auto& spawnPos : m_spawnPositions)
	{
		// プレイヤーから一番近いスポーン位置を探す
		if (Geometry::GetDistance(playerPos, spawnPos) < Geometry::GetDistance(playerPos, nearestPos)) // 現在の最近点よりも近い場合
		{
			// 最近点を更新
			nearestPos = spawnPos;
		}
	}
	return nearestPos;
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
