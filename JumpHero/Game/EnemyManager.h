#pragma once

class Camera;
class GameManager;
class Input;
class Map;
class Actor;
class EnemyBase;

/// <summary>
/// 敵管理用クラス
/// 敵の生成、更新を行う
/// </summary>
class EnemyManager
{
public:
	EnemyManager(Camera* camera, GameManager* gameManager);

	void Update(Input& input);

	void Draw();

	// マップのデータを受け取って宝箱を生成
	void SpawnEnemy(Map* map);

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<Actor*>& actors);

	size_t GetChestNum();

private:

	Camera* m_pCamera; // 宝箱で使うカメラを参照
	GameManager* m_pGameManager; // イベント通知をするクラスを参照
	std::list<std::shared_ptr<EnemyBase>> m_enemies;
};



