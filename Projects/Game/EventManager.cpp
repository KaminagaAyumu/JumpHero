#include "EventControls.h"
#include "EventSensors.h"

#include "EventManager.h"

EventManager::EventManager() : 
	m_eventIndex(0)
{
}

EventManager::~EventManager()
{
}

void EventManager::Update()
{

}

void EventManager::Draw() const
{

}

TriggerType EventManager::ToTriggerType(const std::string strData)
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

ActionType EventManager::ToActionType(const std::string strData)
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
	return false;
}

void EventManager::RunAction(const EventData& data)
{

}

void EventManager::RunCommonAction(const EventData& data)
{

}