#include "LifeUp.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kLifeUpDefaultWidth = 40.0f;
	constexpr float kLifeUpDefaultHeight = 40.0f;

	constexpr int kAddScore = 1000;
}

LifeUp::LifeUp(const Position2& pos)
{
	m_pos = pos;
	m_colRect = { {m_pos},kLifeUpDefaultWidth,kLifeUpDefaultHeight };
	m_colCircle = { {m_pos},kLifeUpDefaultWidth / 2 };
}

void LifeUp::Init()
{

}

void LifeUp::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void LifeUp::Draw()
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

void LifeUp::OnCollected(GameManager& gameManager)
{
	// 1UPが取得された際の処理を行わせる
	gameManager.OnItemCollected(Types::ItemType::LifeUp);
}

void LifeUp::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}