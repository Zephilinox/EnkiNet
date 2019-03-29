#pragma once

//STD
#include <vector>
#include <map>
#include <functional>

//LIBS
#include <spdlog/spdlog.h>

//SELF
#include "EnkiNet/Entity.hpp"
#include "EnkiNet/GameData.hpp"
#include "EnkiNet/Networking/RPC.hpp"
#include "EnkiNet/Networking/RPCManager.hpp"

namespace enki
{
	struct ChildEntityCreationInfo
	{
		std::string type;
		std::string name;
		Packet spawnInfo;
	};

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

		template <typename... Args>
		void registerEntityChildren(std::string type, Args... args);

		Entity* createEntity(EntityInfo info);
		void createNetworkedEntity(EntityInfo info);

		Entity* createEntity(EntityInfo info, Packet& spawnInfo);
		void createNetworkedEntity(EntityInfo info, Packet& spawnInfo);

		Entity* getEntity(EntityID entityID);
		bool entityExists(EntityID entityID);

		void deleteEntity(EntityID entityID);
		std::vector<Entity*> findEntitiesByType(std::string type) const;
		std::vector<Entity*> findEntitiesByName(std::string name) const;
		std::vector<Entity*> findEntitiesByOwner(ClientID owner) const;
		std::vector<Entity*> findEntitiesByParent(EntityID parent) const;
		std::vector<Entity*> findEntitiesByPredicate(std::function<bool(const Entity&)> predicate) const;

		template <typename T = Entity>
		T* findEntityByType(std::string type) const;

		template <typename T = Entity>
		T* findEntityByName(std::string name) const;

		template <typename T = Entity>
		T* findEntityByPredicate(std::function<bool(const Entity&)> predicate) const;

		RPCManager rpc_man;

	private:
		void sendAllNetworkedEntitiesToClient(ClientID client_id);

		std::map<EntityID, std::unique_ptr<Entity>> entities;
		std::map<std::string, std::vector<ChildEntityCreationInfo>> entities_child_types;
		std::map<std::string, BuilderFunction> builders;

		EntityID ID = 1;
		EntityID localID = -1;
		GameData* game_data;

		ManagedConnection mc1;
		ManagedConnection mc2;
		ManagedConnection mc3;

		bool network_ready = false;
		std::shared_ptr<spdlog::logger> console;
		std::uint32_t total_network_ticks = 0;
	};

	template <typename T, typename... Args>
	void Scenegraph::registerEntity(std::string type, Args... args)
	{
		builders[type] = [=](EntityInfo info)
		{
			return std::make_unique<T>(info, this->game_data, args...);
		};
	}

	template <typename... Args>
	void Scenegraph::registerEntityChildren(std::string type, Args... args)
	{
		entities_child_types[type] = { args... };

		for (const auto& child_type : entities_child_types[type])
		{
			if (!builders.count(child_type.type))
			{
				entities_child_types[type] = {};
				console->error("aaaaaaaaaaaaaaaaaaaaa");
				std::abort();
				//etc
			}
		}
	}

	template <typename T>
	T* Scenegraph::findEntityByType(std::string type) const
	{
		for (const auto& ent : entities)
		{
			if (ent.second->info.type == type)
			{
				return static_cast<T*>(ent.second.get());
			}
		}

		return nullptr;
	}

	template <typename T>
	T* Scenegraph::findEntityByName(std::string name) const
	{
		for (const auto& ent : entities)
		{
			if (ent.second->info.name == name)
			{
				return static_cast<T*>(ent.second.get());
			}
		}

		return nullptr;
	}

	template <typename T>
	T* Scenegraph::findEntityByPredicate(std::function<bool(const Entity&)> predicate) const
	{
		for (const auto& ent : entities)
		{
			if (predicate(*ent.second.get()))
			{
				return static_cast<T*>(ent.second.get());
			}
		}

		return nullptr;
	}
}