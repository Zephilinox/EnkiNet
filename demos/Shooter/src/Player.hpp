#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>
#include <EnkiNet/Signals/Signal.hpp>
#include <SFML/Graphics.hpp>

class Player : public Entity
{
public:
	Player(EntityInfo info, GameData* data);

	void onSpawn();

	void input(sf::Event& e);
	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void serialize(Packet& p);
	void deserialize(Packet& p);

private:
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Vector2f dir;
	float speed = 300;

	ManagedConnection mc1;
	ManagedConnection mc2;
	ManagedConnection mc3;
};