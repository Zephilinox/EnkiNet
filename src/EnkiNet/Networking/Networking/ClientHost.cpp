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
		if (p.getHeader().type == PacketType::ENTITY)
		{
			auto console = spdlog::get("console");
			auto info = p.read<EntityInfo>();
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

void ClientHost::sendPacket([[maybe_unused]]enet_uint8 channel_id, Packet* p, [[maybe_unused]]enet_uint32 flags)
{
	auto console = spdlog::get("console");
	//console->info("client sending packet");
	p->info.senderID = id;
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}