#pragma once
#include <sstream>

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

	struct TextData
	{
		std::string id;
		int page;
		std::string textData;
	};

	// テキストのデータを格納する配列
	std::vector<TextData> m_textData;


};

