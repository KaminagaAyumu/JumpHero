#include "MiniGameScene.h"
#include "SceneController.h"
#include "ClearScene.h"
#include "PauseScene.h"
#include "../Game/GameManager.h"
#include "../Game/CollisionManager.h"
#include "../Game/Actor.h"
#include "../Game/Effect/EffectManager.h"
#include "../Game/Event/EventManager.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Utility/Game.h"
#include "../Utility/Input.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kDescriptionInterval = 10; // 説明の表示を行う時間
	constexpr int kDescriptionBorderMargin = 50; // 説明表示の上下の余白

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kHeaderMargin = 60; // ヘッダー(見出し)の余白

}

MiniGameScene::MiniGameScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo) :
	SceneBase(controller),
	m_fadeColor(0xffffff),
	m_updateFunc(&MiniGameScene::FadeInUpdate),
	m_drawFunc(&MiniGameScene::FadeDraw),
	m_pGameManager(gameManager)
{
	m_frameCount = kFadeInterval;

	m_bg = std::make_shared<Bg>();
	m_bg->Init();
	// マップの初期化方法模索中
	if (stageNo == 0)
	{
		printfDx(L"チュートリアル用のマップです\n");
	}
	m_pMap = std::make_shared<Map>(stageNo,true);
	m_pMap->Init();

	m_pCamera = std::make_shared<Camera>(m_pMap->GetMapSize());

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pEffectManager->SetCamera(m_pCamera);

	m_pGameManager->MiniGameInit(m_pMap,m_pCamera,m_pEffectManager);

	m_pCamera->SetTargetProvider([this]() {return m_pGameManager->GetPlayerPos(); });

	m_pCollisionManager = std::make_unique<CollisionManager>();

	// イベントセンサーのポインタを取得
	m_pEventSensors = std::make_shared<EventSensors>();
	m_pEventControls = std::make_shared<EventControls>();

	// イベントの内容を格納
	SetEventFunc();

	m_pEventManager = std::make_unique<EventManager>();
	if (!m_pEventManager->LoadCommonEventData(stageNo, true) || !m_pEventManager->LoadEventData(stageNo, true))
	{
		assert(false && "イベントデータの読み込みに失敗しました");
	}

	// イベントのデータをイベントマネージャーに渡す
	m_pEventManager->SetEvents(m_pEventControls, m_pEventSensors);

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
		m_updateFunc = &MiniGameScene::NormalUpdate;
		m_drawFunc = &MiniGameScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::NormalUpdate(Input& input)
{
	// ポーズボタンが押されたら
	if (input.IsTriggered("Pause"))
	{
		// ポーズシーンをプッシュする(このシーンに戻ることも可能にする)
		m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
		return;
	}

	// カメラの更新
	m_pCamera->Update();

	// エフェクトマネージャーを更新
	m_pEffectManager->Update();

	// ゲームマネージャーの更新
	m_pGameManager->Update(input);

	// 次のフレームのためにゲーム内オブジェクトを更新
	m_pActors.clear(); // オブジェクトをリセット
	m_pActors.reserve(m_pGameManager->GetActorNum()); // 現在のオブジェクトの総数分要素を確保 
	m_pGameManager->PushActors(m_pActors); // ゲームマネージャーからオブジェクトを受け取る

	// 当たり判定を行う
	if (!m_pGameManager->IsSkipCollision()) // 当たり判定をスキップしない場合
	{
		m_pCollisionManager->CheckCollision(m_pActors);
	}

	// 背景とマップの更新(現在何もやっていない)
	m_bg->Update();
	m_pMap->Update();

	if (m_pGameManager->IsClear()) // ゲームマネージャーがクリアと判定したら
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
	m_bg->Draw(m_pCamera.get());
	m_pMap->Draw(m_pCamera.get());

	for (auto& actor : m_pActors)
	{
		// Actorの描画関数を呼ぶためにlockする
		if (auto pActor = actor.lock())
		{
			pActor->Draw();
		}
	}

	m_pGameManager->Draw();

	m_pEffectManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: NormalDraw", 0xffffff);
#endif
}

void MiniGameScene::FadeDraw()
{
	m_bg->Draw(m_pCamera.get());
	m_pMap->Draw(m_pCamera.get());

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

void MiniGameScene::GetBalloonNum() const
{
}

void MiniGameScene::SetEventFunc()
{
}
