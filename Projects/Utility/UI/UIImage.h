#pragma once
#include "UIBase.h"
#include "../Geometry.h"

/// <summary>
/// 画像を表示するクラス
/// </summary>
class UIImage : public UIBase
{
public:
	UIImage();
	virtual ~UIImage();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">画像のハンドル</param>
	/// <param name="size">表示したいサイズ</param>
	/// <param name="pos">表示左上座標</param>
	void Init(int handle, const Size& size, const Position2& pos);

	void Update()override;

	void Draw()const override;

	bool IsAlive()const override;

private:
	int m_handle; // 画像ハンドル
	Size m_size; // 画像のサイズ
	Position2 m_pos; // 画像を表示する座標

};

