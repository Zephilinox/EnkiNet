#include "Scenegraph.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

//SELF
#include "Networking/Networking/ServerHost.hpp"
#include "Networking/Networking/ClientHost.hpp"
#include "Networking/Networking/ClientStandard.hpp"

Scenegraph::Scenegraph(GameData* game_data)
	: game_data(game_data)
{
}

void Scenegraph::enable_networking()
{
	if (network_ready)
	{
		return;
	}

	if (game_data->getNetworkManager()->server)
	{
		mc1 = game_data->getNetworkManager()->server->on_packet_received.connect([this](Packet p)
		{
			if (p.get_header().type == ENTITY_CREATION)
			{
				EntityInfo info;
				p >> info;
				info.ownerID = p.info.senderID;
				createNetworkedEntity(info);
			}

			if (p.get_header().type == CONNECTED)
			{
				sendNetworkedEntities();
				createNetworkedEntity(EntityInfo{ "Paddle", "Paddle 2", 0, p.info.senderID });
			}

			if (p.get_header().type == ENTITY)
			{
				game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
			}

			if (p.get_header().type == ENTITY_RPC)
			{
				EntityInfo info;
				p >> info;
				if (info.ownerID == p.info.senderID)
				{
					game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
				}
			}
		});
	}
	
	if (game_data->getNetworkManager()->client)
	{
		mc2 = game_data->getNetworkManager()->client->on_packet_received.connect([this](Packet p)
		{
			if (p.get_header().type == ENTITY_CREATION)
			{
				EntityInfo info;
				p >> info;
				createEntity(info);
			}

			if (p.get_header().type == ENTITY)
			{
				EntityInfo info;
				p >> info;
				if (entities.count(info.ID))
				{
					auto ent = getEntity(info.ID);
					//when a listen server client sends a fake packet to the server, the server will send it back to the client.
					//Probably no harm if we didn't exclude it since there should be no delay, but we will anyway.
					if (info.ownerID == p.info.senderID &&
						game_data->getNetworkManager()->client->getID() != p.info.senderID)
					{
						ent->deserialize(p);
					}
				}
			}

			if (p.get_header().type == ENTITY_RPC)
			{
				EntityInfo info;
				p >> info;
				std::string name;
				p >> name;
				if (entityExists(info.ID))
				{
					p.reset_read_position();
					auto ent = getEntity(info.ID);
					rpcs.receive(p, ent);
				}
				else
				{
					auto console = spdlog::get("console");
					console->error("Received an RPC packet for an entity that does not exist");
				}
			}
		});
	}

	network_ready = true;
}

void Scenegraph::input(sf::Event& e)
{
	for (auto& ent : entities)
	{
		ent.second->input(e);
	}
}

void Scenegraph::update(float dt)
{
	for (auto& ent : entities)
	{
		ent.second->update(dt);
	}
}

void Scenegraph::draw(sf::RenderWindow& window) const
{
	for (const auto& ent : entities)
	{
		ent.second->draw(window);
	}
}

void Scenegraph::registerBuilder(std::string type, std::function<std::unique_ptr<Entity>(EntityInfo)> builder)
{
	builders[type] = builder;
}

void Scenegraph::registerReceiver(std::string type, std::function<void(Entity*, Packet, RPCManager*)> receiver)
{
	rpc_receivers[type] = receiver;
}

Entity* Scenegraph::createEntity(EntityInfo info)
{
	if (info.name == "" || info.type == "")
	{
		auto console = spdlog::get("console");
		console->error("No name or type for this entity info");
		console->error("name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
	}

	if (info.ID == 0)
	{
		info.ID = localID++;
	}

	//info gets assigned to the entity here through being passed to the Entity base class constructor
	entities[info.ID] = builders.at(info.type)(info);
	entities[info.ID]->onSpawn();

	return entities[info.ID].get();
}

void Scenegraph::createNetworkedEntity(EntityInfo info)
{
	auto console = spdlog::get("console");
	auto net_man = game_data->getNetworkManager();

	if (info.name == "" || info.type == "")
	{
		console->error("No name or type for this networked entity info");
		console->error("name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
	}
	else
	{
		console->info("Creating networked. name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
	}

	if (game_data->getNetworkManager()->server)
	{
		if (info.ID == 0)
		{
			info.ID = ID++;
		}
	}

	if (info.ownerID == 0)
	{
		if (network_ready && net_man->client)
		{
			info.ownerID = net_man->client->getID();
		}
		else if (network_ready)
		{
			info.ownerID = 1;
		}
	}

	if (network_ready && net_man->server)
	{
		Packet p({ PacketType::ENTITY_CREATION });
		p << info;
		if (info.type == "")
		{
			console->error("No type1");
			console->error("name {} type {} ID {} ownerID {} parentID {} senderID {}", info.name, info.type, info.ID, info.ownerID, info.parentID, p.info.senderID);
		}
		else
		{
			console->info("Creating networked as server to all clients. name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
		}
		net_man->server->sendPacketToAllClients(0, &p);
	}
	else if (network_ready && net_man->client)
	{
		Packet p({ PacketType::ENTITY_CREATION });
		p << info;
		if (info.type == "")
		{
			console->error("No type2");
			console->error("name {} type {} ID {} ownerID {} parentID {} senderID {}", info.name, info.type, info.ID, info.ownerID, info.parentID, p.info.senderID);
		}
		else
		{
			console->info("Creating networked as client to server. name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
		}
		net_man->client->sendPacket(0, &p);
	}
	else
	{
		console->error("Tried to create networked entity when network is not running or scenegraph isn't network ready");
	}
}

void Scenegraph::sendNetworkedEntities()
{
	if (network_ready && game_data->getNetworkManager()->server)
	{
		auto console = spdlog::get("console");
		for (auto& ent : entities)
		{
			EntityInfo info = ent.second->info;

			if (info.type == "")
			{
				console->error("No type3");
				console->error("name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
			}
			else
			{
				console->info("sending networked. name {} type {} ID {} ownerID {} parentID {}", info.name, info.type, info.ID, info.ownerID, info.parentID);
			}

			{
				Packet p({ PacketType::ENTITY_CREATION });
				p << info;
				game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [](const ClientInfo& client)
				{
					if (client.id == 1)
					{
						return false;
					}

					return true;
				});
			}

			{
				Packet p({ PacketType::ENTITY });
				p << info;
				ent.second->serialize(p);

				game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [](const ClientInfo& client)
				{
					if (client.id == 1)
					{
						return false;
					}

					return true;
				});
			}
		}
	}
}

Entity* Scenegraph::getEntity(uint32_t entityID)
{
	return entities.at(entityID).get();
}

bool Scenegraph::entityExists(uint32_t entityID)
{
	return entities.count(entityID);
}
