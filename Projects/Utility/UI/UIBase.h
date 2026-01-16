#pragma once

/// <summary>
/// UI関連の基底クラス
/// </summary>
class UIBase
{
public:

	enum class LifeTime
	{
		Scene, // シーンが遷移したら消える
		Grobal // 明示的に消すまで消去されない
	};
	UIBase();
	virtual ~UIBase() = default;
	virtual void Update()abstract;
	virtual void Draw()const abstract;
	/// <summary>
	/// UIが存在しているかの判定を行う
	/// </summary>
	/// <returns>true : 存在している false : 存在しない</returns>
	virtual bool IsAlive()const { return true; }

	/// <summary>
	/// 生存時間を明示的に消すまで消さないようにする
	/// </summary>
	void KeepAcrossScenes() { m_lifeTime = LifeTime::Grobal; }

private:
	// UIの生存時間(デフォルトはシーン遷移で消える)
	LifeTime m_lifeTime;
};

