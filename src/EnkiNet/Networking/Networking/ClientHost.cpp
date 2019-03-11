#include "ClientHost.hpp"

//SELF
#include "../../Entity.hpp"

void ClientHost::initialize()
{
	initialized = true;
	auto console = spdlog::get("EnkiNet");
	console->info("Client Initialized");
}

void ClientHost::deinitialize()
{
	initialized = false;
	auto console = spdlog::get("EnkiNet");
	console->info("Client Deinitialized");
}

void ClientHost::sendPacket([[maybe_unused]]enet_uint8 channel_id, Packet* p, [[maybe_unused]]enet_uint32 flags)
{
	auto header = p->getHeader();
	header.timeSent = enet_time_get();
	p->setHeader(header);

	auto console = spdlog::get("EnkiNet");
	//console->info("client sending packet");
	p->info.senderID = id;
	p->info.timeReceived = enet_time_get();
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}