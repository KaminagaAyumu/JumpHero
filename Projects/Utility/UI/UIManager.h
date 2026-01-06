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
	void Draw();

private:
	std::vector<std::shared_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

};

