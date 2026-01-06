#pragma once
#include "UIBase.h"
#include "../Geometry.h"
#include <string>

/// <summary>
/// テキストとウィンドウを表示するUIクラス
/// </summary>
class UITextWindow : public UIBase
{
public:
	UITextWindow();
	virtual ~UITextWindow();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="text">表示するテキストの内容</param>
	/// <param name="size">ウィンドウのサイズ</param>
	void Init(std::string text, Size size);

	void Update()override;
	void Draw()const override;

private:
	Position2 m_pos; // 現在の表示座標(中心座標)
	Position2 m_targetPos; // 指定位置に移動するための座標
	std::string m_text; // 表示するテキスト
	Size m_size; // ウィンドウのサイズ

};

