#include "UITextWindow.h"
#include "../StringFunction.h"
#include "DxLib.h"

UITextWindow::UITextWindow() :
	m_pos{},
	m_targetPos{},
	m_size{}
{
}

UITextWindow::~UITextWindow()
{
}

void UITextWindow::Init(std::string text, Size size)
{
	m_text = text; // テキストの内容を設定
	m_size = size; // ウィンドウのサイズを設定
}

void UITextWindow::Update()
{
	m_pos.x++;
	m_pos.y++;
}

void UITextWindow::Draw() const
{
	DrawBox(m_pos.x - m_size.width / 2, m_pos.y - m_size.height / 2,
		m_pos.x + m_size.width / 2, m_pos.y + m_size.height / 2,
		GetColor(0, 0, 0), TRUE); // ウィンドウの背景を描画
	auto text = StringFunction::WStringFromString(m_text);
	DrawString(m_pos.x - m_size.width / 2 + 10, m_pos.y - m_size.height / 2 + 10,
		text.c_str(), GetColor(255, 255, 255)); // テキストを描画
}
