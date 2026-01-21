#include "UIGauge.h"
#include "DxLib.h"

UIGauge::UIGauge() : 
	m_frameHandle(-1),
	m_fillHandle(-1),
	m_value(0.0f),
	m_pos{},
	m_size{}
{
}

UIGauge::~UIGauge()
{
}

void UIGauge::SetHandle(int frameHandle, int fillHandle)
{
	m_frameHandle = frameHandle;
	m_fillHandle = fillHandle;
}

void UIGauge::Init(const Size& size, const Position2& pos)
{
	m_size = size;
	m_pos = pos;
}

void UIGauge::Update()
{

}

void UIGauge::Draw()const
{
	DrawBox(static_cast<int>(m_pos.x),
		static_cast<int>(m_pos.y),
		static_cast<int>(m_pos.x) + m_size.width,
		static_cast<int>(m_pos.y) + m_size.height,
		0x000000, true);

	int filledW = static_cast<int>(m_size.width * m_value + 0.5f);
	if (filledW > 0)
	{
		DrawBox(static_cast<int>(m_pos.x),
			static_cast<int>(m_pos.y),
			static_cast<int>(m_pos.x) + filledW,
			static_cast<int>(m_pos.y) + m_size.height,
			0xaaaa44, true);
	}

}

bool UIGauge::IsAlive()const
{
	return true;
}

void UIGauge::SetValue(float value)
{
	if (value < 0.0f)
	{
		value = 0.0f;
	}
	if (value > 1.0f)
	{
		value = 1.0f;
	}
	m_value = value;
}
