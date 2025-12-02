#pragma once
#include "ItemBase.h"


class Coin : public ItemBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="scoreFunc">スコア加算用の関数ポインタ</param>
	Coin(const Position2& pos, std::function<void(int)> scoreFunc);
	virtual ~Coin() = default;

	virtual void Init()override;
	virtual void Update(Input&)override;
	virtual void Draw()override;


private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	// スコア加算用関数ポインタ
	std::function<void(int)> m_scoreFunc;

};

