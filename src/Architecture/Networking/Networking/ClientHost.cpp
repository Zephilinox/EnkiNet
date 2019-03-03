#include "ClientHost.hpp"

//SELF
#include "../../Entity.hpp"

void ClientHost::initialize()
{
	initialized = true;
	auto console = spdlog::get("console");
	console->info("Client Initialized");
	
	mc1 = on_packet_received.connect([](Packet p)
	{
		if (p.get_header().type == PacketType::ENTITY)
		{
			auto console = spdlog::get("console");
			EntityInfo info;
			p >> info;
			//console->info("EntityPacket for {} owned by {} of type {}", info.ID, info.ownerID, info.type);
		}
	});
}

void ClientHost::deinitialize()
{
	initialized = false;
	auto console = spdlog::get("console");
	console->info("Client Deinitialized");
}

void ClientHost::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	auto console = spdlog::get("console");
	//console->info("client sending packet");
	p->info.senderID = id;
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}