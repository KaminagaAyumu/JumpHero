#pragma once
#include "../../Utility/Animation.h"

/// <summary>
/// エフェクト管理クラスの基底クラス
/// </summary>
class EffectBase
{
public:
	EffectBase();
	virtual ~EffectBase() = default;

	virtual void Update()abstract;
	virtual void Draw()abstract;

	bool IsAlive()const { return m_isAlive; }

protected:
	int m_handle; // 画像ハンドル

	bool m_isAlive; // 存在フラグ

	Animation m_animation; // エフェクトを動かすためのアニメーションクラス

};

