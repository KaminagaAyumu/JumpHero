#include "UIImage.h"
#include "DxLib.h"

UIImage::UIImage() : 
	m_handle(-1),
	m_size{},
	m_pos{}
{

}

UIImage::~UIImage()
{
}

void UIImage::Init(int handle, const Size& size, const Position2& pos)
{
	m_handle = handle;
	m_size = size;
	m_pos;
}

void UIImage::Update()
{

}

void UIImage::Draw() const
{
	
}

bool UIImage::IsAlive()const
{
	return true;
}