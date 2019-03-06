#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "../Architecture/Entity.hpp"
#include "../Architecture/Timer.hpp"

class Paddle : public Entity
{
public:
	Paddle(EntityInfo info, GameData* game_data);

	virtual void onSpawn();

	virtual void input(sf::Event& e);
	virtual void update(float dt);
	virtual void draw(sf::RenderWindow& window) const;

	virtual void serialize(Packet& p);
	virtual void deserialize(Packet& p);

	sf::Sprite sprite;
	sf::Texture texture;

	void setColour(int r, int g, int b);

private:
	ManagedConnection mc1;
	bool interpolation_enabled = false;
	sf::Sprite latest_sprite;
	float latest_network_y = 0;
	float last_interpolation_y = 0;
	float interpolation_y = 0;
	Timer interpolation_timer;
};