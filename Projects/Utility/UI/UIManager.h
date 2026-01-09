#pragma once
#include <memory>
#include <list>
#include <vector>
#include "../Geometry.h"

class Map;
class UIBase;
class UITextWindow;
class UIText;

/// <summary>
/// UI関連の管理用クラス
/// </summary>
class UIManager
{
public:
	UIManager();
	virtual ~UIManager();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// テキストウィンドウを作成する関数
	/// </summary>
	/// <param name="text"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	/// <returns></returns>
	std::weak_ptr<UITextWindow> CreateTextWindow(const std::string& text, const Size& size, const Position2& pos);
	
	/// <summary>
	/// テキストを作成する関数
	/// </summary>
	/// <param name="handle">フォントのハンドル</param>
	/// <param name="text"></param>
	/// <param name="pos"></param>
	/// <returns></returns>
	std::weak_ptr<UIText> CreateText(int handle, const std::string& text, const Position2& pos);

private:
	std::vector<int> m_fontHandles; // フォントハンドルを格納する配列
	std::list<std::shared_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

};

