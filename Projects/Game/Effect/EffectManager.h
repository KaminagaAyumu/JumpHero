#pragma once


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

private:
	std::vector<int> m_effectHandles;



};

