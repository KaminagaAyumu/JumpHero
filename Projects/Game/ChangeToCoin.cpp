#include "ChangeToCoin.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kChangeToCoinDefaultWidth = 40.0f;
	constexpr float kChangeToCoinDefaultHeight = 40.0f;

	constexpr float kPopChestUpPositionY = 40.0f; // 宝箱から出たときの上昇位置Y
	constexpr float kDropSpeed = 2.0f; // 宝箱から出たときの落下速度
}

ChangeToCoin::ChangeToCoin(const Position2& pos, int handle, bool isPopChest)
{
	m_pos = pos;
	m_colRect = { {m_pos},kChangeToCoinDefaultWidth,kChangeToCoinDefaultHeight };
	m_colCircle = { {m_pos},kChangeToCoinDefaultWidth / 2 };
	m_graphHandle = handle;
	if(isPopChest) // 宝箱から出た場合
	{
		m_updateFunc = &ChangeToCoin::DropUpdate;
	}
	else // 宝箱以外からの出現の場合
	{
		m_updateFunc = &ChangeToCoin::FloatingUpdate;
	}
}

void ChangeToCoin::Init()
{

}

void ChangeToCoin::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
	(this->*m_updateFunc)();
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

bool ChangeToCoin::IsAppear()
{
	// 宝箱から出た場合、上昇位置Yに達していなければ出現していない
	return m_updateFunc == &ChangeToCoin::DropUpdate && m_pos.y != kPopChestUpPositionY;
}

void ChangeToCoin::DropUpdate()
{
	float endY = m_pos.y - kPopChestUpPositionY;
	if (m_pos.y > endY)
	{
		m_pos.y -= kDropSpeed;
	}
	else
	{
		m_pos.y = endY;
	}
}

void ChangeToCoin::FloatingUpdate()
{
}

void ChangeToCoin::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		// 出現中の場合何もしない
		if(IsAppear)
		{
			return;
		}
		m_isExist = false;
	}
}