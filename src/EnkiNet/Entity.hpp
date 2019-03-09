#pragma once

//LIBS
#include <SFML/Graphics.hpp>

//SELF
#include "Networking/Packet.hpp"
#include "GameData.hpp"
#include "Networking/Networking/Client.hpp"

struct EntityInfo
{
	std::string type = "";
	std::string name = "";

	uint32_t ID = 0;
	uint32_t ownerID = 0;
	uint32_t parentID = 0;
};

inline Packet& operator <<(Packet& p, EntityInfo& e)
{
	p << e.ID
		<< e.ownerID
		<< e.type
		<< e.name
		<< e.parentID;
	return p;
}

inline Packet& operator >>(Packet& p, EntityInfo& e)
{
	
	p >> e.ID
		>> e.ownerID
		>> e.type
		>> e.name
		>> e.parentID;
	return p;
}

struct Entity
{
public:
	Entity(EntityInfo info, GameData* game_data);
	virtual ~Entity() = default;

	virtual void onSpawn() = 0;

	virtual void input(sf::Event& e) = 0;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) const = 0;

	inline bool isOwner() const
	{
		if (!game_data->getNetworkManager()->client)
		{
			return info.ownerID == 0;
		}

		return game_data->getNetworkManager()->client->getID() == info.ownerID;
	}

	//not pure virtual, just so that users don't have to write out an empty function for non-networked entities
	virtual void serialize([[maybe_unused]]Packet& p)
	{

	}

	virtual void deserialize([[maybe_unused]]Packet& p)
	{

	}

	EntityInfo info;
	GameData* game_data;

private:
};