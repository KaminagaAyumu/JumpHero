#include "UIGauge.h"
#include "DxLib.h"

UIGauge::UIGauge() : 
	m_frameHandle(-1),
	m_fillHandle(-1),
	m_fillSrcW(0),
	m_fillSrcH(0),
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
	GetGraphSize(m_fillHandle, &m_fillSrcW, &m_fillSrcH);
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
	// 枠(空ゲージの中身含む)
	DrawExtendGraph(static_cast<int>(m_pos.x),
		static_cast<int>(m_pos.y),
		static_cast<int>(m_pos.x) + m_size.width,
		static_cast<int>(m_pos.y) + m_size.height,
		m_frameHandle, true);

	int cutW = static_cast<int>(m_fillSrcW * m_value);
	if (cutW > 0)
	{
		DrawRectExtendGraph(static_cast<int>(m_pos.x),
			static_cast<int>(m_pos.y),
			static_cast<int>(m_pos.x) + m_size.width * m_value,
			static_cast<int>(m_pos.y) + m_size.height,
			0, 0, m_fillSrcW * m_value, m_fillSrcH, m_fillHandle, true);
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
