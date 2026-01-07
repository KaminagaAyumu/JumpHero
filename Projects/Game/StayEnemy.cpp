#include "StayEnemy.h"

StayEnemy::StayEnemy(const Position2& pos, Player* player, Map* map) :
	EnemyBase(player, map)
{
	m_pos = pos;
}

void StayEnemy::Init()
{
}

void StayEnemy::Update(Input&)
{
}

void StayEnemy::Draw()
{
}

void StayEnemy::ChangeToItem(int time)
{
}

void StayEnemy::IsCollision(const Types::CollisionInfo& info)
{
}
