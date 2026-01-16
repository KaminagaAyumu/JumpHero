#include <memory>
#include "TitleScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../Utility/Game.h"
#include "../Utility/Bg.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/Application.h"
#include "../Utility/GameType.h"
#include <string>
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kStartTextMargin = 50; // スタート方法を示すテキストの表示位置調整
}


TitleScene::TitleScene(SceneController& controller) : SceneBase(controller),
m_frameCount(kFadeInterval),
m_updateFunc(&TitleScene::FadeInUpdate),
m_drawFunc(&TitleScene::FadeDraw)
{
	m_titleImageHandle = LoadGraph(L"data/logo.png");
	m_bg = std::make_shared<Bg>();
	m_soundManager = Application::GetInstance().GetSoundManager();
	m_soundManager->LoadSoundClip("test", L"data/sound/testBGM.mp3",SoundBus::BGM,1.0f,true);
	m_soundManager->LoadSoundClip("select", L"data/sound/selectBGM.wav", SoundBus::BGM, 1.0f, true);
	m_soundManager->LoadSoundClip("testSE", L"data/sound/testSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->PlayBGM("test",0.0f);
	m_pUIManager = Application::GetInstance().GetUIManager();
	auto test = m_pUIManager->CreateText(Types::FontType::Large, "STARTかAボタンでスタート", { Game::kScreenWidth / 2,
		Game::kScreenHeight / 2 + kStartTextMargin});
}

TitleScene::~TitleScene()
{
	DeleteGraph(m_titleImageHandle);
}

void TitleScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void TitleScene::Draw()
{
	(this->*m_drawFunc)();
}

void TitleScene::FadeInUpdate(Input& input)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &TitleScene::NormalUpdate;
		m_drawFunc = &TitleScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	m_soundManager->Update();
	//m_pUIManager->Update();
	// STARTボタンもしくはAボタンが押されたら
	if (input.IsTriggered("OK"))
	{
		// フェードアウト後、ゲームシーンへ移行
		m_updateFunc = &TitleScene::FadeOutUpdate;
		m_drawFunc = &TitleScene::FadeDraw;
		m_frameCount = 0;
		m_soundManager->Play("testSE", 1.0f, false);
		m_soundManager->CrossFadeBGM("select", 120.0f);
		return;
	}
}

void TitleScene::FadeOutUpdate(Input& input)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalDraw()
{
	m_bg->Draw();
	DrawRotaGraph(
		Game::kScreenWidth / 2,
		Game::kScreenHeight / 2,
		1.0f,
		0.0f,
		m_titleImageHandle,
		TRUE
	);

	m_pUIManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"TitleScene: NormalDraw", 0xFFFFFF);
	//DrawString(Game::kScreenWidth / 2 - 20,
		//Game::kScreenHeight / 2 + 50, L"※違います", 0xFFFFFF);
	DrawString(Game::kScreenWidth / 2 - 60,
		Game::kScreenHeight / 2, L"JumpHero", 0xFFFFFF);
#endif
	DrawString(Game::kScreenWidth / 2 - 100,
		Game::kScreenHeight / 2 + 100, L"STARTかAボタンでスタート", 0xFFFFFF);

}

void TitleScene::FadeDraw()
{
	m_bg->Draw();
	/*DrawRotaGraph(
		Game::kScreenWidth / 2,
		Game::kScreenHeight / 2,
		1.0f,
		0.0f,
		m_titleImageHandle,
		TRUE
	);*/
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawString(0, 0, L"TitleScene: FadeDraw", 0xFFFFFF);
#endif
}
