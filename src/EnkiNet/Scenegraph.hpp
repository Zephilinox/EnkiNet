#pragma once

//STD
#include <vector>
#include <map>
#include <functional>

//LIBS
#include <spdlog/spdlog.h>

//SELF
#include "Entity.hpp"
#include "GameData.hpp"
#include "Networking/RPC.hpp"

class Scenegraph
{
public:
	using BuilderFunction = std::function<std::unique_ptr<Entity>(EntityInfo)>;

	Scenegraph(GameData* game_data);

	void enableNetworking();
	void input(sf::Event& e);
	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void registerEntity(std::string type, BuilderFunction builder);

	template <typename T, typename... Args>
	void registerEntity(std::string type, Args... args);
	
	Entity* createEntity(EntityInfo info);
	void createNetworkedEntity(EntityInfo info);

	Entity* getEntity(EntityID entityID);
	bool entityExists(EntityID entityID);

	RPCManager rpcs;

private:
	void sendAllNetworkedEntitiesToClient(ClientID client_id);

	std::map<uint32_t, std::unique_ptr<Entity>> entities;
	std::map<std::string, BuilderFunction> builders;

	int ID = 1;
	int localID = -1;
	GameData* game_data;
	
	ManagedConnection mc1;
	ManagedConnection mc2;

	bool network_ready = false;
	std::shared_ptr<spdlog::logger> console;
};

template <typename T, typename... Args>
void Scenegraph::registerEntity(std::string type, Args... args)
{
	builders[type] = [=](EntityInfo info)
	{
		return std::make_unique<T>(info, this->game_data, args...);
	};
}