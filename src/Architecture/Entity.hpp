#pragma once

//LIBS
#include <SFML/Graphics.hpp>

struct EntityInfo
{
	std::string type = "";
	std::string name = "";

	uint32_t ID = 0;
	uint32_t ownerID = 0;
	uint32_t parentID = 0;
};

struct Entity
{
public:
	Entity(EntityInfo info);

	virtual ~Entity() = default;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) const = 0;

	inline bool isOwner()
	{
		//todo
	}

	EntityInfo info;

private:
};