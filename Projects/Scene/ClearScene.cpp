#include <memory>
#include "ClearScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "../Game/GameManager.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "DxLib.h"


namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr float kScoreAddRate = 0.15f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
}

ClearScene::ClearScene(SceneController& controller, std::shared_ptr<GameManager> gameManager) :
	SceneBase(controller),
	m_pGameManager(gameManager),
	m_updateFunc(&ClearScene::FadeInUpdate),
	m_drawFunc(&ClearScene::FadeDraw),
	m_fadeColor(0xffffff),
	m_resultScore(0),
	m_gameScore(0)
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
	m_gameScore = m_pGameManager->GetScore();
	// スコアの更新処理
	if (m_resultScore < m_gameScore) // リザルト表示スコアがゲームで獲得したスコアより小さい場合
	{
		int add = static_cast<int>((m_gameScore - m_resultScore) * kScoreAddRate); // ゲーム獲得スコアとの差分を計算
		if (add <= kScoreThreshold) // 加算の値が閾値を超えたら
		{
			m_resultScore = m_gameScore; // スコアを現在のスコアに合わせる
		}
		else
		{
			m_resultScore += add; // スコアを更新
		}
		if (m_resultScore > m_gameScore) // スコアが現在のスコアを超えた場合
		{
			m_resultScore = m_gameScore; // スコアを現在のスコアに合わせる
		}
	}
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
	DrawFormatString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 60, 0xffffff, L"score : %d", m_resultScore);

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
	DrawFormatString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 60, 0xffffff, L"score : %d", m_resultScore);


	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: FadeDraw", 0xffffff);
#endif
}
