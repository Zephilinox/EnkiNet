#include "Game.hpp"

//STD
#include <iostream>

//LIBS
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <EnkiNet/Networking/Networking/ServerHost.hpp>

//SELF
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Collision.hpp"
#include "PlayerText.hpp"

Game::Game()
{
	spdlog::stdout_color_mt("console");
	auto console = spdlog::get("console");
	game_data = std::make_unique<GameData>();
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 360), "EnkiNet");
	scenegraph = std::make_unique<Scenegraph>(game_data.get());
	game_data->scenegraph = scenegraph.get();

	scenegraph->registerEntity<PlayerText>("PlayerText3");
	scenegraph->registerEntity<PlayerText>("PlayerText2");
	scenegraph->registerEntityChildren("PlayerText2", "PlayerText3");
	scenegraph->registerEntity<PlayerText>("PlayerText");
	scenegraph->registerEntityChildren("PlayerText", "PlayerText2");
	scenegraph->registerEntity<Paddle>("Paddle");
	scenegraph->registerEntityChildren("Paddle", "PlayerText");
	scenegraph->registerEntity<Ball>("Ball");
	scenegraph->registerEntity<Collision>("Collision");

	//scenegraph->createEntity(EntityInfo{ "Collision", "Collision"});

	if (!font.loadFromFile("resources/arial.ttf"))
	{
		console->error("Failed to load font");
	}

	score1.setFont(font);
	score1.setCharacterSize(30);
	score1.setFillColor(sf::Color(0, 150, 255));
	score1.setPosition(320 - 60, 180);
	score1.setString("0");

	score2.setFont(font);
	score2.setCharacterSize(30);
	score2.setFillColor(sf::Color(220, 25, 25));
	score2.setPosition(320 + 60, 180);
	score2.setString("0");

	run();
}

void Game::run()
{
	//start delta time at a sane value
	dt = 1.0f / 60.0f;

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

		if (e.type == sf::Event::GainedFocus)
		{
			game_data->window_active = true;
		}

		if (e.type == sf::Event::Closed)
		{
			game_data->window_active = false;
		}

		if (e.type == sf::Event::KeyPressed)
		{
			if (e.key.code == sf::Keyboard::F1)
			{
				if (game_data->getNetworkManager()->network_send_rate == 10)
				{
					game_data->getNetworkManager()->network_send_rate = 60;
				}
				else
				{
					game_data->getNetworkManager()->network_send_rate = 10;
				}
			}
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
			scenegraph->createNetworkedEntity(EntityInfo{ "Paddle", "Paddle 1" });
			scenegraph->createNetworkedEntity(EntityInfo{ "Ball", "Ball" });
			GameData* game_data_ptr = game_data.get();
			mc1 = game_data->getNetworkManager()->on_network_tick.connect([game_data_ptr]()
			{
				Packet p({ PacketType::COMMAND });
				p << std::string("Scores");
				p << game_data_ptr->score1;
				p << game_data_ptr->score2;
				game_data_ptr->getNetworkManager()->server->sendPacketToAllClients(0, &p);
			});

			mc3 = game_data->getNetworkManager()->server->on_packet_received.connect([game_data_ptr](Packet p)
			{
				auto console = spdlog::get("console");
				console->info("server received {}. sent at {} and received at {}, delta of {}", p.getHeader().type, p.getHeader().timeSent, p.info.timeReceived, p.info.timeReceived - p.getHeader().timeSent);
				
				if (p.getHeader().type == PacketType::CONNECTED)
				{
					game_data_ptr->scenegraph->createNetworkedEntity(EntityInfo{ "Paddle", "Paddle 2", 0, p.info.senderID });
				}
			});
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
		{
			game_data->getNetworkManager()->startClient();
			scenegraph->enableNetworking();
			GameData* game_data_ptr = game_data.get();
			mc2 = game_data->getNetworkManager()->client->on_packet_received.connect([game_data_ptr](Packet p)
			{
				auto console = spdlog::get("console");
				console->info("client received {}. sent at {} and received at {}, delta of {}", p.getHeader().type, p.getHeader().timeSent, p.info.timeReceived, p.info.timeReceived - p.getHeader().timeSent);

				if (p.getHeader().type == PacketType::COMMAND)
				{
					std::string id = p.read<std::string>();
					if (id == "Scores")
					{
						game_data_ptr->score1 = p.read<int>();
						game_data_ptr->score2 = p.read<int>();
					}
				}
			});
		}
	}

	score1.setString(std::to_string(game_data->score1));
	score2.setString(std::to_string(game_data->score2));
	scenegraph->update(dt);
}

void Game::draw() const
{
	window->clear({ 230, 230, 230, 255 });
	window->draw(score1);
	window->draw(score2);
	scenegraph->draw(*window.get());
	window->display();
}