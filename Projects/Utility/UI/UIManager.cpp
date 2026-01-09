#include <string>
#include "UIManager.h"
#include "UITextWindow.h"
#include "UIText.h"
#include "DxLib.h"

UIManager::UIManager()
{
	// UIコンテナを初期化
	m_pUIElements.clear();
	int handle = CreateFontToHandle(
		L"アンニャントロマン",
		28,
		4,
		DX_FONTTYPE_ANTIALIASING_8X8,
		-1,
		0,
		FALSE
	);
	m_fontHandles.push_back(handle);
}

UIManager::~UIManager()
{
	for (auto& handle : m_fontHandles)
	{
		DeleteFontToHandle(handle);
	}
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

std::weak_ptr<UIText> UIManager::CreateText(int handle, const std::string& text, const Position2& pos)
{
	auto ptr = std::make_shared<UIText>();
	ptr->Init(m_fontHandles[0], text, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}
