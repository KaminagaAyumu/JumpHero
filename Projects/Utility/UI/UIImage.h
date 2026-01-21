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

	void Init(int handle, const Size& size, const Position2& pos);

	void Update()override;

	void Draw()const override;

	bool IsAlive()const override;

private:
	int m_handle; // 画像ハンドル
	Size m_size; // 画像のサイズ
	Position2 m_pos; // 画像を表示する座標

};

