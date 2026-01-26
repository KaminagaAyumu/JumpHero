#pragma once
#include "Geometry.h"

// プロトタイプ宣言
class Camera;

/// <summary>
/// 背景画像を表示するためのクラス
/// </summary>
class Bg
{
public:
	Bg();
	virtual ~Bg();

	void Init();

	void Update();

	void Draw();

	void Draw(std::weak_ptr<Camera> camera);

private:
	int m_bgHandle;
	Position2 m_pos;
	std::vector<int> m_bgHandles;

	/// <summary>
	/// 斜めにループして背景を表示する更新処理
	/// </summary>
	void LoopUpdate();

	/// <summary>
	/// 斜めにループして背景を表示する描画処理
	/// </summary>
	void LoopDraw();

};

