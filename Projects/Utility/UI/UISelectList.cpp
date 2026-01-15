#include "UISelectList.h"

UISelectList::UISelectList() : 
	m_pos{},
	m_fontHandle(-1),
	m_color(0),
	m_frameCount(0)
{
}

UISelectList::~UISelectList()
{
}

void UISelectList::Init(int handle, std::string text, const Position2& pos)
{
}

void UISelectList::Update()
{
}

void UISelectList::Draw() const
{
}

bool UISelectList::IsAlive() const
{
	return false;
}
