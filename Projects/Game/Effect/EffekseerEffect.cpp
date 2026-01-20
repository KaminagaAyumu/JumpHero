#include "../../Utility/Geometry.h"
#include "EffekseerEffect.h"
#include "EffekseerForDxLib.h"

EffekseerEffect::EffekseerEffect(int handle, const Position2& pos)
{
	m_effect = PlayEffekseer2DEffect(handle);

	SetPosPlayingEffekseer2DEffect(m_effect, pos.x, pos.y, 0);
}

EffekseerEffect::~EffekseerEffect()
{
	StopEffekseer2DEffect(m_effect);
}
