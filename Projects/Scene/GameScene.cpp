#include "GameScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "ClearScene.h"
#include "MissScene.h"
#include "../Utility/Game.h"
#include "../Game/Player.h"
#include "../Game/Chest.h"
#include "../Game/EnemyBase.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Game/ItemBase.h"
#include "../Game/GameManager.h"
#include "../Game/CollisionManager.h"
#include "../Game/ChestManager.h"
#include "../Game/ItemManager.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kEnemyCount = 5; // 敵の数

	constexpr int kSpaceChipNo = 79; // マップチップの透明部分
	constexpr int kChestChipNo = 46; // マップチップの宝箱部分

	//constexpr Vector2 kScrollPos = { 100.0f,0.0f }; // スクロール加算用
}

GameScene::GameScene(SceneController& controller) : SceneBase(controller),
m_updateFunc(&GameScene::FadeInUpdate),
m_drawFunc(&GameScene::FadeDraw),
m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;
	m_chestOpenNum = 0;

	m_bg = std::make_shared<Bg>();
	m_bg->Init();
	m_pMap = std::make_shared<Map>();
	m_pMap->Init();

	m_pGameManager = std::make_shared<GameManager>(m_pMap.get(),m_pActors);
	m_pGameManager->Init();
	
	m_pCollisionManager = std::make_unique<CollisionManager>();
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
#ifdef _DEBUG
	// プレイヤーと敵を初期化するコマンド(上入力しながらLボタン)
	if (input.IsTriggered("LShift") && input.IsPressed("Up"))
	{
		clsDx();
		printfDx(L"初期化コマンドを検知\n");
	}
#endif

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
		m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void GameScene::NormalDraw()
{
	m_bg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());

	for (auto& actor : m_pActors)
	{
		actor->Draw();
	}

	m_pGameManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"GameScene: NormalDraw", 0xffffff);
#endif

}

void GameScene::FadeDraw()
{
	m_bg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"GameScene: FadeDraw", 0xffffff);
#endif
}

