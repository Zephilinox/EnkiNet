#include "ClientHost.hpp"

//SELF
#include "../Entity.hpp"

void ClientHost::initialize()
{
	initialized = true;
	std::cout << "Client Initialized\n";
	
	mc1 = on_packet_received.connect([](Packet p)
	{
		if (p.getID() == hash("Entity"))
		{
			EntityInfo info;
			p >> info;
			std::cout << "EntityPacket for " << info.networkID << " owned by " << info.ownerID << " of type " << info.type << "\n";
		}
	});
}

void ClientHost::deinitialize()
{
	initialized = false;
	std::cout << "Client Deinitialized\n";
}

void ClientHost::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	std::cout << "client sending packet\n";
	p->senderID = id;
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}