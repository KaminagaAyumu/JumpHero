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

	constexpr int kDescriptionInterval = 60; // 説明の表示を行う時間
	constexpr int kDescriptionBorderMargin = 50; // 説明表示の上下の余白

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
		m_frameCount = 0; // 説明で使うためカウンタの値をリセット
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::DescriptionUpdate(Input& input)
{
	m_frameCount++;
	if (IsEndDescription())
	{
		m_frameCount = kDescriptionInterval; // 枠表示用にカウンタを最大値に固定
		if (input.IsTriggered("OK"))
		{
			m_updateFunc = &MiniGameScene::NormalUpdate;
			m_drawFunc = &MiniGameScene::NormalDraw;
			m_frameCount = 0; // フェードアウト用にカウンタを0に設定
			return; // 念のため処理を抜ける
		}
	}
	
}

void MiniGameScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("OK"))
	{
		// シーンの終了へ
		m_fadeColor = 0xffffff; // 白でフェードアウト
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
	int ScreenCenterX = Game::kScreenWidth / 2;
	int ScreenCenterY = Game::kScreenHeight / 2;
	// 出現割合の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kDescriptionInterval);

	int frameHeight = (Game::kScreenHeight - kDescriptionBorderMargin) - ScreenCenterY;
	frameHeight = static_cast<int>(frameHeight * rate);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(kDescriptionBorderMargin, ScreenCenterY - frameHeight, Game::kScreenWidth - kDescriptionBorderMargin, ScreenCenterY + frameHeight, 0x440044, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"ゲームの説明", 0xffffff);
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

bool MiniGameScene::IsEndDescription() const
{
	// フレームカウンタが説明を表示する時間を超えていたらtrueを返す
	return m_frameCount >= kDescriptionInterval;
}
