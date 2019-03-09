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

	Timer timer;
	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<Scenegraph> scenegraph;
	std::unique_ptr<GameData> game_data;
	sf::Font font;

	ManagedConnection mc1;
	ManagedConnection mc2;
	ManagedConnection mc3;
};