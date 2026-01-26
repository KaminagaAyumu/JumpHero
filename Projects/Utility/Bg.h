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

	using UpdateFunc_t = std::function<void()>;
	UpdateFunc_t m_update;

	using DrawFunc_t = std::function<void(std::shared_ptr<Camera>)>;
	DrawFunc_t m_draw;

	/// <summary>
	/// 斜めにループして背景を表示する更新処理
	/// </summary>
	void LoopUpdate();

	/// <summary>
	/// Xのみ無限にスクロールする更新処理
	/// </summary>
	void ScrollXUpdate();

	/// <summary>
	/// 斜めにループして背景を表示する描画処理
	/// </summary>
	void LoopDraw(std::shared_ptr<Camera>);

	/// <summary>
	/// カメラのX座標のスクロールを行う描画処理
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void ScrollXDraw(std::shared_ptr<Camera> camera);

};

