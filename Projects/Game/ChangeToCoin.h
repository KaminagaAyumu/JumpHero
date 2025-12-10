#pragma once
#include "ItemBase.h"

/// <summary>
/// アイテム : 敵をコインに変えるアイテムクラス
/// </summary>
class ChangeToCoin : public ItemBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="handle">画像ハンドル</param>
	ChangeToCoin(const Position2& pos, int handle);
	virtual ~ChangeToCoin() = default;

	virtual void Init()override;
	virtual void Update(Input&)override;
	virtual void Draw()override;

	/// <summary>
	/// アイテムが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーの参照</param>
	virtual void OnCollected(GameManager& gameManager)override;

private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;
};

