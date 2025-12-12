#pragma once
#include "Actor.h"

class Map;

/// <summary>
/// プレイヤーのアクションでアイテムを生成する宝箱クラス
/// </summary>
class Chest : public Actor
{
public:
	Chest();
	Chest(int x, int y, Map* map, int handle, bool isHidden);
	virtual ~Chest();
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	void SetIsOpen(bool isOpen) { m_isOpen = isOpen; }
	bool IsOpen()const { return m_isOpen; }
	bool IsHidden()const { return m_state == ChestState::Hidden; }

	/// <summary>
	/// 隠し宝箱が出現するときの処理
	/// </summary>
	void AppearChest();

	/// <summary>
	/// 宝箱が開いたときの処理 マップのデータを変える用
	/// </summary>
	void OpenChest();

private:
	enum class ChestState
	{
		Hidden, // 隠し宝箱
		Normal, // 通常の宝箱
		Opened // 開いた宝箱
	};

	int m_graphHandle; // 画像ハンドル
	bool m_isOpen; // 開いているかどうか

	ChestState m_state; // 宝箱の状態

	Map* m_pMap; // マップのデータを変更できるようにポインタを取得
	Position2Int m_chipPos; // マップチップ位置 

};

