#include <string>
#include "UITextWindow.h"

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

}

void UITextWindow::Draw() const
{

}
