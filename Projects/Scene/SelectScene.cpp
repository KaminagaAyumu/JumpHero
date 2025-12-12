#include <memory>
#include "SelectScene.h"
#include "SceneController.h"
#include "GameScene.h"
#include "TutorialScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kMinSelectIndex = 0; // 選択肢の最小インデックス
	constexpr int kSelectOptionNum = 3; // 選択肢の数

}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&SelectScene::FadeInUpdate),
	m_drawFunc(&SelectScene::FadeDraw),
	m_fadeColor(0x000000),
	m_selectIndex(kMinSelectIndex)
{
	m_frameCount = kFadeInterval;
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
		if(m_selectIndex < kSelectOptionNum)
		{
			m_selectIndex++;
		}
	}
	if (input.IsTriggered("Up"))
	{
		if(m_selectIndex > kMinSelectIndex)
		{
			m_selectIndex--;
		}
	}

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &SelectScene::FadeOutUpdate;
		m_drawFunc = &SelectScene::FadeDraw;
		return; // 念のため処理を抜ける
	}

}

void SelectScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		if (m_selectIndex == 0)
		{
			// チュートリアルシーンに遷移
			m_controller.ChangeScene(std::make_shared<TutorialScene>(m_controller));
		}
		else
		{
			// ゲームシーンに遷移
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_selectIndex));
		}
		return; // 念のため処理を抜ける
	}
}

void SelectScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);
	DrawBox(20, m_selectIndex * 50 + 100, 300, m_selectIndex * 50 + 150, 0xff5500, TRUE);

#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: NormalDraw", 0xffffff);
	if (m_selectIndex > 0)
	{
		DrawFormatString(0, 30, 0xffffff, L"stage : %d", m_selectIndex);
	}
	else
	{
		DrawString(0, 30, L"stage : tutorial",0xffffff);
	}
#endif
}

void SelectScene::FadeDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene: FadeDraw", 0xffffff);
#endif
}