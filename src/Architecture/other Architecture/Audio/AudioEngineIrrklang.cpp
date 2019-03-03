#include "AudioEngineIrrklang.hpp"

//STD
#include <cassert>
#include <string>

//LIB
#include <irrKlang.h>

AudioEngineIrrklang::AudioEngineIrrklang(const std::string& audio_path)	
	: AudioEngine(audio_path)
{
	audio_engine.reset(irrklang::createIrrKlangDevice());

	assert(audio_engine);
}

std::unique_ptr<Sound> AudioEngineIrrklang::play(const std::string& name, bool loop)
{
	auto sound = audio_engine->play2D(std::string(audio_path + name).c_str(), loop, true);
	sound->setIsPaused(false);
	auto s = std::make_unique<SoundIrrklang>();
	s->sound_source = sound;
	return s;
}
