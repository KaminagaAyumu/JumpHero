#include "UIFormatText.h"
#include "../StringFunction.h"
#include <algorithm>
#include "DxLib.h"

namespace
{
	constexpr int kDefaultColor = 0xffffff; // デフォルトの文字色(白)

	constexpr int kMinTypeFrame = 1;
}

UIFormatText::UIFormatText() :
	m_pos{},
	m_fontHandle(-1),
	m_color(kDefaultColor),
	m_frameCount(0),
	m_isAlive(true),
	m_isCenter(false),
	m_visibleTextNum(-1), // 初期状態は-1
	m_typeFrame(0)
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
	m_wText = StringFunction::WStringFromString(m_text); // テキストをワイド文字列にしたものを取得
}

void UIFormatText::Update()
{
	m_frameCount++;

	// プロバイダがない場合処理を行わない
	if (!m_provider)
	{
		UpdateTypewriter();
		return;
	}
	// プロバイダをセット
	CheckProvider();
	UpdateTypewriter();
}

void UIFormatText::Draw() const
{
	const int nowTextNum = std::clamp(m_visibleTextNum, 0, static_cast<int>(m_wText.size()));
	std::wstring visibleText(m_wText.begin(), m_wText.begin() + nowTextNum);
	if (m_visibleTextNum < 0)
	{
		visibleText = m_wText;
	}

	// テキストの幅を取得
	const int width = GetDrawStringWidthToHandle(visibleText.c_str(), nowTextNum, m_fontHandle);

	// 中央ぞろえかどうかを判定
	if (m_isCenter)
	{
		// テキストを中央揃えにするためにX座標を調整
		int adjustedX = static_cast<int>(m_pos.x) - width / 2;
		// テキストを描画
		DrawStringToHandle(adjustedX, static_cast<int>(m_pos.y), visibleText.c_str(), m_color, m_fontHandle);
	}
	else
	{
		// 左端から表示
		DrawStringToHandle(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), visibleText.c_str(), m_color, m_fontHandle);
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
	// 1文字を表示するフレームを取得
	// 最低でも1以上にする
	m_typeFrame = max(kMinTypeFrame, typeFrame);

	m_visibleTextNum = 0; // 表示されている文字数をセット

	m_frameCount = 0; // フレームカウンタをリセット

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
		m_wText = StringFunction::WStringFromString(m_text);
	}
}

void UIFormatText::UpdateTypewriter()
{
	// 文字を出すフレームが設定されていなければ以下の処理を行わない
	if (m_typeFrame <= 0)
	{
		return;
	}

	// 最大文字数を取得
	const int maxTextNum = static_cast<int>(m_wText.size());
	// 文字がすべて表示されたらそれ以上更新しない
	if (m_visibleTextNum >= maxTextNum)
	{
		return;
	}

	int addNum = m_frameCount / m_typeFrame;
	if (addNum > m_visibleTextNum)
	{
		m_visibleTextNum = min(addNum, maxTextNum);
	}

}
