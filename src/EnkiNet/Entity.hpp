#pragma once

//LIBS
//For whatever reason fmt and enet conflict a bit because of the ordering of winsock includes, so defining this fixes that
#define WIN32_LEAN_AND_MEAN
#include <spdlog/fmt/ostr.h>

//SELF
#include "Networking/Packet.hpp"
#include "GameData.hpp"
#include "Networking/Networking/Client.hpp"

namespace enki
{
	using EntityID = std::int32_t;

	struct EntityInfo
	{
		std::string type = "";
		std::string name = "";

		EntityID ID = 0;
		ClientID ownerID = 0;
		EntityID parentID = 0;
	};

	inline bool operator ==(const EntityInfo& lhs, const EntityInfo& rhs)
	{
		return lhs.ID == rhs.ID &&
			lhs.name == rhs.name &&
			lhs.type == rhs.type &&
			lhs.ownerID == rhs.ownerID &&
			lhs.parentID == rhs.parentID;
	}

	inline std::ostream& operator <<(std::ostream& os, const EntityInfo& info)
	{
		return os << "Type: " << info.type << ", Name: " << info.name << ", ID: " << info.ID << ", ownerID: " << info.ownerID << ", parentID: " << info.parentID;
	}

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

	class Entity
	{
	public:
		Entity(EntityInfo info, GameData* game_data)
			: info(info)
			, game_data(game_data)
		{
		}

		virtual ~Entity() = default;

		virtual void onSpawn() {};
		virtual void input([[maybe_unused]]sf::Event& e) {};
		virtual void update([[maybe_unused]]float dt) {};
		virtual void draw([[maybe_unused]]sf::RenderWindow& window) const {};
		virtual void serialize([[maybe_unused]]Packet& p) {}
		virtual void deserialize([[maybe_unused]]Packet& p) {}

		inline bool isOwner() const
		{
			if (!game_data->getNetworkManager()->client)
			{
				return info.ownerID == 0;
			}

			return game_data->getNetworkManager()->client->getID() == info.ownerID;
		}

		EntityInfo info;
		GameData* game_data;
		bool remove = false;

	private:
	};
}