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
	bool isLoop = false; // ループするかどうか
};

/// <summary>
/// サウンドを管理するクラス
/// </summary>
class SoundManager
{
public:

	SoundManager();
	virtual ~SoundManager();

	/// <summary>
	/// 更新処理を行う
	/// </summary>
	void Update();

	/// <summary>
	/// サウンド全体の音量を設定する
	/// </summary>
	/// <param name="volume">設定する音量</param>
	void SetMasterVolume(float volume);

	/// <summary>
	/// サウンドをロードする
	/// </summary>
	/// <param name="soundID">サウンドクリップのID</param>
	/// <param name="path">ハンドルのパス</param>
	/// <param name="bus">サウンドの種類</param>
	/// <param name="volume">サウンドの初期音量</param>
	/// <param name="isLoop">ループするかどうか</param>
	/// <returns>true : ロード成功 false : ロード失敗</returns>
	bool LoadSoundClip(const std::string& soundID, const std::wstring& path, SoundBus bus, float volume, bool isLoop);

	void Play(const std::string& soundID, float volume, bool restart);

private:

	// サウンドクリップのコンテナ
	std::unordered_map<std::string, SoundClip> m_soundClips;
	// サウンド全体のボリューム
	float m_masterVolume;
	// サウンドの種類ごとのボリューム
	std::unordered_map<SoundBus, float> m_busVolume;

};

