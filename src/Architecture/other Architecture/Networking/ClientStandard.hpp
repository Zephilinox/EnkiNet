#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Client.hpp"
#include "../GameData.hpp"

class ClientStandard : public Client
{
public:
	ClientStandard(GameData* game_data)
		: Client(game_data)
	{}

	~ClientStandard() = default;

	void initialize() final;
	void deinitialize() final;

	void processPackets() final;

	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;

	inline bool isConnected() const final
	{
		return isConnecting() && connected_to_server;
	}

	inline bool isConnecting() const final
	{
		return client.is_connecting_or_connected();
	}

private:
	enetpp::client client;
	bool connected_to_server = false;

	ManagedConnection mc1;
};