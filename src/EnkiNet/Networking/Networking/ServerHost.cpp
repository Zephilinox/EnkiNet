#include "ServerHost.hpp"

void ServerHost::initialize()
{
	initialized = true;
	auto console = spdlog::get("EnkiNet");
	console->info("Server Initialized");

	auto client_init = [this](ClientInfo& client, const char* ip)
	{
		auto console = spdlog::get("EnkiNet");
		client.id = getNextUID();

		//Tell the client what its ID is
		Packet p({PacketType::CLIENT_INITIALIZED});
		p << client.id;
		console->info("Client {} initialized. IP = {}" , client.id, ip);
		sendPacketToOneClient(client.id, 0, &p);
	};

	mc1 = on_packet_received.connect([this](Packet p)
	{
		if (p.getHeader().type == PacketType::DISCONNECTED)
		{
			free_ids.push(p.info.senderID);
			auto console = spdlog::get("EnkiNet");
			console->info("ServerHost:\tpushed new free id {}", p.info.senderID);
		}
	});

	server.start_listening(enetpp::server_listen_params<ClientInfo>()
		.set_max_client_count(game_data->getNetworkManager()->max_clients)
		.set_channel_count(game_data->getNetworkManager()->channel_count)
		.set_listen_port(game_data->getNetworkManager()->server_port)
		.set_initialize_client_function(std::move(client_init)));
}
void ServerHost::deinitialize()
{
	initialized = false;
	auto console = spdlog::get("EnkiNet");
	console->info("Server Deinitialized");
	server.stop_listening();
}

void ServerHost::processPackets()
{
	auto on_client_connected = [&](ClientInfo& client)
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Client {} Connected.", client.id);
		Packet p({PacketType::CONNECTED});
		p.info.senderID = client.id;
		pushPacket(std::move(p));
	};

	auto on_client_disconnected = [&](ClientID client_uid)
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Client {} Disconnected", client_uid);
		Packet p({ PacketType::DISCONNECTED });
		p.info.senderID = client_uid;
		pushPacket(std::move(p));
	};

	auto on_client_data_received = [&](ClientInfo& client, const enet_uint8* data, size_t data_size)
	{
		auto console = spdlog::get("EnkiNet");
		//console->info("Data received from Client {}. Size = {}", client.get_id(), data_size);
		Packet p(data, data_size);
		p.info.senderID = client.id;
		pushPacket(std::move(p));
	};

	server.consume_events(std::move(on_client_connected),
		std::move(on_client_disconnected),
		std::move(on_client_data_received));
}

void ServerHost::sendPacketToOneClient(ClientID client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	auto console = spdlog::get("EnkiNet");
	//console->info("Server sending packet to client {}", client_id);
	if (client_id != 1)
	{
		auto data = reinterpret_cast<const enet_uint8*>(p->getBytes().data());
		server.send_packet_to(client_id, channel_id, data, p->getSize(), flags);
	}
	else
	{
		p->resetReadPosition();
		game_data->getNetworkManager()->client->on_packet_received.emit(*p);
	}
}

void ServerHost::sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	auto console = spdlog::get("EnkiNet");
	//console->info("Server sending packet to all clients");
	auto data = reinterpret_cast<const enet_uint8*>(p->getBytes().data());
	server.send_packet_to_all_if(channel_id, data, p->getSize(), flags, []([[maybe_unused]]const ClientInfo& client) {return true; });

	p->resetReadPosition();
	game_data->getNetworkManager()->client->on_packet_received.emit(*p);
}

void ServerHost::sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate)
{
	auto console = spdlog::get("EnkiNet");
	//console->info("Server sending packet to some clients\n");
	auto data = reinterpret_cast<const enet_uint8*>(p->getBytes().data());
	server.send_packet_to_all_if(channel_id, data, p->getSize(), flags, predicate);

	if (predicate({ 1 }))
	{
		p->resetReadPosition();
		game_data->getNetworkManager()->client->on_packet_received.emit(*p);
	}
}

ClientID ServerHost::getNextUID()
{
	auto console = spdlog::get("EnkiNet");
	if (free_ids.empty())
	{
		console->info("ServerHost:\tno free ids, giving {}", next_uid);
		return next_uid++;
	}
	else
	{
		auto id = free_ids.front();
		console->info("ServerHost:\tooh free ids, giving {}", id);
		free_ids.pop();
		return id;
	}
}
