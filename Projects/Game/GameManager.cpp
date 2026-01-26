#include "GameManager.h"
#include "Actor.h"
#include "Player.h"
#include "ChestManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "Effect/EffectManager.h"
#include "../Utility/Camera.h"
#include "../Utility/Input.h"
#include "../Utility/Map.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFirstLife = 3; // プレイヤーの残機初期化用

	constexpr int kEnemySpawnTime = 600; // 敵がスポーンするまでの時間

	constexpr float kGoalWidth = 40.0f;
	constexpr float kGoalHeight = 60.0f;

	constexpr float	kScoreAddRate = 0.2f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr int kCoinAddScore = 1000; // コイン取得時のスコア

	constexpr int kBalloonForChangeToCoin = 5; // 敵をコインに変えるアイテムを落とすために必要な風船の数
}

GameManager::GameManager() :
	m_frameCount(0),
	m_score(0),
	m_currentScore(0),
	m_life(kFirstLife),
	m_medalNum(0),
	m_balloonNum(0),
	m_balloonCounter(0),
	m_totalBalloonNum(0),
	m_isMiniGame(false),
	m_isTutorial(false),
	m_isOpenGoal(false)
{
	// アイテムを取った際のラムダ式定義
	// 風船を取った時
	m_itemCollectFunc[Types::ItemType::Balloon] = [this](const Position2& pos)
		{
			m_balloonNum++; // 風船の数を加算
			m_balloonCounter++; // 風船取得カウンターを加算
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::Impact, pos);
		};
	// 強化メダルを取った時
	m_itemCollectFunc[Types::ItemType::UpgradeMedal] = [this](const Position2& pos)
		{
			m_medalNum++; // メダルの数を加算
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos);
		};
	// 1UPを取った時
	m_itemCollectFunc[Types::ItemType::LifeUp] = [this](const Position2& pos)
		{
			m_life++; // 残機を加算
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos);
		};
	// コインを取った時
	m_itemCollectFunc[Types::ItemType::Coin] = [this](const Position2& pos)
		{
			AddScore(kCoinAddScore); // スコアを加算
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos);
		};
	// 敵をコインに変えるアイテムを取った時
	m_itemCollectFunc[Types::ItemType::ChangeToCoin] = [this](const Position2& pos)
		{
			ChangeEnemyToCoin(); // 敵をすべてアイテム化
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::Star, pos);
		};
	m_itemCollectFunc[Types::ItemType::AttackItem] = [this](const Position2& pos)
		{
			// 攻撃アイテム取得時の処理
			m_pPlayer->AttackableStart();
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos);
		};


}

GameManager::~GameManager()
{
}

void GameManager::Init(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors, bool isTutorial)
{
	m_pMap = map;
	auto pMap = m_pMap.lock();
	m_pEffectManager = effectManager;
	m_pUIManager = uiManager;
	m_pPlayer = std::make_shared<Player>(map, weak_from_this());
	m_pPlayer->SetCamera(camera);
	m_pPlayer->Init();
	m_pChestManager = std::make_unique<ChestManager>(camera, this);
	m_pChestManager->SpawnChest(map);
	m_pChestManager->PushActors(actors);
	m_pItemManager = std::make_unique<ItemManager>(camera, weak_from_this());
	m_pItemManager->FirstSpawnItem(map);
	m_pItemManager->PushActors(actors);
	m_pEnemyManager = std::make_unique<EnemyManager>(camera, m_pPlayer.get(), weak_from_this(), map);

	m_life = kFirstLife;
	m_medalNum = 0;
	m_balloonNum = 0;
	m_balloonCounter = 0;
	m_totalBalloonNum = m_pItemManager->GetFirstBalloonNum();
	m_isMiniGame = false;
	m_isTutorial = isTutorial;
}

void GameManager::MiniGameInit(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors)
{
	m_pMap = map;
	auto pMap = m_pMap.lock();
	m_pEffectManager = effectManager;
	m_pUIManager = uiManager;
	m_pPlayer->InitMap(pMap);
	m_pPlayer->SetCamera(camera);
	m_pPlayer->Init();
	m_pEnemyManager->Init(pMap,camera);
	m_pChestManager->Init(camera);
	m_pItemManager->Init(camera);
	m_pItemManager->FirstSpawnItem(map);
	m_pItemManager->PushActors(actors);

	m_balloonNum = 0; // 取得した風船の数をリセット
	m_balloonCounter = 0; // 風船カウンタをリセット
	m_totalBalloonNum = m_pItemManager->GetFirstBalloonNum(); // 風船の総数を改めて取得
	m_isMiniGame = true; // ミニゲームフラグをセット
	m_isOpenGoal = false; // ゴールのアクティブ状態をリセット
}

void GameManager::Update(Input& input)
{
#ifdef _DEBUG
	// printfを初期化するコマンド(上入力しながらLボタン)
	if (input.IsTriggered("LShift") && input.IsPressed("Up"))
	{
		clsDx();
	}
#endif
	m_frameCount++;

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

	if(IsGameOver()) // ゲームオーバー状態なら更新処理を行わない
	{
		printfDx(L"Game Over\n");
		return;
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
	//DrawFormatString(10, 100, 0xffffff, L"残機:%d", m_life);
	//DrawFormatString(10, 120, 0xffffff, L"メダルの所持数:%d", m_medalNum);
	//DrawFormatString(10, 140, 0xffffff, L"風船を取った数:%d", m_balloonNum);
	m_pEnemyManager->Draw();
}

void GameManager::PushActors(std::vector<std::weak_ptr<Actor>>& actors)
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

void GameManager::ChangeEnemyToCoin()
{
	m_pEnemyManager->ChangeToItemAll();
}

bool GameManager::IsSkipCollision() const
{
	return IsClear() || m_pPlayer->IsMiss();
}

bool GameManager::IsClear() const
{
	// ここの内容は今後イベントマネージャーに任せます

	// ゴールが開いていない場合判定しない
	if (!m_isOpenGoal)
	{
		return false;
	}

	auto pMap = m_pMap.lock();

	// ゴールの座標
	Position2 goalPos = pMap->GetGoalPosToMap();
	// ゴールの範囲を設定
	Rect2D goalRange = { goalPos, kGoalWidth, kGoalHeight };
	// プレイヤーの矩形を取得
	Rect2D playerRect = m_pPlayer->GetColRect();
	// 確実に当たっていない状況を判定する
	if (playerRect.GetLeft() > goalRange.GetRight())	return false;
	if (playerRect.GetTop() > goalRange.GetBottom())	return false;
	if (playerRect.GetRight() < goalRange.GetLeft())	return false;
	if (playerRect.GetBottom() < goalRange.GetTop())	return false;
	return true;
}

bool GameManager::IsGameOver() const
{
	// 残機が0以下で、プレイヤーのミス状態が終わればゲームオーバー
	return m_life <= 0 && !m_pPlayer->IsMiss();
}

void GameManager::OnItemCollected(const Types::ItemType& type, const Position2& pos)
{
	if (m_itemCollectFunc.contains(type))
	{
		m_itemCollectFunc[type](pos);
	}
	else
	{
		assert(false && L"アイテムを取った際の関数が見つかりませんでした");
	}
}

void GameManager::DropItem(const Position2& pos)
{
	m_pItemManager->SpawnItem(pos,Types::ItemType::UpgradeMedal);
	if(IsDropChangeToCoin()) // 敵をコインに変えるアイテムを落とすか判定
	{
		m_pItemManager->SpawnItem(pos, Types::ItemType::ChangeToCoin);
	}
}

void GameManager::DropItem(const Position2& pos, const Types::ItemType& type)
{
	m_pItemManager->SpawnItem(pos, type);
	// チュートリアルでない場合
	if (IsDropChangeToCoin() && !m_isTutorial) // 敵をコインに変えるアイテムを落とすか判定
	{
		m_pItemManager->SpawnItem(pos, Types::ItemType::ChangeToCoin);
	}
}

void GameManager::SpawnFloatingItem(const Position2& pos)
{
	m_balloonCounter = 0;
	m_pItemManager->SpawnFloatingChangeToCoin(pos, m_pMap);
}

void GameManager::SpawnEnemy(const Position2& pos, int formNum)
{
	m_pEnemyManager->SpawnEnemy(pos, formNum);
}

void GameManager::PowerUpPlayer()
{
	// メダルを持っていれば強化を行う
	if (m_medalNum > 0)
	{
		if (m_pPlayer->PowerUp()) // プレイヤーの強化ができれば
		{
			m_medalNum--; // メダルを一つ減らす
		}
		else // 強化が出来なければ
		{
			// 何もしない
		}
	}
	else // メダルを持っていなければ
	{
		// 何もしない
#ifdef _DEBUG
		printfDx(L"メダルがないので強化できない\n");
#endif
	}
}

void GameManager::ChangeFreezePlayer()
{
	m_pPlayer->FreezeChange();
}

void GameManager::OpenChestToPosition(int x, int y)
{
	m_pChestManager->OpenChestAtPosition(x,y);
}

bool GameManager::IsExceededPlayer(const Position2& area)
{
	return m_pPlayer->GetPos().x >= area.x;
}

bool GameManager::IsPowerUpPlayer()
{
	return m_pPlayer->IsPowerUp();
}

bool GameManager::IsGetBalloon(int balloonNum, bool isAll)
{
	if (isAll)
	{
		// 風船を取得した数分のカウンタから比較
		return m_balloonNum >= balloonNum;
	}
	else
	{
		// リセットありの風船のカウンタから比較
		return m_balloonCounter >= balloonNum;
	}
}

void GameManager::SetBarrierPlayer(const ActivePosition2& barrier)
{
	m_pPlayer->SetBarrier(barrier);
}

const size_t GameManager::GetActorNum() const
{
	// +1しているのはプレイヤーの数
	return 1 + m_pChestManager->GetChestNum() + m_pItemManager->GetItemNum() + m_pEnemyManager->GetEnemyNum();
}

const Position2& GameManager::GetPlayerPos() const
{
	return m_pPlayer->GetPos();
}

float GameManager::GetBalloonCounterRate() const
{
	return static_cast<float>(m_balloonCounter) / kBalloonForChangeToCoin;
}

float GameManager::GetChangeToCoinTimeRate() const
{
	return m_pEnemyManager->GetItemTimeRate();
}

void GameManager::NotifyItemEvent(Types::ItemType type)
{
	m_itemPickEvent.push_back({ type, m_frameCount });
}

bool GameManager::IsItemPicked(Types::ItemType itemType)
{
	for (auto it = m_itemPickEvent.begin(); it != m_itemPickEvent.end(); it++)
	{
		if (it->type == itemType)
		{
			m_itemPickEvent.erase(it);
			return true;
		}
	}
	return false;
}

void GameManager::RequestCreateEffect(Types::EffectType effectType, const Position2& pos)
{
	if (auto manager = m_pEffectManager.lock())
	{
		manager->CreateEffekseerEffect(effectType, pos);
	}
}

bool GameManager::IsDropChangeToCoin()
{
	if(m_balloonCounter >= kBalloonForChangeToCoin) // 風船を5個取ったら
	{
		// ここでリセットしているのがよくない
		m_balloonCounter = 0; // カウンターをリセット
		return true;
	}
	return false;
}
