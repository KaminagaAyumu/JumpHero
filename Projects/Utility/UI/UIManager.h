#pragma once
#include <memory>
#include <list>
#include "../Geometry.h"

class Map;
class UIBase;
class UITextWindow;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();

	void Init();
	void Update();
	void Draw();

	UITextWindow* CreateTextWindow(const std::string& text, const Size& size, const Position2& pos);

private:
	std::list<std::shared_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

};

