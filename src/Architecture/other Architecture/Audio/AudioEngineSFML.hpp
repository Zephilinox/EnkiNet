#pragma once

//STD
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

//LIB
#include <SFML/Audio.hpp>

//SELF
#include "AudioEngine.hpp"

class SoundSFML : public Sound
{
public:
	void stop() final
	{
		if (sound)
		{
			sound->stop();
		}
	}

	void setMuted(bool mute) final
	{
		if (sound)
		{
			if (mute)
			{
				sound->pause();
			}
			else
			{
				sound->play();
			}
		}
	}

	bool isPlaying() final
	{
		if (sound)
		{
			return sound->getStatus() != sf::Sound::Status::Stopped;
		}

		return false;
	}

	sf::Sound* sound;
};

class AudioEngineSFML : public AudioEngine
{
public:
	AudioEngineSFML(const std::string& audio_path);
	AudioEngineSFML() = default;

	std::unique_ptr<Sound> play(const std::string& name, bool loop = false) final;
	
private:
	std::unordered_map<std::string, sf::SoundBuffer> buffers;
	std::vector<std::unique_ptr<sf::Sound>> sounds;
};