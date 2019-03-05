#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "../Architecture/Entity.hpp"

class Collision : public Entity
{
public:
	Collision(EntityInfo info, GameData* game_data);

	virtual void onSpawn();
	virtual void input(sf::Event& e);
	virtual void update(float dt);
	virtual void draw(sf::RenderWindow& window) const;

	virtual void serialize(Packet& p);
	virtual void deserialize(Packet& p);

private:
	sf::Rect<float> ball_collider;
	sf::Rect<float> paddle1_collider;
	sf::Rect<float> paddle2_collider;
};