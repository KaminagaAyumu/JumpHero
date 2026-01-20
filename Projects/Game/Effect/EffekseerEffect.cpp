#include "../../Utility/Geometry.h"
#include "../../Utility/Camera.h"
#include <memory>
#include "EffekseerEffect.h"
#include "EffekseerForDxLib.h"

EffekseerEffect::EffekseerEffect(int handle, const Position2& pos)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	SetPosPlayingEffekseer2DEffect(m_effect, pos.x, pos.y, 0);
}

EffekseerEffect::~EffekseerEffect()
{
	StopEffekseer2DEffect(m_effect);
}

void EffekseerEffect::Update()
{
	// カメラが存在する場合
	if (auto camera = m_pCamera.lock())
	{
		float screenX = m_pos.x - camera->scroll.x;
		float screenY = m_pos.y - camera->scroll.y;
		SetPosPlayingEffekseer2DEffect(m_effect, screenX, screenY, 0);
	}
}

bool EffekseerEffect::IsAlive()const
{
	return IsEffekseer2DEffectPlaying(m_effect) == 0;
}
