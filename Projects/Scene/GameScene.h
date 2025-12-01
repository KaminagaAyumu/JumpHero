#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

// プロトタイプ宣言
class Actor;
class Player;
class EnemyBase;
class Chest;
class Bg;
class Map;
class Camera;
class ItemBase;
class GameManager;
class CollisionManager;

/// <summary>
/// ゲームのメインとなるシーン
/// </summary>
class GameScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	GameScene(SceneController& controller);
	virtual ~GameScene();
	/// <summary>
	/// シーンの更新処理
	/// </summary>
	/// <param name="input">入力情報を取得するクラス</param>
	void Update(Input& input) override;
	/// <summary>
	/// シーンの描画処理
	/// </summary>
	void Draw() override;

private:
	uint32_t m_fadeColor; // 単色フェード用の色
	
	int m_chestOpenNum; // 宝箱を開ける用

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void MissUpdate(Input&); // プレイヤーがミスになった時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (GameScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (GameScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	
	// 背景画像を管理するクラスのポインタ
	std::shared_ptr<Bg> m_bg;

	// ゲームシーン内の状態管理用ポインタ
	std::shared_ptr<GameManager> m_pGameManager;

	// マップのデータを取得するポインタ
	std::shared_ptr<Map> m_pMap;

	std::vector<Actor*> m_pActors; // ゲームシーン内に存在するオブジェクトを管理するコンテナ
	
	std::unique_ptr<CollisionManager> m_pCollisionManager; // 当たり判定管理クラス
};

