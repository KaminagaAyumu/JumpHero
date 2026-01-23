#pragma once
#include "../../Utility/Geometry.h"
#include "../../Utility/GameType.h"

class EffekseerResourceManager;
class EffekseerEffect;
class Camera;

/// <summary>
/// ゲーム内のエフェクトを管理するクラス
/// </summary>
class EffectManager
{
public:
	EffectManager(std::shared_ptr<EffekseerResourceManager> effectResourceManager);
	virtual ~EffectManager();

	void Update();

	void Draw();

	void CreateEffekseerEffect(Types::EffectType type, const Position2& pos);

	/// <summary>
	/// カメラをセットする
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void SetCamera(std::weak_ptr<Camera> camera) { m_pCamera = camera; };

private:
	std::vector<int> m_effectHandles;
	std::vector<int> m_effekseerResourceHandles;
	std::list<std::shared_ptr<EffekseerEffect>> m_effekseerEffects;
	std::shared_ptr<EffekseerResourceManager> m_pEffekseerResourceManager;
	std::weak_ptr<Camera> m_pCamera; // 画面がスクロールする際に使う

};

