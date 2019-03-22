#include "Game.hpp"

//STD
#include <iostream>

//LIBS
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <EnkiNet/Networking/ServerHost.hpp>

//SELF
#include "Player.hpp"

Game::Game()
{
	spdlog::stdout_color_mt("console");
	auto console = spdlog::get("console");
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 360), "EnkiNet");

	game_data = std::make_unique<enki::GameData>();
	scenegraph = std::make_unique<enki::Scenegraph>(game_data.get());
	network_manager = std::make_unique<enki::NetworkManager>();
	game_data->scenegraph = scenegraph.get();
	game_data->network_manager = network_manager.get();

	scenegraph->registerEntity<Player>("Player", window.get());

	run();
}

void Game::run()
{
	window->setFramerateLimit(120);

	//auto console = spdlog::get("console");
	while (window->isOpen())
	{
		game_data->network_manager->update();

		input();
		update();
		draw();

		dt = timer.getElapsedTime();
		//console->info("FPS: {}", 1.0f / dt);
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
			//game_data->window_active = true;
		}
		else if (e.type == sf::Event::LostFocus)
		{
			//game_data->window_active = false;
		}

		scenegraph->input(e);
	}
}

void Game::update()
{
	if (/*game_data->window_active &&*/
		!game_data->network_manager->server &&
		!game_data->network_manager->client)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			game_data->network_manager->startHost();
			scenegraph->enableNetworking();
			scenegraph->createNetworkedEntity({ "Player", "Player 1" });

			mc1 = game_data->network_manager->server->on_packet_received.connect([gd = game_data.get(), this](enki::Packet p)
			{
				if (p.getHeader().type == enki::PacketType::CONNECTED)
				{
					//not already part of the game
					if (!players.count(p.info.senderID))
					{
						scenegraph->createNetworkedEntity({ "Player", "Player " + p.info.senderID, 0, p.info.senderID });
						players.insert(p.info.senderID);
					}
				}
			});
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
		{
			game_data->network_manager->startClient();
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