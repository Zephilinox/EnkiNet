#pragma once

//STD
#include <vector>

//LIB
#include <SFML/Graphics.hpp>
#include <EnkiNet/Scenegraph.hpp>

enum Tile
{
	Unknown = -1,
	Floor,
	Wall,
	HealthPickup,
	Spawnpoint
};

class MapManager
{
public:
	MapManager(enki::Scenegraph* scenegraph, enki::NetworkManager* network_manager);

	void draw(sf::RenderWindow& window) const;

	Tile getTile(sf::Vector2i mapPos);

	sf::Vector2i worldPosToMapPos(sf::Vector2f);
	sf::Vector2f mapPosToWorldPos(sf::Vector2i);

private:
	int width;
	int height;
	int tileSize = 64;
	std::vector<std::vector<Tile>> map;
	enki::Scenegraph* scenegraph;
	enki::NetworkManager* network_manager;
};