#pragma once
#include "EnemyBase.h"

/// <summary>
/// 動かない敵クラス
/// </summary>
class StayEnemy : public EnemyBase
{
	StayEnemy(const Position2& pos, Player* player, Map* map);

	virtual ~StayEnemy() = default;
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	virtual void ChangeToItem(int time) override;

private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;
};

