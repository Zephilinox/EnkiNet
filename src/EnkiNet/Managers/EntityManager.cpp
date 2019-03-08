#include "EntityManager.hpp"

//STD
#include <iostream>

//SELF
#include "../GameData.hpp"
#include "../Networking/Server.hpp"
#include "../../Client/Unit.hpp"
#include "../../Client/TerrainTile.h"

EntityManager::EntityManager(GameData* game_data)
	: game_data(game_data)
{
	auto serverReceivePacket = [this](Packet p)
	{			
		switch(p.getID())
		{
			case hash("Entity"):
			{
				EntityInfo info;
				p >> info;
				Entity* ent = getEntity(info.networkID);

				//Make sure the entity packet is all valid, then send it to the other client.
				if (ent && //exists
					ent->entity_info.ownerID == info.ownerID && //owners match
					ent->entity_info.type == info.type && //types match
					info.ownerID == p.senderID) //client owns it
				{
					this->game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [senderID = p.senderID](const ClientInfo& ci)
					{
						return ci.id != senderID;
					});
				}
				else
				{
					std::cout << "uhoh... server received invalid Entitypacketfrom client " + p.senderID << "\n";
				}
			} break;
		}	
	};

	auto clientReceivePacket = [this](Packet p)
	{
		switch (p.getID())
		{
			case hash("Entity"):
			{
				//Find the entity and give them the packet to deserialize
				EntityInfo info;
				p >> info;
				Entity* ent = getEntity(info.networkID);

				if (ent)
				{
					if (!ent->isOwner())
					{
						ent->receivePacket(Packet(p));
					}
					else
					{
						std::cout << "huh... received EntityPacket for an entity that we own?\n";
					}
				}
				else
				{
					std::cout << "uhoh, received EntityPacket for an entity that doesn't exist?\n";
				}
			} break;
		}
	};

	if (game_data->getNetworkManager()->server)
	{
		mc1 = game_data->getNetworkManager()->server->on_packet_received.connect(std::move(serverReceivePacket));
	}

	mc2 = game_data->getNetworkManager()->client->on_packet_received.connect(std::move(clientReceivePacket));
}

void EntityManager::update(float dt)
{
	for (auto& ent : entities)
	{
		ent->update(dt);
	}
}

void EntityManager::render() const
{
	for (const auto& ent : entities)
	{
		if (ent->entity_info.type == hash("Unit"))
		{
			Unit* unit = static_cast<Unit*>(ent.get());
			ASGE::Sprite* unit_sprite = unit->getCurrentSprite();
			if (!withinView(unit_sprite))
			{
				continue;
			}

			if(unit->getCurrentTile()->getIsVisible())
			{
				unit_sprite->xPos(unit->getXPosition() - offset_x);
				unit_sprite->yPos(unit->getYPosition() - offset_y);

				ent->render(game_data->getRenderer());

				unit_sprite->xPos(unit->getXPosition() + offset_x);
				unit_sprite->yPos(unit->getYPosition() + offset_y);
			}
		}
		else
		{
			ent->render(game_data->getRenderer());
		}
	}
}

Entity* EntityManager::getEntity(uint32_t networkID)
{
	auto it = std::find_if(entities.begin(), entities.end(), [&](const auto& ent)
	{
		return ent->entity_info.networkID == networkID;
	});

	if (it != entities.end())
	{
		return it->get();
	}

	return nullptr;
}

void EntityManager::applyOffset(float x, float y)
{
	offset_x = x;
	offset_y = y;
}

bool EntityManager::withinView(ASGE::Sprite* sprite) const
{
	float x = sprite->xPos() - offset_x;
	float y = sprite->yPos() - offset_y;

	if (x + sprite->width()> 0 &&
		x < game_data->getWindowWidth() &&
		y + sprite->height() > 0 &&
		y < game_data->getWindowHeight())
	{
		return true;
	}

	return false;
}
