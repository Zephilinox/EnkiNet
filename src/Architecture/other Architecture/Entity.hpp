#pragma once

//LIB
#include <Engine/Renderer.h>

//SELF
#include "Messages/Message.hpp"
#include "Networking/Packet.hpp"
#include "GameData.hpp"
#include "Networking/Client.hpp"

class EntityManager;

struct EntityInfo
{
	uint32_t networkID;
	uint32_t ownerID;
	HashedID type;
};

inline Packet& operator <<(Packet& p, EntityInfo& e)
{
	p << e.networkID
		<< e.ownerID
		<< e.type;
	return p;
}

inline Packet& operator >>(Packet& p, EntityInfo& e)
{
	p >> e.networkID
		>> e.ownerID
		>> e.type;
	return p;
}

class Entity
{
public:
	Entity(GameData* game_data, EntityManager* ent_man, HashedID type = hash("Entity"))
		: game_data(game_data)
		, ent_man(ent_man)
	{
		entity_info.type = type;
	};
	
	virtual void onSpawn() {};

	virtual ~Entity() = default;
	virtual void update(float dt) = 0;
	virtual void render(ASGE::Renderer* renderer) const = 0;

	virtual void serialize(Packet& p) = 0;
	virtual void deserialize(Packet& p) = 0;

	inline bool isOwner()
	{
		return entity_info.ownerID == game_data->getNetworkManager()->client->getID();
	}

	inline void receivePacket(Packet&& p)
	{
		deserialize(p);
	}

	inline void sendPacket()
	{
		Packet p;
		p.setID(hash("Entity"));

		p << entity_info;
		serialize(p);

		game_data->getNetworkManager()->client->sendPacket(0, &p);
	}

	EntityInfo entity_info;
	GameData* game_data;
	EntityManager* ent_man;
};