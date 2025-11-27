#include "TutorialScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間
}

TutorialScene::TutorialScene(SceneController& controller) : 
	SceneBase(controller),
	m_updateFunc(&TutorialScene::FadeInUpdate),
	m_drawFunc(&TutorialScene::FadeDraw)
{
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
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void TutorialScene::NormalDraw()
{
#ifdef _DEBUG
	DrawString(0, 0, L"TutorialScene: NormalDraw", 0xffffff);
#endif
}
void TutorialScene::FadeDraw()
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"TutorialScene: FadeDraw", 0xffffff);
#endif
}