#include "Paddle.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>
#include <EnkiNet/Scenegraph.hpp>

Paddle::Paddle(EntityInfo info, GameData* game_data)
	: Entity(info, game_data)
{
}

void Paddle::onSpawn()
{
	texture.loadFromFile("resources/Paddle.png");
	sprite.setTexture(texture);

	if (info.name == "Paddle 1")
	{
		sprite.setPosition(32, 180);
		sprite.setColor(sf::Color(0, 150, 255));
	}
	else
	{
		sprite.setPosition(640 - 64, 180);
		sprite.setColor(sf::Color(220, 25, 25));
	}

	latest_sprite = sprite;
	latest_sprite.setColor(sf::Color(50, 50, 50, 100));

	if (isOwner())
	{
		mc1 = game_data->getNetworkManager()->on_network_tick.connect([this]()
		{
			Packet p({ PacketType::ENTITY_UPDATE });
			p << this->info;
			serialize(p);
			this->game_data->getNetworkManager()->client->sendPacket(0, &p);
		});

		auto console = spdlog::get("console");
		console->info("paddle creating net entity.\n\t{}", info);
		game_data->scenegraph->createNetworkedEntity({ "PlayerText", "PlayerText " + info.name, 0, info.ownerID, info.ID });
	}

	game_data->scenegraph->rpcs.add("Paddle", "setColour", &Paddle::setColour);
}

void Paddle::input(sf::Event& e)
{
	if (e.type == sf::Event::KeyPressed)
	{
		if (!isOwner())
		{
			if (e.key.code == sf::Keyboard::F2)
			{
				if (interpolation_enabled)
				{
					sprite.setPosition(latest_sprite.getPosition());
				}
				else
				{
					latest_sprite.setPosition(sprite.getPosition());
				}

				interpolation_enabled = !interpolation_enabled;
				latest_network_y = sprite.getPosition().y;
				last_interpolation_y = sprite.getPosition().y;
			}
		}
		else
		{
			if (e.key.code == sf::Keyboard::Num3)
			{
				game_data->scenegraph->rpcs.call(&Paddle::setColour, std::string("setColour"), game_data->getNetworkManager(), this, std::rand() % 255, std::rand() % 255, std::rand() % 255);
			}
		}
	}
}

void Paddle::update(float dt)
{
	if (info.name == "Paddle 1" && isOwner())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			sprite.move(0, -200.0f * dt);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			sprite.move(0, 200.0f * dt);
		}
	}
	else if (info.name == "Paddle 2" && isOwner())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			sprite.move(0, -200.0f * dt);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			sprite.move(0, 200.0f * dt);
		}
	}

	if (isOwner())
	{
		if (sprite.getPosition().y < 0)
		{
			sprite.setPosition(sprite.getPosition().x, 0);
		}

		if (sprite.getPosition().y > 360 - 128)
		{
			sprite.setPosition(sprite.getPosition().x, 360 - 128);
		}
	}
	else if (interpolation_enabled)
	{
		if (std::abs(latest_network_y - last_interpolation_y) > 64)
		{
			sprite.setPosition(sprite.getPosition().x, latest_network_y);
			last_interpolation_y = latest_network_y;
		}
		else
		{
			float network_delay = 1.0f / 10.0f;
			float interpolation_time = interpolation_timer.getElapsedTime();
			float interpolation_percent = float(1.0 - ((network_delay - interpolation_time) / network_delay));

			float interpolated_y = last_interpolation_y + interpolation_percent * (latest_network_y - last_interpolation_y);
			sprite.setPosition(sprite.getPosition().x, interpolated_y);
		}
	}
}

void Paddle::draw(sf::RenderWindow& window) const
{
	window.draw(sprite);

	if (!isOwner() && interpolation_enabled)
	{
		window.draw(latest_sprite);
	}
}

void Paddle::serialize(Packet& p)
{
	p << sprite.getPosition().x << sprite.getPosition().y;
}

void Paddle::deserialize(Packet& p)
{
	float x = p.read<float>();

	last_interpolation_y = sprite.getPosition().y;
	p >> latest_network_y;

	if (interpolation_enabled)
	{
		latest_sprite.setPosition(x, latest_network_y);
		interpolation_timer.restart();
	}
	else
	{
		sprite.setPosition(x, latest_network_y);
	}
}

void Paddle::setColour(int r, int g, int b)
{
	sprite.setColor(sf::Color(uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(255)));
}
