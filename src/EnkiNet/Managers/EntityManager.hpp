#pragma once

//STD
#include <vector>

//SELF
#include "../Entity.hpp"
#include "../GameData.hpp"

class EntityManager
{
public:
	EntityManager(GameData* game_data);

	void update(float dt);
	void render() const;

	template <class T>
	void createEntityRequest();

	template <class T>
	void createEntityForClient(uint32_t ownerID);

	template <class T>
	Entity* spawnEntity(EntityInfo info);

	Entity* getEntity(uint32_t networkID);

	void applyOffset(float x, float y);

	//refactor stuff so that this can stay private
	std::vector<std::unique_ptr<Entity>> entities;

private:
	bool withinView(ASGE::Sprite* sprite) const;

	GameData* game_data;
	uint32_t next_network_id = 1;

	ManagedConnection mc1;
	ManagedConnection mc2;

	float offset_x = 0;
	float offset_y = 0;
};

template <class T>
void EntityManager::createEntityRequest()
{
	assert(game_data->getNetworkManager()->client);
	std::cout << "createEntityRequest\n";
	//Ask server to create our entity
	T ent(game_data, this);
	ent.entity_info.ownerID = game_data->getNetworkManager()->client->getID();

	Packet p;
	p.setID(hash("CreateEntity"));
	p.senderID = game_data->getNetworkManager()->client->getID();
	p << ent.entity_info;

	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

template <class T>
void EntityManager::createEntityForClient(uint32_t ownerID)
{
	assert(game_data->getNetworkManager()->server);
	std::cout << "createEntityForClient " << ownerID << "\n";
	T ent(game_data, this);
	EntityInfo info = ent.entity_info;
	info.networkID = next_network_id;
	next_network_id++;
	info.ownerID = ownerID;

	//Tell clients to create this entity
	Packet p;
	p.setID(hash("CreateEntity"));
	p.senderID = 1;
	p << info;

	game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
}

template <class T>
Entity* EntityManager::spawnEntity(EntityInfo info)
{
	assert(game_data->getNetworkManager()->client);

	std::cout << "spawning entity\n";

	entities.emplace_back(std::make_unique<Unit>(this->game_data, this));
	auto ent = entities.back().get();
	ent->entity_info = info;
	ent->onSpawn();

	return ent;
}