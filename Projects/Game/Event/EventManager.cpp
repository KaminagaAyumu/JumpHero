#include <memory>
#include "EventManager.h"
#include <fstream>
#include <sstream>
#include "../../Utility/Input.h"


namespace
{
	constexpr int kEventPathSizeMax = 255; // ファイルのパスの最大サイズ

	constexpr int kEventDataSize = 5;
	constexpr int kCommonEventDataSize = 6;
	constexpr int kEventId = 0;
	constexpr int kTriggerType = 1;
	constexpr int kTriggerParam = 2;
	constexpr int kActionType = 3;
	constexpr int kActionParam = 4;
	constexpr int kOnce = 5;
}

EventManager::EventManager() : 
	m_eventIndex(0)
{
}

EventManager::~EventManager()
{
}

void EventManager::Update(Input& input)
{
	m_isInput = input.IsTriggered("OK");

	for (auto& common : m_commonEventData)
	{
		// すでにアクションが行われていて一度しか行わないイベントなら行わない
		if (common.isOnce && common.isInvoked) continue;
		if (CheckTrigger(common))
		{
			RunCommonAction(common);
			if (common.isOnce)
			{
				common.isInvoked = true;
			}
		}
	}

	/*if (CheckTrigger(m_eventData[m_eventIndex]))
	{
		RunAction(m_eventData[m_eventIndex]);
	}*/
}

void EventManager::Draw() const
{

}

void EventManager::SetEvents(const std::shared_ptr<EventControls>& controls, const std::shared_ptr<EventSensors>& sensors)
{
	m_pControls = controls;
	m_pSensors = sensors;
}

bool EventManager::LoadEventData(int stageNo)
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
		if (row.size() >= kEventDataSize)
		{
			data.id = std::stoi(row[kEventId]);
			data.triggerType = ToTriggerType(row[kTriggerType]);
			data.triggerParam = row[kTriggerParam];
			data.actionType = ToActionType(row[kActionType]);
			data.actionParam = row[kActionParam];
			m_eventData.push_back(data);
		}
	}

	return true;
}

bool EventManager::LoadCommonEventData(int stageNo)
{
	// ステージ番号に対応したパスを取得する変数
	wchar_t filePath[kEventPathSizeMax];

	std::swprintf(filePath, kEventPathSizeMax, L"data/event/stage%d_common.csv", stageNo);

	std::ifstream file(filePath);
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

		CommonEventData data;
		if (row.size() >= kCommonEventDataSize)
		{
			data.id = std::stoi(row[kEventId]);
			data.triggerType = ToTriggerType(row[kTriggerType]);
			data.triggerParam = row[kTriggerParam];
			data.actionType = ToActionType(row[kActionType]);
			data.actionParam = row[kActionParam];
			data.isOnce = row[kOnce] == "TRUE" ? true : false; // TRUEという文字列ならtrue、それ以外ならfalseにする
			m_commonEventData.push_back(data);
		}
	}

	return true;
}

int EventManager::GetParamNum(const std::string& param)
{
	return std::stoi(param);
}

TriggerType EventManager::ToTriggerType(const std::string& strData)
{
	if (strData == "GameStart") return TriggerType::GameStart;
	if (strData == "EnterArea") return TriggerType::EnterArea;
	if (strData == "TimeElapsed") return TriggerType::TimeElapsed;
	if (strData == "EventEnd") return TriggerType::EventEnd;
	if (strData == "OpenChest") return TriggerType::OpenChest;
	if (strData == "GetItem") return TriggerType::GetItem;
	if (strData == "EnemySpawn") return TriggerType::EnemySpawn;
	if (strData == "PlayerPowerUp") return TriggerType::PlayerPowerUp;
	if (strData == "NoTrigger") return TriggerType::NoTrigger;
	return TriggerType::NoTrigger; // ここまで来たら不正な値なのでNoTriggerを返す
}

ActionType EventManager::ToActionType(const std::string& strData)
{
	if (strData == "ShowText") return ActionType::ShowText;
	if (strData == "DropItem") return ActionType::DropItem;
	if (strData == "SpawnEnemy") return ActionType::SpawnEnemy;
	if (strData == "FreezeGame") return ActionType::FreezeGame;
	if (strData == "UnFreezeGame") return ActionType::UnFreezeGame;
	if (strData == "FreezePlayer") return ActionType::FreezePlayer;
	if (strData == "UnFreezePlayer") return ActionType::UnFreezePlayer;
	if (strData == "LookCamera") return ActionType::LookCamera;
	if (strData == "ReturnCamera") return ActionType::ReturnCamera;
	if (strData == "PowerUp") return ActionType::PowerUp;
	if (strData == "WaitInput") return ActionType::WaitInput;
	if (strData == "SetBarrier") return ActionType::SetBarrier;
	if (strData == "UnlockBarrier") return ActionType::UnlockBarrier;
	if (strData == "ActiveGoal") return ActionType::ActiveGoal;
	return ActionType::NoAction; // ここまで来たら不正な値なのでNoActionを返す
}

bool EventManager::CheckTrigger(const EventData& data)
{
	// 関数を呼ぶためにlockする
	auto sensors = m_pSensors.lock();
	if (!sensors) return false; // lock出来なかった場合処理を終わる

	switch (data.triggerType)
	{
	case TriggerType::OpenChest:
	{
		int chestNo = GetParamNum(data.triggerParam);
		return sensors->isOpenChestFunc(chestNo);
	}
		break;
	default:
		break;
	}

	return false;
}

void EventManager::RunAction(const EventData& data)
{
	// 関数を呼ぶためにlockする
	auto controls = m_pControls.lock();
	if (!controls) return; // lock出来なかった場合処理を終わる

	switch (data.actionType)
	{
	case ActionType::ShowText:

	case ActionType::DropItem:
	{
		int chestNo = GetParamNum(data.triggerParam);
		controls->dropItemFunc(chestNo, data.actionParam);
	}
		break;
	default:
		break;
	}

}

void EventManager::RunCommonAction(const EventData& data)
{
	// 関数を呼ぶためにlockする
	auto controls = m_pControls.lock();
	if (!controls) return; // lock出来なかった場合処理を終わる

	switch (data.actionType)
	{
	case ActionType::DropItem:
	{
		int chestNo = GetParamNum(data.triggerParam);
		controls->dropItemFunc(chestNo, data.actionParam);
	}
		break;
	default:
		break;
	}
}