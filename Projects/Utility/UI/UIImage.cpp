#include "UIImage.h"
#include "DxLib.h"

UIImage::UIImage() : 
	m_handle(-1),
	m_size{},
	m_pos{},
	m_isAlive(true)
{

}

UIImage::~UIImage()
{
}

void UIImage::Init(int handle, const Size& size, const Position2& pos)
{
	m_handle = handle;
	m_size = size;
	m_pos = pos;
}

void UIImage::Update()
{

}

void UIImage::Draw() const
{
	// 画像を描画
	DrawExtendGraph(static_cast<int>(m_pos.x),
		static_cast<int>(m_pos.y),
		static_cast<int>(m_pos.x) + m_size.width,
		static_cast<int>(m_pos.y) + m_size.height,
		m_handle, true);
}

bool UIImage::IsAlive()const
{
	return m_isAlive;
}

void UIImage::Close()
{
	m_isAlive = false;
}
