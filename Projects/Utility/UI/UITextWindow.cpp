#include "UITextWindow.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr float startPosLeftX = -400.0f; // 画面外左側の開始位置X座標
}

UITextWindow::UITextWindow() :
	m_pos{},
	m_startPos{},
	m_targetPos{},
	m_size{},
	m_state(TextWindowState::Appearing),
	m_appearRate(0.0f),
	m_appearDuration(30.0f)
{
}

UITextWindow::~UITextWindow()
{
}

void UITextWindow::Init(std::string text, Size size, const Position2& target)
{
	m_text = text; // テキストの内容を設定
	m_size = size; // ウィンドウのサイズを設定
	m_targetPos = target; // 目標位置を設定
	m_pos = m_startPos; // 初期位置を設定
}

void UITextWindow::Update()
{
	if (m_state == TextWindowState::Appearing || m_state == TextWindowState::Disappearing)
	{
		m_appearRate += 0.1f / m_appearDuration;
		if (m_appearRate >= 1.0f)
		{
			m_appearRate = 1.0f;
			m_state = (m_state == TextWindowState::Appearing) ? TextWindowState::Visible : TextWindowState::Hidden;
		}
		m_pos = Geometry::LerpVec2(m_startPos, m_targetPos, m_appearRate); // 線形補間で位置を更新
	}
}

void UITextWindow::Draw() const
{
	DrawBox(m_pos.x - m_size.width / 2, m_pos.y - m_size.height / 2,
		m_pos.x + m_size.width / 2, m_pos.y + m_size.height / 2,
		GetColor(0, 0, 0), TRUE); // ウィンドウの背景を描画
	auto text = StringFunction::WStringFromString(m_text);
	int width = GetDrawFormatStringWidth(L"%s", text.c_str());
	DrawString(m_pos.x + width / 2, m_pos.y,
		text.c_str(), GetColor(255, 255, 255)); // テキストを描画
}

bool UITextWindow::IsAlive() const
{
	return m_state != TextWindowState::Hidden;
}

void UITextWindow::ShowFromRight()
{
	m_startPos = { startPosLeftX, m_targetPos.y };
	m_appearRate = 0.0f;
	m_state = TextWindowState::Appearing;
}
