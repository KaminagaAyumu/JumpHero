#pragma once

class Camera;

/// <summary>
/// Effekseerのエフェクトを生成するクラス
/// </summary>
class EffekseerEffect
{
public:
	EffekseerEffect(int handle, const Position2& pos);
	~EffekseerEffect();

	void Update();

	bool IsAlive()const;

	/// <summary>
	/// カメラをセットする
	/// </summary>
	/// <param name="camera"></param>
	void SetCamera(std::weak_ptr<Camera> camera) { m_pCamera = camera; }

private:
	int m_effect; // 実際に表示するエフェクト

	Position2 m_pos; // 表示する座標

	std::weak_ptr<Camera> m_pCamera; // カメラがある際に使う
};

