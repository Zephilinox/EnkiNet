#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>
#include <EnkiNet/Signals/Signal.hpp>
#include <SFML/Graphics.hpp>

//SELF
#include "CustomData.hpp"

class Bullet : public enki::Entity
{
public:
	Bullet(enki::EntityInfo info, enki::GameData* data, CustomData* custom_data, sf::RenderWindow* window);

	void onSpawn(enki::Packet& p) final;

	void update(float dt) final;
	void draw(sf::RenderWindow& window) const final;

	void serializeOnConnection(enki::Packet& p) final;
	void deserializeOnConnection(enki::Packet& p) final;
	void serializeOnTick(enki::Packet& p) final;
	void deserializeOnTick(enki::Packet& p) final;
	
	void handleCollision();
	sf::Vector2f getPosition() const;
	float getRotation() const;
	sf::Color getColour() const;
	unsigned int getWarpCount() const;

	bool isAlive() const;

private:
	CustomData* custom_data;
	sf::RenderWindow* window;

	sf::Texture bullet_tex;
	sf::Sprite bullet;

	float speed = 300;
	sf::Vector2f velocity = { 1, 1 };

	unsigned int warp_count = 0;
	bool alive = true;
};