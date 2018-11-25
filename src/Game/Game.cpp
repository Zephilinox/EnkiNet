#include "Game.hpp"

//LIBS
#include <SFML/Graphics.hpp>

//STD
#include <iostream>

class Thing : public Entity
{
public:
	Thing(EntityInfo info)
		: Entity(info)
	{
		std::cout << "hi";
	}

	virtual void update(float dt)
	{
		std::cout << "hi";
	}

	virtual void draw(sf::RenderWindow& window) const
	{

	}
};

Game::Game()
{
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 720), "Hi");

	scenegraph.registerBuilder("Thing", [](EntityInfo info)
	{
		return std::make_unique<Thing>(info);
	});

	scenegraph.createEntity(EntityInfo{ "Thing", "Thing 1" });
	run();
}

void Game::run()
{
	while (window->isOpen())
	{
		input();
		update();
		draw();
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

}

void Game::draw() const
{
	window->clear({ 100, 20, 20, 255 });
	window->display();
}