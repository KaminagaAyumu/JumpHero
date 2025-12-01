#include "ItemBase.h"
#include "DxLib.h"
#include "../Utility/Camera.h"

namespace
{
	constexpr float kItemDefaultWidth = 45.0f;
	constexpr float kItemDefaultHeight = 45.0f;
}

ItemBase::ItemBase() : 
	Actor(Types::ActorType::Item),
	m_isExist(true)
{
}

ItemBase::ItemBase(float x, float y) : 
	Actor(Types::ActorType::Item),
	m_isExist(true)
{
	m_pos = { x,y };
	m_colRect = { {m_pos},kItemDefaultWidth,kItemDefaultHeight };
	m_colCircle = { {m_pos},kItemDefaultWidth / 2 };
}

ItemBase::~ItemBase()
{
}

void ItemBase::Init()
{
	m_pos = { 500.0f,400.0f };
	m_colRect = { {m_pos},kItemDefaultWidth,kItemDefaultHeight };
	m_colCircle = { {m_pos},kItemDefaultWidth / 2 };
}

void ItemBase::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void ItemBase::Draw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	if (m_isExist)
	{
		DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
		m_colCircle.Draw(drawX,drawY);
		m_colRect.Draw(drawX, drawY);
	}
}

void ItemBase::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		printfDx(L"Item : プレイヤーと衝突しました\n");
		m_scoreFunc(1000);
		m_isExist = false;
	}
}

void ItemBase::SetScoreFunc(std::function<void(int)> scoreFunc)
{
	m_scoreFunc = scoreFunc;
}
