#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "../Architecture/Entity.hpp"

class Ball : public Entity
{
public:
	Ball(EntityInfo info, GameData* game_data);

	virtual void onSpawn();
	virtual void input(sf::Event& e);
	virtual void update(float dt);
	virtual void draw(sf::RenderWindow& window) const;
	virtual void serialize(Packet& p);
	virtual void deserialize(Packet& p);

	sf::Sprite sprite;
	sf::Texture texture;

	bool moving_left = true;
	int y_dir = 1;
	float y_speed = 300;

private:
	ManagedConnection mc1;
	ManagedConnection mc2;
};