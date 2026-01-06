#pragma once
#include <unordered_map>

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

struct BGMTrack
{
	std::string soundID; // サウンドID
	int handle = -1; // ハンドル(初期状態は-1)
	float volume; // 音量
	bool isActive; // 再生中かどうか
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
	/// サウンドのバスごとの音量を設定する
	/// </summary>
	/// <param name="bus">サウンドの種類</param>
	/// <param name="volume">設定する音量</param>
	void SetBusVolume(SoundBus bus, float volume);

	/// <summary>
	/// 特定の種類のサウンドの音量を取得する
	/// </summary>
	/// <param name="bus">サウンドの種類</param>
	/// <returns>サウンドの音量</returns>
	float GetBusVolume(SoundBus bus) const;

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

	/// <summary>
	/// サウンドを再生する
	/// </summary>
	/// <param name="soundID">サウンドID</param>
	/// <param name="volume">サウンドの大きさ</param>
	/// <param name="restart">再生を上書きするかどうか</param>
	void Play(const std::string& soundID, float volume, bool restart);

	/// <summary>
	/// BGMを再生する(フェードインしながら再生可能)
	/// </summary>
	/// <param name="soundID">サウンドID</param>
	/// <param name="fadeTime">フェードの時間</param>
	void PlayBGM(const std::string& soundID, float fadeTime);

	/// <summary>
	/// クロスフェードでBGMを切り替える
	/// </summary>
	/// <param name="soundID">次のBGMのID</param>
	/// <param name="fadeTime">フェードインの時間</param>
	void CrossFadeBGM(const std::string& soundID, float fadeTime);

	/// <summary>
	/// サウンドを止める
	/// </summary>
	/// <param name="soundID">サウンドID</param>
	void Stop(const std::string& soundID);

private:

	// サウンドクリップのコンテナ
	std::unordered_map<std::string, SoundClip> m_soundClips;
	// サウンド全体のボリューム
	float m_masterVolume;
	// サウンドの種類ごとのボリューム
	std::unordered_map<SoundBus, float> m_busVolume;

	// BGM関連
	BGMTrack m_bgmA;
	BGMTrack m_bgmB;

	/// <summary>
	/// BGMのフェーズを表す列挙体
	/// </summary>
	enum class BGMPhase
	{
		Idle, // 待機中
		CrossFading // クロスフェード中
	};
	BGMPhase m_bgmPhase; // 現在のBGMフェーズ
	float m_bgmFadeTime; // フェードにかける時間
	float m_bgmFadeTimer; // フェードの経過時間

	/// <summary>
	/// マスターボリュームを考慮した音量をサウンドクリップに設定する
	/// </summary>
	/// <param name="clip">サウンドクリップ</param>
	/// <param name="volume">音量</param>
	void ApplyVolumeToHandle(const SoundClip& clip, float volume) const;
	
	/// <summary>
	/// 割合からDxLibの音量(0~255)に変換
	/// </summary>
	/// <param name="rate">音量の割合</param>
	/// <returns>DxLibの音量</returns>
	int ToDxLibVolume(float rate) const;

	/// <summary>
	/// BGMを指定したトラックで再生する
	/// </summary>
	/// <param name="track">BGMトラック(AorB)</param>
	/// <param name="soundID">サウンドのID</param>
	/// <param name="volume">BGMの音量</param>
	void StartBGMOnTrack(BGMTrack& track, const std::string& soundID, float volume);

	/// <summary>
	/// BGMトラックの再生を停止する
	/// </summary>
	/// <param name="track">BGMトラック(AorB)</param>
	void StopBGMTrack(BGMTrack& track);
};

