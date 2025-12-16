#include <vector>
#include "TutorialManager.h"
#include "../Utility/StringFunction.h"
#include <fstream>
#include "DxLib.h"

TutorialManager::TutorialManager()
{
}

TutorialManager::~TutorialManager()
{
}

bool TutorialManager::LoadEventData()
{
	std::ifstream file("data/textData.csv");
	if (!file) // ファイルの読み込みに失敗した場合
	{
		return false; // ロード失敗とする
	}
	std::string line;

	while (std::getline(file, line))
	{

		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> row;
		bool isHeader = true;

		while (getline(stream, field, ','))
		{
			// 最初の一行は読み込まない
			if (isHeader)
			{
				isHeader = false;
				continue;
			}
			row.push_back(field);
		}
		if (row.size() >= 4)
		{
			EventData data;
			data.id = std::stoi(row[0]);
			data.triggerType = ToTriggerType(row[1]);
			data.triggerParam = row[2];
			data.actionType = ToActionType(row[3]);
			m_eventData.push_back(data);
		}
	}

	return true;
}

TriggerType TutorialManager::ToTriggerType(const std::string strData)
{
	if (strData == "EnterArea") return TriggerType::EnterArea;
	if (strData == "GetItem") return TriggerType::GetItem;
	if (strData == "EnemySpawn") return TriggerType::EnemySpawn;
}

ActionType TutorialManager::ToActionType(const std::string strData)
{
	if (strData == "ShowText") return ActionType::ShowText;
	if (strData == "FreezeGame") return ActionType::FreezeGame;
	if (strData == "UnfreezeGame") return ActionType::UnfreezeGame;
	if (strData == "PowerUp") return ActionType::PowerUp;
	if (strData == "WaitInput") return ActionType::WaitInput;
}
