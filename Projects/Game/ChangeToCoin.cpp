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

	constexpr float kScale = 1.0f; // 拡大率
}

ChangeToCoin::ChangeToCoin(const Position2& pos, int handle, bool isPopChest)
{
	m_pos = pos;
	m_startY = pos.y;
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
	m_startY = m_pos.y;
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
		DrawRotaGraph(drawX, drawY, kScale, 0.0f, m_graphHandle, true);
		//DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void ChangeToCoin::OnCollected(GameManager& gameManager)
{
	// 敵をコインに変えるアイテムが取得された際の処理を行わせる
	gameManager.OnItemCollected(Types::ItemType::ChangeToCoin);
	gameManager.NotifyItemEvent(Types::ItemType::ChangeToCoin);
}

bool ChangeToCoin::IsAppear()
{
	// コインの出現が終了する位置Y
	float endY = m_startY - kPopChestUpPositionY;
	// 宝箱から出た場合、上昇位置Yに達していなければ出現していない
	return m_updateFunc == &ChangeToCoin::DropUpdate && m_pos.y != endY;
}

void ChangeToCoin::DropUpdate()
{
	// コインの出現が終了する位置Y
	float endY = m_startY - kPopChestUpPositionY;
	if (m_pos.y > endY) // まだ上昇位置Yに達していない場合
	{
		m_pos.y -= kDropSpeed; // 上昇
	}
	else // 上昇位置Yに達した場合
	{
		m_pos.y = endY; // 位置を調整
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
		if(IsAppear())
		{
			return;
		}
		m_isExist = false; // それ以外の場合は消滅させる
	}
}