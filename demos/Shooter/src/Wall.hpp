#pragma once

//LIBS
#include <EnkiNet/Entity.hpp>

class Wall : public enki::Entity
{
public:
	Wall(enki::EntityInfo info, enki::GameData* game_data);

	void onSpawn(enki::Packet& p) final;
	void draw(sf::RenderWindow& window) const final;

	void serializeOnConnection(enki::Packet& p) final;
	void deserializeOnConnection(enki::Packet& p) final;

private:
	sf::Texture texture;
	sf::Sprite sprite;
};