#pragma once

//STD
#include <iostream>

//LIB
#include <enetpp/server.h>

//SELF
#include "Client.hpp"
#include "Server.hpp"

class ServerHost : public Server
{
public:
	ServerHost(GameData* game_data)
		: Server(game_data)
	{}

	~ServerHost() final = default;

	void initialize() final;
	void deinitialize() final;

	void processPackets() final;

	void sendPacketToOneClient(ClientID client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;
	void sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;
	void sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) final;
	void sendPacketToAllExceptOneClient(ClientID client_id_excluded, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;

	inline bool isListening() const final
	{
		return server.is_listening();
	}

	inline const std::vector<ClientInfo*>& getConnectedClients() const final
	{
		return server.get_connected_clients();
	}

private:
	ClientID getNextUID();

	enetpp::server<ClientInfo> server;
	ClientID next_uid = 2;
	std::queue<ClientID> free_ids;

	ManagedConnection mc1;
};