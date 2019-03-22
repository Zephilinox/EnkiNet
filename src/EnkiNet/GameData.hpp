#pragma once

//LIBS
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <SFML/Graphics.hpp>

//SELF
#include "Managers/NetworkManager.hpp"

namespace enki
{
	class Scenegraph;

	//oof todo
	class GameData
	{
	public:
		GameData();

		NetworkManager* network_manager = nullptr;
		Scenegraph* scenegraph = nullptr;

		bool window_active = true;
		//todo
		int score1 = 0;
		int score2 = 0;
	};
}