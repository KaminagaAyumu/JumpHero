#pragma once

class EffekseerEffect;

/// <summary>
/// ゲーム内のエフェクトを管理するクラス
/// </summary>
class EffectManager
{
public:
	EffectManager();
	virtual ~EffectManager();

	void Update();

	void Draw();

	void CreateEffekseerEffect(Types::EffectType type, const Position2& pos);

private:
	std::vector<int> m_effectHandles;
	std::list<std::shared_ptr<EffekseerEffect>> m_effekseerEffects;

};

