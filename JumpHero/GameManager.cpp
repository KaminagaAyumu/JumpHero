#include "GameManager.h"
#include <DxLib.h>
#include "Game/Player.h"

namespace
{
	constexpr int kFirstLife = 3;
}

GameManager::GameManager(Player* player) :
	m_score(0),
	m_currentScore(0),
	m_life(kFirstLife),
	m_medalNum(0),
	m_balloonNum(0),
	m_isClear(false)
{
	m_pPlayer = player;
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

void GameManager::Update()
{
	// スコアの更新処理
	if (m_score < m_currentScore) // スコアが現在のスコアより小さい場合
	{
		float add = (m_currentScore - m_score) * 0.2f; // 現在のスコアとの差分を計算
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
}

void GameManager::Draw()
{
	DrawFormatString(10, 80, 0xffffff, L"スコア:%d", m_score);
	DrawFormatString(10, 100, 0xffffff, L"残機:%d", m_life);
	DrawFormatString(10, 120, 0xffffff, L"メダルの所持数:%d", m_medalNum);
	DrawFormatString(10, 140, 0xffffff, L"風船を取った数:%d", m_balloonNum);
}

void GameManager::AddScore(int score)
{
	m_currentScore += score;
}
