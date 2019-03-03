#pragma once

//SELF
#include "AudioEngine.hpp"

class SoundNone : public Sound
{
public:
	void stop() final
	{
	}

	void setMuted(bool mute) final
	{
	}

	bool isPlaying() final
	{
		return false;
	}
};

class AudioEngineNone : public AudioEngine
{
public:
	AudioEngineNone(const std::string& audio_path);
	~AudioEngineNone() noexcept = default;

	std::unique_ptr<Sound> play(const std::string& name, bool loop = false) noexcept override final;
};