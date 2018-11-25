#include "Game.hpp"

//STD
#include <iostream>

//LIBS
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

//SELF
#include "Paddle.hpp"
#include "Ball.hpp"

Game::Game()
{
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 720), "Hi");

	scenegraph.registerBuilder("Paddle", [](EntityInfo info)
	{
		return std::make_unique<Paddle>(info);
	});

	scenegraph.registerBuilder("Ball", [](EntityInfo info)
	{
		return std::make_unique<Ball>(info);
	});

	scenegraph.createEntity(EntityInfo{ "Paddle", "Paddle 1" });
	scenegraph.createEntity(EntityInfo{ "Paddle", "Paddle 2" });
	scenegraph.createEntity(EntityInfo{ "Ball", "Ball" });

	run();
}

void Game::run()
{
	timer.restart();
	input();
	update();
	draw();

	dt = timer.getElapsedTime();
	timer.restart();

	while (window->isOpen())
	{
		input();
		update();
		draw();

		dt = timer.getElapsedTime();
		timer.restart();
	}
}

void Game::input()
{
	sf::Event e;
	while (window->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			window->close();
		}
	}
}

void Game::update()
{
	scenegraph.update(dt);
}

void Game::draw() const
{
	window->clear({ 100, 20, 20, 255 });
	scenegraph.draw(*window.get());
	window->display();
}