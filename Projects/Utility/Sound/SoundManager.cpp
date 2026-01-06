#include <unordered_map>
#include <string>
#include "SoundManager.h"

namespace
{
	constexpr float kDefaultMasterVolume = 1.0f;
}

SoundManager::SoundManager()
{
	m_masterVolume = kDefaultMasterVolume;

	m_busVolume = 
	{
		{SoundBus::BGM, 1.0f},
		{SoundBus::SE, 1.0f}
	};
}

SoundManager::~SoundManager()
{
}
