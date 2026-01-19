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
	m_frameCount(0),
	m_isAlive(true)
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
	// テキストの内容をワイド文字列に変換
	auto text = StringFunction::WStringFromString(m_text);
	// テキストの幅を取得
	int width = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(m_text.length()), m_fontHandle);
	// テキストを中央揃えにするためにX座標を調整
	int adjustedX = static_cast<int>(m_pos.x) - width / 2;
	// テキストを描画
	DrawStringToHandle(adjustedX, static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
}

bool UIText::IsAlive() const
{
	return m_isAlive;
}
