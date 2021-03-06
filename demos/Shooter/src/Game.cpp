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
#include "Managers/MapManager.hpp"
#include "Floor.hpp"
#include "Wall.hpp"
#include "HealthSpawner.hpp"

Game::Game()
{
	spdlog::stdout_color_mt("console");
	auto console = spdlog::get("console");

	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 720), "EnkiNet");
	input_manager.window = window.get();

	game_data = std::make_unique<enki::GameData>();
	custom_data = std::make_unique<CustomData>();

	custom_data->input_manager = &input_manager;

	scenegraph = std::make_unique<enki::Scenegraph>(game_data.get());
	auto enki_logger = spdlog::get("EnkiNet");
	enki_logger->set_level(spdlog::level::err);

	network_manager = std::make_unique<enki::NetworkManager>();

	game_data->scenegraph = scenegraph.get();
	game_data->network_manager = network_manager.get();
	game_data->custom = custom_data.get();

	scenegraph->registerEntity<Player>("Player", window.get());
	scenegraph->registerEntity<class Floor>("Floor");
	scenegraph->registerEntity<class Wall>("Wall");
	scenegraph->registerEntity<HealthSpawner>("HealthSpawner");

	run();
}

void Game::run()
{
	//window->setFramerateLimit(120);

	enki::Timer fpsTimer;
	auto console = spdlog::get("console");
	while (window->isOpen())
	{
		input();
		update();
		draw();

		dt = timer.getElapsedTime();
		if (fpsTimer.getElapsedTime() > 0.5f)
		{
			console->info("FPS: {}", 1.0f / dt);
			fpsTimer.restart();
		}
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
			custom_data->window_active = true;
		}
		else if (e.type == sf::Event::LostFocus)
		{
			custom_data->window_active = false;
		}

		scenegraph->input(e);
	}
}

void Game::update()
{
	network_manager->update();
	input_manager.update();

	static bool networking = false;

	if (!networking && custom_data->window_active)
	{
		if (input_manager.isKeyPressed(sf::Keyboard::Key::S))
		{
			networking = true;
			network_manager->startHost();
			scenegraph->enableNetworking();

			map_manager = std::make_unique<MapManager>(scenegraph.get(), network_manager.get());
			custom_data->map_manager = map_manager.get();
			map_manager->createMap();

			scenegraph->createNetworkedEntity({ "Player", "Player 1" });

			mc1 = network_manager->server->on_packet_received.connect([this](enki::Packet p)
			{
				if (p.getHeader().type == enki::PacketType::CONNECTED)
				{
					//not already part of the game
					if (!players.count(p.info.senderID))
					{
						scenegraph->createNetworkedEntity({ "Player", "Player " + std::to_string(p.info.senderID), 0, p.info.senderID });
						players.insert(p.info.senderID);
					}
				}
			});
		}

		if (input_manager.isKeyPressed(sf::Keyboard::Key::C))
		{
			networking = true;
			network_manager->startClient();
			scenegraph->enableNetworking();

			map_manager = std::make_unique<MapManager>(scenegraph.get(), network_manager.get());
			custom_data->map_manager = map_manager.get();
		}
	}

	scenegraph->update(dt);
}

void Game::draw() const
{
	window->clear({ 0, 0, 0, 255 });
	scenegraph->draw(*window.get());
	window->display();
}