#pragma once
#include <memory>
#include <list>
#include <vector>

class Actor;
class Map;
class Input;
class Camera;
class Chest;
class GameManager;

/// <summary>
/// ゲーム中の宝箱を管理するクラス
/// 宝箱の生成、表示を行う
/// </summary>
class ChestManager
{
public:
	ChestManager(Camera* camera, GameManager* gameManager);

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
	GameManager* m_pGameManager; // イベント通知をするクラスを参照
	std::list<std::shared_ptr<Chest>> m_chests;

};

