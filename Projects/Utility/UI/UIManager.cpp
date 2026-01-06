#include <string>
#include "UIManager.h"
#include "UITextWindow.h"

UIManager::UIManager()
{
	// UIコンテナを初期化
	m_pUIElements.clear();
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
}

void UIManager::Update()
{
	if (m_pUIElements.empty()) return;

	for (auto& element : m_pUIElements)
	{
		element->Update();
	}

	m_pUIElements.remove_if(
		[](const std::shared_ptr<UIBase>& element) 
		{
			return !element->IsAlive();
		}
	);


}

void UIManager::Draw()
{
	for (auto& element : m_pUIElements)
	{
		element->Draw();
	}
}

UITextWindow* UIManager::CreateTextWindow(const std::string& text, const Size& size, const Position2& pos)
{
	auto ptr = std::make_shared<UITextWindow>();
	ptr->Init(text, size, pos);
	m_pUIElements.push_back(ptr);
	return ptr.get();
}
