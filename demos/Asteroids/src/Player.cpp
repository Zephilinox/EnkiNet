#include "Player.hpp"

//STD
#include <experimental/vector>

//LIBS
#include <EnkiNet/Scenegraph.hpp>

Player::Player(enki::EntityInfo info, enki::GameData* data, CustomData* custom_data, sf::RenderWindow* window)
	: Entity(info, data)
	, custom_data(custom_data)
	, window(window)
{
	network_tick_rate = 1;
	//game_data->scenegraph->rpc_man.add(enki::RPCType::RemoteAndLocal, "Player", "shoot", &Player::shoot);
}

void Player::onSpawn([[maybe_unused]]enki::Packet& p)
{
	auto console = spdlog::get("console");
	if (!ship_tex.loadFromFile("resources/ship.png"))
	{
		console->error(":(");
	}

	ship.setTexture(ship_tex);
	ship.setOrigin(ship_tex.getSize().x / 2,
		ship_tex.getSize().y / 2);
	ship.setPosition(1280 / 2, 720 / 2);

	if (info.ownerID == 1)
	{
		ship.setColor(sf::Color(0, 100, 200)); //blue
	}
	else if (info.ownerID == 2)
	{
		ship.setColor(sf::Color(200, 60, 60)); //red
	}
	else if (info.ownerID == 3)
	{
		ship.setColor(sf::Color(60, 200, 60)); //green
	}
	else if (info.ownerID == 4)
	{
		ship.setColor(sf::Color(200, 160, 60)); //orange
	}

	view = window->getDefaultView();
}

void Player::update(float dt)
{
	if (!isOwner() || !custom_data->window_active)
	{
		return;
	}

	auto input_manager = custom_data->input_manager;

	if (input_manager->isMouseButtonDown(sf::Mouse::Button::Left))
	{
		if (shootTimer.getElapsedTime() > shootDelay)
		{
			sf::Vector2f pos = input_manager->getMouseWorldPos();
			//game_data->scenegraph->rpc_man.call(&Player::shoot, "shoot", game_data->network_manager, this, pos.x, pos.y);
			shootTimer.restart();
		}
	}
	
	float shipRotRads = ship.getRotation() * (3.1415 / 180.0f);
	float shipSin = std::sin(shipRotRads);
	float shipCos = std::cos(shipRotRads);

	if (input_manager->isKeyDown(sf::Keyboard::Key::W))
	{
		velocity.x += speed * shipSin * dt;
		velocity.y += -1 * speed * shipCos * dt;
	}

	if (input_manager->isKeyDown(sf::Keyboard::Key::S))
	{
		velocity.x += -1 * speed * shipSin * dt;
		velocity.y += speed * shipCos * dt;
	}

	if (input_manager->isKeyDown(sf::Keyboard::Key::A))
	{
		ship.rotate(-200 * dt);
	}

	if (input_manager->isKeyDown(sf::Keyboard::Key::D))
	{
		ship.rotate(200 * dt);
	}

	float length = std::sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y));

	if (input_manager->isKeyDown(sf::Keyboard::Key::LShift))
	{
		if (length > 5 && velocity.x != 0 && velocity.y != 0)
		{
			sf::Vector2f normVelocity;
			if (length != 0)
			{
				normVelocity.x = velocity.x / length;
				normVelocity.y = velocity.y / length;
			}

			velocity.x -= speed * normVelocity.x * dt;
			velocity.y -= speed * normVelocity.y * dt;
		}
		else
		{
			velocity.x = 0;
			velocity.y = 0;
		}
	}
	
	length = std::sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y));

	if (length > max_velocity_length)
	{
		velocity.x *= max_velocity_length / length;
		velocity.y *= max_velocity_length / length;
	}

	ship.move(velocity * dt);

	if (ship.getPosition().x + (ship_tex.getSize().x / 2) <= 0)
	{
		ship.setPosition(window->getView().getSize().x, ship.getPosition().y);
	}
	else if (ship.getPosition().x - (ship_tex.getSize().x / 2) >= window->getView().getSize().x)
	{
		ship.setPosition(0, ship.getPosition().y);
	}
	else if (ship.getPosition().y + (ship_tex.getSize().y / 2) <= 0)
	{
		ship.setPosition(ship.getPosition().x, window->getView().getSize().y);
	}
	else if (ship.getPosition().y - (ship_tex.getSize().y / 2) >= window->getView().getSize().y)
	{
		ship.setPosition(ship.getPosition().x, 0);
	}

	if (input_manager->isKeyDown(sf::Keyboard::Key::F1))
	{
		handleCollision();
	}
}

void Player::draw(sf::RenderWindow& window_) const
{
	window_.draw(ship);

	if (was_damaged && flashing_timer.getElapsedTime() < flashing_duration)
	{
		int milli = flashing_timer.getElapsedTime<enki::Timer::milliseconds>();
		int percentage = 20;
		int rem_milli = milli % 1000 % (percentage * 10); //ignore seconds and get a percentage of the remainder
		if (rem_milli < percentage * 5)
		{
			window_.draw(ship);
		}
	}
}

void Player::serializeOnTick(enki::Packet& p)
{
	p.writeCompressedFloat(ship.getPosition().x, 0, 1280, 0.01f);
	p.writeCompressedFloat(ship.getPosition().y, 0, 720, 0.01f);
	p.writeCompressedFloat(ship.getRotation(), 0, 360, 0.01f);
}

void Player::deserializeOnTick(enki::Packet& p)
{
	float x = p.readCompressedFloat(0, 1280, 0.01f);
	float y = p.readCompressedFloat(0, 720, 0.01f);
	ship.setPosition(x, y);
	ship.setRotation(p.readCompressedFloat(0, 360, 0.01f));
}

sf::Vector2f Player::getPosition() const
{
	return ship.getPosition();
}

bool Player::isInvincible() const
{
	return was_damaged;
}

int Player::getLives() const
{
	return lives;
}

void Player::handleCollision()
{
	if (lives > 0)
	{
		lives--;
	}

	was_damaged = true;
	flashing_timer.restart();
}
