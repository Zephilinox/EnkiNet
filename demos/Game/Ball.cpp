#include "Ball.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

//SELF
#include "../Architecture/Networking/Networking/ServerHost.hpp"
#include "../Architecture/Scenegraph.hpp"

Ball::Ball(EntityInfo info, GameData* game_data)
	: Entity(info, game_data)
{
}

void Ball::onSpawn()
{
	fmt::print("ID={} name={} owner={} parent={} type={}\n", info.ID, info.name, info.ownerID, info.parentID, info.type);
	texture.loadFromFile("Ball.png");
	sprite.setTexture(texture);
	sprite.setPosition(320, 180);

	if (isOwner())
	{
		if (true)
		{
			mc1 = game_data->getNetworkManager()->on_network_tick.connect([this]()
			{
				Packet p({ PacketType::ENTITY });
				p << this->info;
				serialize(p);
				this->game_data->getNetworkManager()->client->sendPacket(0, &p);
			});
		}

		if (game_data->getNetworkManager()->server)
		{
			mc2 = game_data->getNetworkManager()->server->on_packet_received.connect([this](Packet p)
			{
				if (p.get_header().type == PacketType::CONNECTED)
				{
					sprite.setPosition(320, 180);
				}
			});
		}
	}
}

void Ball::input([[maybe_unused]]sf::Event& e)
{
}

void Ball::update(float dt)
{
	if (isOwner())
	{
		if (!game_data->scenegraph->entityExists(4))
		{
			return;
		}

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

void Ball::serialize(Packet& p)
{
	p << sprite.getPosition().x << sprite.getPosition().y;
}

void Ball::deserialize(Packet& p)
{
	float x;
	float y;
	p >> x;
	p >> y;
	sprite.setPosition(x, y);
}
