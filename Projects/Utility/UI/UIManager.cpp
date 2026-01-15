#include <string>
#include "UIManager.h"
#include "UITextWindow.h"
#include "UIText.h"
#include "DxLib.h"

namespace
{
	const wchar_t* kMainFontName = L"アンニャントロマン"; // メインで使うフォント名

	// 小サイズのフォントハンドルの設定
	constexpr int kSmallFontSize		= 28;	// 文字の大きさ
	constexpr int kSmallFontEdgeSize	= 1;	// 文字の縁取りの大きさ

	// 中サイズのフォントハンドルの設定
	constexpr int kMediumFontSize		= 40;	// 文字の大きさ
	constexpr int kMediumFontEdgeSize	= 2;	// 文字の縁取りの大きさ

	// 大サイズのフォントハンドルの設定
	constexpr int kLargeFontSize		= 60;	// 文字の大きさ
	constexpr int kLargeFontEdgeSize	= 2; 	// 文字の縁取りの大きさ

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

std::weak_ptr<UITextWindow> UIManager::CreateTextWindowPaged(const std::string& id, const std::vector<TextData>& pages, const Size& size, const Position2& pos, float appearDuration)
{
	auto ptr = std::make_shared<UITextWindow>();
	ptr->Init("", size, pos);
	ptr->SetPages(id, pages);
	ptr->ShowPaging(appearDuration);
	ptr->SetHandle(m_fontHandles[0]);
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
	// フォントハンドル(小サイズ)を作成
	int handle = CreateFontToHandle(
		kMainFontName,
		kSmallFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kSmallFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(中サイズ)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kMediumFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kMediumFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(大サイズ)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kLargeFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kLargeFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);
}
