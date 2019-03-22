#pragma once

//STD
#include <set>

//SELF
#include <EnkiNet/Scenegraph.hpp>
#include <EnkiNet/Timer.hpp>
#include <EnkiNet/Networking/RPC.hpp>
#include <EnkiNet/GameData.hpp>

class Game
{
public:
	Game();

private:
	void run();
	void input();
	void update();
	void draw() const;

	float dt = 1.0f / 60.0f;

	enki::Timer timer;
	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<enki::Scenegraph> scenegraph;
	std::unique_ptr<enki::GameData> game_data;
	std::unique_ptr<enki::NetworkManager> network_manager;

	std::set<enki::ClientID> players;

	enki::ManagedConnection mc1;
};