#pragma once
#include "SceneBase.h"

/// <summary>
/// シーンを一時停止するクラス
/// </summary>
class PauseScene : public SceneBase
{
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	PauseScene(SceneController& controller);
	virtual ~PauseScene();
	/// <summary>
	/// シーンの更新処理
	/// </summary>
	/// <param name="input">入力情報を取得するクラス</param>
	void Update(Input& input) override;
	/// <summary>
	/// シーンの描画処理
	/// </summary>
	void Draw() override;
};

