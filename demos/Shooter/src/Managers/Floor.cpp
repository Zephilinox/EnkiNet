#include "Floor.hpp"

Floor::Floor(enki::EntityInfo info, enki::GameData* game_data)
	: Entity(info, game_data)
{
	if (!texture.loadFromFile("resources/floor.png"))
	{
		auto console = spdlog::get("console");
		console->error("Failed to load floor");
	}

	sprite.setTexture(texture);
}

void Floor::draw(sf::RenderWindow& window) const
{
	window.draw(sprite);
}
