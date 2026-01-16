#include <memory>
#include "PauseScene.h"
#include "SceneController.h" 
#include "TitleScene.h"
#include "SelectScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UISelectList.h"

#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kPopInterval = 30; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

}

PauseScene::PauseScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&PauseScene::FadeInUpdate),
	m_drawFunc(&PauseScene::FadeDraw),
	m_fadeColor(0x000000),
	m_transitionInterval(kPopInterval)
{
	m_frameCount = kPopInterval;

	m_pUIManager = std::make_unique<UIManager>();

	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, { 300,300 }, {Game::kScreenWidth / 2, Game::kScreenHeight / 2});
	auto list = m_pSelectList.lock();
	list->AddOption("ポーズを解除", [this]() 
		{
			// ポーズしたシーンに戻る
			m_controller.PopScene();
		});
	list->AddOption("ステージセレクトへ戻る", [this]()
		{
			// ステージセレクトシーンに遷移
			m_controller.ResetScene(std::make_shared<SelectScene>(m_controller));
		});
	list->AddOption("タイトルへ戻る", [this]() 
		{
			// ゲームシーンに遷移
			m_controller.ResetScene(std::make_shared<TitleScene>(m_controller));
		});

}

PauseScene::~PauseScene()
{
}

void PauseScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void PauseScene::Draw()
{
	(this->*m_drawFunc)();
}

void PauseScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &PauseScene::NormalUpdate;
		m_drawFunc = &PauseScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void PauseScene::NormalUpdate(Input& input)
{
	m_pUIManager->Update();
	if (input.IsTriggered("Down"))
	{
		//m_soundManager->Play("cursor_se", 1.0f, true);

		auto list = m_pSelectList.lock();
		list->MoveCursor(1);
	}
	if (input.IsTriggered("Up"))
	{
		//m_soundManager->Play("cursor_se", 1.0f, true);

		auto list = m_pSelectList.lock();
		list->MoveCursor(-1);
	}

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		auto list = m_pSelectList.lock();
		if (list->GetCursor() == 0)
		{
			m_transitionInterval = kPopInterval;
		}
		else
		{
			m_transitionInterval = kFadeInterval;
		}
		m_updateFunc = &PauseScene::FadeOutUpdate;
		m_drawFunc = &PauseScene::FadeDraw;
		return; // 念のため処理を抜ける
	}

}

void PauseScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= m_transitionInterval)
	{
		// フェードアウト完了
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void PauseScene::NormalDraw()
{

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111166, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	m_pUIManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"PauseScene: NormalDraw", 0xffffff);
#endif
}

void PauseScene::FadeDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111166, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_transitionInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"PauseScene: FadeDraw", 0xffffff);
#endif
}
