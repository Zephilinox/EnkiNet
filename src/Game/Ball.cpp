#include "Ball.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

Ball::Ball(EntityInfo info)
	: Entity(info)
{
	fmt::print("ID={} name={} owner={} parent={} type={}\n", info.ID, info.name, info.ownerID, info.parentID, info.type);
	texture.loadFromFile("Ball.png");
	sprite.setTexture(texture);
	sprite.setPosition(150, 100);
}

void Ball::update(float dt)
{
	sprite.setPosition(sprite.getPosition() + sf::Vector2f{ 0.0f, 100 * dt});
}

void Ball::draw(sf::RenderWindow& window) const
{
	window.draw(sprite);
}
