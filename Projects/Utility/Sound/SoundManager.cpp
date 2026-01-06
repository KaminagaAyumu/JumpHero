#include <unordered_map>
#include <string>
#include "SoundManager.h"
#include <algorithm>
#include "DxLib.h"

namespace
{
	constexpr float kMaxMasterVolume = 1.0f;
}

SoundManager::SoundManager()
{
	m_masterVolume = kMaxMasterVolume;

	m_busVolume = 
	{
		{SoundBus::BGM, 1.0f},
		{SoundBus::SE, 1.0f}
	};
}

SoundManager::~SoundManager()
{
}

void SoundManager::Update()
{

}

void SoundManager::SetMasterVolume(float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_masterVolume = std::clamp(volume, 0.0f, kMaxMasterVolume);
}

bool SoundManager::LoadSoundClip(const std::string& soundID, const std::wstring& path, SoundBus bus, float volume, bool isLoop)
{
	// IDが既にロードされていたらこの時点でtrueを返す
	if (m_soundClips.count(soundID)) return true;
	// パスからサウンドデータをロード
	int handle = LoadSoundMem(path.c_str());
	// ハンドルの取得に失敗したらfalseを返す
	if (handle == -1) return false;
	// サウンドのコンテナに追加
	m_soundClips[soundID] = SoundClip{ handle, bus, volume, isLoop };
	// ここまで来たらロードできているのでtrueを返す
	return true;
}

void SoundManager::Play(const std::string& soundID, float volume, bool restart)
{
}

void SoundManager::ApplyVolumeToHandle(const SoundClip& clip, float volume) const
{
}
