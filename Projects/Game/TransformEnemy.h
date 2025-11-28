#pragma once
#include "EnemyBase.h"

class Player;
class Map;

/// <summary>
/// 変身する敵クラス
/// </summary>
class TransformEnemy : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="map">マップのポインタ</param>
	TransformEnemy(Player* player, Map* map);
	
	// デストラクタ(今は特に使わないのでdefault)
	virtual ~TransformEnemy() = default;
	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;

	


private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	// 更新処理用関数群
	using UpdateFunc_t = void(TransformEnemy::*)(); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ
	void NormalUpdate();
	void SeekerUpdate();
	void FireBallUpdate();

	// 描画処理用関数群
	using DrawFunc_t = UpdateFunc_t; // 関数の形式自体は更新処理と同じなので別名とする
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	void NormalDraw();
	void SeekerDraw();
	void FireBallDraw();

	virtual void CheckHitMap() override;

	/// <summary>
	/// 敵の状態を表す列挙型
	/// </summary>
	enum class EnemyForm
	{
		Normal, // 原作のミイラ
		PlayerSeeker, // 原作の鳥
		FireBall, // 原作の火の玉
		Skull, // 原作のどくろ
	};

	// 敵の状態管理用
	EnemyForm m_form;

	// 時間カウンタ
	int m_frameCount;

	// 敵が折り返した回数をカウントする
	int m_turnCount;

};

