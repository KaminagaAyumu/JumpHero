#include "UIFormatText.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kDefaultColor = 0xffffff; // デフォルトの文字色(白)
}

UIFormatText::UIFormatText() :
	m_pos{},
	m_fontHandle(-1),
	m_color(kDefaultColor),
	m_frameCount(0),
	m_isAlive(true),
	m_isCenter(false)
{
}

UIFormatText::~UIFormatText()
{
}

void UIFormatText::Init(int handle, std::string text, const Position2& pos)
{
	m_fontHandle = handle;
	m_text = text;
	m_pos = pos;
}

void UIFormatText::Update()
{
	m_frameCount++;

	// プロバイダがない場合処理を行わない
	if (!m_provider) return;
	// プロバイダをセット
	CheckProvider();
}

void UIFormatText::Draw() const
{
	// テキストの内容をワイド文字列に変換
	auto text = StringFunction::WStringFromString(m_text);
	// テキストの幅を取得
	int width = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(m_text.length()), m_fontHandle);
	// 中央ぞろえかどうかを判定
	if (m_isCenter)
	{
		// テキストを中央揃えにするためにX座標を調整
		int adjustedX = static_cast<int>(m_pos.x) - width / 2;
		// テキストを描画
		DrawStringToHandle(adjustedX, static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
	}
	else
	{
		// 左端から表示
		DrawStringToHandle(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
	}
}

bool UIFormatText::IsAlive() const
{
	return m_isAlive;
}

void UIFormatText::SetProvider(std::function<std::string()> provider)
{
	// プロバイダをセット
	m_provider = std::move(provider);
	// セットと同時に実際のテキストの内容も変更
	CheckProvider();
}

void UIFormatText::SetCenter()
{
	m_isCenter = true;
}

void UIFormatText::ShowTypewriter(int typeFrame, bool isFadeOut)
{
}

void UIFormatText::CheckProvider()
{
	// プロバイダがセットされていない場合処理を抜ける
	if (!m_provider) return;
	
	// 現在のテキストの内容をプロバイダからチェック
	const std::string now = m_provider();
	
	// 現在のテキストのデータがプロバイダのテキストと一致していない場合
	if (now != m_text)
	{
		// テキストを変更
		m_text = now;
	}
}
