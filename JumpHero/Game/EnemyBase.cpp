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

EnemyBase::EnemyBase(Player* player) :
	Actor(Types::ActorType::Enemy),
	m_type(EnemyType::Normal),
	m_pPlayer(player),
	m_pMap(nullptr),
	m_direction{},
	m_isHitPlayer(false),
	m_isSeeking(false),
	m_isDead(false),
	m_directionChangeCount(0)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Init()
{
	m_pos = { 300.0f + GetRand(200) ,300.0f };
	m_colCircle = { m_pos,20.0f };
	m_colRect = { m_pos, kGraphWidth, kGraphHeight };
	if (GetRand(1) == 0)
	{
		m_type = EnemyType::Normal;
	}
	else
	{
		m_type = EnemyType::PlayerSeeker;
	}
}

void EnemyBase::Update(Input&)
{
	m_directionChangeCount++;
	if (m_type == EnemyType::PlayerSeeker)
	{
		Vector2 toPlayer = m_pPlayer->GetPos() - m_pos;
		// プレイヤーからのベクトルでどちらの座標軸のほうが遠いかを計測
		if (fabsf(toPlayer.x) > fabsf(toPlayer.y))
		{
			// x軸のほうが遠い
			toPlayer.y = 0.0f;
		}
		else
		{
			// y軸のほうが遠い
			toPlayer.x = 0.0f;
		}
		toPlayer.Normalize();
		m_pos += toPlayer * 0.5f;

	}
	else
	{
		if (m_pos.y < kSeekPosY)
		{
			if (m_directionChangeCount >= kMaxDirectionChangeCount)
			{
				int dir = GetRand(5) % 5;
				switch (dir)
				{
				case 0:
					m_direction = { 0.0f,0.0f };
					break;
				case 1:
					m_direction = { 1.0f,0.0f };
					break;
				case 2:
					m_direction = { -1.0f,0.0f };
					break;
				case 3:
					m_direction = { 0.0f,1.0f };
					break;
				case 4:
					m_direction = { 0.0f,-1.0f };
					break;
				}
				m_directionChangeCount = 0;
			}
			m_direction.Normalize();
			m_pos += m_direction * 0.5f;
		}
		else
		{
			m_isSeeking = true;
		}
		if (m_isSeeking)
		{
			Vector2 toPlayer = m_pPlayer->GetPos() - m_pos;
			toPlayer.Normalize();
			m_direction = toPlayer;
			m_pos += m_direction * 3.0f;
		}

	}

	m_colCircle.pos = m_pos;
	m_colRect.pos = m_pos;

	// プレイヤーと当たっているかどうか
	// この処理はIsCollisionに移動しました
	//m_isHitPlayer = IsCollisionCircleToCircle(m_pPlayer->GetColCircle());
	//m_isHitPlayer = IsCollisionCircleToRect(m_targetPlayer->GetColRect());

}

void EnemyBase::Draw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
	if (m_type == EnemyType::Normal)
	{
		if (m_isHitPlayer)
		{
			DrawBox(static_cast<int>(drawX - 16), static_cast<int>(drawY - 16),
			static_cast<int>(drawX + 16), static_cast<int>(drawY + 16),
			0xff0000, true);
		}
		else
		{
			DrawBox(static_cast<int>(drawX - 16), static_cast<int>(drawY - 16),
			static_cast<int>(drawX + 16), static_cast<int>(drawY + 16),
			0x00ff00, true);
		}

	}
	else
	{
		if (m_isHitPlayer)
		{
			DrawBox(static_cast<int>(drawX - 16), static_cast<int>(drawY - 16),
			static_cast<int>(drawX + 16), static_cast<int>(drawY + 16),
			0x0000ff, true);
		}
		else
		{
			DrawBox(static_cast<int>(drawX - 16), static_cast<int>(drawY - 16),
			static_cast<int>(drawX + 16), static_cast<int>(drawY + 16),
			0xffff00, true);
		}
	}
}

void EnemyBase::IsCollision(const Types::CollisionInfo& info)
{
}

void EnemyBase::CheckHitMap()
{

}