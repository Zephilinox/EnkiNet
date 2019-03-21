#include "Ball.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

//SELF
#include <EnkiNet/Networking/Networking/ServerHost.hpp>
#include <EnkiNet/Scenegraph.hpp>

Ball::Ball(enki::EntityInfo info, enki::GameData* game_data)
	: Entity(info, game_data)
{
}

void Ball::onSpawn()
{
	texture.loadFromFile("resources/Ball.png");
	sprite.setTexture(texture);
	sprite.setPosition(320, 180);

	if (isOwner())
	{
		mc1 = game_data->getNetworkManager()->on_network_tick.connect([this]()
		{
			enki::Packet p({ enki::PacketType::ENTITY_UPDATE });
			p << this->info;
			serialize(p);
			this->game_data->getNetworkManager()->client->sendPacket(0, &p);
		});

		if (game_data->getNetworkManager()->server)
		{
			mc2 = game_data->getNetworkManager()->server->on_packet_received.connect([this](enki::Packet p)
			{
				if (p.getHeader().type == enki::PacketType::CONNECTED)
				{
					sprite.setPosition(320, 180);
				}
			});
		}
	}
}

void Ball::update(float dt)
{
	if (game_data->scenegraph->findEntitiesByType("Paddle").size() != 2)
	{
		return;
	}

	if (isOwner())
	{
		if (moving_left)
		{
			sprite.move(-300 * dt, 0);
			if (sprite.getPosition().x < -64)
			{
				sprite.setPosition(320 - 16, 180 - 16);
				y_speed = float(std::rand() % 300 + 100);
				game_data->score2++;
				moving_left = false;
			}
		}
		else
		{
			sprite.move(300 * dt, 0);
			if (sprite.getPosition().x > 640 + 32)
			{
				sprite.setPosition(320 - 16, 180 - 16);
				y_speed = float(std::rand() % 300 + 100);
				game_data->score1++;
				moving_left = true;
			}
		}

		if (y_dir == 1)
		{
			sprite.move(0, -y_speed * dt);
			if (sprite.getPosition().y < 0)
			{
				y_dir = -1;
				y_speed = float(std::rand() % 300 + 100);
			}
		}
		else if (y_dir == -1)
		{
			sprite.move(0, y_speed * dt);
			if (sprite.getPosition().y > 360 - 32)
			{
				y_dir = 1;
				y_speed = float(std::rand() % 300 + 100);
			}
		}
	}
}

void Ball::draw(sf::RenderWindow& window) const
{
	window.draw(sprite);
}

void Ball::serialize(enki::Packet& p)
{
	p << sprite.getPosition().x << sprite.getPosition().y;
}

void Ball::deserialize(enki::Packet& p)
{
	float x = p.read<float>();
	float y = p.read<float>();
	sprite.setPosition(x, y);
}
