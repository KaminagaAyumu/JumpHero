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
	Position2 m_pos; // 現在の表示座標(中心座標)
	Position2 m_targetPos; // 指定位置に移動するための座標
	std::string m_text; // 表示するテキスト
	Size m_size; // ウィンドウのサイズ(widthとheight)

};

