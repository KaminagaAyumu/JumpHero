#pragma once
class TextManager
{
public:
	TextManager();
	virtual ~TextManager();

	void Init();
	void Update();
	void Draw();

	void LoadTextData();

private:

	// テキストのID
	std::vector<int> m_textIDs;
	// テキストのデータ
	std::vector<std::string> m_textData;


};

