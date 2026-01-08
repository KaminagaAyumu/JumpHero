#include "StayEnemy.h"

namespace
{
	// 描画関連
	constexpr float kWidth = 40.0f;	// 敵の実際の幅
	constexpr float kHeight = 40.0f;	// 敵の実際の高さ

	constexpr float kCoinWidth = 30.0f;	// 敵がコイン化した際の幅
	constexpr float kCoinHeight = 30.0f;	// 敵がコイン化した際の高さ

	// 更新処理関連
	constexpr int	kDeadTime = 300;		// 敵がやられて消えるの時間

	// 演出関連
	constexpr float	kItemWarningRate = 0.20f;	// アイテム化が終わりそうなことを示す時間の割合
	constexpr float kWarningFrashCycle = 0.30f;	// 点滅の周期
	constexpr int	kMaxFadeRate = 255;		// フェード率の最大値
}

StayEnemy::StayEnemy(const Position2& pos, Player* player, Map* map) :
	EnemyBase(player, map)
{
	m_updateFunc = &StayEnemy::NormalUpdate;
	m_pos = pos;
}

void StayEnemy::Init()
{
	m_colRect = { m_pos,kWidth,kHeight };
	m_colCircle = { m_pos,kWidth / 2 };
}

void StayEnemy::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void StayEnemy::Draw()
{
	(this->*m_drawFunc)();
}

void StayEnemy::ChangeToItem(int time)
{
}

void StayEnemy::IsCollision(const Types::CollisionInfo& info)
{
}

void StayEnemy::NormalUpdate(Input&)
{
}

void StayEnemy::ItemUpdate(Input&)
{
}

void StayEnemy::DeadUpdate(Input&)
{
}

void StayEnemy::NormalDraw()
{

}

void StayEnemy::ItemDraw()
{

}