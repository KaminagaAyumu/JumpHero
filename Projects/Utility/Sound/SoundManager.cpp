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
		// デフォルトの音量を設定
		{SoundBus::BGM, 0.5f},
		{SoundBus::SE, 0.75f}
	};

	m_bgmPhase = BGMPhase::Idle; // 現在のBGMフェーズ
	m_bgmFadeTime = 0.0f; // フェードにかける時間
	m_bgmFadeTimer = 0.0f; // フェードの経過時間
}

SoundManager::~SoundManager()
{
	for (auto& pair : m_soundClips)
	{
		// サウンドメモリの解放
		DeleteSoundMem(pair.second.handle);
	}
	// サウンドクリップのコンテナをクリア
	m_soundClips.clear();
}

void SoundManager::Update()
{
	if (m_bgmPhase != BGMPhase::CrossFading || !m_crossBGMInfo.isActive)
	{
		return;
	}

	auto& info = m_crossBGMInfo;

	// フェード値が0以上の場合
	if (info.durationSec <= 0.0f)
	{
		if (info.fadeOutTrack)
		{
			info.fadeOutTrack->volume = info.outEnd;
			ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume *
				GetBusVolume(m_soundClips[info.fadeOutTrack->soundID].bus) *
				m_soundClips[info.fadeOutTrack->soundID].defaultRate *
				info.fadeOutTrack->volume),
				info.fadeOutTrack->handle);
			StopBGMTrack(*info.fadeOutTrack);
		}
		if (info.fadeInTrack)
		{
			info.fadeInTrack->volume = info.inEnd;
			ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume *
				GetBusVolume(m_soundClips[info.fadeInTrack->soundID].bus) *
				m_soundClips[info.fadeInTrack->soundID].defaultRate *
				info.fadeInTrack->volume),
				info.fadeInTrack->handle);
		}
		// クロスフェード情報を初期化
		info = {};
		m_bgmPhase = BGMPhase::Idle;
		return;
	}

	info.elapsedSec++;
	float t = std::clamp(info.elapsedSec / info.durationSec, 0.0f, 1.0f);

	// 音量を適用
	if (info.fadeOutTrack)
	{
		info.fadeOutTrack->volume = info.outStart + (info.outEnd - info.outStart) * t;
		const auto& clip = m_soundClips[info.fadeOutTrack->soundID];
		ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * info.fadeOutTrack->volume), info.fadeOutTrack->handle);
	}

	if (info.fadeInTrack)
	{
		info.fadeInTrack->volume = info.inStart + (info.inEnd - info.inStart) * t;
		const auto& clip = m_soundClips[info.fadeInTrack->soundID];
		ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * info.fadeInTrack->volume), info.fadeInTrack->handle);
	}

	if (t >= 1.0f)
	{
		if (info.fadeOutTrack)
		{
			StopBGMTrack(*info.fadeOutTrack);
		}
		info = {};
		m_bgmPhase = BGMPhase::Idle;
	}

}

void SoundManager::SetMasterVolume(float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_masterVolume = std::clamp(volume, 0.0f, kMaxMasterVolumeRate);
}

void SoundManager::SetBusVolume(SoundBus bus, float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_busVolume[bus] = std::clamp(volume, 0.0f, kMaxMasterVolumeRate);
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
	int playType = clip.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	// 再び鳴らすかどうかを判定してフラグを設定
	if (!restart)
	{
		// 既に再生中の場合は何もしない
		if (CheckSoundMem(clip.handle) == 1) return;
	}
	// サウンドを再生
	PlaySoundMem(clip.handle, playType);
}

void SoundManager::PlayBGM(const std::string& soundID, float fadeTime)
{
	// 現在のBGMを止めてから新しいBGMを再生する
	StopBGMTrack(m_bgmA);
	StopBGMTrack(m_bgmB);

	StartBGMOnTrack(m_bgmA, soundID, fadeTime > 0.0f ? 0.0f : 1.0f); // フェードインがある場合は最初は音量0で再生

	if (fadeTime > 0.0f)
	{
		m_bgmPhase = BGMPhase::CrossFading;
		m_bgmFadeTime = fadeTime;
		m_bgmFadeTimer = 0.0f;
	}
	else
	{
		m_bgmPhase = BGMPhase::Idle;
	}

}

void SoundManager::CrossFadeBGM(const std::string& soundID, float fadeTime)
{
	if (!m_bgmA.isActive && !m_bgmB.isActive)
	{
		// どちらのトラックも再生されていない場合は通常のBGM再生を行う
		PlayBGM(soundID, 0.0f);
		return;
	}

	// フェードアウト中のトラックとフェードイン中のトラックを切り替える
	BGMTrack* currentTrack = nullptr;
	BGMTrack* newTrack = nullptr;

	if (m_bgmA.isActive && m_bgmB.isActive)
	{
		currentTrack = (m_bgmA.volume >= m_bgmB.volume) ? &m_bgmA : &m_bgmB;
		newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;
	}
	else
	{
		currentTrack = (m_bgmA.isActive) ? &m_bgmA : &m_bgmB;
		newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;
	}

	// 新しいトラックでBGMを開始
	StopBGMTrack(*newTrack); // 念のため停止しておく
	StartBGMOnTrack(*newTrack, soundID, 0.0f); // 最初は音量0で再生

	m_crossBGMInfo.fadeOutTrack = currentTrack;
	m_crossBGMInfo.fadeInTrack = newTrack;
	m_crossBGMInfo.fadeTime = fadeTime;
	m_crossBGMInfo.fadeCount = 0.0f;
	m_crossBGMInfo.isActive = true;

	m_bgmPhase = BGMPhase::CrossFading;
}

void SoundManager::Stop(const std::string& soundID)
{
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが存在するかを判定
	if (it == m_soundClips.end()) return;
	// サウンドを止める
	StopSoundMem(it->second.handle);
}

void SoundManager::StopBGM(float fadeOutTime)
{
	if (fadeOutTime <= 0.0f)
	{
		StopBGMTrack(m_bgmA);
		StopBGMTrack(m_bgmB);
		m_bgmPhase = BGMPhase::Idle;
		return;
	}

	if (!m_bgmA.isActive && !m_bgmB.isActive)
	{
		// どちらのトラックも再生されていない場合は何もしない
		return;
	}

	// フェードアウト中のトラックとフェードイン中のトラックを切り替える
	BGMTrack* currentTrack = (m_bgmA.isActive) ? &m_bgmA : &m_bgmB;
	BGMTrack* newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;
	StopBGMTrack(*newTrack); // 念のため停止しておく
	newTrack->volume = 0.0f; // 音量0に設定
	newTrack->isActive = false; // 非アクティブに設定
	m_bgmPhase = BGMPhase::CrossFading;
	// フェード時間を設定
	m_bgmFadeTime = fadeOutTime;
	m_bgmFadeTimer = 0.0f;

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
	int volume = static_cast<int>(std::round(std::clamp(rate, 0.0f, 1.0f)) * kMaxVolume);
	// 0~255の間に収まるようにする
	return std::clamp(volume, 0, kMaxVolume);
}

void SoundManager::StartBGMOnTrack(BGMTrack& track, const std::string& soundID, float volume)
{
	auto& clip = m_soundClips[soundID];
	track.soundID = soundID;
	track.handle = clip.handle;
	track.volume = volume;
	track.isActive = true;
	ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * track.volume), track.handle);
	PlaySoundMem(track.handle, DX_PLAYTYPE_LOOP); // ループ前提で再生
}

void SoundManager::StopBGMTrack(BGMTrack& track)
{
	// トラックがアクティブかどうかを判定
	if (track.isActive)
	{
		StopSoundMem(track.handle);
	}
	track = BGMTrack{}; // トラック情報をリセット
}

