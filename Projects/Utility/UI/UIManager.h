#pragma once

class Map;
class UIBase;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();

	void Init();
	void Update();
	void Draw(Map* map);

private:
	std::vector<std::unique_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

};

