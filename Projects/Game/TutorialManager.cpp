#include <vector>
#include "TutorialManager.h"
#include "GameManager.h"
#include "../Utility/Map.h"
#include "../Utility/StringFunction.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "Player.h"
#include <fstream>
#include "DxLib.h"

namespace
{
	constexpr int kTextWindowMargin = 25;

	constexpr int kEnemySpawnPosChipNo = 1;

	constexpr int kSpawnPosChipNo = 7; // 敵のスポーン時にカメラが見る位置のマップチップ番号
	constexpr int kGoalPosChipNo = 8; // ゴールを見せる時にカメラが見る位置のマップチップ番号
}

TutorialManager::TutorialManager(GameManager* gameManager, TextManager* textManager, Map* map, Player* player) :
	m_timeCount(0),
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
	InitEventPos();
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Update(Input& input)
{
	if (m_eventIndex >= static_cast<int>(m_eventData.size()))
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

bool TutorialManager::InitEventPos()
{
	m_areaPos.clear(); // イベント発火座標を初期化
	m_chestPos.clear(); // 宝箱の座標を初期化
	int chestId = 0; // 宝箱で使用するためのID

	// マップの右端からループ
	for (int x = 0; x < m_pMap->GetMapWidth(); x++)
	{
		for (int y = 0; y < m_pMap->GetMapHeight(); y++)
		{
			// イベント発火IDと座標を取得
			int chipNo = m_pMap->GetEventData(x, y);
			if (m_pMap->IsEventFlagTile(chipNo))
			{
				int areaId = chipNo; // エリアのIDを設定
				float tileSize = m_pMap->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 areaPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_areaPos[areaId] = areaPos; // IDと座標を設定
			}

			if (chipNo == kSpawnPosChipNo)
			{
				float tileSize = m_pMap->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 cameraPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_cameraPos[kSpawnPosChipNo] = cameraPos; // IDと座標を設定
			}
			if (chipNo == kGoalPosChipNo)
			{
				float tileSize = m_pMap->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 cameraPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_cameraPos[kGoalPosChipNo] = cameraPos; // IDと座標を設定
			}

			// 宝箱のIDと座標を取得
			chipNo = m_pMap->GetPositioningData(x, y);
			if (m_pMap->IsChestTile(chipNo))
			{
				chestId++; // IDを増加(1から始まる)
				float tileSize = m_pMap->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 chestPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_chestPos[chestId] = chestPos; // IDと座標を設定
			}

			if (chipNo == kEnemySpawnPosChipNo)
			{
				float tileSize = m_pMap->GetTileSize(); // マップチップのサイズを取得
				Position2 spawnPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_spawnPos.push_back(spawnPos);
			}
		}
	}

	return true;
}

const Position2* TutorialManager::FindAreaPos(int areaId) const
{
	auto it = m_areaPos.find(areaId); // IDに対応する座標を探す
	// IDが配列の終端でなければ座標を返す、それ以外ならnullptrを返す
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

bool TutorialManager::IsOpenChest(int chestNum)
{
	float tileSize = m_pMap->GetTileSize();
	int x = m_pMap->WorldPosToMapPos(m_chestPos[chestNum].x, tileSize);
	int y = m_pMap->WorldPosToMapPos(m_chestPos[chestNum].y, tileSize);
	if (m_pMap->GetPositioningData(x, y) == 0)
	{
		return true;
	}
	return false;
}

bool TutorialManager::IsGetItem(std::string param)
{
	auto type = Types::ItemType::Coin;
	if (param == "coin") // テスト用
	{
		type = Types::ItemType::Coin;
	}
	if (param == "medal")
	{
		type = Types::ItemType::UpgradeMedal;
	}
	if (param == "toitem")
	{
		type = Types::ItemType::ChangeToCoin;
	}

	return m_pGameManager->IsItemPicked(type);
}

int TutorialManager::GetParamNum(std::string param)
{
	return std::stoi(param);
}

void TutorialManager::LookCamera(std::string param)
{
	// パラメータを見てカメラが見る位置を決める
	// メモ:一時変数などでポジションを入れると破棄されてしまうので注意
	if (param == "spawnPos")
	{
		m_pGameManager->SetCameraTarget(&m_cameraPos[kSpawnPosChipNo]);
	}
	if (param == "goalPos")
	{
		m_pGameManager->SetCameraTarget(&m_cameraPos[kGoalPosChipNo]);
	}
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
	if (strData == "GameStart") return TriggerType::GameStart;
	if (strData == "EnterArea") return TriggerType::EnterArea;
	if (strData == "TimeElapsed") return TriggerType::TimeElapsed;
	if (strData == "OpenChest") return TriggerType::OpenChest;
	if (strData == "GetItem") return TriggerType::GetItem;
	if (strData == "EnemySpawn") return TriggerType::EnemySpawn;
	if (strData == "PlayerPowerUp") return TriggerType::PlayerPowerUp;
	if (strData == "NoTrigger") return TriggerType::NoTrigger;
}

ActionType TutorialManager::ToActionType(const std::string strData)
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
}

bool TutorialManager::CheckTrigger(const EventData& data)
{
	switch (data.triggerType)
	{
	case TriggerType::GameStart:
		return true;
	case TriggerType::EnterArea:
	{
		int areaNum = GetParamNum(data.triggerParam);
		return IsEnterArea(areaNum);
	}
	case TriggerType::TimeElapsed:
	{
		m_timeCount++;
		int endTime = GetParamNum(data.triggerParam);
		if (m_timeCount >= endTime)
		{
			m_timeCount = 0;
			return true;
		}
	}
	break;
	case TriggerType::OpenChest:
	{
		int chestNum = GetParamNum(data.triggerParam);
		return IsOpenChest(chestNum);
	}
	case TriggerType::GetItem:
		return IsGetItem(data.triggerParam);
	case TriggerType::EnemySpawn:
		break;
	case TriggerType::PlayerPowerUp:
		return m_pPlayer->IsPowerUp();
		break;
	case TriggerType::NoTrigger:
		return true;
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
	case ActionType::DropItem:
	{
		Types::ItemType type; // 生成するアイテムが何かを判別する
		// 宝箱の番号を取得
		int chestNum = GetParamNum(data.triggerParam);
		if (data.actionParam == "coin")
		{
			type = Types::ItemType::Coin;
		}
		if (data.actionParam == "medal")
		{
			type = Types::ItemType::UpgradeMedal;
		}
		if (data.actionParam == "toitem")
		{
			type = Types::ItemType::ChangeToCoin;
			// 通常のアイテム(コイン)をまず生成
			m_pGameManager->DropItem(m_chestPos[chestNum].x, m_chestPos[chestNum].y, Types::ItemType::Coin);
		}
		
		m_pGameManager->DropItem(m_chestPos[chestNum].x, m_chestPos[chestNum].y, type);
	}
		break;
	case ActionType::SpawnEnemy:
		for (auto& pos : m_spawnPos)
		{
			m_pGameManager->SpawnEnemy(pos.x,pos.y, 1);
		}
		break;
	case ActionType::FreezeGame:
		m_isFreezeGame = true;
		break;
	case ActionType::UnFreezeGame:
		m_isFreezeGame = false;
		break;
	case ActionType::FreezePlayer:
		m_pPlayer->FreezeChange();
		break;
	case ActionType::UnFreezePlayer:
		m_pPlayer->FreezeChange();
		break;
	case ActionType::LookCamera:
	{
		// カメラが見る位置を設定
		LookCamera(data.actionParam);
		// カメラの補正が終わっていなければ
		if (!m_pGameManager->IsCameraLerpEnd())
		{
			return; // ここを抜けないようにする
		}
	}
		break;
	case ActionType::ReturnCamera:
		m_pGameManager->SetCameraTarget(&m_pPlayer->GetPos());
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
				m_isShowTextWindow = false;
			}

		}
		if (m_isShowTextWindow)
		{
			return;
		}
		break;
	}
	m_eventIndex++;
}
