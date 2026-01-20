#pragma once

/// <summary>
/// Effekseerのエフェクトを生成するクラス
/// </summary>
class EffekseerEffect
{
public:
	EffekseerEffect(int handle, const Position2& pos);
	~EffekseerEffect();



private:
	int m_effect; // 実際に表示するエフェクト

};

