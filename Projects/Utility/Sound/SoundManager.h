#pragma once

/// <summary>
/// サウンドの種類を持つ列挙体
/// </summary>
enum class SoundBus
{
	BGM, // BGM
	SE, // SE
	Master, // マスター(実体は持たないようにする)
};

/// <summary>
/// サウンドの状態を持つ構造体
/// ハンドル、種類、初期音量、ループの有無を持つ
/// </summary>
struct SoundClip
{
	int handle = -1; // ハンドル(初期状態は-1)
	SoundBus bus = SoundBus::Master; // サウンドの種類(初期状態はMaster)
	float defaultRate = 1.0f; // 初期音量の大きさ(最大1.0)
	bool loop = false; // ループするかどうか
};

/// <summary>
/// サウンドを管理するクラス
/// </summary>
class SoundManager
{
public:

	SoundManager();
	virtual ~SoundManager();


private:

	// サウンドクリップのコンテナ
	std::unordered_map<std::string, SoundClip> m_soundClips;
	// サウンド全体のボリューム
	float m_masterVolume;
	// サウンドの種類ごとのボリューム
	std::unordered_map<SoundBus, float> m_busVolume;

};

