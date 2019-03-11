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

class Player : public Entity
{
public:
	Player(EntityInfo info, GameData* data, sf::RenderWindow* window);

	void onSpawn();

	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void serialize(Packet& p);
	void deserialize(Packet& p);

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

	ManagedConnection mc1;
	ManagedConnection mc2;
	ManagedConnection mc3;
	sf::RenderWindow* window = nullptr;

	std::vector<Line> lines;

	Timer shootTimer;
	float shootDelay = 0.1f;
};