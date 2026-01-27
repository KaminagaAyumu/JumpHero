#include "UISelectList.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kDefaultPaddingY = 10; // リストの上下端からテキストまでの余白の初期値
	constexpr int kDefaultItemSpacing = 100; // テキストとテキストの間の余白の初期値

	constexpr int kWindowAlpha = 128; // テキストを表示する範囲の透明度
}

UISelectList::UISelectList() : 
	m_pos{},
	m_size{},
	m_fontHandle(-1),
	m_color(0),
	m_itemSpacing(kDefaultItemSpacing),
	m_frameCount(0),
	m_cursor(0)
{
	m_items.clear();
}

UISelectList::~UISelectList()
{
}

void UISelectList::AddOption(const std::string& text, std::function<void()> onSelect)
{
	m_items.push_back(OptionItem{ text, std::move(onSelect) });

	// 初回追加時にカーソルを0にする
	if (m_items.size() == 1)
	{
		m_cursor = 0;
	}
}

void UISelectList::Init(int fontHandle, const Size& size, const Position2& pos)
{
	m_fontHandle = fontHandle;
	m_size = size;
	m_pos = pos;
}

void UISelectList::Update()
{
	m_frameCount++;
}

void UISelectList::Draw() const
{
	// テキストとカーソルをサイズの範囲内に描画する
	// テキストは左詰め
	// 上から順に均等に項目を並べる
	// 選択している項目が分かるようにするが、やり方(大きくしたり少し右寄りにするなど)は未定

	const int halfW = m_size.width / 2;
	const int halfH = m_size.height / 2;
	const int top = static_cast<int>(m_pos.y) - halfH;
	const int bottom = static_cast<int>(m_pos.y) + halfH;
	const int left = static_cast<int>(m_pos.x) - halfW;
	const int right = static_cast<int>(m_pos.x) + halfW;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kWindowAlpha);
	DrawBox(left, top, right, bottom, 0xff2200, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int y = top + kDefaultPaddingY;

	for (int i = 0; i < static_cast<int>(m_items.size()); i++)
	{
		const bool isSelected = (i == m_cursor);

		if (isSelected)
		{
			int offsetY = static_cast<int>(std::sin(m_frameCount * 0.12f) * 3.5f);
			DrawBox(left + kDefaultPaddingY,
				y - 4 + offsetY,
				right - kDefaultPaddingY,
				y + m_itemSpacing - 4 + offsetY,
				0x444444, true);
		}

		const auto& text = m_items[i].text;
		auto wText = StringFunction::WStringFromString(text); // ワイド文字列に変換
		auto width = GetDrawStringWidthToHandle(wText.c_str(), static_cast<int>(m_items[i].text.length()), m_fontHandle);
		auto height = GetFontSizeToHandle(m_fontHandle);
		DrawStringToHandle(left + kDefaultPaddingY, y + height / 2, wText.c_str(),isSelected ? 0xffffff : 0xff00ff, m_fontHandle);
		y += m_itemSpacing;
		// y座標がサイズを超えたらループを抜ける
		if (y > bottom - kDefaultPaddingY)
		{
			break;
		}
	}

}

bool UISelectList::IsAlive() const
{
	// 存在判定を変えられるようにしなければならない
	return true;
}

void UISelectList::MoveCursor(int dir)
{
	// 選択肢がない場合
	if (m_items.empty() || dir == 0)
	{
		// カーソルを動かさない
		return;
	}
	
	// カーソルの処理の説明の仕方が分かりません
	m_cursor = (m_cursor + dir + static_cast<int>(m_items.size())) % static_cast<int>(m_items.size());


}

void UISelectList::TriggerSelect()
{
	// 選択肢がない場合
	if (m_items.empty())
	{
		return;
	}

	// 選択肢が選択できるものである場合
	if (0 <= m_cursor && m_cursor < static_cast<int>(m_items.size()))
	{
		// 選択された際の処理が存在する場合
		if (m_items[m_cursor].onSelect)
		{
			// 選択された処理を行う
			m_items[m_cursor].onSelect();
		}
	}
}
