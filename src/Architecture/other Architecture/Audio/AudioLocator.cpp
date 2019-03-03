#include "AudioLocator.hpp"

//STD
#include <functional>
#include <string>

const std::string AudioLocator::path("../../Resources/Sounds/");
std::unique_ptr<AudioEngine> AudioLocator::audio(std::make_unique<AudioEngineNone>(path));

void AudioLocator::set(AudioEngine* audio_engine)
{
	if (audio_engine)
	{
		audio.reset(audio_engine);
	}
	else
	{
		audio = std::move(std::make_unique<AudioEngineNone>(path));
	}
}

void AudioLocator::set(int engine_type)
{
	try
	{
		switch (engine_type)
		{
		case AudioEngineType::IrrKlang:
			audio = std::move(std::make_unique<AudioEngineIrrklang>(path));
			break;
		case AudioEngineType::SFML:
			audio = std::move(std::make_unique<AudioEngineSFML>(path));
			break;
		case AudioEngineType::None:
		default:
			audio = std::move(std::make_unique<AudioEngineNone>(path));
			break;
		}
	}
	catch (...)
	{
		audio = std::move(std::make_unique<AudioEngineNone>(path));
	}
}