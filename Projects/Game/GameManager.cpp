#include "GameManager.h"
#include "Actor.h"
#include "Player.h"
#include "ChestManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "../Utility/Camera.h"
#include "../Utility/Input.h"
#include "../Utility/Map.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFirstLife = 3; // プレイヤーの残機初期化用

	constexpr int kEnemySpawnTime = 600; // 敵がスポーンするまでの時間

	// ゴールフラグの位置(今後はマップから取得などにする)
	constexpr float kGoalPosX = 4250.5f;
	constexpr float kGoalPosY = 157.5f;

	constexpr float	kScoreAddRate = 0.2f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr int kCoinAddScore = 1000; // コイン取得時のスコア
}

GameManager::GameManager(Map* map, std::vector<Actor*>& actors) :
	m_frameCount(0),
	m_score(0),
	m_currentScore(0),
	m_life(kFirstLife),
	m_medalNum(0),
	m_balloonNum(0)
{
	m_pMap = map;
	m_pCamera = std::make_unique<Camera>();
	m_pPlayer = std::make_unique<Player>();
	m_pPlayer->SetCamera(m_pCamera.get());
	m_pPlayer->SetMap(map);
	m_pPlayer->Init();
	m_pChestManager = std::make_unique<ChestManager>(m_pCamera.get(), this);
	m_pChestManager->SpawnChest(map);
	m_pChestManager->PushActors(actors);
	m_pItemManager = std::make_unique<ItemManager>(m_pCamera.get(),this);
	m_pEnemyManager = std::make_unique<EnemyManager>(m_pCamera.get(), m_pPlayer.get(), this, map);

	m_pCamera->SetTarget(m_pPlayer.get());

	// アイテムを取った際のラムダ式定義
	// 風船を取った時
	m_itemCollectFunc[Types::ItemType::Balloon] = [this]()
		{
			m_balloonNum++; // 風船の数を加算
		};
	// 強化メダルを取った時
	m_itemCollectFunc[Types::ItemType::UpgradeMedal] = [this]()
		{
			m_medalNum++; // メダルの数を加算
		};
	// 1UPを取った時
	m_itemCollectFunc[Types::ItemType::LifeUp] = [this]()
		{
			m_life++; // 残機を加算
		};
	// コインを取った時
	m_itemCollectFunc[Types::ItemType::Coin] = [this]()
		{
			AddScore(kCoinAddScore); // スコアを加算
		};


}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	m_life = kFirstLife;
	m_medalNum = 0;
	m_balloonNum = 0;
}

void GameManager::Update(Input& input)
{
	m_frameCount++;
	m_pCamera->Update();

	// スコアの更新処理
	if (m_score < m_currentScore) // スコアが現在のスコアより小さい場合
	{
		int add = static_cast<int>((m_currentScore - m_score) * kScoreAddRate); // 現在のスコアとの差分を計算
		if (add <= kScoreThreshold) // 加算の値が閾値を超えたら
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

	// クリア状態かミス状態の時はプレイヤー以外の更新処理を行わない
	if (IsClear() || m_pPlayer->IsMiss())
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
	// 仮の表示用
	DrawFormatString(10, 80, 0xffffff, L"スコア:%d", m_score);
	DrawFormatString(10, 100, 0xffffff, L"残機:%d", m_life);
	DrawFormatString(10, 120, 0xffffff, L"メダルの所持数:%d", m_medalNum);
	DrawFormatString(10, 140, 0xffffff, L"風船を取った数:%d", m_balloonNum);
}

void GameManager::PushActors(std::vector<Actor*>& actors)
{
	actors.push_back(m_pPlayer.get());
	m_pChestManager->PushActors(actors);
	m_pItemManager->PushActors(actors);
	m_pEnemyManager->PushActors(actors);
}

void GameManager::AddScore(int score)
{
	m_currentScore += score;
}

bool GameManager::IsSkipCollision() const
{
	return IsClear() || m_pPlayer->IsMiss();
}

bool GameManager::IsClear() const
{
	// プレイヤーの座標がゴールの位置に来たら
	return m_pPlayer->GetPos().x >= kGoalPosX && m_pPlayer->GetPos().y <= kGoalPosY;
}

void GameManager::OnItemCollected(const Types::ItemType& type)
{
	if (m_itemCollectFunc.contains(type))
	{
		m_itemCollectFunc[type]();
	}
	else
	{
		assert(false && L"アイテムを取った際の関数が見つかりませんでした");
	}
}

void GameManager::DropItem(int x, int y)
{
	m_pItemManager->SpawnItem(x, y);
}

const size_t GameManager::GetActorNum() const
{
	// +1しているのはプレイヤーの数
	return 1 + m_pChestManager->GetChestNum() + m_pItemManager->GetItemNum() + m_pEnemyManager->GetEnemyNum();
}
