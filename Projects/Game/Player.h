#pragma once
#include "Actor.h"

class Input;
class Map;
class GameManager;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Actor
{
public:
	// コンストラクタ
	Player(Map* map, GameManager* gameManager);

	// デストラクタ
	virtual ~Player();

	void Init() override;
	void Update(Input&) override;
	void Draw() override;

	/// <summary>
	/// ミスしているかどうかを返す
	/// </summary>
	/// <returns>true: ミス状態 false: ミスしていない</returns>
	bool IsMiss() const { return m_isMiss; }

	/// <summary>
	/// 地面についているかどうかを返す
	/// </summary>
	/// <returns>true: 地面についている false: 地面についていない</returns>
	bool IsGround() const { return m_isGround; }

	/// <summary>
	/// ミスした瞬間の処理
	/// </summary>
	void MissStart();

	/// <summary>
	/// プレイヤーをパワーアップする
	/// </summary>
	/// <returns>true : パワーアップできる false : パワーアップできない</returns>
	bool PowerUp();

private:
	Vector2 m_direction; // 進む方向
	Vector2 m_velocity; // 進む速度

	int m_graphHandle; // プレイヤーのグラフィックハンドル

	int m_frameCount; // フレームカウンタ
	int m_jumpCount; // ジャンプカウンタ(プレイヤーの強化状態で使用)
	int m_level; // プレイヤーの強化状態
	bool m_isGround; // 地面についているか
	bool m_isHover; // ジャンプ中にボタンを押したかどうか
	bool m_isMiss; // ミスになったかどうか
	bool m_isOffsetX; // マップとの左右補正を行うかどうか(消去する可能性あり)
	bool m_isOpenChest; // 宝箱を開けられるかどうか(消去する可能性あり)
	bool m_isLevelDown; // レベルダウンしたかどうか
	float m_currentFloorY; // 現在接地している床の座標

	// マップの生ポインタ
	Map* m_pMap;

	// ゲームマネージャーの生ポインタ
	GameManager* m_pGameManager;

	/// <summary>
	/// 当たっているかどうか
	/// </summary>
	/// <param name="info"></param>
	void IsCollision(const Types::CollisionInfo& info) override;

	/// <summary>
	/// ジャンプ開始時の処理
	/// </summary>
	void JumpStart();

	/// <summary>
	/// プレイヤーのパワーダウンを判定する
	/// </summary>
	void CheckPowerDown();

	// 更新処理用関数群
	void EntryUpdate(Input&); // 登場状態の時
	void JumpUpdate(Input&); // ジャンプ状態の時
	void GroundUpdate(Input&); // 床についている時
	void MissUpdate(Input&); // ミス処理の時
	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(Player::*)(Input&);
	UpdateFunc_t m_update;

	// 描画処理用関数群
	void EntryDraw(); // 登場状態の時
	void JumpDraw(); // ジャンプ状態の時
	void GroundDraw(); // 床についている時
	void MissDraw(); // ミス処理の時
	// 描画処理用関数ポインタの型定義
	using DrawFunc_t = void(Player::*)();
	DrawFunc_t m_draw;

	bool IsOpenChestX()const;
};

