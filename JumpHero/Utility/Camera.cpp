#include "Camera.h"
#include "Game.h"
#include "Game/Actor.h"
#include <cassert>

namespace
{
	constexpr int kMapWidth = 4365;
	constexpr int kMapHeight = 720;

	constexpr float kLerpSpeed = 0.05f;
}

Camera::Camera() :
	scroll{},
	m_pos{},
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
	// カメラがスクロール開始位置より右にある時はスクロールする
	if (scroll.x > kMapWidth - Game::kScreenWidth)
	{
		scroll.x = kMapWidth - Game::kScreenWidth;
	}

	// カメラがスクロール開始位置より上にいるときはスクロールさせない
	if (scroll.y < 0)
	{
		scroll.y = 0;
	}
	// カメラがスクロール開始位置より下にいるときはスクロールする
	if (scroll.y > kMapHeight - Game::kScreenHeight)
	{
		scroll.y = kMapHeight - Game::kScreenHeight;
	}
}
