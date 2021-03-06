#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "EnkiNet/Networking/Client.hpp"
#include "EnkiNet/Networking/ServerHost.hpp"
#include "EnkiNet/GameData.hpp"

namespace enki
{
	class ClientHost : public Client
	{
	public:
		ClientHost();
		~ClientHost() final;

		//Called by the network manager in a different thread
		void processPackets() final {};

		//Send a packet to the server directly
		void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;

		inline bool isConnected() const final
		{
			return true;
		}

		inline bool isConnecting() const final
		{
			return true;
		}

		//This must be assigned after creating a ClientHost
		//Used to send packets to the local server directly
		//Cannot be passed through in constructor due to ServerHost requiring access to the client
		Server* server;

	private:
	};
}