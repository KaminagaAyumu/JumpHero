#pragma once
#include "Actor.h"

class Input;
class Map;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Actor
{
public:
	// コンストラクタ
	Player();

	// デストラクタ
	virtual ~Player();

	void Init() override;
	void Update(Input&) override;
	void Draw() override;

	void IsCollision(const Types::CollisionInfo& info) override;

	
	// 更新処理用関数群
	void JumpUpdate(Input&); // ジャンプ状態の時
	void GroundUpdate(Input&); // 床についている時
	void MissUpdate(Input&); // ミス処理の時
	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(Player::*)(Input&);
	UpdateFunc_t m_update;

	// 描画処理用関数軍
	void JumpDraw(); // ジャンプ状態の時
	void GroundDraw(); // 床についている時
	void MissDraw(); // ミス処理の時
	// 描画処理用関数ポインタの型定義
	using DrawFunc_t = void(Player::*)();
	DrawFunc_t m_draw;

	/// <summary>
	/// ジャンプ開始時の処理
	/// </summary>
	void JumpStart();

	/// <summary>
	/// ミスした瞬間の処理
	/// </summary>
	void MissStart();

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
	/// マップとの当たり判定を取るためにBgクラスを受け取る
	/// </summary>
	/// <param name="bg">Bgクラスのスマートポインタ</param>
	/// <note>仮作成中(もう少しいいやり方があるかも)</note>
	void GetMap(Map* map) { m_pMap = map; }

private:
	Vector2 m_direction; // 進む方向
	Vector2 m_velocity; // 進む速度

	int m_graphHandle; // プレイヤーのグラフィックハンドル

	int m_jumpCount; // ジャンプ中の時間
	int m_missCount; // ミスしたときの時間カウンタ
	bool m_isGround; // 地面についているか
	bool m_isHover; // ジャンプ中にボタンを押したかどうか
	bool m_isMiss; // ミスになったかどうか
	bool m_isOffsetX; // マップとの左右補正を行うかどうか(消去する可能性あり)
	float m_currentFloorY; // 現在接地している床の座標

	// マップの生ポインタ
	Map* m_pMap;


#ifdef _DEBUG
	float m_maxPosY; // プレイヤーの最高点を記録
	Position2 m_lastMaxPos; // 前回の最高点
#endif

};

