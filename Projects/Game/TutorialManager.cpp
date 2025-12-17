#include <vector>
#include "TutorialManager.h"
#include "GameManager.h"
#include "../Utility/Map.h"
#include "../Utility/StringFunction.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "Player.h"
#include <fstream>
#include "DxLib.h"

namespace
{
	constexpr int kTextWindowMargin = 25;
}

TutorialManager::TutorialManager(GameManager* gameManager, TextManager* textManager, Map* map, Player* player) :
	m_eventIndex(0),
	m_isInput(false),
	m_isFreezeGame(false),
	m_isShowTextWindow(false)
{
	m_pGameManager = gameManager;
	m_pTextManager = textManager;
	m_pMap = map;
	m_pPlayer = player;
	LoadEventData();
	InitAreaPos();
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

	m_isInput = input.IsTriggered("OK");

	if (CheckTrigger(m_eventData[m_eventIndex]))
	{
		RunAction(m_eventData[m_eventIndex]);
	}
	//DrawEventData(m_eventIndex);
}

void TutorialManager::Draw() const
{
	if (m_isShowTextWindow)
	{
		DrawTextWindow();
	}
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

bool TutorialManager::InitAreaPos()
{
	m_areaPos.clear();
	for (int x = 0; x < m_pMap->GetMapWidth(); x++)
	{
		for (int y = 0; y < m_pMap->GetMapHeight(); y++)
		{
			int chipNo = m_pMap->GetEventData(x, y);
			if (m_pMap->IsEventFlagTile(chipNo))
			{
				int areaId = chipNo;
				float tileSize = m_pMap->GetTileSize();
				Position2 areaPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_areaPos[areaId] = areaPos;
			}
		}
	}

	return true;
}

const Position2* TutorialManager::FindAreaPos(int areaId) const
{
	auto it = m_areaPos.find(areaId);
	return it != m_areaPos.end() ? &it->second : nullptr;
}

bool TutorialManager::IsEnterArea(int areaNum)
{
	// プレイヤーの座標が指定エリアに達したら
	if (m_pPlayer->GetPos().x >= m_areaPos[areaNum].x)
	{
		return true;
	}
	return false;
}

int TutorialManager::GetAreaNum(std::string param)
{
	return std::stoi(param);
}

void TutorialManager::DrawTextWindow() const
{
	DrawBox(kTextWindowMargin, kTextWindowMargin, Game::kScreenWidth - kTextWindowMargin, Game::kScreenHeight * 0.5f, 0x333333,true);
	auto str = StringFunction::WStringFromString(m_textPager.pages[m_textPager.index].textData);
	int width = GetDrawFormatStringWidth(L"%s", str.c_str());
	DrawFormatString(width / 2, 200, 0xffffff, L"%s", str.c_str());
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
	{
		int areaNum = GetAreaNum(data.triggerParam);
		return IsEnterArea(areaNum);
	}
	case TriggerType::GetItem:
		break;
	case TriggerType::EnemySpawn:
		break;
	}
	return false;
}

void TutorialManager::RunAction(const EventData& data)
{
	switch (data.actionType)
	{
	case ActionType::ShowText:
	{
		std::string id = data.actionParam;
		auto pages = m_pTextManager->GetAllPageText(id);
		if (pages.empty())
		{
			printfDx(L"テキストが存在しない\n");
			return;
		}
		m_textPager.id = id;
		m_textPager.pages = pages;
		m_textPager.index = 0;
		m_textPager.isActive = true;
		
		m_isShowTextWindow = true;
	}
		break;
	case ActionType::FreezeGame:
		m_isFreezeGame = true;
		break;
	case ActionType::UnfreezeGame:
		break;
	case ActionType::PowerUp:
		break;
	case ActionType::WaitInput:
		if (m_isInput)
		{
			if (m_textPager.isActive)
			{
				m_textPager.index++;
				if (m_textPager.index >= m_textPager.pages.size())
				{
					m_textPager.isActive = false;
					m_textPager.index = m_textPager.pages.size() - 1;
				}
			}
		}
		else
		{
			if (!m_textPager.isActive)
			{
				m_isFreezeGame = false;
				m_isShowTextWindow = false;
			}

		}
		if (m_isFreezeGame)
		{
			return;
		}
		break;
	}
	m_eventIndex++;
}
