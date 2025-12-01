#include "EnemyBase.h"
#include "Player.h"
#include "../Utility/Camera.h"
#include "../Utility/Map.h"
#include "../Utility/Game.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	constexpr int kSeekPosY = 600;
	constexpr int kMaxDirectionChangeCount = 120;
	constexpr int kGraphWidth = 45;
	constexpr int kGraphHeight = 45;

	// この定数はマップから取得できるようにするかもしれない
	constexpr int kChipSize = 45; // マップ1つの大きさ
	constexpr float kChipScale = 1.0f; // マップの拡大率
}

EnemyBase::EnemyBase(Player* player, Map* map) :
	Actor(Types::ActorType::Enemy),
	m_pPlayer(player),
	m_pMap(map),
	m_direction{},
	m_isDead(false)
{
}

EnemyBase::~EnemyBase()
{
}



void EnemyBase::IsCollision(const Types::CollisionInfo& info)
{
}

bool EnemyBase::IsOnScreen() const
{
	Rect2D screenRange = m_pCamera->GetNowScreenArea();
	// 矩形の範囲内に居たらtrue、それ以外はfalse
	return m_pos.x >= screenRange.GetLeft() && m_pos.x <= screenRange.GetRight() && m_pos.y >= screenRange.GetTop() && m_pos.y <= screenRange.GetBottom();
}
