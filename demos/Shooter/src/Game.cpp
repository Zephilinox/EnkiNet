#include "Game.hpp"

//STD
#include <iostream>

//LIBS
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <EnkiNet/Networking/Networking/ServerHost.hpp>

//SELF
#include "Player.hpp"

Game::Game()
{
	spdlog::stdout_color_mt("console");
	auto console = spdlog::get("console");
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 360), "EnkiNet");

	game_data = std::make_unique<GameData>();
	scenegraph = std::make_unique<Scenegraph>(game_data.get());
	game_data->scenegraph = scenegraph.get();

	scenegraph->registerEntity("Player", [&](EntityInfo info)
	{
		return std::make_unique<Player>(info, game_data.get(), window.get());
	});

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
			scenegraph->createNetworkedEntity({ "Player", "Player 1" });

			mc1 = game_data->getNetworkManager()->server->on_packet_received.connect([gd = game_data.get(), this](Packet p)
			{
				if (p.getHeader().type == PacketType::CONNECTED)
				{
					scenegraph->createNetworkedEntity({ "Player", "Player 2", 0, p.info.senderID });
				}
			});
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
		{
			game_data->getNetworkManager()->startClient();
			scenegraph->enableNetworking();
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