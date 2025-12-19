#pragma once
#include "UIBase.h"
#include "../Geometry.h"

/// <summary>
/// テキストとウィンドウを表示するUIクラス
/// </summary>
class UITextWindow : public UIBase
{
public:
	UITextWindow();
	virtual ~UITextWindow();

	void Update()override;
	void Draw()const override;

private:
	Position2 m_pos; // 表示座標(中心座標)
	std::string m_text; // 表示するテキスト

};

