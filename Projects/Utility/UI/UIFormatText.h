#pragma once
#include "UIBase.h"
#include "../Geometry.h"
#include <string>
#include <functional>


class UIFormatText : public UIBase
{
public:
	UIFormatText();
	virtual ~UIFormatText();
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

	/// <summary>
	/// 表示するテキストを返すプロバイダをセットする
	/// </summary>
	/// <param name="provider">プロバイダ</param>
	void SetProvider(std::function<std::string()> provider);

	/// <summary>
	/// 中央ぞろえでテキストを表示するようにする
	/// </summary>
	void SetCenter();

private:
	Position2 m_pos; // 表示座標
	std::string m_text; // 表示するテキスト
	std::function<std::string()> m_provider; // テキストの内容を取得するプロバイダ
	int m_fontHandle; // フォントのハンドル
	int m_color; // 文字の色
	int m_frameCount; // 表示してからのフレーム数
	bool m_isAlive; // 存在フラグ
	bool m_isCenter; // 中央ぞろえでテキストを表示するかどうか

	/// <summary>
	/// プロバイダからテキストデータを変更する
	/// </summary>
	void CheckProvider();

};

