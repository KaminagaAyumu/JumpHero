#include <memory>
#include "SelectScene.h"
#include "SceneController.h"
#include "GameScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&SelectScene::FadeInUpdate),
	m_drawFunc(&SelectScene::FadeDraw),
	m_fadeColor(0xffffff),
	m_selectIndex(0)
{
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
	if (input.IsTriggered("Down"))
	{
		if(m_selectIndex < 1)
		{
			m_selectIndex++;
		}
	}
	if (input.IsTriggered("Up"))
	{
		if(m_selectIndex > 0)
		{
			m_selectIndex--;
		}
	}

}

void SelectScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void SelectScene::NormalDraw()
{
#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: NormalDraw", 0xffffff);
#endif
}

void SelectScene::FadeDraw()
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: FadeDraw", 0xffffff);
#endif
}