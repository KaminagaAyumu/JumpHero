#pragma once
#include <list>
#include <functional>

class Actor;
class ItemBase;
class Input;
class Camera;
class GameManager;

/// <summary>
/// アイテム管理用クラス
/// アイテムの生成、更新
/// </summary>
class ItemManager
{
public:
	ItemManager(Camera* camera, GameManager* gameManager);
	/// <summary>
	/// シーン上のアイテムの更新処理を行う関数
	/// </summary>
	void Update(Input& input);

	/// <summary>
	/// シーン上のアイテムの描画処理を行う関数
	/// </summary>
	void Draw();

	/// <summary>
	/// 指定した座標にアイテムの追加を行う
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="scoreFunc">スコア関係の関数</param>
	void SpawnItem(int x, int y);

	/// <summary>
	/// ゲーム内オブジェクトにアイテムを追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<Actor*>& actors);

	size_t GetItemNum();

private:
	Camera* m_pCamera; // アイテムで使うカメラを取得
	GameManager* m_pGameManager; // アイテムで使うゲームマネージャーを取得
	std::list<std::shared_ptr<ItemBase>> m_pItems; // シーン上のアイテム管理用コンテナ


};

