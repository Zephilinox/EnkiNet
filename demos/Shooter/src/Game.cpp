#include "Game.hpp"

//STD
#include <iostream>

//LIBS
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <EnkiNet/Networking/Networking/ServerHost.hpp>

Game::Game()
{
	spdlog::stdout_color_mt("console");
	auto console = spdlog::get("console");
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 360), "EnkiNet");

	game_data = std::make_unique<GameData>();
	scenegraph = std::make_unique<Scenegraph>(game_data.get());
	game_data->scenegraph = scenegraph.get();

	if (!font.loadFromFile("resources/arial.ttf"))
	{
		console->error("Failed to load font");
	}

	run();
}

void Game::run()
{
	window->setFramerateLimit(120);

	while (window->isOpen())
	{
		game_data->getNetworkManager()->update();

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
		else if (e.type == sf::Event::GainedFocus)
		{
			game_data->window_active = true;
		}
		else if (e.type == sf::Event::LostFocus)
		{
			game_data->window_active = false;
		}

		scenegraph->input(e);
	}
}

void Game::update()
{
	if (game_data->window_active && !game_data->getNetworkManager()->server && !game_data->getNetworkManager()->client)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			game_data->getNetworkManager()->startHost();
			scenegraph->enableNetworking();
			mc1 = game_data->getNetworkManager()->on_network_tick.connect([gd = game_data.get()]()
			{
			});

			mc3 = game_data->getNetworkManager()->server->on_packet_received.connect([gd = game_data.get()](Packet p)
			{
			});
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
		{
			game_data->getNetworkManager()->startClient();
			scenegraph->enableNetworking();
			mc2 = game_data->getNetworkManager()->client->on_packet_received.connect([gd = game_data.get()](Packet p)
			{
			});
		}
	}

	scenegraph->update(dt);
}

void Game::draw() const
{
	window->clear({ 230, 230, 230, 255 });
	scenegraph->draw(*window.get());
	window->display();
}