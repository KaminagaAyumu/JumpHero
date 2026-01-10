#pragma once
#include <string>
#include <unordered_map>

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
	EnemyManager(Camera* camera,Player* player, GameManager* gameManager, Map* map);
	virtual ~EnemyManager();

	void Init(Map* map);

	void Update(Input&);

	void Draw();

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<std::weak_ptr<Actor>>& actors);

	size_t GetEnemyNum();

	/// <summary>
	/// 敵をアイテムに変える処理
	/// </summary>
	void ChangeToItemAll();

	/// <summary>
	/// 敵を指定座標にスポーンさせる
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="spawnNum">スポーンする敵の番号</param>
	void SpawnEnemy(const Position2& pos, int spawnNum);

private:

	/// <summary>
	/// 敵の生成位置をマップから読み込む
	/// </summary>
	/// <param name="map">読み込むマップ</param>
	void LoadSpawnPositions(Map* map);

	// 座標のデータを受け取って敵を生成
	void SpawnEnemy(const Position2& pos);

	/// <summary>
	/// プレイヤーに最も近いスポーン位置を探す
	/// </summary>
	/// <param name="playerPos">プレイヤーの位置</param>
	/// <returns>敵スポーン位置</returns>
	const Position2& SearchNearestSpawnPosition(const Position2& playerPos);

	Camera* m_pCamera; // 敵が使うカメラを参照
	Player* m_pPlayer; // 敵が使うプレイヤーを参照
	GameManager* m_pGameManager; // イベント通知をするクラスを参照
	Map* m_pMap; // 敵生成用にマップを参照
	std::list<std::shared_ptr<EnemyBase>> m_enemies;

	std::vector<Position2> m_spawnPositions; // 敵の生成位置リスト

	std::vector<int> m_graphHandles; // 敵の画像ハンドルリスト

	std::unordered_map<std::string, Animation> m_transformAnimations; // 変身する敵のアニメーションリスト

	int m_frameCount; // フレーム数をカウント
	int m_itemTime; // アイテム状態の時間

	/// <summary>
	/// 敵がアイテム化状態になっているか
	/// </summary>
	/// <returns>true : アイテム状態 false : アイテム状態以外</returns>
	bool IsChangeToItem();
};



