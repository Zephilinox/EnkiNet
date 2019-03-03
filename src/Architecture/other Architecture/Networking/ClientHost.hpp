#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Client.hpp"
#include "ServerHost.hpp"
#include "../GameData.hpp"

class ClientHost : public Client
{
public:
	ClientHost(GameData* game_data)
		: Client(game_data)
	{
		id = 1;
	}

	~ClientHost() = default;

	void initialize() final;
	void deinitialize() final;

	void processPackets() final
	{};

	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;

	inline bool isConnected() const final
	{
		return true;
	}

	inline bool isConnecting() const final
	{
		return true;
	}

private:
	ManagedConnection mc1;
};