#include <memory>
#include <vector>
#include "GameManager.h"
#include "DxLib.h"
#include "Actor.h"
#include "Player.h"
#include "ChestManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "../Utility/Camera.h"
#include "../Utility/Input.h"
#include "../Utility/Map.h"

namespace
{
	constexpr int kFirstLife = 3;
}

GameManager::GameManager(Player* player, Camera* camera, Map* map, std::vector<Actor*>& actors) :
	m_score(0),
	m_currentScore(0),
	m_life(kFirstLife),
	m_medalNum(0),
	m_balloonNum(0),
	m_isClear(false)
{
	m_pPlayer = player;
	m_pChestManager = std::make_unique<ChestManager>(camera,this);
	m_pChestManager->SpawnChest(map);
	m_pChestManager->PushActors(actors);
	m_pItemManager = std::make_unique<ItemManager>(camera);
	m_pEnemyManager = std::make_unique<EnemyManager>(camera, player, this);
	m_pEnemyManager->SpawnEnemy(Position2{ 300.0f,300.0f }, map);
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	m_life = kFirstLife;
	m_medalNum = 0;
	m_balloonNum = 0;
	m_isClear = false;
}

void GameManager::Update(Input& input)
{
	// スコアの更新処理
	if (m_score < m_currentScore) // スコアが現在のスコアより小さい場合
	{
		int add = static_cast<int>((m_currentScore - m_score) * 0.2f); // 現在のスコアとの差分を計算
		if (add <= 0.9f)
		{
			m_score = m_currentScore; // スコアを現在のスコアに合わせる
		}
		else
		{
			m_score += add; // スコアを更新
		}
		if (m_score > m_currentScore) // スコアが現在のスコアを超えた場合
		{
			m_score = m_currentScore; // スコアを現在のスコアに合わせる
		}
	}

	// プレイヤーがゴールの地点に来たら
	if (m_pPlayer->GetPos().x >= 4250.5f && m_pPlayer->GetPos().y <= 157.5f)
	{
		m_isClear = true;
	}

	// クリア状態かミス状態の時はプレイヤー以外の更新処理を行わない
	if (m_isClear || m_pPlayer->IsMiss())
	{
		m_pPlayer->Update(input);
	}
	else
	{
		m_pPlayer->Update(input);
		m_pChestManager->Update(input);
		m_pItemManager->Update(input);
		m_pEnemyManager->Update(input);
	}
	

}

void GameManager::Draw() const
{
	DrawFormatString(10, 80, 0xffffff, L"スコア:%d", m_score);
	DrawFormatString(10, 100, 0xffffff, L"残機:%d", m_life);
	DrawFormatString(10, 120, 0xffffff, L"メダルの所持数:%d", m_medalNum);
	DrawFormatString(10, 140, 0xffffff, L"風船を取った数:%d", m_balloonNum);
}

void GameManager::PushActors(std::vector<Actor*>& actors)
{
	actors.push_back(m_pPlayer);
	m_pChestManager->PushActors(actors);
	m_pItemManager->PushActors(actors);
	m_pEnemyManager->PushActors(actors);
}

void GameManager::AddScore(int score)
{
	m_currentScore += score;
}

void GameManager::DropItem(int x, int y)
{
	m_pItemManager->SpawnItem(x, y);
}

const size_t& GameManager::GetActorNum() const
{
	// +1しているのはプレイヤーの数
	return 1 + m_pChestManager->GetChestNum() + m_pItemManager->GetItemNum() + m_pEnemyManager->GetEnemyNum();
}
