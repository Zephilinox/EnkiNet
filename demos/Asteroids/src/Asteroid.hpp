#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>
#include <EnkiNet/Signals/Signal.hpp>
#include <SFML/Graphics.hpp>

//SELF
#include "CustomData.hpp"

class Asteroid : public enki::Entity
{
public:
	Asteroid(enki::EntityInfo info, enki::GameData* data, CustomData* custom_data, sf::RenderWindow* window);

	void onSpawn(enki::Packet& p) final;

	void update(float dt) final;
	void draw(sf::RenderWindow& window) const final;

	void serializeOnTick(enki::Packet& p) final;
	void deserializeOnTick(enki::Packet& p) final;
	
	void handleCollision();
	void split();

	bool isAlive() const;
	bool canSplit() const;
	sf::Vector2f getPosition() const;
	float getRadius() const;
	float getRotation() const;

private:
	void createShape(unsigned sides);

	sf::ConvexShape shape;

	CustomData* custom_data;
	sf::RenderWindow* window;

	float speed = 300;
	float radius = 10;
	float rotation_speed = 200;
	sf::Vector2f velocity = {1, 1};
	bool colliding = false;
	bool alive = true;
};