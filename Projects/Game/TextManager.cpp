#include <vector>
#include "TextManager.h"
#include "../Utility/StringFunction.h"
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
	
}

void TextManager::LoadTextData()
{
	std::ifstream file("data/textData.csv");
	std::string line;

	// ヘッダかどうかを判別する
	bool isHeader = true;

	while (std::getline(file, line))
	{

		if (isHeader) // 最初の一行は読み込まない
		{
			isHeader = false;
			continue;
		}
		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> row;

		while (getline(stream, field, ','))
		{
			row.push_back(field);
		}
		if (row.size() >= 3)
		{
			TextData data;
			data.id = row[0];
			data.page = std::stoi(row[1]);
			data.textData = row[2];
			m_textData[data.id].push_back(data);
		}
	}

}

std::string TextManager::GetText(const std::string& id)
{
	return std::string();
}
