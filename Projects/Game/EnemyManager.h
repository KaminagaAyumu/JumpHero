#pragma once

class Camera;
class GameManager;
class Map;
class Actor;
class Player;
class EnemyBase;
class Input;

/// <summary>
/// 敵管理用クラス
/// 敵の生成、更新を行う
/// </summary>
class EnemyManager
{
public:
	EnemyManager(Camera* camera,Player* player, GameManager* gameManager);

	void Update(Input&);

	void Draw();

	// 座標のデータを受け取って敵を生成
	void SpawnEnemy(const Position2& pos, Map* map);

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<Actor*>& actors);

	size_t GetEnemyNum();

private:

	Camera* m_pCamera; // 敵が使うカメラを参照
	Player* m_pPlayer; // 敵が使うプレイヤーを参照
	GameManager* m_pGameManager; // イベント通知をするクラスを参照
	std::list<std::shared_ptr<EnemyBase>> m_enemies;
};



