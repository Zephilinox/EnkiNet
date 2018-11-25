#include "Paddle.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

Paddle::Paddle(EntityInfo info)
	: Entity(info)
{
	fmt::print("ID={} name={} owner={} parent={} type={}\n", info.ID, info.name, info.ownerID, info.parentID, info.type);
	texture.loadFromFile("Paddle.png");
	sprite.setTexture(texture);

	if (info.name == "Paddle 1")
	{
		sprite.setPosition(100, 100);
	}
	else
	{
		sprite.setPosition(200, 100);
	}
}

void Paddle::update(float dt)
{
}

void Paddle::draw(sf::RenderWindow& window) const
{
	window.draw(sprite);
}
