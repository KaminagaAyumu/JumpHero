#pragma once
#include "Geometry.h"
#include <memory>

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

	void Draw(std::shared_ptr<Camera> camera);

private:
	int m_bgHandle;
	Position2 m_pos;


};

