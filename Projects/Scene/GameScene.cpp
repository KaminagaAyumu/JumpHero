#include "GameScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "PauseScene.h"
#include "ClearScene.h"
#include "MiniGameScene.h"
#include "MissScene.h"
#include "../Utility/Game.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Game/Player.h"
#include "../Game/Chest.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Game/ItemBase.h"
#include "../Game/GameManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Game/TextManager.h"
#include "../Game/CollisionManager.h"
#include "../Game/ChestManager.h"
#include "../Game/ItemManager.h"
#include "../Game/Event/EventManager.h"
#include "../Game/Event/EventControls.h"
#include "../Game/Event/EventSensors.h"
#include "../Game/PositionRegistry.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kEnemyCount = 5; // 敵の数

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kMiniGameStageNo = 1; // ミニゲーム用ステージ番号


	constexpr int kSpawnPosChipNo = 7; // 敵のスポーン時にカメラが見る位置のマップチップ番号
	constexpr int kGoalPosChipNo = 8; // ゴールを見せる時にカメラが見る位置のマップチップ番号

	//constexpr Vector2 kScrollPos = { 100.0f,0.0f }; // スクロール加算用
}

GameScene::GameScene(SceneController& controller, int stageNo) : SceneBase(controller),
m_updateFunc(&GameScene::FadeInUpdate),
m_drawFunc(&GameScene::FadeDraw),
m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;
	m_chestOpenNum = 0;

	m_bg = std::make_shared<Bg>();
	m_bg->Init();
	m_pMap = std::make_shared<Map>(stageNo,false);
	m_pMap->Init();

	m_pGameManager = std::make_shared<GameManager>(m_pMap.get(),m_pActors);
	m_pGameManager->Init();

	m_pUIManager = std::make_unique<UIManager>();

	m_pTextManager = std::make_unique<TextManager>();
	
	m_pCollisionManager = std::make_unique<CollisionManager>();

	m_pSoundManager = Application::GetInstance().GetSoundManager();
	m_pSoundManager->LoadSoundClip("game", L"data/sound/stage3BGM.mp3", SoundBus::BGM, 1.0f, true);
	m_pSoundManager->CrossFadeBGM("game", 120.0f);

	m_pPositionRegistry = std::make_unique<PositionRegistry>();
	m_pPositionRegistry->InitPositions(m_pMap); // マップのデータから座標情報を取得

	// イベントセンサーのポインタを取得
	m_pEventSensors = std::make_shared<EventSensors>();
	m_pEventControls = std::make_shared<EventControls>();

	// イベントの内容を格納
	SetEventFunc();

	m_pEventManager = std::make_unique<EventManager>();
	if (!m_pEventManager->LoadCommonEventData(stageNo) || !m_pEventManager->LoadEventData(stageNo))
	{
		assert(false && "イベントデータの読み込みに失敗しました");
	}

	// イベントのデータをイベントマネージャーに渡す
	m_pEventManager->SetEvents(m_pEventControls, m_pEventSensors);

}

GameScene::~GameScene()
{
}

void GameScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void GameScene::Draw()
{
	(this->*m_drawFunc)();
}

void GameScene::FadeInUpdate(Input& input)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &GameScene::NormalUpdate;
		m_drawFunc = &GameScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void GameScene::NormalUpdate(Input& input)
{
	// ポーズボタンが押されたら
	if (input.IsTriggered("Pause"))
	{
		// ポーズシーンをプッシュする(このシーンに戻ることも可能にする)
		m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
		return;
	}

	// OKボタンが押されたかどうかを判定する
	m_isInputOK = input.IsTriggered("OK");

	m_pUIManager->Update();

	m_pEventManager->Update();

	// ゲームを止めるフラグが立っていたら
	if (m_pEventManager->IsFreezeGame())
	{
		// オブジェクトの更新を止める
		return;
	}

	// ゲームマネージャーの更新
	m_pGameManager->Update(input);

	// 次のフレームのためにゲーム内オブジェクトを更新
	m_pActors.clear(); // オブジェクトをリセット
	m_pActors.reserve(m_pGameManager->GetActorNum()); // 現在のオブジェクトの総数分要素を確保 
	m_pGameManager->PushActors(m_pActors); // ゲームマネージャーからオブジェクトを受け取る

	// 当たり判定を行う
	if (!m_pGameManager->IsSkipCollision()) // 当たり判定をスキップしない場合
	{
		m_pCollisionManager->CheckCollision(m_pActors);
	}

	// 背景とマップの更新(現在何もやっていない)
	m_bg->Update();
	m_pMap->Update();

	if (m_pGameManager->IsClear()) // ゲームマネージャーがクリアと判定したら
	{
		// このシーンの終了処理
		m_fadeColor = 0xffffff; // フェードを白フェードにする
		m_updateFunc = &GameScene::FadeOutUpdate;
		m_drawFunc = &GameScene::FadeDraw;
		return;
	}

}

void GameScene::MissUpdate(Input&)
{
}

void GameScene::FadeOutUpdate(Input& input)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<MiniGameScene>(m_controller,m_pGameManager, kMiniGameStageNo));
		return; // 念のため処理を抜ける
	}
}

void GameScene::NormalDraw()
{
	m_bg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());

	for (auto& actor : m_pActors)
	{
		// Actorの描画関数を呼ぶためにlockする
		if (auto pActor = actor.lock())
		{
			pActor->Draw();
		}
	}

	m_pUIManager->Draw();

	m_pGameManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"GameScene: NormalDraw", 0xffffff);
#endif

}

void GameScene::FadeDraw()
{
	m_bg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());
	m_pGameManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"GameScene: FadeDraw", 0xffffff);
#endif
}

void GameScene::SetEventFunc()
{
	// -----------------------------------------------------
	// イベントセンサーの関数
	// -----------------------------------------------------

	// 指定エリアに入ったかどうかの関数を定義
	m_pEventSensors->isEnterAreaFunc = [this](int areaNo)
		{
			// プレイヤーを持っているゲームマネージャーがエリアに入ったかどうかを判定する
			return m_pGameManager->IsExceededPlayer(m_pPositionRegistry->GetAreaPos(areaNo));
		};

	// 指定番号の宝箱が開いたかどうかの関数を定義
	m_pEventSensors->isOpenChestFunc = [this](int chestNo)
		{
			float tileSize = m_pMap->GetTileSize();
			int x = m_pMap->WorldPosToMapPos(m_pPositionRegistry->GetChestPos(chestNo).x, tileSize);
			int y = m_pMap->WorldPosToMapPos(m_pPositionRegistry->GetChestPos(chestNo).y, tileSize);
			if (m_pMap->GetPositioningData(x, y) == 0)
			{
				return true;
			}
			return false;
		};

	// 指定されたアイテムが取得されたかどうかの関数を定義
	m_pEventSensors->isGetItemFunc = [this](const std::string& id)
		{
			auto type = Types::ItemType::Coin;
			if (id == "coin") // テスト用
			{
				type = Types::ItemType::Coin;
			}
			if (id == "medal")
			{
				type = Types::ItemType::UpgradeMedal;
			}
			if (id == "toitem")
			{
				type = Types::ItemType::ChangeToCoin;
			}
			if (id == "balloon")
			{
				type = Types::ItemType::Balloon;
			}

			return m_pGameManager->IsItemPicked(type);
		};

	// プレイヤーがパワーアップしているかどうかの関数を定義
	m_pEventSensors->isPowerUpFunc = [this]()
		{
			return m_pGameManager->IsPowerUpPlayer();
		};

	// OKボタンが押されたかどうかの関数を定義
	m_pEventSensors->isPressedButtonFunc = [this]()
		{
			return m_isInputOK;
		};

	// カメラの補正が終わったかどうかの関数を定義
	m_pEventSensors->isCameraLerpEndFunc = [this]()
		{
			return m_pGameManager->IsCameraLerpEnd();
		};

	// 敵のスポーン位置を取得する関数を定義
	m_pEventSensors->getSpawnPositionsFunc = [this]()->const std::vector<Position2>&
		{
			return m_pPositionRegistry->GetSpawnPosAll();
		};

	// -----------------------------------------------------
	// イベントコントロールの関数
	// -----------------------------------------------------

	// テキストウィンドウを表示する関数を定義
	m_pEventControls->showTextWindowFunc = [this](const std::string& id, const Size& size, const Position2& pos, float duration)
		{
			// ページ付きのテキストウィンドウを作成
			auto pages = m_pTextManager->GetAllPageText(id);
			auto ptr = m_pUIManager->CreateTextWindowPaged(id, pages, size, pos, duration);
			return ptr;
		};
	
	// アイテムをドロップする関数を定義
	m_pEventControls->dropItemFunc = [this](int chestNo, const std::string& itemType)
		{
			Types::ItemType type; // 生成するアイテムが何かを判別する
			// 宝箱の番号を取得
			if (itemType == "coin")
			{
				type = Types::ItemType::Coin;
			}
			if (itemType == "medal")
			{
				type = Types::ItemType::UpgradeMedal;
			}
			if (itemType == "toitem")
			{
				type = Types::ItemType::ChangeToCoin;
				// 通常のアイテム(コイン)をまず生成
				m_pGameManager->DropItem(m_pPositionRegistry->GetChestPos(chestNo), Types::ItemType::Coin, false);
			}

			m_pGameManager->DropItem(m_pPositionRegistry->GetChestPos(chestNo), type, false);
		};

	// 敵を指定座標にスポーンさせる関数を定義
	m_pEventControls->spawnEnemiesFunc = [this](const std::vector<Position2>& pos, int formNo)
		{
			for (const auto& enemyPos : pos)
			{
				m_pGameManager->SpawnEnemy(enemyPos, formNo);
			}
		};

	// プレイヤーのフリーズ状態を変更する関数を定義
	m_pEventControls->changePlayerFreezeFunc = [this]()
		{
			m_pGameManager->ChangeFreezePlayer();
		};

	// カメラが見るターゲットを変更する関数を定義
	m_pEventControls->lookCameraFunc = [this](const std::string& key)
		{
			if (key == "spawnPos")
			{
				m_pGameManager->SetCameraTarget(m_pPositionRegistry->GetCameraPos(kSpawnPosChipNo));
			}
			else if (key == "goalPos")
			{
				m_pGameManager->SetCameraTarget(m_pPositionRegistry->GetCameraPos(kGoalPosChipNo));
			}
		};

	// カメラが見るターゲットをプレイヤーに戻す関数を定義
	m_pEventControls->returnCameraFunc = [this]()
		{
			m_pGameManager->ReturnCameraPlayer();
		};

	// バリアのアクティブ状態をセットする関数
	m_pEventControls->setBarrierActiveFunc = [this](int barrierNo)
		{
			// 指定IDのバリアをアクティブ状態にする
			m_pPositionRegistry->SetBarrierActive(barrierNo, true);
			// バリアをセットする
			m_pGameManager->SetBarrierPlayer(m_pPositionRegistry->GetBarrierPos(barrierNo));
		};

	// バリアを非アクティブにする関数
	m_pEventControls->setBarrierInActiveFunc = [this](int barrierNo)
		{
			m_pPositionRegistry->SetBarrierActive(barrierNo, false);

			m_pGameManager->SetBarrierPlayer(m_pPositionRegistry->GetBarrierPos(barrierNo));
		};

}

