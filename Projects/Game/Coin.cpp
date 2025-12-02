#include <functional>
#include "Coin.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kCoinDefaultWidth = 30.0f;
	constexpr float kCoinDefaultHeight = 30.0f;

	constexpr int kAddScore = 1000;
}

Coin::Coin(const Position2& pos)
{
	m_pos = pos;
	m_colRect = { {m_pos},kCoinDefaultWidth,kCoinDefaultHeight };
	m_colCircle = { {m_pos},kCoinDefaultWidth / 2 };
}

void Coin::Init()
{
	
}

void Coin::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void Coin::Draw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	if (m_isExist)
	{
		DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void Coin::OnCollected(GameManager& gameManager)
{
	// コインが取得された際の処理を行わせる
	gameManager.OnItemCollected(Types::ItemType::Coin);
}

void Coin::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}