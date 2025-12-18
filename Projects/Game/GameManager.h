#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <deque>
#include "../Utility/GameType.h"
#include "../Utility/Geometry.h"

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
	void MiniGameInit(Map* map); // 一旦マップを取得するだけで使う
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
	/// ゲームオーバーかどうか
	/// </summary>
	/// <returns>true : ゲームオーバー false : ゲームオーバー状態ではない</returns>
	bool IsGameOver()const;

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
	/// アイテムを指定して生成する通知をItemManagerに送る関数
	/// </summary>
	/// <param name="x">生成X座標</param>
	/// <param name="y">生成Y座標</param>
	/// <param name="type">アイテムの種類</param>
	void DropItem(int x, int y, const Types::ItemType& type);

	/// <summary>
	/// プレイヤーを強化する
	/// </summary>
	void PowerUpPlayer();

	/// <summary>
	/// 座標指定した場所の宝箱を開ける
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	void OpenChestToPosition(int x, int y);

	/// <summary>
	/// カメラのターゲットをセットする
	/// </summary>
	/// <param name="pos">カメラの座標</param>
	void SetCameraTarget(const Position2* pos);

	/// <summary>
	/// カメラの補正が終わったかどうか
	/// </summary>
	/// <returns>true : 終わった false : 終わっていない</returns>
	bool IsCameraLeapEnd();

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
	/// プレイヤーを取得させる
	/// </summary>
	/// <returns>プレイヤーポインタ</returns>
	/// <note>ゲームマネージャー自身が管理しているが、他でも参照したいクラスが存在するため作っている</note>
	Player* GetPlayer()const { return m_pPlayer.get(); }

	/// <summary>
	/// 現在のスコアを取得する
	/// リザルト用
	/// </summary>
	/// <returns>スコア</returns>
	const int GetScore()const { return m_currentScore; }

	/// <summary>
	/// 残機を1減らす(仮)
	/// </summary>
	void LifeDown() { m_life--; }

	/// <summary>
	/// アイテムを取得するイベントを設定
	/// </summary>
	/// <param name="type">どのアイテムか</param>
	void NotifyItemEvent(Types::ItemType type);

	/// <summary>
	/// 指定されたアイテムが取得されたかどうか
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	bool IsItemPicked(Types::ItemType itemType);

private:
	int m_frameCount; // フレームカウンタ

	// ゲーム内データ関連
	int m_score; // 表示用のスコア
	int m_currentScore; // ゲーム内のスコア
	int m_life; // 残機数
	int m_medalNum; // 強化メダルの所持数
	int m_balloonNum; // 風船をどれだけ取ったか
	int m_balloonCounter; // 風船の数のカウンタ
	int m_totalBalloonNum; // そのステージに存在する風船の総数
	bool m_isMiniGame; // ミニゲーム中かどうか

	std::unordered_map<Types::ItemType, std::function<void()>> m_itemCollectFunc;

	/// <summary>
	/// アイテムが取得される時のイベント
	/// </summary>
	struct ItemPickUpEvent
	{
		Types::ItemType type; // どのアイテムか
		int frameCount; // どの順序かを確認するためにフレーム数を格納
	};

	std::deque<ItemPickUpEvent> m_itemPickEvent; // アイテムを取得したかどうかのイベント

	// 各オブジェクト管理クラスの参照用
	Map* m_pMap;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<ChestManager> m_pChestManager;
	std::unique_ptr<ItemManager> m_pItemManager;
	std::unique_ptr<EnemyManager> m_pEnemyManager;

	/// <summary>
	/// 敵をコインに変えるアイテムを生成するかどうか
	/// </summary>
	/// <returns>true : 生成する false : 生成しない</returns>
	bool IsDropChangeToCoin();

};

