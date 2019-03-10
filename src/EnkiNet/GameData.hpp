#pragma once

//LIBS
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <SFML/Graphics.hpp>

//SELF
#include "Managers/NetworkManager.hpp"

class Scenegraph;

class GameData
{
public:
	GameData();

	NetworkManager* getNetworkManager() noexcept;
	Scenegraph* scenegraph = nullptr;

	bool window_active = true;
	int score1 = 0;
	int score2 = 0;

private:
	NetworkManager network_manager;
};
