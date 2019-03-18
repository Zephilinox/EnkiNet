#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Client.hpp"
#include "ServerHost.hpp"
#include "../../GameData.hpp"

namespace enki
{
	class ClientHost : public Client
	{
	public:
		ClientHost(GameData* game_data);
		~ClientHost() final;

		void processPackets() final {};
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
	};
}