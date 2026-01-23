#include "../../Utility/Geometry.h"
#include "../../Utility/Camera.h"
#include <memory>
#include "EffekseerEffect.h"
#include "EffekseerForDxLib.h"



EffekseerEffect::EffekseerEffect():
	m_effect(-1),
	m_pos{}
{
}

EffekseerEffect::~EffekseerEffect()
{
	StopEffekseer2DEffect(m_effect);
}

void EffekseerEffect::Init(int handle, const Position2& pos)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	InitPos();
}

void EffekseerEffect::Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	m_pCamera = camera;

	InitPos();
}

void EffekseerEffect::Update()
{
	// カメラが存在する場合
	if (auto camera = m_pCamera.lock())
	{
		// スクロールを含めて座標を設定
		float screenX = m_pos.x - camera->scroll.x;
		float screenY = m_pos.y - camera->scroll.y;
		SetPosPlayingEffekseer2DEffect(m_effect, screenX, screenY, 0);
	}
	// カメラが存在しない場合は指定された座標から動かない
}

bool EffekseerEffect::IsAlive()const
{
	return IsEffekseer2DEffectPlaying(m_effect) == 0;
}

void EffekseerEffect::InitPos()
{
	// カメラが存在する場合
	if (auto camera = m_pCamera.lock())
	{
		// スクロールを含めて座標を設定
		float screenX = m_pos.x - camera->scroll.x;
		float screenY = m_pos.y - camera->scroll.y;
		SetPosPlayingEffekseer2DEffect(m_effect, screenX, screenY, 0);
	}
	else // カメラがない場合
	{
		// 指定された座標をそのまま設定
		SetPosPlayingEffekseer2DEffect(m_effect, m_pos.x, m_pos.y, 0);
	}
}
