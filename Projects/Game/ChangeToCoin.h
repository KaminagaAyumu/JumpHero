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
	/// <param name="isPopChest">宝箱から生成かどうか</param>
	ChangeToCoin(const Position2& pos, int handle, bool isPopChest);
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

	float m_startY; // 初期Y座標

	/// <summary>
	/// 出現中かどうか
	/// </summary>
	/// <returns>true : 出現中 false : 出現が終わっている</returns>
	bool IsAppear();

	void DropUpdate(); // 宝箱から出現したときの更新処理
	void FloatingUpdate(); // 浮遊状態の更新処理

	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(ChangeToCoin::*)();
	UpdateFunc_t m_updateFunc;

	virtual void IsCollision(const Types::CollisionInfo& info) override;
};

