#include <memory>
#include <vector>
#include "TutorialScene.h"
#include "SceneController.h"
#include "PauseScene.h"
#include "TitleScene.h"
#include "MiniGameScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Game/TextManager.h"
#include "../Game/TutorialManager.h"
#include "../Game/GameManager.h"
#include "../Game/CollisionManager.h"
#include "../Game/Actor.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
}

TutorialScene::TutorialScene(SceneController& controller) : 
	SceneBase(controller),
	m_updateFunc(&TutorialScene::FadeInUpdate),
	m_drawFunc(&TutorialScene::FadeDraw),
	m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;

	m_pTextManager = std::make_unique<TextManager>();
	
	m_pBg = std::make_unique<Bg>();
	m_pBg->Init();
	m_pMap = std::make_unique<Map>(0, false);
	m_pMap->Init();

	m_pGameManager = std::make_shared<GameManager>(m_pMap.get(), m_pActors);
	m_pGameManager->Init();

	m_pTutorialManager = std::make_unique<TutorialManager>(m_pGameManager.get(), m_pTextManager.get(), m_pMap.get(), m_pGameManager->GetPlayer());

	m_pCollisionManager = std::make_unique<CollisionManager>();

	m_pSoundManager = Application::GetInstance().GetSoundManager();
	m_pSoundManager->LoadSoundClip("tutorial", L"data/sound/tutorialBGM.mp3", SoundBus::BGM, 1.0f,true);
	m_pSoundManager->CrossFadeBGM("tutorial", 120.0f);
}

TutorialScene::~TutorialScene()
{
}

void TutorialScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void TutorialScene::Draw()
{
	(this->*m_drawFunc)();
}

void TutorialScene::FadeInUpdate(Input& input)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &TutorialScene::NormalUpdate;
		m_drawFunc = &TutorialScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}
void TutorialScene::NormalUpdate(Input& input)
{
	// ポーズボタンが押されたら
	if (input.IsTriggered("Pause"))
	{
		// ポーズシーンをプッシュする(このシーンに戻ることも可能にする)
		m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
		return;
	}
	m_pTutorialManager->Update(input);

	if (m_pTutorialManager->IsFreezeGame())
	{
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

	m_pBg->Update();
	m_pMap->Update();

	if (m_pGameManager->IsClear())
	{
		// クリアしたらミニゲームシーンへ移行
		m_fadeColor = 0xffffff; // 白にフェードアウト
		m_updateFunc = &TutorialScene::FadeOutUpdate;
		m_drawFunc = &TutorialScene::FadeDraw;
		return;
	}
}
void TutorialScene::MissUpdate(Input& input)
{

}
void TutorialScene::FadeOutUpdate(Input& input)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<MiniGameScene>(m_controller, m_pGameManager, 0));
		return; // 念のため処理を抜ける
	}
}

void TutorialScene::NormalDraw()
{
	m_pBg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());

	for (auto& actor : m_pActors)
	{
		actor->Draw();
	}

	m_pGameManager->Draw();

	m_pTutorialManager->Draw();

	//m_pTextManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"TutorialScene: NormalDraw", 0xffffff);
#endif
}
void TutorialScene::FadeDraw()
{
	m_pBg->Draw(m_pGameManager->GetCamera());
	m_pMap->Draw(m_pGameManager->GetCamera());

	for (auto& actor : m_pActors)
	{
		actor->Draw();
	}

	m_pGameManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"TutorialScene: FadeDraw", 0xffffff);
#endif
}