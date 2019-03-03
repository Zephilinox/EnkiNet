#pragma once

//STD
#include <vector>
#include <map>
#include <functional>

//SELF
#include "Entity.hpp"
#include "GameData.hpp"
#include "Networking/RPC.hpp"

class Scenegraph
{
public:
	Scenegraph(GameData* game_data);

	void enable_networking();
	void input(sf::Event& e);
	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void registerBuilder(std::string type, std::function<std::unique_ptr<Entity>(EntityInfo)> builder);

	Entity* createEntity(EntityInfo info);
	void createNetworkedEntity(EntityInfo info);
	void sendNetworkedEntities();

	Entity* getEntity(uint32_t entityID);
	bool entityExists(uint32_t entityID);

	RPCManager rpcs;

private:
	std::map<uint32_t, std::unique_ptr<Entity>> entities;
	std::map<std::string, std::function<std::unique_ptr<Entity>(EntityInfo)>> builders;
	int ID = 1;
	int localID = 10000;
	GameData* game_data;
	
	ManagedConnection mc1;
	ManagedConnection mc2;

	bool network_ready = false;
};