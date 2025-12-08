#include <memory>
#include "MiniGameScene.h"
#include "SceneController.h"
#include "ClearScene.h"
#include "../Game/GameManager.h"
#include "../Utility/Game.h"
#include "../Utility/Input.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

}

MiniGameScene::MiniGameScene(SceneController& controller, std::shared_ptr<GameManager> gameManager) :
	SceneBase(controller),
	m_fadeColor(0xffffff),
	m_updateFunc(&MiniGameScene::FadeInUpdate),
	m_drawFunc(&MiniGameScene::FadeDraw),
	m_pGameManager(gameManager)
{
	m_frameCount = kFadeInterval;
}

MiniGameScene::~MiniGameScene()
{
}

void MiniGameScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void MiniGameScene::Draw()
{
	(this->*m_drawFunc)();
}

void MiniGameScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &MiniGameScene::DescriptionUpdate;
		m_drawFunc = &MiniGameScene::DescriptionDraw;
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::DescriptionUpdate(Input& input)
{

}

void MiniGameScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0xffffff;
		m_updateFunc = &MiniGameScene::FadeOutUpdate;
		m_drawFunc = &MiniGameScene::FadeDraw;
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller,m_pGameManager));
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::NormalDraw()
{

#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: NormalDraw", 0xffffff);
#endif
}

void MiniGameScene::DescriptionDraw()
{
#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: DescriptionDraw", 0xffffff);
#endif
}

void MiniGameScene::FadeDraw()
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: FadeDraw", 0xffffff);
#endif
}
