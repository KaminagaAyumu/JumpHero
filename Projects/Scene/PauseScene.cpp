#include "PauseScene.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
}

PauseScene::PauseScene(SceneController& controller) :
	SceneBase(controller)
	//m_updateFunc(&SelectScene::FadeInUpdate),
	//m_drawFunc(&SelectScene::FadeDraw),
	//m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;
}

PauseScene::~PauseScene()
{
}

void PauseScene::Update(Input& input)
{
	//(this->*m_updateFunc)(input);
}

void PauseScene::Draw()
{
	//(this->*m_drawFunc)();
}
