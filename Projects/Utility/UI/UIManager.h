#pragma once
#include <memory>
#include <list>
#include <vector>
#include "../Geometry.h"
#include "../GameType.h"
#include "../../Game/TextManager.h"

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
	/// ページ付きのテキストウィンドウを作成する関数
	/// </summary>
	/// <param name="id"></param>
	/// <param name="pages"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	/// <param name="appearDuration"></param>
	/// <returns></returns>
	std::weak_ptr<UITextWindow> CreateTextWindowPaged(const std::string& id, const std::vector<TextData>& pages, const Size& size, const Position2& pos, float appearDuration);

	/// <summary>
	/// テキストを作成する関数
	/// </summary>
	/// <param name="fontType">フォントのタイプ</param>
	/// <param name="text">テキストの内容</param>
	/// <param name="pos">表示する中心座標</param>
	/// <returns></returns>
	std::weak_ptr<UIText> CreateText(Types::FontType fontType, const std::string& text, const Position2& pos);

private:
	std::vector<int> m_fontHandles; // フォントハンドルを格納する配列
	std::list<std::shared_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

	/// <summary>
	/// フォントをロードする関数(初期化時に呼ぶ)
	/// </summary>
	void LoadFonts();

};

