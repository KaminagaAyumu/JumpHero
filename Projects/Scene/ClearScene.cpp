#include <memory>
#include "ClearScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../Game/GameManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UISelectList.h"
#include "../Utility/UI/UIFormatText.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "DxLib.h"


namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr float kScoreAddRate = 0.15f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kClearDispMargin = 50;
	constexpr int kScoreDispMargin = -60;

	constexpr int kStageMaxNum = 2; // ステージの合計数

	const Size kNormalListSize = { 200,300 };
	const Position2 kNormalListPos = { Game::kScreenWidth / 2, 750 };

	const Size kLastStageListSize = { 200,200 };
	const Position2 kLastStageListPos = { Game::kScreenWidth / 2, 500 };

}

ClearScene::ClearScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo) :
	SceneBase(controller),
	m_pGameManager(gameManager),
	m_updateFunc(&ClearScene::FadeInUpdate),
	m_drawFunc(&ClearScene::FadeDraw),
	m_fadeColor(0xffffff),
	m_resultScore(0),
	m_gameScore(0)
{
	m_frameCount = kFadeInterval;

	m_stageNo = stageNo;

	m_pUIManager = std::make_shared<UIManager>();

	// ステージ番号をチェック
	// 最後のステージだった場合は次のステージへ遷移しないようにする
	if (m_stageNo >= kStageMaxNum)
	{
		m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kLastStageListSize, kLastStageListPos);
		auto list = m_pSelectList.lock();
		list->AddOption("ステージセレクトへ", [this]()
		{
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		});
		list->AddOption("タイトルへ", [this]()
			{
					m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			});
	}
	else
	{
		m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kNormalListSize, kNormalListPos);
		auto list = m_pSelectList.lock();
		// 次のステージへ遷移するリストを追加
		list->AddOption("次のステージへ", [this]()
			{
				m_stageNo++; // 次のステージへ遷移するように加算
				m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_stageNo));
			});
		list->AddOption("ステージセレクトへ", [this]()
		{
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		});
		list->AddOption("タイトルへ", [this]()
			{
					m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			});
	}	

	m_pScoreText = m_pUIManager->CreateFormatText(Types::FontType::Small, "", { Game::kScreenWidth / 2 + kScoreDispMargin, Game::kScreenHeight / 2 + kScoreDispMargin });
	auto score = m_pScoreText.lock();
	score->SetProvider([this]() 
		{
			return std::string("スコア:") + std::to_string(GetScore());
		});

	m_pClearText = m_pUIManager->CreateText(Types::FontType::Large, "クリア！", { Game::kScreenWidth / 2, kClearDispMargin });
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
	m_pUIManager->Update();

	m_gameScore = m_pGameManager->GetCurrentScore();

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

	if (m_resultScore != m_gameScore)
	{
		return;
	}

	if (input.IsTriggered("Up"))
	{
		auto list = m_pSelectList.lock();
		list->MoveCursor(-1);
	}
	if (input.IsTriggered("Down"))
	{
		auto list = m_pSelectList.lock();
		list->MoveCursor(1);
	}

	// 決定ボタンを押したとき
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
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void ClearScene::NormalDraw()
{
	m_pUIManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: NormalDraw", 0xffffff);
#endif
}

void ClearScene::FadeDraw()
{
	m_pUIManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: FadeDraw", 0xffffff);
#endif
}
