#include "UIText.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kDefaultColor = 0xffffff; // デフォルトの文字色(白)
	constexpr int kAliveTime = 600; // テキストが表示されている時間
}

UIText::UIText() : 
	m_pos{},
	m_fontHandle(-1),
	m_color(kDefaultColor),
	m_frameCount(0)
{
}

UIText::~UIText()
{
}

void UIText::Init(int handle, std::string text, const Position2& pos)
{
	m_fontHandle = handle;
	m_text = text;
	m_pos = pos;
}

void UIText::Update()
{
	m_frameCount++;
}

void UIText::Draw() const
{
	auto text = StringFunction::WStringFromString(m_text);
	DrawStringToHandle(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
}

bool UIText::IsAlive() const
{
	return m_frameCount <= kAliveTime;
}
