#include "AudioEngineNone.hpp"

AudioEngineNone::AudioEngineNone(const std::string& audio_path)
	: AudioEngine(audio_path)
{
}

std::unique_ptr<Sound> AudioEngineNone::play(const std::string& name, bool loop) noexcept
{
	return std::make_unique<SoundNone>();
}
