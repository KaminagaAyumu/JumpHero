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
#include "../Utility/UI/UISelectList.h"
#include "../Utility/UI/UITextWindow.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	const Size kSelectListSize = { 400, 700 };
	const Position2 kSelectListPos = { 300, 350 };

}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&SelectScene::FadeInUpdate),
	m_drawFunc(&SelectScene::FadeDraw),
	m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;

	m_pBg = std::make_shared<Bg>();

	m_soundManager = Application::GetInstance().GetSoundManager();
	m_soundManager->LoadSoundClip("cursor_se", L"data/sound/SE/cursorSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->LoadSoundClip("ok_se", L"data/sound/SE/okSE.mp3", SoundBus::SE, 1.0f, false);

	m_pUIManager = std::make_unique<UIManager>();

	m_pEffectManager = std::make_shared<EffectManager>(controller.GetEffekseerResourceManager());

	m_pTextManager = std::make_unique<TextManager>();

	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kSelectListSize, kSelectListPos);
	auto list = m_pSelectList.lock();
	list->AddOption("チュートリアル",[this]()
		{
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 0));
		});
	list->AddOption("ステージ1", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 1));
		});
	list->AddOption("ステージ2", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 2));
		});

	
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
	}
	if (input.IsTriggered("Up"))
	{
		m_soundManager->Play("cursor_se", 1.0f, true);
		
		auto list = m_pSelectList.lock();
		list->MoveCursor(-1);
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

void SelectScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		//m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_selectIndex));
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
	/*if (cursor == 0)
	{
		auto window = m_pDescriptionWindow.lock();

		
	}*/
}
