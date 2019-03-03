#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "../Architecture/Entity.hpp"

class PlayerText : public Entity
{
public:
	PlayerText(EntityInfo info, GameData* game_data);

	virtual void input(sf::Event& e);
	virtual void update(float dt);
	virtual void draw(sf::RenderWindow& window) const;

	virtual void serialize(Packet& p);
	virtual void deserialize(Packet& p);

private:
	sf::Font font;
	sf::Text label;
};