#include "../../Game/TextManager.h"
#include "UITextWindow.h"
#include "../StringFunction.h"
#include <cmath>
#include "DxLib.h"

namespace
{
	constexpr float kStartPosLeftX = -400.0f; // 画面外左側の開始位置X座標

	constexpr int kDefaultAliveTime = 660; // デフォルトの表示時間（フレーム数）
}

UITextWindow::UITextWindow() :
	m_pos{},
	m_startPos{},
	m_targetPos{},
	m_size{},
	m_targetSize{},
	m_state(TextWindowState::Appearing),
	m_appearRate(0.0f),
	m_appearDuration(0.0f),
	m_isChangePos(false),
	m_isChangeSize(false),
	m_aliveFrame(kDefaultAliveTime),
	m_fontHandle(-1),
	m_windowGraphHandle(-1),
	m_textPager{}
{
}

UITextWindow::~UITextWindow()
{
}

void UITextWindow::Init(std::string text, const Size& size, const Position2& target, int fontHandle, int windowGraphHandle)
{
	m_text = text; // テキストの内容を設定
	m_targetSize = size; // 目標サイズを設定
	m_targetPos = target; // 目標位置を設定
	m_pos = m_startPos; // 初期位置を設定
	// ハンドルを取得
	m_fontHandle = fontHandle;
	m_windowGraphHandle = windowGraphHandle;
}

void UITextWindow::SetPages(const std::string& id, const std::vector<TextData>& pages)
{
	m_textPager.id = id;
	m_textPager.pages = pages;
	m_textPager.index = 0;
	m_textPager.isActive = false; // セット時には非アクティブにする
}

void UITextWindow::Update()
{
	m_aliveFrame--;
	if (m_state == TextWindowState::Appearing || m_state == TextWindowState::Disappearing)
	{
		m_appearRate += 0.1f / m_appearDuration;
		if (m_appearRate >= 1.0f)
		{
			m_appearRate = 1.0f;
			m_state = (m_state == TextWindowState::Appearing) ? TextWindowState::Visible : TextWindowState::Hidden;
		}
		if (m_isChangePos)
		{
			m_pos = Geometry::LerpVec2(m_startPos, m_targetPos, m_appearRate); // 線形補間で位置を更新
		}
		if (m_isChangeSize)
		{
			//m_size.height = static_cast<int>(m_targetSize.height * m_appearRate);
			m_size.height = static_cast<int>(std::lerp(m_size.height, m_targetSize.height, m_appearRate)); // 線形補間でサイズを更新
		}
	}
	// 時間で消える処理
	//if (m_aliveFrame <= 0)
	//{
	//	if (m_state == TextWindowState::Visible)
	//	{
	//		m_state = TextWindowState::Disappearing;
	//		m_appearRate = 0.0f;
	//		//m_appearDuration = 10.0f; // 消えるアニメーションの時間を設定
	//		if (m_isChangePos)
	//		{
	//			m_targetPos = m_startPos; // 目標位置を開始位置に設定
	//			m_startPos = m_pos; // 現在の位置を開始位置に設定
	//		}
	//		if (m_isChangeSize)
	//		{
	//			m_targetSize = { m_size.width, 0 }; // 目標サイズを高さ0に設定
	//		}
	//	}
	//}
}

void UITextWindow::Draw() const
{
	// ウィンドウの画像ハンドルを取得していれば
	if (m_windowGraphHandle != -1)
	{
		DrawExtendGraph(m_pos.x - m_size.width / 2, m_pos.y - m_size.height / 2,
			m_pos.x + m_size.width / 2, m_pos.y + m_size.height / 2,
			m_windowGraphHandle, false); // ウィンドウの背景を描画
	}
	else // ハンドルを取得していなければ
	{
		// 黒で背景を描画
		DrawBox(m_pos.x - m_size.width / 2, m_pos.y - m_size.height / 2,
			m_pos.x + m_size.width / 2, m_pos.y + m_size.height / 2,
			GetColor(0, 0, 0), TRUE); // ウィンドウの背景を描画
	}

	auto text = StringFunction::WStringFromString(m_text);
	int width = 0; // 文字を中央ぞろえで表示するための変数
	if (m_fontHandle != -1) // フォントのハンドルがある場合
	{
		width = GetDrawFormatStringWidthToHandle(m_fontHandle, L"%s", text.c_str());
		DrawStringToHandle(m_pos.x - width / 2, m_pos.y,
		text.c_str(), GetColor(255, 255, 255), m_fontHandle);
	}
	else // フォントのハンドルがない場合
	{
		width = GetDrawFormatStringWidth(L"%s", text.c_str());
		DrawString(m_pos.x - width / 2, m_pos.y,
		text.c_str(), GetColor(255, 255, 255)); // テキストを描画
	}
}

bool UITextWindow::AdvancePages()
{
	if (!m_textPager.isActive)
	{
		// ページがアクティブでない場合ページが終わった
		return false;
	}

	if (m_state != TextWindowState::Visible)
	{
		// 存在はしているがページはめくらない
		return true;
	}

	// テキストが最後のページの場合
	if (m_textPager.index >= m_textPager.pages.size() - 1)
	{
		// ウィンドウを閉じる
		CloseWindow(m_appearDuration);
		// ページが終わったとする
		return false;
	}

	// テキストのページをめくる
	m_textPager.index++;
	// テキストを設定
	ApplyCurrentPageText();

	// ここまで来たらページがまだめくれるのでtrueとする
	return true;
}

void UITextWindow::CloseWindow(float disappearDuration)
{
	// テキストを非アクティブにする
	m_textPager.isActive = false;
	// ステートを消える際の状態にする
	m_state = TextWindowState::Disappearing;
	// 表示率をリセット
	m_appearRate = 0.0f;
	// 目標サイズを高さ0に設定
	m_targetSize = { m_size.width, 0 };
	// 消えるまでの時間を設定
	m_appearDuration = disappearDuration;
}

bool UITextWindow::IsAlive() const
{
	return m_state != TextWindowState::Hidden;
}

void UITextWindow::ShowPaging(float duration)
{
	if (m_textPager.pages.empty())
	{
		// ページテキストがない場合処理を行わない
		return;
	}
	m_textPager.isActive = true; // テキストをアクティブにする
	m_textPager.index = 0; // ページを初期化
	ApplyCurrentPageText(); // テキストの内容を設定

	m_appearDuration = duration;
	AppearFromCenter(duration);
}

void UITextWindow::ShowFromRight(float duration)
{
	m_startPos = { kStartPosLeftX, m_targetPos.y };
	m_appearRate = 0.0f;
	m_appearDuration = duration;
	m_size = m_targetSize;
	m_state = TextWindowState::Appearing;
	m_isChangePos = true;
}

void UITextWindow::AppearFromCenter(float duration)
{
	m_pos = m_targetPos;
	m_size = { m_targetSize.width, 0 };
	m_appearRate = 0.0f;
	m_appearDuration = duration;
	m_state = TextWindowState::Appearing;
	m_isChangeSize = true;
}

void UITextWindow::ApplyCurrentPageText()
{
	if (m_textPager.pages.empty())
	{
		// テキストのページがない場合は処理を行わない
		return;
	}
	// テキストのインデックスに対応するテキストを取得
	const auto& data = m_textPager.pages[m_textPager.index];
	// 現在のテキストをテキストデータにする
	m_text = data.textData;
}
