#include "Scenegraph.hpp"

//SELF
#include "Networking/Networking/ServerHost.hpp"
#include "Networking/Networking/ClientHost.hpp"
#include "Networking/Networking/ClientStandard.hpp"

Scenegraph::Scenegraph(GameData* game_data)
	: game_data(game_data)
{
	console = spdlog::get("EnkiNet");
}

void Scenegraph::enableNetworking()
{
	if (network_ready)
	{
		return;
	}

	network_ready = true;

	if (game_data->getNetworkManager()->server)
	{
		mc1 = game_data->getNetworkManager()->server->on_packet_received.connect([this](Packet p)
		{
			if (p.getHeader().type == ENTITY_CREATION)
			{
				auto info = p.read<EntityInfo>();
				info.ownerID = p.info.senderID;
				createNetworkedEntity(info);
			}
			else if (p.getHeader().type == CONNECTED)
			{
				sendAllNetworkedEntitiesToClient(p.info.senderID);
			}
			else if (p.getHeader().type == ENTITY_UPDATE)
			{
				//Don't send entity updates back to the sender
				game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [sender=p.info.senderID](const ClientInfo& client)
				{
					return sender != client.id;
				});
			}
			else if (p.getHeader().type == ENTITY_RPC)
			{
				auto info = p.read<EntityInfo>();
				if (entityExists(info.ID))
				{
					auto ent = getEntity(info.ID);
					if (info == ent->info &&
						info.ownerID == p.info.senderID)
					{
						//Don't send entity updates back to the sender
						game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [sender = p.info.senderID](const ClientInfo& client)
						{
							return sender != client.id;
						});
					}
				}
			}
		});
	}
	
	if (game_data->getNetworkManager()->client)
	{
		mc2 = game_data->getNetworkManager()->client->on_packet_received.connect([this](Packet p)
		{
			if (p.getHeader().type == ENTITY_CREATION)
			{
				auto info = p.read<EntityInfo>();
				createEntity(info);
				//todo on ent creation also serialize all its stuff?
			}
			else if (p.getHeader().type == ENTITY_UPDATE)
			{
				auto info = p.read<EntityInfo>();
				if (entityExists(info.ID))
				{
					auto ent = getEntity(info.ID);
					if (info == ent->info)
					{
						ent->deserialize(p);
					}
					else
					{
						console->error("Received entity update with invalid info.\n\t{}\n\tVS\n\t{}\n\tSender ID = {}, Client ID = {}", info, ent->info, p.info.senderID, game_data->getNetworkManager()->client->getID());
					}
				}
				else
				{
					console->error("Received entity update for nonexistant entity.\n\t{}", info);
				}
			}
			else if (p.getHeader().type == ENTITY_RPC)
			{
				auto info = p.read<EntityInfo>();
				if (entityExists(info.ID))
				{
					auto ent = getEntity(info.ID);
					if (info == ent->info &&
						info.ownerID == p.info.senderID)
					{
						p.resetReadPosition();
						rpcs.receive(p, ent);
					}
				}
				else
				{
					console->error("Received an RPC packet for an entity that does not exist.\n\t{}", info);
				}
			}
		});
	}
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

void Scenegraph::registerEntity(std::string type, BuilderFunction builder)
{
	builders[type] = builder;
}

Entity* Scenegraph::createEntity(EntityInfo info)
{
	if (info.name == "" || info.type == "")
	{
		console->error("Invalid info when creating entity.\n\t{}", info);
	}

	if (info.ID == 0)
	{
		info.ID = localID--;
	}

	//info gets assigned to the entity here through being passed to the Entity base class constructor
	entities[info.ID] = builders.at(info.type)(info);
	entities[info.ID]->onSpawn();

	return entities[info.ID].get();
}

void Scenegraph::createNetworkedEntity(EntityInfo info)
{
	auto net_man = game_data->getNetworkManager();

	if (info.name == "" || info.type == "")
	{
		console->error("Invalid info when creating networked entity.\n\t{}", info);
		return;
	}

	if (net_man->server && info.ID == 0)
	{
		info.ID = ID++;
	}

	if (info.ownerID == 0 && network_ready)
	{
		if (net_man->client)
		{
			info.ownerID = net_man->client->getID();
		}
		else
		{
			info.ownerID = 1;
		}
	}

	if (network_ready)
	{
		Packet p({ PacketType::ENTITY_CREATION });
		p << info;

		if (net_man->server)
		{
			console->info("Creating networked entity as the server to all clients.\n\t{}", info);
			net_man->server->sendPacketToAllClients(0, &p);
		}
		else if (net_man->client)
		{
			console->info("Creating networked entity as the client to the server.\n\t{}", info);
			net_man->client->sendPacket(0, &p);
		}
		else
		{
			console->error("Tried to create networked entity when network isn't running");
		}
	}
	else
	{
		console->error("Tried to create networked entity when scenegraph isn't network ready");
	}
}

void Scenegraph::sendAllNetworkedEntitiesToClient(ClientID client_id)
{
	if (network_ready && game_data->getNetworkManager()->server)
	{
		for (auto& ent : entities)
		{
			EntityInfo info = ent.second->info;

			if (info.name == "" || info.type == "")
			{
				console->error("Invalid info when sending on connection of client {} for entity.\n\t{}", client_id, info);
			}
			else
			{
				console->info("Sending networked entity to client {}.\n\t{}", client_id, info);
			}

			{
				Packet p({ PacketType::ENTITY_CREATION });
				p << info;
				game_data->getNetworkManager()->server->sendPacketToOneClient(client_id, 0, &p);
			}

			{
				Packet p({ PacketType::ENTITY_UPDATE });
				p << info;
				ent.second->serialize(p);
				game_data->getNetworkManager()->server->sendPacketToOneClient(client_id, 0, &p);
			}
		}
	}
}

Entity* Scenegraph::getEntity(EntityID entityID)
{
	return entities.at(entityID).get();
}

bool Scenegraph::entityExists(EntityID entityID)
{
	return entities.count(entityID);
}