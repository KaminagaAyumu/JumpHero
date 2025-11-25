#pragma once
#include "Actor.h"

class Input;

class ItemBase : public Actor
{
public:
	ItemBase();
	ItemBase(float x, float y);
	virtual ~ItemBase();

	virtual void Init()override;
	virtual void Update(Input&)override;
	virtual void Draw()override;

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	void SetIsExist(bool isExist) { m_isExist = isExist; }
	/// <summary>
	/// アイテムの存在フラグ
	/// </summary>
	/// <returns>true : 存在する false : 存在しない</returns>
	bool IsExist() const { return m_isExist; }

private:

	enum class ItemType
	{
		Balloon,
		UpgradeMedal,
		LifeUp,
		Coin
	};

	bool m_isExist;

};

