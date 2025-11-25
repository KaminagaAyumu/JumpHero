#pragma once

class Actor;
class Map;
class Input;
class Camera;
class Chest;

class ChestManager
{
public:
	ChestManager(Camera* camera);

	void Update(Input& input);

	void Draw();

	// マップのデータを受け取って宝箱を生成
	void SpawnChest(Map* map);

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<Actor*>& actors);

	size_t GetChestNum();

private:
	
	Camera* m_pCamera; // 宝箱で使うカメラを参照
	std::list<std::shared_ptr<Chest>> m_chests;

};

