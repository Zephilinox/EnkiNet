#pragma once

//LIB
#include <enetpp/client.h>
#include <spdlog/spdlog.h>

//SELF
#include "EnkiNet/Signals/Signal.hpp"
#include "EnkiNet/Networking/Packet.hpp"
#include "EnkiNet/Timer.hpp"

namespace enki
{
	class ClientInfo
	{
	public:
		ClientID id;

		//required by enetpp
		ClientID get_id() const noexcept
		{
			return id;
		}
	};

	class Server
	{
	public:
		virtual ~Server() = default;

		virtual void processPackets() = 0;

		virtual void sendPacketToOneClient(ClientID client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
		virtual void sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
		virtual void sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) = 0;
		virtual void sendPacketToAllExceptOneClient(ClientID client_id_excluded, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;

		virtual bool isListening() const = 0;
		virtual const std::vector<ClientInfo*>& getConnectedClients() const = 0;

		void update();
		void pushPacket(Packet&& p);

		Signal<Packet> on_packet_received;
	
	protected:
		Server() = default;

	private:
		std::mutex mutex;
		std::queue<Packet> packets;

		Timer packetsTimer;
		uint32_t packetsReceived = 0;
	};
}