#include "Player.hpp"

Player::Player(EntityInfo info, GameData* data, sf::RenderWindow* window)
	: Entity(info, data)
	, window(window)
{
}

void Player::onSpawn()
{
	auto console = spdlog::get("console");
	if (!texture.loadFromFile("resources/player.png"))
	{
		console->error(":(");
	}

	sprite.setTexture(texture);
	sprite.setOrigin(16, 16);

	if (info.name == "Player 1")
	{
		sprite.setColor(sf::Color(0, 100, 200));
	}
	else
	{
		sprite.setColor(sf::Color(200, 60, 60));
	}

	if (isOwner())
	{
		mc1 = game_data->getNetworkManager()->on_network_tick.connect([this]()
		{
			Packet p({ PacketType::ENTITY });
			p << info;
			serialize(p);
			game_data->getNetworkManager()->client->sendPacket(0, &p);
		});
	}
}

void Player::update(float dt)
{
	if (!isOwner() || !game_data->window_active)
	{
		return;
	}

	dir = sf::Vector2f(0, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		dir.y -= 1;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		dir.x -= 1;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		dir.y += 1;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		dir.x += 1;
	}

	if (dir.x != 0 || dir.y != 0)
	{
		float length = std::sqrtf((dir.x * dir.x) + (dir.y * dir.y));
		dir /= length;
		sprite.move(dir * speed * dt);
	}

	auto mousePos = sf::Mouse::getPosition(*window);
	auto distance = static_cast<sf::Vector2f>(mousePos) - sprite.getPosition();
	float length = std::sqrtf((distance.x * distance.x) + (distance.y * distance.y));
	distance /= length;
	float rads = std::atan2(distance.y, distance.x);
	sprite.setRotation((rads * 180.0f) / 3.1415f);
}

void Player::draw(sf::RenderWindow& window_) const
{
	window_.draw(sprite);
}

void Player::serialize(Packet& p)
{
	p << sprite.getPosition().x
		<< sprite.getPosition().y;
	p.writeCompressedFloat(sprite.getRotation(), -360, 360, 0.01f);
}

void Player::deserialize(Packet& p)
{
	float x = p.read<float>();
	float y = p.read<float>();
	sprite.setPosition(x, y);
	float rot;
	p.readCompressedFloat(rot, -360, 360, 0.01f);
	sprite.setRotation(rot);
}
