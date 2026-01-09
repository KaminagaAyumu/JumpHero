#pragma once
#include "SceneBase.h"

class TextManager;
class TutorialManager;
class Map;
class Bg;
class GameManager;
class CollisionManager;
class Actor;
class SoundManager;

/// <summary>
/// ゲームの基本操作を教える用のシーン
/// </summary>
class TutorialScene : public SceneBase
{
public:
	TutorialScene(SceneController& controller);
	virtual ~TutorialScene();

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

	unsigned int m_fadeColor; // フェードの色

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void MissUpdate(Input&); // プレイヤーがミスになった時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (TutorialScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (TutorialScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	std::unique_ptr<TextManager> m_pTextManager;			// テキスト管理クラス
	std::unique_ptr<TutorialManager> m_pTutorialManager;	// チュートリアルイベント管理クラス
	std::unique_ptr<Map> m_pMap;							// マップクラス
	std::unique_ptr<Bg> m_pBg;								// 背景クラス
	std::shared_ptr<GameManager> m_pGameManager;			// ゲーム管理クラス
	std::unique_ptr<CollisionManager> m_pCollisionManager;	// 衝突管理クラス
	std::vector<std::weak_ptr<Actor>> m_pActors;							// アクターの配列
	std::shared_ptr<SoundManager> m_pSoundManager;			// サウンド管理クラス

};

