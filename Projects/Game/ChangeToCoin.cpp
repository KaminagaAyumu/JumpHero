#include "ChangeToCoin.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kChangeToCoinDefaultWidth = 40.0f;
	constexpr float kChangeToCoinDefaultHeight = 40.0f;

	constexpr int kAddScore = 1000;
}

ChangeToCoin::ChangeToCoin(const Position2& pos)
{
	m_pos = pos;
	m_colRect = { {m_pos},kChangeToCoinDefaultWidth,kChangeToCoinDefaultHeight };
	m_colCircle = { {m_pos},kChangeToCoinDefaultWidth / 2 };
}

void ChangeToCoin::Init()
{

}

void ChangeToCoin::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void ChangeToCoin::Draw()
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

void ChangeToCoin::OnCollected(GameManager& gameManager)
{
	// 風船が取得された際の処理を行わせる
	gameManager.OnItemCollected(Types::ItemType::ChangeToCoin);
}

void ChangeToCoin::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}