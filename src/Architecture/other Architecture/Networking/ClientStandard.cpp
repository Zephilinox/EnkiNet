#include "ClientStandard.hpp"

//STD
#include <iostream>

//SELF
#include "../Entity.hpp"

void ClientStandard::initialize()
{
	initialized = true;
	std::cout << "Client Initialized\n";
	client.connect(enetpp::client_connect_params()
		.set_channel_count(game_data->getNetworkManager()->getChannelCount())
		.set_server_host_name_and_port(game_data->getNetworkManager()->getServerIP(), game_data->getNetworkManager()->getServerPort()));

	mc1 = on_packet_received.connect([](Packet p)
	{
		std::cout << "Received Packet " << p.getID() << "\n";

		if (p.getID() == hash("ClientInitialized"))
		{
			p.resetSerializePosition();
			uint32_t id;
			p >> id;
			std::cout << "Our ID is " << id << "\n";
		}

		if (p.getID() == hash("Entity"))
		{
			EntityInfo info;
			p >> info;
			std::cout << "EntityPacket for " << info.networkID << " owned by " << info.ownerID << " of type " << info.type << "\n";
		}
	});
}

void ClientStandard::deinitialize()
{
	initialized = false;
	std::cout << "Client Deinitialized\n";
	client.disconnect();
}

void ClientStandard::processPackets()
{
	auto on_connected = [this]()
	{
		connected_to_server = true;
		std::cout << "Connected\n";
		Packet p;
		p.setID(hash("Connected"));
		p.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_disconnected = [this]()
	{
		connected_to_server = false;
		std::cout << "Disconnected\n";
		Packet p;
		p.setID(hash("Disconnected"));
		p.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_data_received = [this](const enet_uint8* data, size_t data_size)
	{
		Packet p(data, data_size);

		if (p.getID() == hash("ClientInitialized"))
		{
			p >> id;
			std::cout << "Our ID is " << id << "\n";
		}

		p.resetSerializePosition();
		p.senderID = 1;
		pushPacket(std::move(p));
	};

	client.consume_events(std::move(on_connected),
		std::move(on_disconnected),
		std::move(on_data_received));
}

void ClientStandard::sendPacket(enet_uint8 channel_id, Packet * p, enet_uint32 flags)
{
	std::cout << "client sending packet\n";
	client.send_packet(channel_id, p->buffer.data(), p->buffer.size(), flags);
}

