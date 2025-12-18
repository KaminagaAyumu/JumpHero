#include "PauseScene.h"
#include "SceneController.h" 
#include "TitleScene.h"
#include "SelectScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"

#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kMinSelectIndex = 0; // 選択肢の最小インデックス
	constexpr int kSelectOptionNum = 2; // 選択肢の数
}

PauseScene::PauseScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&PauseScene::FadeInUpdate),
	m_drawFunc(&PauseScene::FadeDraw),
	m_fadeColor(0x000000),
	m_selectIndex(kMinSelectIndex)
{
	m_frameCount = kFadeInterval;
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
	if (input.IsTriggered("Down"))
	{
		if (m_selectIndex < kSelectOptionNum)
		{
			m_selectIndex++;
		}
	}
	if (input.IsTriggered("Up"))
	{
		if (m_selectIndex > kMinSelectIndex)
		{
			m_selectIndex--;
		}
	}

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &PauseScene::FadeOutUpdate;
		m_drawFunc = &PauseScene::FadeDraw;
		return; // 念のため処理を抜ける
	}

}

void PauseScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		if (m_selectIndex == 0)
		{
			// ポーズしたシーンに戻る
			m_controller.PopScene();
			
		}
		else if (m_selectIndex == 1)
		{
			// ステージセレクトシーンに遷移
			m_controller.ResetScene(std::make_shared<SelectScene>(m_controller));
		}
		else
		{
			// ゲームシーンに遷移
			m_controller.ResetScene(std::make_shared<TitleScene>(m_controller));
		}
		return; // 念のため処理を抜ける
	}
}

void PauseScene::NormalDraw()
{

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111166, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(20, m_selectIndex * 50 + 100, 300, m_selectIndex * 50 + 150, 0xff5500, TRUE);

#ifdef _DEBUG
	DrawString(0, 0, L"PauseScene: NormalDraw", 0xffffff);
	if (m_selectIndex == 0)
	{
		DrawString(0, 30, L"ポーズを解除", 0xffffff);
	}
	else if(m_selectIndex == 1)
	{
		DrawFormatString(0, 30, 0xffffff, L"ステージセレクトへ戻る", m_selectIndex);
	}
	else
	{
		DrawString(0, 30, L"タイトルへ戻る", 0xffffff);
	}
#endif
}

void PauseScene::FadeDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111166, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"PauseScene: FadeDraw", 0xffffff);
#endif
}
