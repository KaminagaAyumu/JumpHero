#include "TransformEnemy.h"
#include "../Utility/Camera.h"
#include "DxLib.h"
#include "../Utility/Map.h"
#include "Player.h"

namespace
{
	constexpr float kEnemyWidth		= 45.0f;	// 敵の実際の幅
	constexpr float kEnemyHeight	= 45.0f;	// 敵の実際の高さ

	constexpr float kGravity		= 0.5f;		// 敵にかかる重力
	constexpr float kMoveSpeed		= 2.0f;		// 左右移動の速さ
}

TransformEnemy::TransformEnemy(const Position2& pos, Player* player, Map* map, EnemyForm changeForm) :
	EnemyBase(player,map),
	m_updateFunc(&TransformEnemy::NormalUpdate),
	m_drawFunc(&TransformEnemy::NormalDraw),
	m_currentForm(EnemyForm::Normal),
	m_nextForm(changeForm),
	m_frameCount(0),
	m_turnCount(0),
	m_isGround(false),
	m_velocity{1.0f,0.0f}
{
	m_pos = pos;
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Init()
{
	m_pos = { 300.0f,300.0f };
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void TransformEnemy::Draw()
{
	(this->*m_drawFunc)();
}

void TransformEnemy::IsCollision(const Types::CollisionInfo& info)
{
}

void TransformEnemy::NormalUpdate(Input&)
{
	m_frameCount++;

	m_pos.y += m_velocity.y * m_direction.y + kGravity * m_frameCount * 0.5f;
	m_pos.x += m_velocity.x;

	m_colRect.pos = m_pos;

	//CheckHitMap();

}

void TransformEnemy::SeekerUpdate(Input&)
{
}

void TransformEnemy::FireBallUpdate(Input&)
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

void TransformEnemy::CheckHitMapX()
{
}

void TransformEnemy::CheckHitMapY()
{
}


//void TransformEnemy::CheckHitMap()
//{
//	Rect2D mapRect; // 当たっているマップを取得する矩形
//	if (m_pMap->IsCollision(m_colRect, mapRect)) // マップと当たっていたら
//	{
//		// 横方向の押し出し判定
//		if (m_velocity.x < 0.0f) // 左に移動している時(左から当たったら)
//		{
//			// 当たったマップの右端X座標 + エネミーの幅の半分の座標に補正
//			m_pos.x = mapRect.GetRight() + kEnemyWidth * 0.5f;
//		}
//		else if(m_velocity.x > 0.0f)// 右に移動している時(左から当たったら)
//		{
//			// 当たったマップの左端X座標 - エネミーの幅の半分の座標に補正
//			m_pos.x = mapRect.GetLeft() - kEnemyWidth * 0.5f;
//		}
//
//		// 縦方向の押し出し判定
//		if (m_velocity.y < 0.0f) // 上に移動している時(下から当たったら)
//		{
//			// 当たったマップの下端Y座標 + エネミーの高さの半分の座標に補正
//			m_pos.y = mapRect.GetBottom() + kEnemyHeight * 0.5f;
//		}
//		else if (m_velocity.y > 0.0f) // 下に移動している時(上から当たったら)
//		{
//			// 当たったマップの上端Y座標 - エネミーの高さの半分の座標に補正
//			m_pos.y = mapRect.GetTop() - kEnemyHeight * 0.5f;
//			m_isGround = true;
//		}
//	}
//}
