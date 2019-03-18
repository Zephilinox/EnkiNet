#pragma once

//LIBS
#include <SFML/Graphics.hpp>
#include <EnkiNet/Entity.hpp>

class Ball : public enki::Entity
{
public:
	Ball(enki::EntityInfo info, enki::GameData* game_data);

	void onSpawn() final;
	void update(float dt) final;
	void draw(sf::RenderWindow& window) const final;
	void serialize(enki::Packet& p) final;
	void deserialize(enki::Packet& p) final;

	sf::Sprite sprite;
	sf::Texture texture;

	bool moving_left = true;
	int y_dir = 1;
	float y_speed = 300;

private:
	enki::ManagedConnection mc1;
	enki::ManagedConnection mc2;
};