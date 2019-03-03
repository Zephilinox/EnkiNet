#include "AudioEngineSFML.hpp"

//STD
#include <experimental/vector>

AudioEngineSFML::AudioEngineSFML(const std::string& audio_path)
	: AudioEngine(audio_path)
{
}

std::unique_ptr<Sound> AudioEngineSFML::play(const std::string& name, bool loop)
{
	if (!buffers[name].loadFromFile(audio_path + name))
	{
		throw "error loading " + audio_path + name;
	}

	std::experimental::erase_if(sounds,
	[&](auto& sound)
	{
		return sound->getStatus() == sf::Sound::Stopped;
	});
	
	sounds.push_back(std::make_unique<sf::Sound>(buffers[name]));
	sounds.back()->setLoop(loop);
	sounds.back()->play();

	auto s = std::make_unique<SoundSFML>();
	s->sound = sounds.back().get();
	return s;
}
