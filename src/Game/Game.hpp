#pragma once

//SELF
#include "../Architecture/Scenegraph.hpp"
#include "../Architecture/Timer.hpp"
#include "../Architecture/Networking/RPC.hpp"

class Game
{
public:
	Game();

private:
	void run();
	void input();
	void update();
	void draw() const;

	std::unique_ptr<sf::RenderWindow> window;
	Timer timer;
	Scenegraph scenegraph;
	RPCManager rpcm;
};