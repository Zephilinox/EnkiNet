#pragma once

//STD
#include <string>
#include <memory>

class Sound
{
public:
	virtual void stop() = 0;
	virtual void setMuted(bool mute) = 0;
	virtual bool isPlaying() = 0;
};

class AudioEngine
{
public:
	AudioEngine(const std::string& audio_path)
		: audio_path(audio_path)
	{};

	virtual ~AudioEngine() noexcept = default;

	virtual std::unique_ptr<Sound> play(const std::string& name, bool loop = false) = 0;
	
	const std::string audio_path;
};