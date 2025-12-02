#include "TransformEnemy.h"
#include "../Utility/Camera.h"
#include "DxLib.h"
#include "../Utility/Map.h"
#include "Player.h"
#include <algorithm>

namespace
{
	// 描画関連
	constexpr float kEnemyWidth				= 45.0f;	// 敵の実際の幅
	constexpr float kEnemyHeight			= 45.0f;	// 敵の実際の高さ

	constexpr float kEnemyCoinWidth			= 30.0f;	// 敵がコイン化した際の幅
	constexpr float kEnemyCoinHeight		= 30.0f;	// 敵がコイン化した際の高さ

	// 更新処理関連
	constexpr float kGravity				= 0.5f;		// 敵にかかる重力
	constexpr float kNormalMoveSpeed		= 0.5f;		// 通常時の左右移動の速さ
	constexpr float kSeekerMoveSpeed		= 0.8f;		// プレイヤーを追い続ける敵の移動の速さ

	constexpr int	kAppearTime				= 60;		// 敵の出現までの時間
	constexpr int	kFormChangeWaitTime		= 180;		// 敵の変身までの時間
	constexpr int	kFormChangeTime			= 30;		// 敵の変身準備までの時間

	constexpr float kMaxDirectionValue		= 1.0f;		// 向きの最大値(大きさ)
	constexpr float kDirectionMagnification = 0.01f;	// 向きの倍率
	
	// 演出関連
	constexpr float	kItemWarningRate		= 0.20f;	// アイテム化が終わりそうなことを示す時間の割合
	constexpr float kWarningFrashCycle		= 0.30f;	// 点滅の周期
	constexpr int	kMaxFadeRate			= 255;		// フェード率の最大値
}

TransformEnemy::TransformEnemy(const Position2& pos, Player* player, Map* map, EnemyForm changeForm) :
	EnemyBase(player, map),
	m_updateFunc(&TransformEnemy::AppearUpdate),
	m_drawFunc(&TransformEnemy::AppearDraw),
	m_currentForm(EnemyForm::Normal),
	m_nextForm(changeForm),
	m_frameCount(0),
	m_itemFormTime(0),
	m_maxItemFormTime(0),
	m_turnCount(0),
	m_isGround(false),
	m_velocity{}
{
	m_direction = {};
	m_pos = pos;
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Init()
{
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Update(Input& input)
{
	if (IsItemMode())
	{
		// フレーム数の更新を行わない
	}
	else
	{
		m_frameCount++; // フレーム数は常に更新し続ける
	}
	(this->*m_updateFunc)(input);
}

void TransformEnemy::Draw()
{
	(this->*m_drawFunc)();
}

void TransformEnemy::ChangeToItem(int time)
{
	m_colRect = { m_pos,kEnemyCoinWidth,kEnemyCoinHeight };
	m_colCircle = { m_pos,kEnemyCoinWidth / 2 };
	m_updateFunc = &TransformEnemy::ItemUpdate;
	m_drawFunc = &TransformEnemy::ItemDraw;
	m_itemFormTime = time;
	m_maxItemFormTime = time;
	return; // 念のためreturn
}

void TransformEnemy::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player) // プレイヤーに当たっている時
	{
		if (IsCanCollision()) // 当たり判定を行えるなら
		{
			// プレイヤーが判定するとしたらダウンキャストするやり方しかわからないため
			// 敵がプレイヤーを持っているので一旦それを使う
			m_pPlayer->MissStart(); // プレイヤーをミスにさせる
		}
		else if (IsItemMode()) // 敵がアイテム化している場合
		{
			m_isDead = true;
		}
	}
}

void TransformEnemy::AppearUpdate(Input&)
{
	// 一定時間たったら出現するようにする
	if (m_frameCount >= kAppearTime)
	{
		// 敵の最初の状態の処理に変更
		m_updateFunc = &TransformEnemy::NormalUpdate;
		m_drawFunc = &TransformEnemy::NormalDraw;
		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}
}

void TransformEnemy::NormalUpdate(Input&)
{
	// 時間経過で敵の姿が変わる(今後別の条件を増やす予定あり)
	if (m_frameCount >= kFormChangeWaitTime)
	{
		// 敵の姿を変える間の処理に変更
		m_updateFunc = &TransformEnemy::TransformUpdate;
		m_drawFunc = &TransformEnemy::TransformDraw;
		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}

	m_velocity.x = kNormalMoveSpeed * m_direction.x;
	m_pos.x += m_velocity.x;
	m_colRect.pos = m_pos;
	CheckHitMapX();

	m_velocity.y += kGravity;
	m_pos.y += m_velocity.y;
	m_colRect.pos = m_pos;
	CheckHitMapY();
	m_colCircle.pos = m_pos;
}

void TransformEnemy::TransformUpdate(Input&)
{
	// 少し待ってから姿が変わるようにしている
	if (m_frameCount >= kFormChangeTime)
	{
		// 最初に受け取った変身先によって処理を変更する
		switch (m_nextForm)
		{
		case EnemyForm::PlayerSeeker:
			m_updateFunc = &TransformEnemy::SeekerUpdate;
			m_drawFunc = &TransformEnemy::SeekerDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		case EnemyForm::FireBall:
			m_updateFunc = &TransformEnemy::FireBallUpdate;
			m_drawFunc = &TransformEnemy::FireBallDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		case EnemyForm::Skull:
			m_updateFunc = &TransformEnemy::SkullUpdate;
			m_drawFunc = &TransformEnemy::SkullDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		default:
			printfDx(L"変身不可\n");
			break;
		}

		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}
}

void TransformEnemy::SeekerUpdate(Input&)
{
	// 移動方向をプレイヤーに向かうようにする
	m_direction = m_pPlayer->GetPos() - m_pos;
	// プレイヤーからどちらの軸の座標のほうが遠いか計測
	if (fabsf(m_direction.x) > fabsf(m_direction.y))
	{
		// x軸のほうが遠い時
		m_direction.y = 0.0f;
	}
	else
	{
		// y軸のほうが遠い時
		m_direction.x = 0.0f;
	}
	// 向きを正規化する
	m_direction.Normalize();
	m_pos += m_direction * kSeekerMoveSpeed;
	m_colCircle.pos = m_pos;
	m_colRect.pos = m_pos;
}

void TransformEnemy::FireBallUpdate(Input&)
{
	// プレイヤーの縦方向だけ追い続ける
	float toPlayerY = m_pPlayer->GetPos().y - m_pos.y; // 敵からプレイヤーに向かうベクトルのY座標
	float yComp = std::clamp(toPlayerY * kDirectionMagnification, -kMaxDirectionValue, kMaxDirectionValue); // -1から1までの間で向きを変える
	m_direction.x = kMaxDirectionValue;
	m_direction.y = yComp;
	m_direction.Normalize();

	m_pos += m_direction * kSeekerMoveSpeed;
	m_colCircle.pos = m_pos;
	m_colRect.pos = m_pos;
}

void TransformEnemy::SkullUpdate(Input&)
{
	// プレイヤーの横方向だけ追い続ける
	float toPlayerX = m_pPlayer->GetPos().x - m_pos.x; // 敵からプレイヤーに向かうベクトルのX座標
	float xComp = std::clamp(toPlayerX * kDirectionMagnification, -kMaxDirectionValue, kMaxDirectionValue); // -1から1までの間で向きを変える
	m_direction.y = kMaxDirectionValue;
	m_direction.x = xComp;
	m_direction.Normalize();

	m_pos += m_direction * kSeekerMoveSpeed;
	m_colCircle.pos = m_pos;
	m_colRect.pos = m_pos;
}

void TransformEnemy::ItemUpdate(Input&)
{
	// アイテム化時間を減らしていって、時間に達したら
	if (--m_itemFormTime <= 0)
	{
		// 元の状態によって処理を変更する
		// ※出現や変身の途中でItemに遷移した際の処理をどうするか未定
		switch (m_currentForm)
		{
		case EnemyForm::Normal:
			m_updateFunc = &TransformEnemy::NormalUpdate;
			m_drawFunc = &TransformEnemy::NormalDraw;
			break;
		case EnemyForm::PlayerSeeker:
			m_updateFunc = &TransformEnemy::SeekerUpdate;
			m_drawFunc = &TransformEnemy::SeekerDraw;
			break;
		case EnemyForm::FireBall:
			m_updateFunc = &TransformEnemy::FireBallUpdate;
			m_drawFunc = &TransformEnemy::FireBallDraw;
			break;
		case EnemyForm::Skull:
			m_updateFunc = &TransformEnemy::SkullUpdate;
			m_drawFunc = &TransformEnemy::SkullDraw;
			break;
		default:
			printfDx(L"変身不可\n");
			break;
		}

		// 当たり判定を元に戻す
		m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
		m_colCircle = { m_pos,kEnemyWidth / 2 };

		m_itemFormTime = 0; // アイテム化時間を0にする
		m_maxItemFormTime = 0; // アイテム化最大時間を0にする
		return; // 念のためreturn
	}
}

void TransformEnemy::AppearDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	DrawString(drawX, drawY, L"敵が出てきます", 0xffffff);
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::NormalDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	DrawBox(static_cast<int>(drawX - kEnemyWidth / 2), static_cast<int>(drawY - kEnemyHeight / 2),
			static_cast<int>(drawX + kEnemyWidth / 2), static_cast<int>(drawY + kEnemyHeight / 2),
			0x0000ff, true);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::TransformDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	DrawString(drawX, drawY, L"変身中です", 0xffffff);
}

void TransformEnemy::SeekerDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	DrawBox(static_cast<int>(drawX - kEnemyWidth / 2), static_cast<int>(drawY - kEnemyHeight / 2),
			static_cast<int>(drawX + kEnemyWidth / 2), static_cast<int>(drawY + kEnemyHeight / 2),
			0xff00ff, true);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::FireBallDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	DrawBox(static_cast<int>(drawX - kEnemyWidth / 2), static_cast<int>(drawY - kEnemyHeight / 2),
			static_cast<int>(drawX + kEnemyWidth / 2), static_cast<int>(drawY + kEnemyHeight / 2),
			0xff0066, true);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::SkullDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	DrawBox(static_cast<int>(drawX - kEnemyWidth / 2), static_cast<int>(drawY - kEnemyHeight / 2),
			static_cast<int>(drawX + kEnemyWidth / 2), static_cast<int>(drawY + kEnemyHeight / 2),
			0x22bb22, true);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::ItemDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);

	// 残り時間が75%消費されたら
	if (m_itemFormTime < m_maxItemFormTime * kItemWarningRate)
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto sinRate = 1.0f - sinf(m_itemFormTime / (m_maxItemFormTime * kItemWarningRate * kWarningFrashCycle) * DX_PI_F * 2);
		auto rate = static_cast<float>(m_itemFormTime) / (m_maxItemFormTime * kItemWarningRate) / 2;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * sinRate));
		DrawString(drawX, drawY, L"もうすぐ敵に戻ります", 0xff0000);
		DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else // それ以前なら
	{
		DrawString(drawX, drawY, L"アイテムに変化中", 0xff0000);
		DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
	}

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif

}

void TransformEnemy::CheckHitMapX()
{
	Rect2D mapRect; // 当たっているマップを取得する矩形
	if (m_pMap->IsCollision(m_colRect, mapRect))
	{
		// マップの矩形にどれだけめり込んでいるかの値
		const float collectionLeft = m_colRect.GetRight() - mapRect.GetLeft(); // マップの左側にどれくらいめり込んでいるか
		const float collectionRight = mapRect.GetRight() - m_colRect.GetLeft(); // マップの右側にどれくらいめり込んでいるか

		if (m_velocity.x < 0.0f) // 左に移動している時(右から当たったら)
		{
			// めり込んだ座標分左に補正
			m_pos.x += collectionRight;

		}
		else if (m_velocity.x > 0.0f)// 右に移動している時(左から当たったら)
		{
			// めり込んだ座標分右に補正
			m_pos.x -= collectionLeft;
		}
		else
		{
			// 移動していないのにめり込んだ場合
		}

		// 一旦x方向の速度を0にする
		//printfDx(L"x補正\n");
		//m_direction.x = 0.0f;
		m_velocity.x = 0.0f;
	}
}

void TransformEnemy::CheckHitMapY()
{
	Rect2D mapRect; // 当たっているマップを取得する矩形
	if (m_pMap->IsCollision(m_colRect, mapRect))
	{
		// マップの矩形にどれだけめり込んでいるかの値
		const float collectionTop = m_colRect.GetBottom() - mapRect.GetTop(); // マップの上側にどれくらいめり込んでいるか
		const float collectionBottom = mapRect.GetBottom() - m_colRect.GetTop(); // マップの下側にどれくらいめり込んでいるか

		if (m_velocity.y < 0.0f) // 上に移動している時(下から当たったら)
		{
			// めり込んだ座標分上に補正
			m_pos.y += collectionBottom;
		}
		else if (m_velocity.y > 0.0f)// 下に移動している時(上から当たったら)
		{
			// めり込んだ座標分下に補正
			m_pos.y -= collectionTop;
		}
		else
		{
			// 移動していないのにめり込んだ場合
		}

		// 一旦y方向の速度を0にする
		//printfDx(L"y補正\n");
		m_velocity.y = 0.0f;
	}
	else
	{
		// 上下に物がないので空中にいるとする
		m_isGround = false;
	}
}

bool TransformEnemy::IsCanCollision() const
{
	// 出現中、変身中、アイテム化中は当たり判定を行わない
	if (m_updateFunc == &TransformEnemy::AppearUpdate || m_updateFunc == &TransformEnemy::TransformUpdate || m_updateFunc == &TransformEnemy::ItemUpdate)
	{
		return false;
	}
	// それ以外は当たり判定可能
	return true;
}

bool TransformEnemy::IsItemMode() const
{
	// 更新処理がアイテム状態の場合
	return m_updateFunc == &TransformEnemy::ItemUpdate;
}


//void TransformEnemy::CheckHitMap()
//{
//	Rect2D mapRect; // 当たっているマップを取得する矩形
//	if (m_pMap->IsCollision(m_colRect, mapRect)) // マップと当たっていたら
//	{
//		// 横方向の押し出し判定
//		if (m_velocity.x < 0.0f) // 左に移動している時(左から当たったら)
//		{
//			// 当たったマップの右端X座標 + エネミーの幅の半分の座標に補正
//			m_pos.x = mapRect.GetRight() + kEnemyWidth * 0.5f;
//		}
//		else if(m_velocity.x > 0.0f)// 右に移動している時(左から当たったら)
//		{
//			// 当たったマップの左端X座標 - エネミーの幅の半分の座標に補正
//			m_pos.x = mapRect.GetLeft() - kEnemyWidth * 0.5f;
//		}
//
//		// 縦方向の押し出し判定
//		if (m_velocity.y < 0.0f) // 上に移動している時(下から当たったら)
//		{
//			// 当たったマップの下端Y座標 + エネミーの高さの半分の座標に補正
//			m_pos.y = mapRect.GetBottom() + kEnemyHeight * 0.5f;
//		}
//		else if (m_velocity.y > 0.0f) // 下に移動している時(上から当たったら)
//		{
//			// 当たったマップの上端Y座標 - エネミーの高さの半分の座標に補正
//			m_pos.y = mapRect.GetTop() - kEnemyHeight * 0.5f;
//			m_isGround = true;
//		}
//	}
//}
