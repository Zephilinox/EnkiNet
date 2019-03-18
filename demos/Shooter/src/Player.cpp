#include "Player.hpp"

//STD
#include <experimental/vector>

//LIBS
#include <EnkiNet/Scenegraph.hpp>

Player::Player(enki::EntityInfo info, enki::GameData* data, sf::RenderWindow* window)
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

	if (!font.loadFromFile("resources/arial.ttf"))
	{
		console->error(":((");
	}
	playerName.setFont(font);
	playerName.setScale(0.3f, 0.3f);
	playerName.setString(info.name);

	hpText.setFont(font);
	hpText.setScale(0.3f, 0.3f);
	hpText.setFillColor(sf::Color::Black);

	if (info.ownerID == 1)
	{
		sprite.setColor(sf::Color(0, 100, 200));
		playerName.setFillColor(sf::Color(0, 100, 200));
	}
	else
	{
		sprite.setColor(sf::Color(200, 60, 60));
		playerName.setFillColor(sf::Color(200, 60, 60));
	}

	if (isOwner())
	{
		mc1 = game_data->getNetworkManager()->on_network_tick.connect([this]()
		{
			enki::Packet p({ enki::PacketType::ENTITY_UPDATE });
			p << info;
			serialize(p);
			game_data->getNetworkManager()->client->sendPacket(0, &p);
		});
	}

	game_data->scenegraph->rpcs.add("Player", "shoot", &Player::shoot);
}

void Player::update(float dt)
{
	for (auto& line : lines)
	{
		float lifeRemaining = line.life / 0.25f;
		line.start.color.a = static_cast<std::uint8_t>(255 * lifeRemaining);
		line.life -= dt;
	}

	std::experimental::erase_if(lines, [](auto& line)
	{
		return line.life <= 0;
	});

	playerName.setPosition(sprite.getPosition().x - 30, sprite.getPosition().y - 30);
	hpText.setPosition(sprite.getPosition().x + 10, sprite.getPosition().y - 30);
	hpText.setString("HP: " + std::to_string(hp));

	if (!isOwner() || !game_data->window_active)
	{
		return;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		if (shootTimer.getElapsedTime() > shootDelay)
		{
			sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));
			game_data->scenegraph->rpcs.call(&Player::shoot, "shoot", game_data->getNetworkManager(), this, pos.x, pos.y);
			shootTimer.restart();
		}
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
	window_.draw(playerName);
	window_.draw(hpText);

	sf::Vertex vertices[2];
	for (const auto& line : lines)
	{
		vertices[0] = line.start;
		vertices[1] = line.end;
		window_.draw(vertices, 2, sf::Lines);
	}
}

void Player::serialize(enki::Packet& p)
{
	//15 bits vs 96 (3 floats)
	p.writeCompressedFloat(sprite.getPosition().x, 0, 640, 0.01f);
	p.writeCompressedFloat(sprite.getPosition().y, 0, 360, 0.01f);
	p.writeCompressedFloat(sprite.getRotation(), 0, 360, 0.01f);
}

void Player::deserialize(enki::Packet& p)
{
	float x = p.readCompressedFloat(0, 640, 0.01f);
	float y = p.readCompressedFloat(0, 360, 0.01f);
	sprite.setPosition(x, y);
	sprite.setRotation(p.readCompressedFloat(0, 360, 0.01f));
}

void Player::shoot(float x, float y)
{
	Line line;
	auto start = sprite.getPosition();
	float rads = sprite.getRotation() * (3.1415f / 180.0f);
	sf::Vector2f rot_vector(std::cos(rads), std::sin(rads));
	rot_vector *= 30.0f;
	start += rot_vector;
	line.start = sf::Vertex(start, sf::Color(0, 0, 0, 255));
	line.end = sf::Vertex(sf::Vector2f(x, y), sf::Color(0, 0, 0, 0));
	line.life = 0.25f;

	lines.push_back(line);
}
