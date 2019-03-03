#include "MusicPlayer.hpp"

//STD
#include <fstream>

//LIBS
#include <jsoncons/json.hpp>

MusicPlayer::MusicPlayer()
{
	try
	{
		std::ifstream file("../../Resources/settings.json");
		jsoncons::json settings;
		file >> settings;
		muted = settings["Audio"]["Music Muted"].as_bool();
	}
	catch (std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

void MusicPlayer::update()
{
	if (current_music_sound && !current_music_sound->isPlaying())
	{
		current_music_sound->stop();
		current_music_sound = nullptr;
		current_music = "";
	}

	if (current_playlist != "")
	{
		if (current_music == "")
		{
			auto range = playlists.equal_range(current_playlist);
			auto range_size = std::distance(range.first, range.second) - 1;

			auto rand_song_index = rng_generator.getRandomInt(0, range_size);
			std::advance(range.first, rand_song_index);
			play(range.first->second);
		}
	}
}

void MusicPlayer::addMusicToPlaylist(std::string playlist, std::string song)
{
	playlists.insert({ playlist, song });
}

void MusicPlayer::startPlaylist(std::string playlist)
{
	if (current_playlist != "" && current_playlist != playlist && current_music_sound)
	{
		current_music_sound->stop();
		current_music_sound = nullptr;
		current_music = "";
	}

	current_playlist = playlist;
}

void MusicPlayer::stopPlaylist(std::string playlist)
{
	current_playlist = "";
}

void MusicPlayer::play(std::string music_name)
{
	if (music_name != current_music)
	{
		if (current_music_sound)
		{
			current_music_sound = nullptr;
			current_music_sound->stop();
			current_music = "";
		}

		current_music_sound = std::move(AudioLocator::get()->play("Music/" + music_name + ".wav"));
		current_music_sound->setMuted(muted);
		current_music = music_name;
	}
}

std::string MusicPlayer::getCurrentMusicName()
{
	return current_music;
}

void MusicPlayer::toggleMuted()
{
	muted = !muted;
	current_music_sound->setMuted(muted);
}
