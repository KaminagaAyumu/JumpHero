#pragma once
#include "UIBase.h"
#include "../Geometry.h"
#include <vector>
#include <string>
#include <functional>

class UISelectList : public UIBase
{
public:
	struct SelectItem
	{
		std::string text; // 表示するテキストの内容
		std::function<void()> onSelect; // 決定されたときに行う処理
	};

	UISelectList();
	virtual ~UISelectList();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">テキストのハンドル</param>
	/// <param name="text">表示するテキストの内容</param>
	/// <param name="pos">表示する座標</param>
	void Init(int handle, std::string text, const Position2& pos);
	void Update()override;
	void Draw()const override;
	bool IsAlive()const override;

private:
	Position2 m_pos; // 表示座標(リスト全体の中心)
	std::string m_text; // 表示するテキスト
	int m_fontHandle; // フォントのハンドル
	int m_color; // 文字の色
	int m_frameCount; // 表示してからのフレーム数
};

