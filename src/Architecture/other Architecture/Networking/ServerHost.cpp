#include "ServerHost.hpp"

void ServerHost::initialize()
{
	initialized = true;
	std::cout << "Server Initialized\n";

	auto client_init = [&](ClientInfo& client, const char* ip)
	{
		client.id = getNextUID();

		//Tell the client what its ID is
		Packet p;
		p.setID(hash("ClientInitialized"));
		p << client.id;
		std::cout << "Client " << client.id << " initialized. IP = " << ip << "\n";
		sendPacketToOneClient(client.id, 0, &p);
	};

	mc1 = on_packet_received.connect([this](Packet p)
	{
		if (p.getID() == hash("Disconnected"))
		{
			free_ids.push(p.senderID);
			std::cout << "ServerHost:\tpushed new free id " << p.senderID << "\n";
		}
	});

	server.start_listening(enetpp::server_listen_params<ClientInfo>()
		.set_max_client_count(game_data->getNetworkManager()->getMaxClients())
		.set_channel_count(game_data->getNetworkManager()->getChannelCount())
		.set_listen_port(game_data->getNetworkManager()->getServerPort())
		.set_initialize_client_function(std::move(client_init)));
}
void ServerHost::deinitialize()
{
	initialized = false;
	std::cout << "Server Deinitialized\n";
	server.stop_listening();
}

void ServerHost::processPackets()
{
	auto on_client_connected = [&](ClientInfo& client)
	{
		std::cout << "Client " << client.id << " Connected\n";
		Packet p;
		p.setID(hash("Connected"));
		p.senderID = client.id;
		pushPacket(std::move(p));
	};

	auto on_client_disconnected = [&](uint32_t client_uid)
	{
		std::cout << "Client " << client_uid << " Disconnected\n";
		Packet p;
		p.setID(hash("Disconnected"));
		p.senderID = client_uid;
		pushPacket(std::move(p));
	};

	auto on_client_data_received = [&](ClientInfo& client, const enet_uint8* data, size_t data_size)
	{
		Packet p(data, data_size);
		p.senderID = client.id;
		pushPacket(std::move(p));
	};

	server.consume_events(std::move(on_client_connected),
		std::move(on_client_disconnected),
		std::move(on_client_data_received));
}

void ServerHost::sendPacketToOneClient(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	std::cout << "server sending packet to client " << client_id << "\n";
	if (client_id != 1)
	{
		server.send_packet_to(client_id, channel_id, p->buffer.data(), p->buffer.size(), flags);
	}
	else
	{
		p->resetSerializePosition();
		game_data->getNetworkManager()->client->on_packet_received.emit(*p);
	}
}

void ServerHost::sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	std::cout << "server sending packet to all clients\n";
	server.send_packet_to_all_if(channel_id, p->buffer.data(), p->buffer.size(), flags, [](const ClientInfo& client) {return true; });

	p->resetSerializePosition();
	game_data->getNetworkManager()->client->on_packet_received.emit(*p);
}

void ServerHost::sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate)
{
	std::cout << "server sending packet to some clients\n";
	server.send_packet_to_all_if(channel_id, p->buffer.data(), p->buffer.size(), flags, predicate);

	if (predicate({ 1 }))
	{
		p->resetSerializePosition();
		game_data->getNetworkManager()->client->on_packet_received.emit(*p);
	}
}

uint32_t ServerHost::getNextUID()
{
	if (free_ids.empty())
	{
		std::cout << "ServerHost:\tno free ids, giving " << next_uid << "\n";
		return next_uid++;
	}
	else
	{
		auto id = free_ids.front();
		std::cout << "ServerHost:\tooh free ids, giving " << id << "\n";
		free_ids.pop();
		return id;
	}
}
