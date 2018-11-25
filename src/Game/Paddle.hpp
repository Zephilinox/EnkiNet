#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "../Architecture/Entity.hpp"

class Paddle : public Entity
{
public:
	Paddle(EntityInfo info);

	virtual void update(float dt);
	virtual void draw(sf::RenderWindow& window) const;

	sf::Sprite sprite;
	sf::Texture texture;
};