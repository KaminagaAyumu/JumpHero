#include <memory>
#include "SelectScene.h"
#include "SceneController.h"
#include "GameScene.h"
#include "../Utility/Input.h"
#include "../Utility/Bg.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Game/TextManager.h"
#include "../Game/Effect/EffectManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UIImage.h"
#include "../Utility/UI/UISelectList.h"
#include "../Utility/UI/UITextWindow.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr float kCrossFadeTime = 120.0f; // BGMをクロスフェードさせる際の時間

	constexpr int kTutorialListNo = 0;
	constexpr int kStage1ListNo = 1;
	constexpr int kStage2ListNo = 2;

	const Size kSelectListSize = { 400, 700 };
	const Position2 kSelectListPos = { 300, 350 };

	const Size kThumbnailSize = { 500,300 };
	const Position2 kThumbnailPos = { 700, 50 };

	constexpr float kWindowAppearDuration = 1.5f; // ウィンドウを表示、非表示するときの割合
	const Size kDescriptionWindowSize = { 500, 250 };
	const Position2 kDescriptionWindowPos = { 950, 520 };

}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&SelectScene::FadeInUpdate),
	m_drawFunc(&SelectScene::FadeDraw),
	m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;

	m_pBg = std::make_shared<Bg>();
	m_pBg->Init();

	m_soundManager = Application::GetInstance().GetSoundManager();
	m_soundManager->LoadSoundClip("cursor_se", L"data/sound/SE/cursorSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->LoadSoundClip("ok_se", L"data/sound/SE/okSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->LoadSoundClip("select", L"data/sound/BGM/selectBGM.wav", SoundBus::BGM, 1.0f, true);
	m_soundManager->CrossFadeBGM("select", kCrossFadeTime);

	m_pUIManager = std::make_unique<UIManager>();

	m_pThumbnail = m_pUIManager->CreateImage(Types::ImageType::TutorialIcon, kThumbnailSize, kThumbnailPos);

	m_pEffectManager = std::make_shared<EffectManager>(controller.GetEffekseerResourceManager());

	m_pTextManager = std::make_unique<TextManager>();

	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kSelectListSize, kSelectListPos);
	auto list = m_pSelectList.lock();
	list->AddOption("チュートリアル",[this]()
		{
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, kTutorialListNo));
		});
	list->AddOption("ステージ1", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, kStage1ListNo));
		});
	list->AddOption("ステージ2", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, kStage2ListNo));
		});
	list->AddOption("ゲームをやめる", [this]() 
		{
			m_updateFunc = &SelectScene::DialogUpdate;
			m_drawFunc = &SelectScene::DialogDraw;
			m_pDialogList = m_pUIManager->CreateSelectList(Types::FontType::Small, kSelectListSize, kSelectListPos);
		});

	m_pDescriptionWindow = m_pUIManager->CreateTextWindow(m_pTextManager->GetFirstPageText("select_tuto"), kDescriptionWindowSize, kDescriptionWindowPos, Types::FontType::Small);
	auto window = m_pDescriptionWindow.lock();
	window->AppearFromCenter(1.0f);

	CheckCursor();
}

SelectScene::~SelectScene()
{
}

void SelectScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void SelectScene::Draw()
{
	(this->*m_drawFunc)();
}

void SelectScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	m_pBg->Update();
	m_pEffectManager->Update();
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &SelectScene::NormalUpdate;
		m_drawFunc = &SelectScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void SelectScene::NormalUpdate(Input& input)
{
	m_pBg->Update();

	m_pUIManager->Update();

	m_pEffectManager->Update();

	if (input.IsTriggered("Down"))
	{
		m_soundManager->Play("cursor_se", 1.0f, true);
		
		auto list = m_pSelectList.lock();
		list->MoveCursor(1);
		CheckCursor();
	}
	if (input.IsTriggered("Up"))
	{
		m_soundManager->Play("cursor_se", 1.0f, true);
		
		auto list = m_pSelectList.lock();
		list->MoveCursor(-1);
		CheckCursor();
	}

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &SelectScene::FadeOutUpdate;
		m_drawFunc = &SelectScene::FadeDraw;
		m_soundManager->Play("ok_se", 1.0f, true);
		return; // 念のため処理を抜ける
	}

}

void SelectScene::DialogUpdate(Input& input)
{

}

void SelectScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	m_pBg->Update();
	if (m_frameCount >= kFadeInterval)
	{
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void SelectScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);

	m_pBg->Draw();

	m_pUIManager->Draw();

	m_pEffectManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: NormalDraw", 0xffffff);
#endif
}

void SelectScene::DialogDraw()
{
	NormalDraw();

}

void SelectScene::FadeDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);

	m_pBg->Draw();

	m_pUIManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: FadeDraw", 0xffffff);
#endif
}

void SelectScene::CheckCursor()
{
	// カーソルの状態を取得するためにlock
	auto list = m_pSelectList.lock();
	// カーソルの場所を取得する
	int cursor = list->GetCursor();
	// カーソルの場所によって説明ウィンドウのテキスト内容を変えるようにしたい
	if (cursor == kTutorialListNo)
	{
		if (auto window = m_pDescriptionWindow.lock())
		{
			window->CloseWindow(kWindowAppearDuration);
		}

		if (auto thumbnail = m_pThumbnail.lock())
		{
			thumbnail->Close();
		}
		
		m_pDescriptionWindow = m_pUIManager->CreateTextWindow(m_pTextManager->GetFirstPageText("select_tuto"), kDescriptionWindowSize, kDescriptionWindowPos, Types::FontType::Small);
		auto justWindow = m_pDescriptionWindow.lock();
		justWindow->AppearFromCenter(kWindowAppearDuration);

		m_pThumbnail = m_pUIManager->CreateImage(Types::ImageType::TutorialIcon, kThumbnailSize, kThumbnailPos);
	}
	else if (cursor == kStage1ListNo)
	{
		if (auto window = m_pDescriptionWindow.lock())
		{
			window->CloseWindow(kWindowAppearDuration);
		}

		if (auto thumbnail = m_pThumbnail.lock())
		{
			thumbnail->Close();
		}

		m_pDescriptionWindow = m_pUIManager->CreateTextWindow(m_pTextManager->GetFirstPageText("select_stage1"), kDescriptionWindowSize, kDescriptionWindowPos, Types::FontType::Small);
		auto justWindow = m_pDescriptionWindow.lock();
		justWindow->AppearFromCenter(kWindowAppearDuration);

		m_pThumbnail = m_pUIManager->CreateImage(Types::ImageType::Stage1Icon, kThumbnailSize, kThumbnailPos);
	}
	else if (cursor == kStage2ListNo)
	{
		if (auto window = m_pDescriptionWindow.lock())
		{
			window->CloseWindow(kWindowAppearDuration);
		}

		if (auto thumbnail = m_pThumbnail.lock())
		{
			thumbnail->Close();
		}

		m_pDescriptionWindow = m_pUIManager->CreateTextWindow(m_pTextManager->GetFirstPageText("select_stage2"), kDescriptionWindowSize, kDescriptionWindowPos, Types::FontType::Small);
		auto justWindow = m_pDescriptionWindow.lock();
		justWindow->AppearFromCenter(kWindowAppearDuration);

		m_pThumbnail = m_pUIManager->CreateImage(Types::ImageType::Stage2Icon, kThumbnailSize, kThumbnailPos);
	}
}
