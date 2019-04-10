#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>
#include <EnkiNet/Signals/Signal.hpp>
#include <SFML/Graphics.hpp>

//SELF
#include "CustomData.hpp"

class Player : public enki::Entity
{
public:
	Player(enki::EntityInfo info, enki::GameData* data, CustomData* custom_data, sf::RenderWindow* window);

	void onSpawn(enki::Packet& p) final;

	void update(float dt) final;
	void draw(sf::RenderWindow& window) const final;

	void serializeOnTick(enki::Packet& p) final;
	void deserializeOnTick(enki::Packet& p) final;
	
private:
	CustomData* custom_data;
	sf::RenderWindow* window;
	sf::View view;

	sf::Texture ship_tex;
	sf::Sprite ship;
	float speed = 300;
	sf::Vector2f velocity;
	float max_velocity_length = 600;

	enki::ManagedConnection mc1;
	enki::ManagedConnection mc2;
	enki::ManagedConnection mc3;
	
	enki::Timer shootTimer;
	float shootDelay = 0.1f;
};