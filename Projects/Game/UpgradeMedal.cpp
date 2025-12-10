#include "UpgradeMedal.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kMedalDefaultWidth = 40.0f;
	constexpr float kMedalDefaultHeight = 40.0f;

	constexpr float kMedalScale = 1.40625f;
}

UpgradeMedal::UpgradeMedal(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kMedalDefaultWidth,kMedalDefaultHeight };
	m_colCircle = { {m_pos},kMedalDefaultWidth / 2 };
	m_graphHandle = handle;
}

void UpgradeMedal::Init()
{

}

void UpgradeMedal::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void UpgradeMedal::Draw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	if (m_isExist)
	{
		DrawRotaGraph(drawX, drawY, kMedalScale, 0.0, m_graphHandle, TRUE);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void UpgradeMedal::OnCollected(GameManager& gameManager)
{
	// 風船が取得された際の処理を行わせる
	gameManager.OnItemCollected(Types::ItemType::UpgradeMedal);
}

void UpgradeMedal::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}