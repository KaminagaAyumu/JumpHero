#pragma once
#include "Actor.h"

class Map;

class Chest : public Actor
{
public:
	Chest();
	Chest(int x, int y, Map* map);
	virtual ~Chest();
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	void SetIsOpen(bool isOpen) { m_isOpen = isOpen; }
	bool IsOpen()const { return m_isOpen; }

	/// <summary>
	/// 宝箱が開いたときの処理
	/// </summary>
	void OpenChest();

private:

	bool m_isOpen;

	Map* m_pMap; // マップのデータを変更できるようにポインタを取得
	Position2Int m_chipPos; // マップチップ位置 

};

