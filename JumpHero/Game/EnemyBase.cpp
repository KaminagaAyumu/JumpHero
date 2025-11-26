#include "EnemyBase.h"
#include "Player.h"
#include "../Camera.h"
#include "../Map.h"
#include <DxLib.h>
#include <cmath>

namespace
{
	constexpr int kSeekPosY = 600;
	constexpr int kMaxDirectionChangeCount = 120;
	constexpr int kGraphWidth = 45;
	constexpr int kGraphHeight = 45;
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