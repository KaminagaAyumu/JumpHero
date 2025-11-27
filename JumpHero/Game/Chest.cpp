#include "Chest.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "Player.h"
#include "ItemManager.h"
#include <DxLib.h>


namespace
{
	constexpr float kDefaultWidth = 45.0f;
	constexpr float kDefaultHeight = 45.0f;
	constexpr float kChipSize = 45.0f;
	constexpr float kChipScale = 1.0f;

	constexpr int kSpaceChipNo = 79;
}

Chest::Chest() :
	Actor(Types::ActorType::Chest),
	m_isOpen(false),
	m_pMap(nullptr),
	m_chipPos{}
{
}

Chest::Chest(int x, int y, Map* map) :
	Actor(Types::ActorType::Chest),
	m_pMap(map),
	m_chipPos{x,y},
	m_isOpen(false)
{
	float tileSize = kChipSize * kChipScale;
	m_pos = { static_cast<float>(m_chipPos.x) * tileSize + tileSize * 0.5f,static_cast<float>(m_chipPos.y) * tileSize + tileSize * 0.5f };
	m_colRect = { m_pos,kDefaultWidth,kDefaultHeight };
}

Chest::~Chest()
{
}

void Chest::Init()
{
}

void Chest::Update(Input&)
{
	m_colRect.pos = m_pos;
}

void Chest::Draw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	if (m_isOpen)
	{
		//DrawGraph(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), m_openGraphHandle, TRUE);
		//DrawString(drawX, drawY, L"アイテム", 0xffffff);
	}
	else
	{
#ifdef _DEBUG
		m_colRect.Draw(drawX, drawY);
#endif
		//DrawGraph(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), m_closeGraphHandle, TRUE);
	}
}

void Chest::IsCollision(const Types::CollisionInfo& info)
{
	// プレイヤーと衝突している時
	if(info.otherType == Types::ActorType::Player)
	{
		printfDx(L"Chest : プレイヤーと衝突しました\n");
		

		//OpenChest(); // 仮で当たったら開くようにする
	}
}

void Chest::OpenChest()
{
	m_isOpen = true;

#ifdef _DEBUG
	if (m_pMap == nullptr)
	{
		printfDx(L"Chest : マップのデータが入っていません\n");
		return;
	}
#endif

	m_pMap->SetMapChip(m_chipPos.x, m_chipPos.y, kSpaceChipNo);
}
