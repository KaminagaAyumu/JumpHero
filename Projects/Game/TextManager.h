#pragma once
#include <sstream>
#include <unordered_map>

class TextManager
{
public:
	TextManager();
	virtual ~TextManager();

	void Init();
	void Update();
	void Draw();

	void LoadTextData();

	/// <summary>
	/// 取得したIDのテキストデータを返す
	/// </summary>
	/// <param name="id">テキストID</param>
	/// <returns>テキストデータ</returns>
	std::string GetText(const std::string& id);

private:

	struct TextData
	{
		std::string id;
		int page;
		std::string textData;
	};

	// テキストのデータを格納する配列
	std::unordered_map<std::string, std::vector<TextData>> m_textData;


};

