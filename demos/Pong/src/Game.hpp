#pragma once

//SELF
#include <EnkiNet/Scenegraph.hpp>
#include <EnkiNet/Timer.hpp>
#include <EnkiNet/Networking/RPC.hpp>
#include <EnkiNet/Managers/NetworkManager.hpp>
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

	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<enki::Scenegraph> scenegraph;
	std::unique_ptr<enki::GameData> game_data;
	std::unique_ptr<enki::NetworkManager> network_manager;
	enki::Timer timer;
	enki::RPCManager rpcm;
	sf::Font font;
	sf::Text score1;
	sf::Text score2;

	enki::ManagedConnection mc1;
	enki::ManagedConnection mc2;
	enki::ManagedConnection mc3;
};