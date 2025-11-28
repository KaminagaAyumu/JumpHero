#include "TransformEnemy.h"
#include "../Utility/Camera.h"
#include "DxLib.h"

namespace
{
	constexpr float kEnemyWidth		= 45.0f;	// 敵の実際の幅
	constexpr float kEnemyHeight	= 45.0f;	// 敵の実際の高さ

	constexpr float kGravity		= 0.5f;		// 敵にかかる重力
}

TransformEnemy::TransformEnemy(Player* player, Map* map) : 
	EnemyBase(player,map),
	m_updateFunc(&TransformEnemy::NormalUpdate),
	m_drawFunc(&TransformEnemy::NormalDraw),
	m_form(EnemyForm::Normal),
	m_frameCount(0),
	m_turnCount(0)
{
}

void TransformEnemy::Init()
{
	m_pos = { 300.0f,300.0f };
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Update()
{
	(this->*m_updateFunc)();
}

void TransformEnemy::Draw()
{
	(this->*m_drawFunc)();
}

void TransformEnemy::IsCollision(const Types::CollisionInfo& info)
{
}

void TransformEnemy::NormalUpdate()
{
	m_frameCount++;

}

void TransformEnemy::SeekerUpdate()
{
}

void TransformEnemy::FireBallUpdate()
{
}

void TransformEnemy::NormalDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::SeekerDraw()
{
}

void TransformEnemy::FireBallDraw()
{
}


void TransformEnemy::CheckHitMap()
{
}
