#include <string>
#include "UIManager.h"
#include "UITextWindow.h"
#include "UIText.h"
#include "DxLib.h"

namespace
{
	const wchar_t* kMainFontName = L"アンニャントロマン"; // メインで使うフォント名

	constexpr int kSmallFontSize = 28;
	constexpr int kMediumFontSize = 40;
	constexpr int kLargeFontSize = 60;

}

UIManager::UIManager()
{
	// UIコンテナを初期化
	m_pUIElements.clear();

	// フォントデータを読み込んで設定
	LoadFonts();
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

std::weak_ptr<UITextWindow> UIManager::CreateTextWindow(const std::string& text, const Size& size, const Position2& pos)
{
	auto ptr = std::make_shared<UITextWindow>();
	ptr->Init(text, size, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIText> UIManager::CreateText(int handle, const std::string& text, const Position2& pos)
{
	auto ptr = std::make_shared<UIText>();
	ptr->Init(m_fontHandles[0], text, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIText> UIManager::CreateText(Types::FontType fontType, const std::string& text, const Position2& pos)
{
	auto ptr = std::make_shared<UIText>();
	ptr->Init(m_fontHandles[static_cast<int>(fontType)], text, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}

void UIManager::LoadFonts()
{
	// フォントハンドル(小さいフォント)を作成
	int handle = CreateFontToHandle(
		kMainFontName,
		kSmallFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_8X8,
		-1,
		0,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(中くらいのフォント)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kMediumFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_8X8,
		-1,
		0,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(大きいフォント)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kLargeFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_8X8,
		-1,
		0,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);
}
