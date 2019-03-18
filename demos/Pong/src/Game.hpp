#pragma once

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

	std::unique_ptr<sf::RenderWindow> window;
	enki::Timer timer;
	std::unique_ptr<enki::Scenegraph> scenegraph;
	enki::RPCManager rpcm;
	std::unique_ptr<enki::GameData> game_data;
	sf::Font font;
	sf::Text score1;
	sf::Text score2;

	enki::ManagedConnection mc1;
	enki::ManagedConnection mc2;
	enki::ManagedConnection mc3;
};