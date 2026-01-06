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

	m_bgmPhase = BGMPhase::Idle; // 現在のBGMフェーズ
	m_bgmFadeTime = 0.0f; // フェードにかける時間
	m_bgmFadeTimer = 0.0f; // フェードの経過時間
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
	if (m_bgmPhase == BGMPhase::CrossFading)
	{
		m_bgmFadeTimer++;
		float t = std::clamp(m_bgmFadeTimer / m_bgmFadeTime, 0.0f, 1.0f);

		// 現在のBGMトラックと新しいBGMトラックの音量を更新
		// AがアクティブかつBが非アクティブ、またはAの音量がBの音量以上の場合、Aがアクティブとみなす
		bool isActiveA = (m_bgmA.isActive);
	
		BGMTrack& currentTrack = isActiveA ? m_bgmA : m_bgmB;
		BGMTrack& newTrack = isActiveA ? m_bgmB : m_bgmA;

		newTrack.volume = std::clamp(t, 0.0f, 1.0f);
		currentTrack.volume = std::clamp(1.0f - t, 0.0f, 1.0f);

		// 音量を適用
		ApplyVolumeToHandle(m_soundClips[currentTrack.soundID], currentTrack.volume);
		ApplyVolumeToHandle(m_soundClips[newTrack.soundID], newTrack.volume);

		if(t >= 1.0f)
		{
			// フェード完了
			StopBGMTrack(currentTrack);
			m_bgmPhase = BGMPhase::Idle;
		}
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

void SoundManager::PlayBGM(const std::string& soundID, float fadeTime)
{
	// 現在のBGMを止めてから新しいBGMを再生する
	StopBGMTrack(m_bgmA);
	StopBGMTrack(m_bgmB);

	StartBGMOnTrack(m_bgmA, soundID, fadeTime > 0.0f ? 0.0f : 1.0f); // フェードインがある場合は最初は音量0で再生

	if(fadeTime > 0.0f)
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
	if(!m_bgmA.isActive && !m_bgmB.isActive)
	{
		// どちらのトラックも再生されていない場合は通常のBGM再生を行う
		PlayBGM(soundID, 0.0f);
		return;
	}

	// フェードアウト中のトラックとフェードイン中のトラックを切り替える
	BGMTrack* currentTrack = (m_bgmA.isActive) ? &m_bgmA : &m_bgmB;
	BGMTrack* newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;

	// 新しいトラックでBGMを開始
	StopBGMTrack(*newTrack); // 念のため停止しておく
	StartBGMOnTrack(*newTrack, soundID, 0.0f); // 最初は音量0で再生

	m_bgmPhase = BGMPhase::CrossFading;
	// フェード時間を設定
	m_bgmFadeTime = max(0.01f,fadeTime);
	m_bgmFadeTimer = 0.0f;

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

