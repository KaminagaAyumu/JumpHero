#pragma once
#include "Actor.h"
#include <functional>

class Input;
class GameManager;

/// <summary>
/// アイテムの基底クラス
/// </summary>
class ItemBase : public Actor
{
public:
	ItemBase();
	virtual ~ItemBase();

	virtual void Init()abstract;
	virtual void Update(Input&)abstract;
	virtual void Draw()abstract;

	/// <summary>
	/// アイテムの存在フラグ
	/// </summary>
	/// <returns>true : 存在する false : 存在しない</returns>
	bool IsExist() const { return m_isExist; }

protected:

	// 存在フラグ
	bool m_isExist;

	virtual void IsCollision(const Types::CollisionInfo& info) abstract;

};

