#pragma once

class Map;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();

	void Init();
	void Update();
	void Draw(Map* map);

private:


};

