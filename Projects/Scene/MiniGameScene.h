#pragma once
#include "SceneBase.h"

class GameManager;

/// <summary>
/// ミニゲーム用シーン
/// ゲームシーンから遷移
/// </summary>
class MiniGameScene : public SceneBase
{
public:
	MiniGameScene(SceneController& controller, std::shared_ptr<GameManager> gameManager);
	virtual ~MiniGameScene();

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
	unsigned int m_fadeColor; // 単色フェード用の色

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void DescriptionUpdate(Input& input); // 説明中の更新処理
	void DescriptionEndUpdate(Input& input); // 説明終了時の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (MiniGameScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void DescriptionDraw(); // 説明中の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (MiniGameScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	bool IsEndDescription() const; // 説明が終了したかどうかを取得する関数

	std::shared_ptr<GameManager> m_pGameManager; // ゲーム管理クラスへのポインタ
};

