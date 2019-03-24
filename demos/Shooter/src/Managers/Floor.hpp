#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>

class Floor : public enki::Entity
{
public:
	Floor(enki::EntityInfo info, enki::GameData* game_data);
	
	void draw(sf::RenderWindow& window) const final;

private:
	sf::Texture texture;
	sf::Sprite sprite;
};