#include "Game.hpp"

//LIBS
#include <SFML/Graphics.hpp>

Game::Game()
{
	run();
}

void Game::run()
{
	auto window = sf::RenderWindow(sf::VideoMode(1280, 720), "Hi");

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear({ 100, 20, 20, 255 });
		window.display();
	}
}

void Game::input()
{

}

void Game::update()
{

}

void Game::draw()
{

}