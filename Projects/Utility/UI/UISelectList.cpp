#include "UISelectList.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kPaddingY = 10; // リストの上下端からテキストまでの余白
}

UISelectList::UISelectList() : 
	m_pos{},
	m_size{},
	m_fontHandle(-1),
	m_color(0),
	m_itemSpacing(0),
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

void UISelectList::Init(int fontHandle, Size size, const Position2& pos)
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
	const int top = m_pos.y - halfH;
	const int bottom = m_pos.y + halfH;
	const int left = m_pos.x - halfW;
	const int right = m_pos.x + halfW;

	int y = top + kPaddingY;

	for (int i = 0; i < static_cast<int>(m_items.size()); i++)
	{
		const bool isSelected = (i == m_cursor);

		const auto& text = m_items[i].text;
		auto wText = StringFunction::WStringFromString(text); // ワイド文字列に変換
		DrawStringToHandle(left + m_itemSpacing, y, wText.c_str(),isSelected ? 0xffffff : 0xff00ff, m_fontHandle);
		y += m_itemSpacing;
		// y座標がサイズを超えたらループを抜ける
		if (y > bottom - kPaddingY)
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
