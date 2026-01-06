#include <memory>
#include <vector>
#include "UIManager.h"
#include "UITextWindow.h"

UIManager::UIManager()
{
	// UIコンテナを初期化
	m_pUIElements.clear();
	auto textWindow = std::make_shared<UITextWindow>();
	textWindow->Init("sss", { 200, 200 });
	m_pUIElements.push_back(textWindow);
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
}

void UIManager::Update()
{
	for (auto& element : m_pUIElements)
	{
		element->Update();
	}
}

void UIManager::Draw(Map* map)
{
	for (auto& element : m_pUIElements)
	{
		element->Draw();
	}
}
