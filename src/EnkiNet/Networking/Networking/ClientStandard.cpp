#include "ClientStandard.hpp"

//STD
#include <iostream>

//SELF
#include "../../Entity.hpp"

void ClientStandard::initialize()
{
	initialized = true;
	auto console = spdlog::get("console");
	console->info("Client Initialized");
	client.connect(enetpp::client_connect_params()
		.set_channel_count(game_data->getNetworkManager()->getChannelCount())
		.set_server_host_name_and_port(game_data->getNetworkManager()->getServerIP(), game_data->getNetworkManager()->getServerPort()));

	mc1 = on_packet_received.connect([](Packet p)
	{
		auto console = spdlog::get("console");
		//console->info("Received Packet {}", p.get_header().type);

		if (p.get_header().type == PacketType::CLIENT_INITIALIZED)
		{
			p.reset_read_position();
			uint32_t id;
			p >> id;
			console->info("Our ID is {}", id);
		}

		if (p.get_header().type == PacketType::ENTITY)
		{
			EntityInfo info;
			p >> info;
			//console->info("EntityPacket for {} owned by {} of type {}", info.ID, info.ownerID, info.type);
		}
	});
}

void ClientStandard::deinitialize()
{
	auto console = spdlog::get("console");
	initialized = false;
	console->info("Client Deinitialized");
	client.disconnect();
}

void ClientStandard::processPackets()
{
	auto on_connected = [this]()
	{
		auto console = spdlog::get("console");
		connected_to_server = true;
		console->info("Connected");
		Packet p({PacketType::CONNECTED, 0});
		p.info.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_disconnected = [this]()
	{
		auto console = spdlog::get("console");
		connected_to_server = false;
		console->info("Disconnected");
		Packet p({ PacketType::DISCONNECTED, 0 });
		p.info.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_data_received = [this](const enet_uint8* data, size_t data_size)
	{
		auto console = spdlog::get("console");
		Packet p(data, data_size);

		if (p.get_header().type == PacketType::CLIENT_INITIALIZED)
		{
			p >> id;
			console->info("Our ID is {} ", id);
		}

		p.reset_read_position();
		p.info.senderID = 1;
		pushPacket(std::move(p));
	};

	client.consume_events(std::move(on_connected),
		std::move(on_disconnected),
		std::move(on_data_received));
}

void ClientStandard::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	auto console = spdlog::get("console");
	//console->info("Client sending packet");
	client.send_packet(channel_id, p->get_data(), p->get_size(), flags);
}

