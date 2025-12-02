#pragma once
#include "EnemyBase.h"

class Player;
class Map;
class Input;

/// <summary>
/// 変身する敵クラス
/// </summary>
class TransformEnemy : public EnemyBase
{
public:

	/// <summary>
	/// 敵の状態を表す列挙型
	/// </summary>
	enum class EnemyForm
	{
		Normal, // 原作のミイラ
		PlayerSeeker, // 原作の鳥
		FireBall, // 原作の火の玉
		Skull, // 原作のどくろ
		Item, // アイテム化状態
	};


	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">敵の初期位置</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="map">マップのポインタ</param>
	/// <param name="changeForm">変わる姿</param>
	TransformEnemy(const Position2& pos,Player* player, Map* map, EnemyForm changeForm);
	
	// デストラクタ(今は特に使わないのでdefault)
	virtual ~TransformEnemy() = default;
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	// 更新処理用関数群
	using UpdateFunc_t = void(TransformEnemy::*)(Input&); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ
	void AppearUpdate(Input&); // 出現の際の更新処理
	void NormalUpdate(Input&); // デフォルト状態の更新処理
	void TransformUpdate(Input&); // 変身中の更新処理
	void SeekerUpdate(Input&); // プレイヤーを追い続ける更新処理
	void FireBallUpdate(Input&); // プレイヤーの縦座標を追い続ける更新処理
	void SkullUpdate(Input&); // プレイヤーの横座標を追い続ける更新処理
	void ItemUpdate(Input&); // 敵がアイテム化中の更新処理

	// 描画処理用関数群
	using DrawFunc_t = void(TransformEnemy::*)(); // 関数の形式自体は更新処理と同じなので別名とする
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	void AppearDraw();
	void NormalDraw(); // デフォルト状態の描画処理
	void TransformDraw(); // 変身中の描画処理
	void SeekerDraw(); // プレイヤーを追い続ける時の描画処理
	void FireBallDraw(); // プレイヤーの縦座標を追い続ける時の描画処理
	void SkullDraw(); // プレイヤーの横座標を追い続けるときの描画処理
	void ItemDraw(); // 敵がアイテム化中の描画処理

	virtual void CheckHitMapX() override;

	virtual void CheckHitMapY() override;

	bool IsCanCollision()const;

	// 敵の状態管理用
	EnemyForm m_currentForm; // 現在の敵の姿
	EnemyForm m_nextForm; // 変わる姿

	// 時間カウンタ(状態が変わるごとにリセットする)
	int m_frameCount;

	// 敵が折り返した回数をカウントする
	int m_turnCount;

	// 床にいるかどうか
	bool m_isGround;

	// 移動の力
	Vector2 m_velocity;
	
};

