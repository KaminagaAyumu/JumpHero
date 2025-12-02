#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include "../Utility/GameType.h"

// プロトタイプ宣言
class Actor;
class Player;
class Camera;
class ChestManager;
class ItemManager;
class ItemBase;
class EnemyManager;
class Input;
class Map;


/// <summary>
/// ゲーム内の状態を管理するクラス
/// オブジェクト管理クラスの実体もここで持つ
/// </summary>
class GameManager
{
public:
	GameManager(Map* map, std::vector<Actor*>& actors);
	virtual ~GameManager();

	void Init();
	void Update(Input& input);
	void Draw() const;

	/// <summary>
	/// ゲームシーンが持っているオブジェクトコンテナに派生クラスを入れる
	/// </summary>
	/// <param name="actors">ゲームシーン内オブジェクトコンテナ</param>
	void PushActors(std::vector<Actor*>& actors);

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコアの値</param>
	void AddScore(int score);

	/// <summary>
	/// 敵をアイテムに変える
	/// </summary>
	void ChangeEnemyToCoin();

	/// <summary>
	/// 当たり判定を行わないかどうか
	/// </summary>
	/// <returns>true : 行わない false : 行う</returns>
	bool IsSkipCollision()const;

	/// <summary>
	/// クリア状態になっているか
	/// </summary>
	/// <returns>true : クリア状態 false : クリア状態ではない</returns>
	bool IsClear()const;

	/// <summary>
	/// アイテムを取得した際、アイテムに対応したラムダ式を行う処理
	/// </summary>
	/// <param name="type">アイテムの種類</param>
	void OnItemCollected(const Types::ItemType& type);

	/// <summary>
	/// アイテムを生成するという通知をItemManagerに送る関数
	/// </summary>
	/// <param name="x">生成X座標</param>
	/// <param name="y">生成Y座標</param>
	void DropItem(int x, int y);

	/// <summary>
	/// ゲーム内に存在するオブジェクトの数を取得
	/// </summary>
	/// <returns>ゲーム内のオブジェクトの数</returns>
	const size_t GetActorNum()const;

	/// <summary>
	/// カメラを取得させる
	/// </summary>
	/// <returns>カメラポインタ</returns>
	/// <note>背景とマップはゲームシーンが管理しているので一旦こうしておく</note>
	Camera* GetCamera()const { return m_pCamera.get(); }

	/// <summary>
	/// 現在のスコアを取得する
	/// リザルト用
	/// </summary>
	/// <returns>スコア</returns>
	const int GetScore()const { return m_currentScore; }

private:
	int m_frameCount; // フレームカウンタ

	// ゲーム内データ関連
	int m_score; // 表示用のスコア
	int m_currentScore; // ゲーム内のスコア
	int m_life; // 残機数
	int m_medalNum; // 強化メダルの所持数
	int m_balloonNum; // 風船をどれだけ取ったか

	std::unordered_map<Types::ItemType, std::function<void()>> m_itemCollectFunc;

	// 各オブジェクト管理クラスの参照用
	Map* m_pMap;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<ChestManager> m_pChestManager;
	std::unique_ptr<ItemManager> m_pItemManager;
	std::unique_ptr<EnemyManager> m_pEnemyManager;

};

