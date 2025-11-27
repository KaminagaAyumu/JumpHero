#include "ClearScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "DxLib.h"


namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
}

ClearScene::ClearScene(SceneController& controller) : 
	SceneBase(controller),
	m_updateFunc(&ClearScene::FadeInUpdate),
	m_drawFunc(&ClearScene::FadeDraw),
	m_fadeColor(0xffffff)
{
	m_frameCount = kFadeInterval;
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void ClearScene::Draw()
{
	(this->*m_drawFunc)();
}

void ClearScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &ClearScene::NormalUpdate;
		m_drawFunc = &ClearScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void ClearScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &ClearScene::FadeOutUpdate;
		m_drawFunc = &ClearScene::FadeDraw;
		return; // 念のため処理を抜ける
	}
}

void ClearScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void ClearScene::NormalDraw()
{
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 30, L"Clear!", 0xffffff);
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"OKボタンでタイトルへ", 0xffffff);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 40);
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + 100, L"(本来は1画面マップへ遷移します)", 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: NormalDraw", 0xffffff);
#endif
}

void ClearScene::FadeDraw()
{
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 30, L"Clear!", 0xffffff);
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"OKボタンでタイトルへ", 0xffffff);
	
	

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: FadeDraw", 0xffffff);
#endif
}
