#include "Application.h"
#include "Input.h"
#include "Sound/SoundManager.h"
#include "../Scene/SceneController.h"
#include "../Scene/TitleScene.h"
#include "Game.h"
#include "DxLib.h"

namespace
{
	constexpr int	kOneFrameNanoSec = 16667; // 1フレームのナノ秒(60FPS)

	const wchar_t* kFontPath = L"data/toroman.ttf"; // プロジェクト内にあるフォントデータのパス
}

Application::Application()
{
	m_soundManager = std::make_shared<SoundManager>();
	// フォントデータをプロジェクトから読み込んで追加する(このプロジェクトの起動時にしか使えない)
	AddFontResourceExW(kFontPath, FR_PRIVATE, nullptr);
}

Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	// ウインドウモード設定
	ChangeWindowMode(true);
	// ウインドウのタイトル変更
	SetMainWindowText(L"JumpHero");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);

	return true;
}

void Application::Run()
{

	Input input;
	SceneController controller;

	// 最初のシーンをタイトルシーンに設定
	controller.ChangeScene(std::make_shared<TitleScene>(controller));
	while (ProcessMessage() != -1)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();


		// ここにゲームの処理を書く
		input.Update(); // 入力情報の更新

		m_soundManager->Update(); // サウンドマネージャーの更新処理

		controller.Update(input); // シーンの更新処理
		controller.Draw(); // シーンの描画処理


		// escキーを押したらゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// 描画した内容を画面に反映する
		ScreenFlip();

		// フレームレート60に固定
		while (GetNowHiPerformanceCount() - start < kOneFrameNanoSec)
		{

		}
	}

}

void Application::Terminate()
{
	// 追加したフォントデータを明示的に削除する
	RemoveFontResourceExW(kFontPath, FR_PRIVATE, nullptr);
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
