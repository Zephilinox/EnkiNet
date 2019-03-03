#pragma once

//STD
#include <memory>

//LIB
#include <irrKlang.h>

//SELF
#include "AudioEngine.hpp"

namespace irrklang
{
	class ISoundEngine;
}

class SoundIrrklang : public Sound
{
public:
	void stop() final
	{
		if (sound_source)
		{
			sound_source->stop();
			sound_source->drop();
			sound_source = nullptr;
		}
	}

	void setMuted(bool mute) final
	{
		if (sound_source)
		{
			sound_source->setIsPaused(mute);
		}
	}

	bool isPlaying() final
	{
		if (sound_source)
		{
			return !sound_source->isFinished();
		}

		return false;
	}

	irrklang::ISound* sound_source;
};

class AudioEngineIrrklang : public AudioEngine
{
public:
	AudioEngineIrrklang(const std::string& audio_path);
	~AudioEngineIrrklang() noexcept = default;

	std::unique_ptr<Sound> play(const std::string& name, bool loop = false) final;

private:
	std::unique_ptr<irrklang::ISoundEngine> audio_engine = nullptr;
};
