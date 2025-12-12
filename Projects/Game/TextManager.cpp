#include <vector>
#include "TextManager.h"
#include <fstream>
#include "DxLib.h"

TextManager::TextManager()
{
	LoadTextData();
}

TextManager::~TextManager()
{
}

void TextManager::Init()
{

}

void TextManager::Update()
{

}

void TextManager::Draw()
{
	for(std::string text : m_textData)
	{
		printfDx(L"%s\n", text.c_str());
	}
}

void TextManager::LoadTextData()
{
	std::ifstream file("data/textData.csv");
	std::string line;

	while (std::getline(file, line))
	{

		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> row;

		while (getline(stream, field, ','))
		{
			// 最初の一行は読み込まない
			bool isHeader = (field == "ID" || field == "テキスト");
			if (isHeader) continue;
			row.push_back(field);
		}
		if (row.size() >= 2)
		{
			int id = std::stoi(row[0]);
			std::string text = row[1];
			m_textData.push_back(text);
		}
	}

}
