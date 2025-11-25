#pragma once
#include "Actor.h"

// プロトタイプ宣言
class Player;
class Map;

class EnemyBase : public Actor
{
public:
	EnemyBase(Player*);
	virtual ~EnemyBase();
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	/// <summary>
	/// マップとの当たり判定を取るためにMapクラスを受け取る
	/// </summary>
	/// <param name="map">Mapクラスのスマートポインタ</param>
	/// <note>仮作成中(もう少しいいやり方があるかも)</note>
	void SetBg(Map* map) { m_pMap = map; }

private:
	enum class EnemyType
	{
		Normal,
		PlayerSeeker,
	};
	EnemyType m_type;

	// プレイヤーの生ポインタ
	// 座標を調べるためにしか使わないが、
	// 更新処理で受け取るのが面倒なためコンストラクタで受け取る
	Player* m_pPlayer;

	// マップと当たり判定を取る用(取得していいのかわからないがとりあえずこれでやってみる)
	Map* m_pMap;

	Vector2 m_direction;

	bool m_isHitPlayer;
	bool m_isSeeking;

	int m_directionChangeCount;

	/// <summary>
	/// マップとの当たり判定を取り、座標補正をする
	/// </summary>
	void CheckHitMap();

};

