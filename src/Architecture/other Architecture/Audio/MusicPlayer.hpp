#pragma once

//STD
#include <string>
#include <map>

//SELF
#include "AudioLocator.hpp"
#include "../Rng.hpp"

class MusicPlayer
{
public:
	MusicPlayer();

	void update();

	void addMusicToPlaylist(std::string playlist, std::string song);
	void startPlaylist(std::string playlist);
	void stopPlaylist(std::string playlist);

	void play(std::string music_name);
	std::string getCurrentMusicName();

	void toggleMuted();

private:
	std::string current_music;
	std::unique_ptr<Sound> current_music_sound;

	std::multimap<std::string, std::string> playlists;
	std::string current_playlist;

	bool muted;
	Rng rng_generator;
};