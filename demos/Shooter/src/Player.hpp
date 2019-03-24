#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>
#include <EnkiNet/Signals/Signal.hpp>
#include <SFML/Graphics.hpp>

struct Line
{
	sf::Vertex start;
	sf::Vertex end;
	float life;
};

class Player : public enki::Entity
{
public:
	Player(enki::EntityInfo info, enki::GameData* data);

	void onSpawn();

	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void serialize(enki::Packet& p);
	void deserialize(enki::Packet& p);

private:
	void shoot(float x, float y);

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Vector2f dir;
	sf::Font font;
	sf::Text hpText;
	sf::Text playerName;
	float speed = 300;
	int hp = 10;

	enki::ManagedConnection mc1;
	enki::ManagedConnection mc2;
	enki::ManagedConnection mc3;

	std::vector<Line> lines;

	enki::Timer shootTimer;
	float shootDelay = 0.1f;
};