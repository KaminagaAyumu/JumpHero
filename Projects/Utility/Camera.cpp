#include "Camera.h"
#include "Game.h"
#include "../Game/Actor.h"
#include <cassert>

namespace
{
	constexpr float kLerpSpeed = 0.05f;
}

Camera::Camera(const Size& size) :
	scroll{},
	m_pos{},
	m_mapSize(size),
	m_pTargetActor(nullptr)
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
	assert(m_pTargetActor != nullptr && L"カメラのターゲット取得失敗");

	// カメラの座標を補正
	m_pos = Geometry::LerpVec2(m_pos, m_pTargetActor->GetPos(), kLerpSpeed);

	// スクロール量の計算
	// カメラの座標を画面中央にずらす
	scroll.x = m_pos.x - Game::kScreenWidth / 2;
	scroll.y = m_pos.y - Game::kScreenHeight / 2;

	// カメラがスクロール開始位置より左にいる時はスクロールさせない
	if (scroll.x < 0)
	{
		scroll.x = 0;
	}
	// カメラがスクロール終了位置より右にある時はそれ以上スクロールさせない
	if (scroll.x > m_mapSize.x - Game::kScreenWidth)
	{
		scroll.x = m_mapSize.x - Game::kScreenWidth;
	}

	// カメラがスクロール開始位置より上にいるときはスクロールさせない
	if (scroll.y < 0)
	{
		scroll.y = 0;
	}
	// カメラがスクロール終了位置より下にいるときはそれ以上スクロールさせない
	if (scroll.y > m_mapSize.y - Game::kScreenHeight)
	{
		scroll.y = m_mapSize.y - Game::kScreenHeight;
	}
}

const Rect2D& Camera::GetNowScreenArea() const
{
	return { scroll.x,scroll.x + Game::kScreenWidth,scroll.y, scroll.y + Game::kScreenWidth };
}
