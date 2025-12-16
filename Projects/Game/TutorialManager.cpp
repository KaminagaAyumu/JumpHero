#include <vector>
#include "TutorialManager.h"
#include "GameManager.h"
#include "TextManager.h"
#include "../Utility/Map.h"
#include "../Utility/StringFunction.h"
#include "../Utility/Input.h"
#include <fstream>
#include "DxLib.h"

TutorialManager::TutorialManager(GameManager* gameManager, TextManager* textManager, Map* map) :
	m_eventIndex(0)
{
	m_pGameManager = gameManager;
	m_pTextManager = textManager;
	m_pMap = map;
	LoadEventData();
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Update(Input& input)
{
	if (m_eventIndex + 1 >= static_cast<int>(m_eventData.size()))
	{
		return;
	}

	if (input.IsTriggered("OK"))
	{
		m_eventIndex++;
	}
	DrawEventData(m_eventIndex);
}

void TutorialManager::DrawEventData(int id)
{
	printfDx(L"%d\n", m_eventData[id].id);
	
	if (m_eventData[id].triggerType == TriggerType::EnterArea)
	{
		printfDx(L"TriggerType :: EnterArea\n");
	}

	auto str = StringFunction::WStringFromString(m_eventData[id].triggerParam);
	{
		printfDx(L"TriggerParam :: %s\n", str.c_str());
	}

	if (m_eventData[id].actionType == ActionType::FreezeGame)
	{
		printfDx(L"TriggerType :: FreezeGame\n");
	}

	str = StringFunction::WStringFromString(m_eventData[id].actionParam);
	{
		printfDx(L"ActionParam :: %s\n", str.c_str());
	}
}

bool TutorialManager::LoadEventData()
{
	std::ifstream file("data/eventData.csv");
	if (!file) // ファイルの読み込みに失敗した場合
	{
		return false; // ロード失敗とする
	}
	std::string line;
	bool isHeader = true;


	while (std::getline(file, line))
	{
		// 最初の一行は読み込まない
		if (isHeader)
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

		EventData data;
		if (row.size() >= 5)
		{
			data.id = std::stoi(row[0]);
			data.triggerType = ToTriggerType(row[1]);
			data.triggerParam = row[2];
			data.actionType = ToActionType(row[3]);
			data.actionParam = row[4];
			m_eventData.push_back(data);
		}
	}

	return true;
}

size_t TutorialManager::GetEventNum()
{
	return size_t(m_eventData.size());
}

bool TutorialManager::IsEnterArea(int areaNum)
{
	
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

bool TutorialManager::CheckTrigger(const EventData& data)
{
	switch (data.triggerType)
	{
	case TriggerType::EnterArea:
		break;
	case TriggerType::GetItem:
		break;
	case TriggerType::EnemySpawn:
		break;
	}
	return false;
}

void TutorialManager::RunAction(const EventData& data)
{
}
