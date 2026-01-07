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
	void InitMap(Map* map);
	void Update(Input&) override;
	void Draw() override;

	/// <summary>
	/// ミスしているかどうかを返す
	/// </summary>
	/// <returns>true: ミス状態 false: ミスしていない</returns>
	bool IsMiss() const;

	/// <summary>
	/// 地面についているかどうかを返す
	/// </summary>
	/// <returns>true: 地面についている false: 地面についていない</returns>
	bool IsGround() const { return m_isGround; }

	/// <summary>
	/// プレイヤーの登場が終わったかどうかを判定
	/// </summary>
	/// <returns>true : 終わった false : 終わっていない</returns>
	bool IsEntryEnd()const;

	/// <summary>
	/// ミスした瞬間の処理
	/// </summary>
	void MissStart();

	/// <summary>
	/// プレイヤーをパワーアップする
	/// </summary>
	/// <returns>true : パワーアップできる false : パワーアップできない</returns>
	bool PowerUp();

	/// <summary>
	/// パワーアップ状態かどうか
	/// </summary>
	/// <returns>true : パワーアップ状態 false : パワーアップしていない</returns>
	bool IsPowerUp();

	/// <summary>
	/// 攻撃可能状態になる
	/// </summary>
	void AttackableStart();

	void DebugClear(const Position2& pos);

	/// <summary>
	/// プレイヤーが動くかどうかのフラグを変える
	/// </summary>
	void FreezeChange();

	/// <summary>
	/// バリアを設定する
	/// </summary>
	/// <param name="barrier">バリアの情報</param>
	void SetBarrier(const ActivePosition2& barrier);

private:
	// マップとの当たり判定を行う際の構造体
	struct ContactFrags
	{
		bool isHitLeft = false;
		bool isHitRight = false;
		bool isHitCeil = false;
		bool isHitGround = false;
	};

	Vector2 m_direction; // 進む方向
	Vector2 m_velocity; // 進む速度
	Position2 m_entryEndPos;
	ActivePosition2 m_barrierPos; // バリア

	int m_graphHandle; // プレイヤーのグラフィックハンドル

	int m_frameCount; // フレームカウンタ
	int m_jumpCount; // ジャンプカウンタ(プレイヤーの強化状態で使用)
	int m_attackCount; // 攻撃カウンタ(プレイヤーの強化状態で使用)
	int m_level; // プレイヤーの強化状態
	float m_prevPosY; // 前フレームのY座標
	bool m_isGround; // 地面についているか
	bool m_isHover; // ジャンプ中にボタンを押したかどうか
	bool m_isMiss; // ミスになったかどうか
	bool m_isLevelDown; // レベルダウンしたかどうか
	bool m_isJumpStart; // ジャンプ開始したかどうか
	bool m_isFreeze; // 止まっているかどうか
	bool m_isAttackable; // 攻撃可能かどうか

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

	/// <summary>
	/// プレイヤーの位置更新とマップとの当たり判定を行う
	/// </summary>
	void MoveOperation(Input& input);

	/// <summary>
	/// マップの番号から宝箱に当たっているかどうかを判定
	/// </summary>
	void CheckHitToChest(Input& input);

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

	/// <summary>
	/// プレイヤーが宝箱の上にいるかどうかを判定する
	/// </summary>
	/// <param name="chestPos">プレイヤーが乗っている宝箱の座標</param>
	/// <returns>true : 宝箱の上にいる false : 宝箱の上にいない</returns>
	bool IsOnChestTop(Position2Int& chestPos);

	/// <summary>
	/// 宝箱を横から開けられるかどうかを判定する
	/// </summary>
	/// <returns>true : 横から開けられる false : 横から開けられない</returns>
	bool IsOpenChestX()const;

};

