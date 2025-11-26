#pragma once


// プロトタイプ宣言
class Actor;
class Player;
class Camera;
class ChestManager;
class ItemManager;
class Input;
class Map;


/// <summary>
/// ゲーム内の状態を管理するクラス
/// オブジェクト管理クラスの実体もここで持つ
/// </summary>
class GameManager
{
public:
	GameManager(Player* player, Camera* camera, Map* map, std::vector<Actor*>& actors);
	virtual ~GameManager();

	void Init();
	void Update(Input& input);
	void Draw() const;

	void PushActors(std::vector<Actor*>& actors);

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコアの値</param>
	void AddScore(int score);

	bool IsClear()const { return m_isClear; }

	/// <summary>
	/// 宝箱を開けるという通知をChestManagerに送る関数
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void ChestOpen(int x, int y);

	/// <summary>
	/// ゲーム内に存在するオブジェクトの数を取得
	/// </summary>
	/// <returns>ゲーム内のオブジェクトの数</returns>
	const size_t& GetActorNum()const;

private:

	int m_score; // 表示用のスコア
	int m_currentScore; // ゲーム内のスコア
	int m_life; // 残機数
	int m_medalNum; // 強化メダルの所持数
	int m_balloonNum; // 風船をどれだけ取ったか
	bool m_isClear; // クリア状態かどうか

	// 各オブジェクト管理クラスの参照用
	Player* m_pPlayer;
	std::unique_ptr<ChestManager> m_pChestManager;
	std::unique_ptr<ItemManager> m_pItemManager;

};

