#include "../../Game/TextManager.h"
#include "UITextWindow.h"
#include "../StringFunction.h"
#include "../Game.h"
#include <cmath>
#include "DxLib.h"

namespace
{
	constexpr float kStartPosLeftX = -400.0f; // 画面外左側の開始位置X座標

	constexpr int kDefaultAliveTime = 660; // デフォルトの表示時間（フレーム数）

	constexpr int kScrollWaitFrame = 60; // スクロールを止める時間
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
	m_isAutoPageMode(false),
	m_isScrollMode(false),
	m_isCenter(false),
	m_aliveFrame(kDefaultAliveTime),
	m_pageCount(0),
	m_pageIntervalFrame(0),
	m_scrollSpeed(0),
	m_scrollOffset(0),
	m_scrollWaitTimer(0),
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

void UITextWindow::EnableAutoPage(int intervalFrame)
{
	m_isAutoPageMode = true;
	m_pageIntervalFrame = intervalFrame;
	m_pageCount = 0;
}

void UITextWindow::EnableScrollMode(int scrollSpeed)
{
	m_isScrollMode = true;
	m_scrollSpeed = scrollSpeed;
	m_scrollOffset = 0;
	m_scrollWaitTimer = 0;
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


	// この処理をどこに入れるのか考え中
	//auto text = StringFunction::WStringFromString(m_text);
	//int textWidth = GetDrawFormatStringWidthToHandle(m_fontHandle, L"%s", text.c_str());
	//if (textWidth > m_size.width)
	//{

	//}
	//else
	//{
	//	// オフセットは不要なので0にしておく
	//	m_scrollOffset = 0;

	//	// テキストを中央ぞろえにする
	//	m_isCenter = true;
	//}


	if (m_state == TextWindowState::Visible)
	{
		if (m_isAutoPageMode)
		{
			m_pageCount++;
			if (m_pageCount >= m_pageIntervalFrame)
			{
				m_pageCount = 0;

				m_textPager.index++;
				if (m_textPager.index >= m_textPager.pages.size())
				{
					m_textPager.index = 0;
				}
				ApplyCurrentPageText();

				// テキストの内容が変わるのでスクロール関連の処理もリセット
				m_scrollOffset = 0;
				m_scrollWaitTimer = kScrollWaitFrame;
			}
		}

		if (m_isScrollMode)
		{
			auto text = StringFunction::WStringFromString(m_text);
			int textWidth = GetDrawFormatStringWidthToHandle(m_fontHandle, L"%s", text.c_str());

			// テキストの幅がウィンドウよりも大きい場合
			if (textWidth > m_size.width)
			{
				// スクロールを行う
				if (m_scrollWaitTimer > 0)
				{
					// スクロールを待つ時間
					m_scrollWaitTimer--;
				}
				else
				{
					// スクロールのオフセットをスピード分増加
					m_scrollOffset += m_scrollSpeed;
					// スクロールが最大(テキストの右端が見える状態)になった場合
					if (m_scrollOffset > textWidth - m_size.width)
					{
						// スクロールを最大値にする
						m_scrollOffset = textWidth - m_size.width;
						// スクロールを待つ時間を設定
						m_scrollWaitTimer = kScrollWaitFrame;
					}
				}
			}
			else // テキストの幅がウィンドウよりも小さい場合
			{
				// オフセットは不要なので0にしておく
				m_scrollOffset = 0;

				// テキストを中央ぞろえにする
				m_isCenter = true;
			}
			
		}
	}
}

void UITextWindow::Draw() const
{
	// ウィンドウの画像ハンドルを取得していれば
	if (m_windowGraphHandle != -1)
	{
		// 黒で背景を描画
		DrawBox(static_cast<int>(m_pos.x) - m_size.width / 2, static_cast<int>(m_pos.y) - m_size.height / 2,
			static_cast<int>(m_pos.x) + m_size.width / 2, static_cast<int>(m_pos.y) + m_size.height / 2,
			GetColor(0, 0, 0), TRUE); // ウィンドウの背景を描画

		DrawExtendGraph(static_cast<int>(m_pos.x) - m_size.width / 2, static_cast<int>(m_pos.y) - m_size.height / 2,
			static_cast<int>(m_pos.x) + m_size.width / 2, static_cast<int>(m_pos.y) + m_size.height / 2,
			m_windowGraphHandle, true); // ウィンドウの背景を描画
	}
	else // ハンドルを取得していなければ
	{
		// 黒で背景を描画
		DrawBox(static_cast<int>(m_pos.x) - m_size.width / 2, static_cast<int>(m_pos.y) - m_size.height / 2,
			static_cast<int>(m_pos.x) + m_size.width / 2, static_cast<int>(m_pos.y) + m_size.height / 2,
			GetColor(0, 0, 0), TRUE); // ウィンドウの背景を描画
	}

	const int halfW = m_size.width / 2;
	const int halfH = m_size.height / 2;
	const int top = static_cast<int>(m_pos.y) - halfH;
	const int bottom = static_cast<int>(m_pos.y) + halfH;
	const int left = static_cast<int>(m_pos.x) - halfW;
	const int right = static_cast<int>(m_pos.x) + halfW;

	SetDrawArea(left, top, right, bottom); // 描画可能範囲をウィンドウの中のみにする

	auto text = StringFunction::WStringFromString(m_text);
	int width = 0; // 文字を中央ぞろえで表示するための変数

	// 文字をスクロールさせるとき
	if (m_isScrollMode)
	{
		// 中央ぞろえにする場合(テキストをスクロールする必要がない時は中央ぞろえにする)
		if (m_isCenter)
		{
			width = GetDrawFormatStringWidthToHandle(m_fontHandle, L"%s", text.c_str());
			int fontSize = GetFontSizeToHandle(m_fontHandle);
			DrawStringToHandle(static_cast<int>(m_pos.x) - width / 2, static_cast<int>(m_pos.y) - fontSize / 2,
			text.c_str(), GetColor(255, 255, 255), m_fontHandle);
		}
		else
		{
			int drawX = left - m_scrollOffset;
			int fontSize = GetFontSizeToHandle(m_fontHandle);
			DrawStringToHandle(drawX, static_cast<int>(m_pos.y) - fontSize / 2, text.c_str(), 0xffffff, m_fontHandle);
		}
	}
	else
	{
		if (m_fontHandle != -1) // フォントのハンドルがある場合
		{
			width = GetDrawFormatStringWidthToHandle(m_fontHandle, L"%s", text.c_str());
			DrawStringToHandle(static_cast<int>(m_pos.x) - width / 2, static_cast<int>(m_pos.y),
			text.c_str(), GetColor(255, 255, 255), m_fontHandle);
		}
		else // フォントのハンドルがない場合
		{
			width = GetDrawFormatStringWidth(L"%s", text.c_str());
			DrawString(static_cast<int>(m_pos.x) - width / 2, static_cast<int>(m_pos.y),
			text.c_str(), GetColor(255, 255, 255)); // テキストを描画
		}
	}
	SetDrawArea(0, 0, Game::kScreenWidth, Game::kScreenHeight); // 描画可能範囲をリセット
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
