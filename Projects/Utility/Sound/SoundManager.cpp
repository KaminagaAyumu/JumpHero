#include <string>
#include "SoundManager.h"
#include <algorithm>
#include "DxLib.h"

namespace
{
	constexpr float kMaxMasterVolumeRate = 1.0f; // マスターボリュームの最大音量の割合

	constexpr int kMaxVolume = 255; // DxLibにおける最大音量の大きさ 
}

SoundManager::SoundManager()
{
	m_masterVolume = kMaxMasterVolumeRate;

	m_busVolume = 
	{
		{SoundBus::BGM, 1.0f},
		{SoundBus::SE, 1.0f}
	};
}

SoundManager::~SoundManager()
{
	for(auto& pair : m_soundClips)
	{
		// サウンドメモリの解放
		DeleteSoundMem(pair.second.handle);
	}
	// サウンドクリップのコンテナをクリア
	m_soundClips.clear();
}

void SoundManager::Update()
{

}

void SoundManager::SetMasterVolume(float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_masterVolume = std::clamp(volume, 0.0f, kMaxMasterVolumeRate);
}

float SoundManager::GetBusVolume(SoundBus bus) const
{
	// サウンドの種類があるかどうかを判定
	auto it = m_busVolume.find(bus);
	// サウンドの種類がある場合
	if (it != m_busVolume.end())
	{
		return it->second; // サウンドの音量を返す
	}
	else
	{
		return kMaxMasterVolumeRate; // デフォルトの音量を返す(とりあえず1.0にしておく)
	}

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
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが存在するかを判定
	if (it == m_soundClips.end()) return;
	// サウンドクリップを取得
	const SoundClip& clip = it->second;
	// ボリュームを適用
	ApplyVolumeToHandle(clip, volume);
	// ループ設定を適用
	int playType = clip.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_NORMAL;
	// 再び鳴らすかどうかを判定してフラグを設定
	if (!restart)
	{
		// 既に再生中の場合は何もしない
		if (CheckSoundMem(clip.handle) == 1) return;
	}
	// サウンドを再生
	PlaySoundMem(clip.handle, playType);
}

void SoundManager::Stop(const std::string& soundID)
{
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが存在するかを判定
	if (it == m_soundClips.end()) return;
	// サウンドを止める
	StopSoundMem(it->second.handle);
}

void SoundManager::ApplyVolumeToHandle(const SoundClip& clip, float volume) const
{
	// サウンドの音量を取得
	float busVolume = GetBusVolume(clip.bus);
	// 音量の割合を計算
	float rate = m_masterVolume * busVolume * clip.defaultRate * volume;
	// 最終的なボリュームを取得
	int vol = ToDxLibVolume(rate);
	// ハンドルに音量を設定
	ChangeVolumeSoundMem(vol, clip.handle);
}

int SoundManager::ToDxLibVolume(float rate) const
{
	// 割合から音量を設定
	int volume = rate * kMaxVolume;
	// 0~255の間に収まるようにする
	return std::clamp(volume, 0, kMaxVolume);
}
