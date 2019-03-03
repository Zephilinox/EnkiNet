#pragma once

//STD
#include <string>
#include <memory>

//SELF
#include "../Audio/AudioEngine.hpp"
#include "../Audio/AudioEngineIrrklang.hpp"
#include "../Audio/AudioEngineSFML.hpp"
#include "../Audio/AudioEngineNone.hpp"

enum AudioEngineType
{
	None = 0,
	IrrKlang = 1,
	SFML = 2
};

class AudioLocator
{
public:
	static void set(AudioEngine* audio_engine);
	static void set(int engine_type);

	inline static AudioEngine* get()
	{
		return audio.get();
	}

private:
	static const std::string path;
	static std::unique_ptr<AudioEngine> audio;
};